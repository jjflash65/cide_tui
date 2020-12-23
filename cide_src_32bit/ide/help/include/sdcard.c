/*
 * 	Doku, siehe http://www.mikrocontroller.net/articles/AVR_FAT32
 *      Neuste Version: http://www.mikrocontroller.net/svnbrowser/avr-fat32/
 *	Autor: Daniel R.
 *
 *      Minimalmodifikationen fuer RSIDE: R. Seelig
 *
 *      Dieses Softwaremodul ist eine "ineinander" kopierte Datei aus den
 *      3 Quelldateien: file.c / fat.c / mmc.c
 */

#include "sdcard_config.h"
#include "sdcard.h"


// timer0 variable
volatile uint8_t 	TimingDelay;	// fuer mmc.c


struct File_t file;			// wichtige dateibezogene daten/variablen

#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
	uint8_t  multiblock_flag;
#endif


//*******************************************************************************************************************************
// funktionsprototypen die nur in dieser datei benutzt werden !
#if (MMC_LS==TRUE)
	static void lsRowsOfClust (fptr_t uputs_ptr, uint32_t start_sec);	// zeigt reihen eines clusters an, ab start_sec, muss zeiger auf eine ausgabe funktion überbeben bekommen
#endif
//*******************************************************************************************************************************


#if (MMC_FILE_EXSISTS==TRUE)
// *******************************************************************************************************************************
// prueft ob es die datei im aktuellen verzeichnis gibt.
// ffopen wuerde die datei direkt anlegen, falls es sie noch nicht gibt!
// *******************************************************************************************************************************
uint8_t ffileExsists ( uint8_t name[]){
	return fat_loadFileDataFromDir(name);
}
#endif

//*******************************************************************************************************************************
// 2 moeglichkeiten beim oeffnen, datei existiert(return MMC_FILE_OPENED) oder muss angelegt werden(return MMC_FILE_CREATED)
// zuerst wird geprueft ob es die datei im verzeichniss gibt. danach wird entschieden, ob die datei geoeffnet wird oder angelegt.
// -beim oeffnen werden die bekannten cluster gesucht maximal MAX_CLUSTERS_IN_ROW in reihe. dann wird der 1. sektor der datei auf
// den puffer fat.sector geladen. jetzt kann man ffread lesen...
// -beim anlegen werden freie cluster gesucht, maximal MAX_CLUSTERS_IN_ROW in reihe. dann wird das struct file gefuellt.
// danach wird der dateieintrag gemacht(auf karte). dort wird auch geprueft ob genügend platz im aktuellen verzeichniss existiert.
// moeglicherweise wird der 1. cluster der datei nochmal geaendert. jetzt ist der erste frei sektor bekannt und es kann geschrieben werden.
//*******************************************************************************************************************************
uint8_t ffopen( uint8_t name[], uint8_t rw_flag){

	uint8_t file_flag = fat_loadFileDataFromDir(name);	//pruefung ob datei vorhanden. wenn vorhanden, laden der daten auf das file struct.

	if( file_flag==TRUE && rw_flag == 'r' ){					/** datei existiert, alles vorbereiten zum lesen **/
		fat_getFatChainClustersInRow( file.firstCluster );		// verkettete cluster aus der fat-chain suchen.
		file.name = name;
		#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
			fat.currentSectorNr = fat.startSectors;
			multiblock_flag = TRUE;
			mmc_multi_block_start_read (fat.startSectors);
			mmc_multi_block_read_sector (fat.sector);
		#else
			fat_loadSector( chain.startSectors );				// laed die ersten 512 bytes der datei auf puffer:sector.
		#endif
		return MMC_FILE_OPENED;
	}

	#if (MMC_WRITE==TRUE)										// anlegen ist schreiben !
	if( file_flag==FALSE && rw_flag == 'c' ){					/** datei existiert nicht, also anlegen !	(nur wenn schreiben option an ist)**/
		file.name = name;										// pointer "sichern" ;)
		fat_getFreeClustersInRow( 2 ); 							// freie cluster aus der fat suchen. wird eigentlich erst bei fat_getFreeRowsOfDir benoetigt und spaeter bei ffwrite !
		fat_makeFileEntry(name,0x20);							// DATEI ANLEGEN auf karte
		#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
			multiblock_flag = FALSE;
			mmc_multi_block_start_write(fat.currentSectorNr);
		#endif
		fat.bufferDirty = TRUE;									// puffer dirty weil geschrieben und noch nicht auf karte.
		return MMC_FILE_CREATED;
	}
	#endif

   	return MMC_FILE_ERROR;
}


//*******************************************************************************************************************************
// schliesst die datei operation ab. eigentlich nur noetig wenn geschrieben/ueberschrieben wurde. es gibt 2 moeglichkeiten :
// 1. die datei wird geschlossen und es wurde ueber die alte datei länge hinaus geschrieben.
// 2. die datei wird geschlossen und man war innerhalb der datei groesse, dann muss nur der aktuelle sektor geschrieben werden.
// der erste fall ist komplizierter, weil ermittelt werden muss wie viele sektoren neu beschrieben wurden um diese zu verketten
// und die neue datei laenge muss ermitt weden. abschließend wird entweder (fall 2) nur der aktuelle sektor geschrieben, oder
// der aktuallisierte datei eintrag und die cluster (diese werden verkettet, siehe fileUpdate() ).
//*******************************************************************************************************************************
uint8_t ffclose(){

	#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
		if(multiblock_flag == TRUE){
			mmc_multi_block_stop_read(); 	// stoppen von multiblock aktion
		}
		else{
			mmc_multi_block_stop_write(); 	// stoppen von multiblock aktion
		}
	#endif

	#if (MMC_WRITE==TRUE) 	/** 2 moeglichkeiten beim schliessen !!	(lesend spielt keine rolle, nichts muss geupdatet werden) **/
		fflushFileData();
	#endif

	file.cntOfBytes = 0;
	file.seek = 0;

	return TRUE;
}


#if (MMC_WRITE==TRUE)
// *******************************************************************************************************************************
// füllt grade bearbeiteten  sektor auf, verkette die bis jetzt benutzten cluster. macht datei eintrag update.
// *******************************************************************************************************************************
void fflushFileData(void){

	uint32_t new_length;
	uint32_t save_currentSectorNr;
	uint16_t 	count;
	uint16_t 	row;

	#if (MMC_ENDIANNESS_LITTLE == TRUE)
		void 				*psector;
	#else
		uint8_t 		*psector;
	#endif

	/** 2 moeglichkeiten ab hier **/
	/** 1.) es wurde ueber die alte datei groesse hinaus geschrieben **/
	if( file.length < (file.seek + file.cntOfBytes) )	{

		new_length = file.seek + file.cntOfBytes;
		save_currentSectorNr = file.currentSectorNr;

		// wenn fat.cntOfBytes == 0 ist der sektor gerade vorher schon geschrieben worden oder es ist noch nichts hinein geschrieben worden. dann wert so setzten, dass die schleife uebersprungen wird
		count = (file.cntOfBytes==0) ? 512:file.cntOfBytes;
		while( count < 512 ){						// sektor ist beschrieben worden, daher rest mit 00 fuellen
			fat.sector[count] = 0x00;
			count += 1;
			fat.bufferDirty = TRUE;
		}

		// cluster chain verketten.
		if(file.cntOfBytes == 0){								// gerade genau irgend einen sektor voll geschrieben.file.currentSectorNr zeigt jetzt auf den neu zu beschreibenden.
			if( file.currentSectorNr-1 >= chain.startSectors ){
				fat_setClusterChain(fat_secToClust(chain.startSectors),fat_secToClust(file.currentSectorNr-1));	// verketten der geschriebenen cluster
			}
		}
		else fat_setClusterChain( fat_secToClust(chain.startSectors),fat_secToClust(file.currentSectorNr) );


		// dateigroesse updaten
		fat_loadSector(file.entrySector);						// laden des sektors mit dem dateieintrag zwecks update :)
		row = file.row;
		row <<=5;  												// um von reihen nummer auf byte zahl zu kommen
		psector =& fat.sector[row+28];

		#if (MMC_ENDIANNESS_LITTLE == TRUE)
			*(uint32_t*)psector = new_length;
		#else
			*psector++ = new_length;
			*psector++ = new_length >> 8;
			*psector++ = new_length >> 16;
			*psector   = new_length >> 24;
		#endif

		file.length = new_length;								// die richtige groesse sichern, evtl beim suchen der datei mit mist vollgeschrieben
		chain.startSectors = save_currentSectorNr;				// da die sektoren bis zu save_currentSectorNr schon verkettet wurden, reicht es jetzt die kette ab da zu betrachten.

		// daruer sorgen, dass der sektor mit dem geaenderten datei eintrag auf die karte geschrieben wird!
		fat.bufferDirty = TRUE;
		fat_loadSector( save_currentSectorNr );
	}

	/** 2.) es wurde nicht ueber die alte datei groesse hinaus geschrieben **/
	else{
		fat_writeSector( file.currentSectorNr );		// einfach den gerade bearbeiteten sektor schreiben reicht, alle dateiinfos stimmen weil nicht ueber die alte groesse hinaus geschrieben
	}

}
#endif

#if (MMC_SEEK==TRUE)
// *******************************************************************************************************************************
// offset byte wird uebergeben. es wird durch die sektoren der datei gespult (gerechnet), bis der sektor mit dem offset byte erreicht
// ist, dann wird der sektor geladen und der zaehler fuer die bytes eines sektors gesetzt. wenn das byte nicht in den sektoren ist,
// die "vorgesucht" wurden, muessen noch weitere sektoren der datei gesucht werden (sec > fat.endSectors).
// *******************************************************************************************************************************
void ffseek(uint32_t offset){

	uint32_t sectors;							// zum durchgehen durch die sektoren der datei

	#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
		uint8_t seek_end;
		seek_end = FALSE;

		if(offset == file.length) {						// zum ende spulen, kann dann nur schreiben sein...
			seek_end = TRUE;
		}
		if(multiblock_flag == FALSE){
			mmc_multi_block_stop_write();
		}
		else {
			mmc_multi_block_stop_read();
		}
	#endif

	#if (MMC_OVER_WRITE==TRUE && MMC_WRITE==TRUE)		// nur wenn ueberschreiben an ist.
		fflushFileData();								// fat verketten und datei update auf der karte !
	#endif

	fat_getFatChainClustersInRow(file.firstCluster);	// suchen von anfang der cluster chain aus, das geht nicht vom ende zum anfang oder so, weil es eine einfach verkettete liste ist !
	sectors = chain.startSectors;							// sektor variable zum durchgehen durch die sektoren
	file.seek = 0;										// weil auch von anfang an der chain gesucht wird mit 0 initialisiert

	// suchen des sektors in dem offset ist
	while( offset >= 512 ){
		sectors += 1;									// da byte nicht in diesem sektor ist, muss hochgezählt werden
		offset -= 512;									// ein sektor weniger in dem das byte sein kann
		file.seek += 512;								// file.seek update, damit bei ffclose() die richtige leange herauskommt
		chain.cntSecs -= 1;								// damit die zu lesenden/schreibenden sektoren in einer reihe stimmen
		if ( chain.cntSecs == 0 ){						// es muessen mehr sektoren der datei gesucht werden. sektoren in einer reihe durchgegangen
			fat_getFatChainClustersInRow(fat_getNextCluster( chain.lastCluster ) );	// suchen von weiteren sektoren der datei in einer reihe
			sectors = chain.startSectors;						// setzen des 1. sektors der neu geladenen, zum weitersuchen !
		}
	}

	fat_loadSector(sectors);  								// sektor mit offset byte laden
	file.cntOfBytes = offset;								// setzen des lese zaehlers

	#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
		if(seek_end == TRUE) {
			mmc_multi_block_start_write(sectors);			// starten von multiblock aktion
			fat.bufferDirty = TRUE;
		}
		else {
			mmc_multi_block_start_read(sectors);			// starten von multiblock aktion
		}
	#endif
}
#endif


#if (MMC_CD==TRUE)
// *******************************************************************************************************************************
// wechselt verzeichniss. start immer im root Dir.
// es MUSS in das direktory gewechselt werden, in dem die datei zum lesen/schreiben ist !
// *******************************************************************************************************************************
uint8_t ffcd( uint8_t name[]){

	if(name[0]==0){									// ZUM ROOTDIR FAT16/32
		fat.dir=0;									// root dir
		return TRUE;
	}

	if( TRUE == fat_loadFileDataFromDir(name) ){	// NICHT ROOTDIR	(fat16/32)
		fat.dir = file.firstCluster;				// zeigt auf 1.cluster des dir	(fat16/32)
		return TRUE;
	}

	return FALSE;									// dir nicht gewechselt (nicht da?) !!
}

//*******************************************************************************************************************************
// wechselt in das parent verzeichniss (ein verzeichniss zurueck !)
// die variable fat.dir enthält den start cluster des direktory in dem man sich grade befindet, anhand diesem,
// kann der "." bzw ".." eintrag im ersten sektor des direktory ausgelesen und das parent direktory bestimmt werden.
//*******************************************************************************************************************************
uint8_t ffcdLower(void){

  if( fat.dir == 0 )return FALSE;			// im root dir, man kann nicht hoeher !

  fat_loadSector(fat_clustToSec(fat.dir));	// laed 1. sektor des aktuellen direktory.
  fat_loadRowOfSector(1);					// ".." eintrag (parent dir) ist 0 wenn parent == root
  fat.dir=file.firstCluster;				// dir setzen

  return TRUE;
}
#endif


#if (MMC_LS==TRUE)
// *******************************************************************************************************************************
// zeigt reihen eines clusters an, wird fuer ffls benoetigt !
// es wird ab dem start sektor start_sec, der dazugehoerige cluster angezeigt. geprueft wird ob es ein richtiger
// eintrag in der reihe ist (nicht geloescht, nicht frei usw). die sektoren des clusters werden nachgeladen.
// die dateien werden mit namen und datei groesse angezeigt.
// *******************************************************************************************************************************
static void lsRowsOfClust (fptr_t uputs_ptr,uint32_t start_sec){

	uint16_t row;				// reihen
	uint8_t sec;				// sektoren
	uint8_t tmp[12];			// tmp string zur umwandlung
	uint8_t i;				// um dateinamen zu lesen

	sec=0;
	do{
		fat_loadSector( start_sec + sec );	// sektoren des clusters laden
		for( row=0; row<512; row+=32 ){		// geht durch reihen des sektors

			if( (fat.sector[row+11]==0x20||fat.sector[row+11]==0x10) && (fat.sector[row]!=0xE5 && fat.sector[row]!=0x00) ){

				// namen extrahieren und anzeigen
				for( i=0 ; i<11 ; i++ ){
					tmp[i]=fat.sector[row+i];
				}
				tmp[i]='\0';
				uputs_ptr(tmp);

				// reihe auf file stuct laden um file.length zu bekommen. koverieren und anzeigen...
				fat_loadRowOfSector(row);
				uputs_ptr((uint8_t*)" ");
				ltostr(file.length,(char*)tmp,12,10);
				uputs_ptr(tmp);
				uputs_ptr((uint8_t*)"\n\r");
			}
		}
	}while( ++sec < fat.secPerClust );
}

// *******************************************************************************************************************************
// zeigt inhalt eines direktory an.
// unterscheidung ob man sich im rootDir befindet noetig, weil bei fat16 im root dir eine bestimmt anzahl sektoren durchsucht
// werden muessen und bei fat32 ab einem start cluster ! ruft lsRowsOfClust auf um cluster/sektoren anzuzeigen.
// *******************************************************************************************************************************
void ffls(fptr_t uputs_ptr){

	uint8_t 		sectors;	// variable um durch sektoren zu zaehlen
	uint32_t 	clusters;	// variable um durch cluster des verzeichnisses zu gehen

	// bestimmen ab welchem cluster eintraege angezeigt werden sollen, bzw in welchem ordner man sich befindet
	clusters = (fat.dir==0) ? fat.rootDir:fat.dir;

	// root-dir fat16 nach eintrag durchsuchen. es bleiben noch 3 moeglichkeiten: nicht root-dir fat16, nicht root-dir fat32 und root-dir fat32
	if(fat.dir==0 && fat.fatType==16){
		sectors = 0;
		do{
			// root-dir eintraege anzeigen
			lsRowsOfClust( uputs_ptr, clusters + sectors );
			sectors += fat.secPerClust;
		}while( sectors < (uint8_t)32 );
	}

	// root-dir fat32 oder nicht root-dir fat32/16, nach eintrag durchsuchen
	else {
		// durch cluster des verzeichnisses gehen und eintraege anzeigen
		while(!((clusters>=0x0ffffff8&&fat.fatType==32)||(clusters>=0xfff8&&fat.fatType==16))){		// prueft ob weitere sektoren zum lesen da sind (fat32||fat16)
			lsRowsOfClust( uputs_ptr, fat_clustToSec(clusters) );									// zeigt reihen des clusters an
			clusters = fat_getNextCluster(clusters);												// liest naechsten cluster des dir-eintrags (unterverzeichniss groeßer 16 einträge)
		}
	}

}
#endif



#if (MMC_WRITE==TRUE && MMC_MKDIR==TRUE)
// *******************************************************************************************************************************
// erstellt einen dir eintrag im aktuellen verzeichnis.
// prueft ob es den den dir-namen schon gibt, dann wird nichts angelegt.
// wenn ok, dann wird ein freier cluster gesucht, als ende markiert, der eintrag ins dir geschrieben.
// dann wird der cluster des dirs aufbereitet. der erste sektor des clusters enthaelt den "." und ".." eintrag.
// der "." hat den 1. cluster des eigenen dirs. der ".." eintrag ist der 1. cluster des parent dirs.
// ein dir wird immer mit 0x00 initialisiert ! also alle eintraege der sektoren des clusters ( bis auf . und .. einträge)!
// *******************************************************************************************************************************
void ffmkdir( uint8_t name[]){

	uint8_t i;								// variable zum zaehlen der sektoren eines clusters.
	uint16_t j;								// variable zum zaehlen der sektor bytes auf dem puffer fat.sector.
	uint8_t l;

	if(TRUE==fat_loadFileDataFromDir(name))			// prueft ob dirname im dir schon vorhanden, wenn ja, abbruch !
		return ;

	// cluster in fat setzen, und ordner eintrg im aktuellen verzeichniss machen.
	fat_getFreeClustersInRow(2);									// holt neue freie cluster, ab 2 ...
	fat_setCluster(fat_secToClust(chain.startSectors),0x0fffffff);	// fat16/32 cluster chain ende setzen.	(neuer ordner in fat)
	file.firstCluster = fat_secToClust(chain.startSectors);			// damit fat_makeFileEntry den cluster richtig setzen kann

	fat_makeFileEntry(name,0x10); 				// legt ordner im partent verzeichnis an.

	// aufbereiten des puffers
	j=511;
	do{
		fat.sector[j]=0x00;						//schreibt puffer fat.sector voll mit 0x00==leer
	}while(j--);

	// aufbereiten des clusters
	for(i=1;i<fat.secPerClust;i++){				// ein dir cluster muss mit 0x00 initialisiert werden !
		fat_writeSector(chain.startSectors+i);	// loeschen des cluster (ueberschreibt mit 0x00), wichtig bei ffls!
	}

	l = 0;
	do{
		fat.sector[l] = ' ';
		fat.sector[l+32] = ' ';
	}while(++l<11);

	fat.sector[l]= 0x10;
	fat.sector[l+32] = 0x10;

	fat.sector[0] = '.';
	fat.sector[32] = '.';
	fat.sector[33] = '.';

	#if (MMC_ENDIANNESS_LITTLE==TRUE)
		void *vsector =& fat.sector[20];

		*(uint16_t*)vsector=(file.firstCluster&0xffff0000)>>16;		// hi word	von cluster (20,2)
		vsector += 6;

		*(uint16_t*)vsector=(file.firstCluster&0x0000ffff);			// low word von cluster (26,2)

		vsector += 26;

		*(uint16_t*)vsector=(fat.dir&0xffff0000)>>16;				// hi word	von cluster (20,2)
		vsector+=6;

		*(uint16_t*)vsector=(fat.dir&0x0000ffff);					// low word von cluster (26,2)

	#else
		uint8_t *psector =& fat.sector[20];

		*psector++=(file.firstCluster&0x00ff0000)>>16;				// hi word	von cluster (20,2)
		*psector=(file.firstCluster&0xff000000)>>24;
		psector += 5;

		*psector++=file.firstCluster&0x000000ff;					// low word von cluster (26,2)
		*psector=(file.firstCluster&0x0000ff00)>>8;

		psector += 25;

		*psector++=(fat.dir&0x00ff0000)>>16;						// hi word	von cluster (20,2)
		*psector=(fat.dir&0xff000000)>>24;
		psector += 5;

		*psector++=fat.dir&0x000000ff;								// low word von cluster (26,2)
		*psector=(fat.dir&0x0000ff00)>>8;


	#endif

	// "." und ".." eintrag auf karte schreiben
	mmc_write_sector(chain.startSectors,fat.sector);
}
#endif



#if (MMC_WRITE==TRUE && MMC_RM==TRUE)
//*******************************************************************************************************************************
// loescht datei/ordner aus aktuellem verzeichniss, wenn es die/den datei/ordner gibt.
// loescht dateien und ordner rekursiv !
//*******************************************************************************************************************************
uint8_t ffrm( uint8_t name[] ){

	#if(MMC_RM_FILES_ONLY==FALSE)
		uint32_t parent;		// vaterverzeichnis cluster
		uint32_t own;			// cluster des eigenen verzeichnis
		uint32_t clustsOfDir;	// um durch die cluster eines dirs zu gehen
		uint8_t cntSecOfClust;	// um durch die sektoren eines clusters zu gehen
	#endif

	uint16_t row;				// um durch die reihen eines sektors zu gehen. springt immer auf das anfangsbyte eines eintrags
	uint8_t fileAttrib;			// zum sichern des dateiattributs

	// datei/ordner nicht vorhanden, dann nicht loschen...
	if(FALSE==fat_loadFileDataFromDir(name)){
		return FALSE;
	}

	// reihe zu byte offset umrechnen, attribut sichern. anhand des attributs wird spaeter weiter entschieden
	row = file.row;
	row = row<<5;
	fileAttrib = fat.sector[row+11];

	#if(MMC_RM_FILES_ONLY==TRUE)
		// keine datei, also abbruch !
		if(fileAttrib!=0x20) return FALSE;
	#endif

	//////// ob ordner oder datei, der sfn und lfn muss geloescht werden!
	fat.bufferDirty = TRUE;						// damit beim laden der geaenderte puffer geschrieben wird
	do{
		fat.sector[row] = 0xE5;
		if( row == 0 ){							// eintraege gehen im vorherigen sektor weiter
			fat_loadSector(fat.lastSector);		// der davor ist noch bekannt. selbst wenn der aus dem cluster davor stammt.
			fat.bufferDirty = TRUE;
			row = 512;							// da nochmal row-=32 gemacht wird, kommt man so auf den anfang der letzten zeile
		}
		row -= 32;
	}while(  fat.sector[row+11]==0x0f && (fat.sector[row]&0x40) != 0x40);		// geht durch alle reihen bis neuer eintrag erkannt wird...

	fat.sector[row] = 0xE5;

	#if(MMC_RM_FILES_ONLY==TRUE)
		fat_delClusterChain(file.firstCluster);	// loescht die zu der datei gehoerige cluster-chain aus der fat.
	#else
		//// ist datei. dann nur noch cluster-chain loeschen und fertig.
		if( fileAttrib == 0x20 ){
			fat_delClusterChain(file.firstCluster);	// loescht die zu der datei gehoerige cluster-chain aus der fat.
		}
		//// ist ordner. jetzt rekursiv durch alle zweige/aeste des baums, den dieser ordner evtl. aufspannt, und alles loeschen!
		else{
			clustsOfDir = file.firstCluster;
			// jetzt eigentlichen ordnerinhalt rekursiv loeschen!
			do{
				do{
					cntSecOfClust=0;
					do{
						fat_loadSector( fat_clustToSec(clustsOfDir) + cntSecOfClust );		// in ordner wechseln / neuen cluster laden zum bearbeiten
						row=0;
						do{
							// nach einem leeren eintrag kommt nix mehr. cntSecOfClust wird auch gesetzt, damit man auch durch die naechste schleife faellt!
							if(fat.sector[row]==0x00){
								cntSecOfClust = fat.secPerClust;
								break;
							}

							// sfn eintrag erkannt. eintrag geloescht markieren, cluster chain loeschen, diesen sektor neu laden!
							if( fat.sector[row]!=0xE5 && fat.sector[row+11]!=0x10 && fat.sector[row+11]!=0x0F ){
								fat_loadRowOfSector(row);
								fat.sector[row] = 0xE5;		// hier evtl. nur pruefen ob file.firstCluster nicht schon in der fat geloescht ist...
								fat.bufferDirty = TRUE;
								fat_delClusterChain(file.firstCluster);
								fat_loadSector( fat_clustToSec(clustsOfDir) + cntSecOfClust );	// aktuellen sektor wieder laden...wurde bei fat_delClusterChain geaendert
							}

							// punkt eintrag erkannt, own und parent sichern!
							if( fat.sector[row]=='.' && row==0 && fat.sector[row+11]==0x10){
								fat_loadRowOfSector(row);
								own = file.firstCluster;
								fat_loadRowOfSector(row+32);
								parent = file.firstCluster;
							}

							// ordner erkannt. jetzt ordner eintrag loeschen, hinein wechseln und alles von vorne...
							if(  fat.sector[row]!='.' && fat.sector[row]!=0xE5 && fat.sector[row+11]==0x10){
								fat_loadRowOfSector(row);
								fat.sector[row] = 0xE5;
								fat.bufferDirty = TRUE;
								clustsOfDir = file.firstCluster;
								fat_loadSector( fat_clustToSec(clustsOfDir) );	// hier wird in das dir gewechselt!
								row = 0;
								cntSecOfClust = 0;
								continue;
							}

							row += 32;
							}while( row < 512 );
					}while( ++cntSecOfClust < fat.secPerClust );
					clustsOfDir = fat_getNextCluster(clustsOfDir);		// holen des folgeclusters um durch die chain zu gehen
				}while( !((clustsOfDir>=0x0ffffff8 && fat.fatType==32) || (clustsOfDir==0xfff8 && fat.fatType==16)) );		// geht durch cluster des dirs.

				// hier ist man am ende eines astes. also ast loeschen und zuruck zum stamm :)
				fat_delClusterChain(own);

				// stamm ist ein weiterer ast wenn parent != fat.dir. ast wird oben geladen und weiter verarbeitet
				clustsOfDir = parent;

			}while(parent!=fat.dir);
		}
	#endif
	return TRUE;				// alles ok, datei/ordner geloescht !
}
#endif


// *******************************************************************************************************************************
// liest 512 bytes aus dem puffer fat.sector. dann werden neue 512 bytes der datei geladen, sind nicht genuegend verkettete
// sektoren in einer reihe bekannt, wird in der fat nachgeschaut. dann werden weiter bekannte nachgeladen...
// *******************************************************************************************************************************
uint8_t ffread(void){

	uint32_t nc;

	if( file.cntOfBytes == 512 ){							// EINEN SEKTOR GLESEN (ab hier 2 moeglichkeiten !)

		chain.cntSecs-=1;										// anzahl der sektoren am stück werden weniger, bis zu 0 dann müssen neue gesucht werden...

		if( 0==chain.cntSecs ){		 						//1.) noetig mehr sektoren der chain zu laden (mit ein bisschen glück nur alle 512*MAX_CLUSTERS_IN_ROW bytes)

			#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
				mmc_multi_block_stop_read ();					// stoppen von multiblock aktion
			#endif

			nc = fat_secToClust( file.currentSectorNr );		// umrechnen der aktuellen sektor position in cluster
			nc = fat_getNextCluster(nc);					// in der fat nach neuem ketten anfang suchen
			fat_getFatChainClustersInRow(nc);				// zusammenhängende cluster/sektoren der datei suchen
			file.currentSectorNr = chain.startSectors - 1;		// hier muss erniedrigt werden, da nach dem if immer ++ gemacht wird

			#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
				mmc_multi_block_start_read (fat.startSectors);	// starten von multiblock lesen ab dem neu gesuchten start sektor der kette.
			#endif
		}

		file.cntOfBytes = 0;

		#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE==FALSE)
			fat.currentSectorNr+=1;
			mmc_multi_block_read_sector (fat.sector);			//2.) bekannte sektoren reichen noch, also einfach nachladen..
		#else
			fat_loadSector(file.currentSectorNr+1);				//2.) die bekannten in einer reihe reichen noch.(nur alle 512 bytes)
		#endif
	}
	return fat.sector[file.cntOfBytes++];
}



#if (MMC_OVER_WRITE==FALSE && MMC_WRITE==TRUE)			// nicht ueberschreibende write funktion
//*******************************************************************************************************************************
// schreibt 512 byte bloecke auf den puffer fat.sector. dann wird dieser auf die karte geschrieben. wenn genuegend feie
// sektoren zum beschreiben bekannt sind(datenmenge zu gross), muss nicht in der fat nachgeschaut werden. sollten nicht genuegend
// zusammenhaengende sektoren bekannt sein, werden die alten verkettet und neue gesucht. es ist noetig sich den letzten bekannten
// einer kette zu merken -> file.lastCluster, um auch nicht zusammenhaengende cluster verketten zu koennen (fat_setClusterChain macht das)!
//*******************************************************************************************************************************
void ffwrite( uint8_t c){

	fat.sector[file.cntOfBytes++] = c;							// schreiben des chars auf den puffer sector und zaehler erhoehen (pre-increment)

	if( file.cntOfBytes == 512 ){ 							/** SEKTOR VOLL ( 2 moeglichkeiten ab hier !) **/
		file.seek += 512;											// position in der datei erhoehen, weil grade 512 bytes geschrieben
		file.cntOfBytes = 0;	  										// ruecksetzen des sektor byte zaehlers
		chain.cntSecs -= 1;											// noch maximal cntSecs sekoren zum beschreiben bekannt...

		#if (MMC_MULTI_BLOCK==TRUE)							/** 1.) genuegend leere zum beschreiben bekannt **/
			mmc_multi_block_write_sector(fat.sector);				// gefuellten sektor schreiben
		#else
			mmc_write_sector(file.currentSectorNr,fat.sector);		// gefuellten sektor schreiben
		#endif

		file.currentSectorNr += 1;									// naechste sektor nummer zum beschreiben.

		if( chain.cntSecs == 0 ){ 							/** 2.) es ist noetig, neue freie zu suchen und die alten zu verketten (mit ein bischen glueck nur alle 512*MAX_CLUSTERS_IN_ROW bytes) **/
			#if (MMC_MULTI_BLOCK==TRUE)
				mmc_multi_block_stop_write();
			#endif
			fat.bufferDirty = FALSE;								// sonst wuerde das suchen von clustern wieder eine schreiboperation ausloesen...
			fat_setClusterChain( fat_secToClust(chain.startSectors) , fat_secToClust(file.currentSectorNr-1) );	// verketten der beschriebenen
			fat_getFreeClustersInRow( chain.lastCluster );				// suchen von leeren sektoren in einer reihe.
			file.currentSectorNr = chain.startSectors;					// setzen des 1. sektors der neuen reihe zum schreiben.
			fat.bufferDirty = TRUE;
			#if (MMC_MULTI_BLOCK==TRUE)
				mmc_multi_block_start_write(fat.currentSectorNr);
			#endif
		}
	}
}
#endif

#if (MMC_OVER_WRITE==TRUE && MMC_WRITE==TRUE)  			// ueberschreibende write funktion
//*******************************************************************************************************************************
// schreibt 512 byte bloecke auf den puffer fat.sector. dann wird dieser auf die karte geschrieben. wenn genuegend feie
// sektoren zum beschreiben bekannt sind, muss nicht in der fat nachgeschaut werden. sollten nicht genuegend zusammenhaengende
// sektoren bekannt sein(datenmenge zu gross), werden die alten verkettet und neue gesucht. es ist noetig sich den letzten bekannten einer
// kette zu merken -> file.lastCluster, um auch nicht zusammenhaengende cluster verketten zu koennen (fat_setClusterChain macht das)!
// es ist beim ueberschreiben noetig, die schon beschriebenen sektoren der datei zu laden, damit man die richtigen daten
// hat. das ist bloed, weil so ein daten overhead von 50% entsteht. da lesen aber schneller als schreiben geht, verliert man nicht 50% an geschwindigkeit.
//*******************************************************************************************************************************
void ffwrite( uint8_t c){

	fat.sector[ file.cntOfBytes++ ]=c;							// schreiben des chars auf den puffer sector und zaehler erhoehen (pre-increment)

	if( file.cntOfBytes==512 ){									/** SEKTOR VOLL ( 3 moeglichkeiten ab hier !) **/

		file.cntOfBytes = 0;	  									// ruecksetzen des sektor byte zaehlers.
		file.seek += 512;											// position in der datei erhoehen, weil grade 512 bytes geschrieben.
		mmc_write_sector( file.currentSectorNr,fat.sector );	/** 1.) vollen sektor auf karte schreiben, es sind noch freie sektoren bekannt**/
		file.currentSectorNr +=1;									// naechsten sektor zum beschreiben.
		chain.cntSecs -=1;											// einen freien sektor zum beschreiben weniger.

		if( chain.cntSecs==0 ){										// ende der bekannten in einer reihe erreicht (freie oder verkettete)
			if( file.seek > file.length ){						/** 2.) ausserhalb der datei, jetzt ist es noetig die beschriebenen cluster zu verketten und neue freie zu suchen	**/
				fat.bufferDirty = FALSE;							// damit nicht durch z.b. fat_getNextCluster nochmal dieser sektor gescchrieben wird, siehe fat_loadSector
				fat_setClusterChain( fat_secToClust(chain.startSectors) , fat_secToClust(file.currentSectorNr-1) );	// verketten der beschriebenen.
				fat_getFreeClustersInRow( chain.lastCluster );		// neue leere sektoren benoetigt, also suchen.
				file.currentSectorNr = chain.startSectors;				// setzen des 1. sektors der neuen reihe zum schreiben.
				fat.bufferDirty = TRUE;
			}
			else {												/** 3.) noch innerhalb der datei, aber es muessen neue verkettete cluster gesucht werden, zum ueberschreiben **/
				fat_getFatChainClustersInRow( fat_getNextCluster(chain.lastCluster) );		// noch innerhalb der datei, deshlab verkettete suchen.
				file.currentSectorNr = chain.startSectors;				// setzen des 1. sektors der neuen reihe zum schreiben.
			}
		}

		if( file.seek <= file.length ){
			mmc_read_sector(file.currentSectorNr,fat.sector);		// wegen ueberschreiben, muss der zu beschreibende sektor geladen werden (zustand 3)...
		}
	}
}
#endif


#if (MMC_WRITE_STRING==TRUE && MMC_WRITE==TRUE)
// *******************************************************************************************************************************
// schreibt string auf karte, siehe ffwrite()
// ein string sind zeichen, '\0' bzw. 0x00 bzw dezimal 0 wird als string ende gewertet !!
// wenn sonderzeichen auf die karte sollen, lieber ffwrite benutzen!
// *******************************************************************************************************************************
void ffwrites( uint8_t *s ){
    while (*s){
    	ffwrite(*s++);
    }
    fat.bufferDirty = TRUE;
  }
#endif


#if (MMC_WRITEN==TRUE && MMC_WRITE==TRUE)
// *******************************************************************************************************************************
// schreibt n zeichen auf karte, siehe ffwrite()
// *******************************************************************************************************************************
void ffwriten( uint8_t *s, uint16_t n ){
	uint16_t i = 0;
    do{
    	ffwrite(*s++);
    }while( ++i < n );
    fat.bufferDirty = TRUE;
  }
#endif


struct Fat_t fat;   		// wichtige daten/variablen der fat
struct Chain_t chain;		// zum verketten undso

//***************************************************************************************************************
// funktionsprototypen die nur in dieser datei benutzt werden und auch nur bedingt mitkompiliert werden!
static uint8_t 	fat_loadFileDataFromCluster(uint32_t sec ,  uint8_t name []);
#if (MMC_WRITE==TRUE)
	static void 			fat_getFreeRowsOfDir(uint32_t dir,uint8_t row_cnt);
	static uint8_t 	fat_getFreeRowsOfCluster(uint32_t secStart, uint8_t rows);
	static void 			fat_addClusterToDir(uint32_t lastClusterOfDir);
	#if (MMC_LFN_SUPPORT==TRUE)
		static void 			fat_makeLfnDataEntrys(uint8_t name[],uint8_t row_count);
		static uint8_t 	fat_lfn_checksum(uint8_t name[]);
	#endif
#endif
#if (MMC_TIME_STAMP==TRUE)
	static  uint16_t 	fat_getTime(void); // infos, siehe config.h in sektion: SCHALTER und ZEIT FUNKTIONEN
	static  uint16_t 	fat_getDate(void);
#endif
//***************************************************************************************************************


#if (MMC_WRITE==TRUE)
//***************************************************************************************************************
// schreibt sektor nummer:sec auf die karte (puffer fat.sector) !!
// setzt bufferFlag=0 da puffer nicht dirty sein kann nach schreiben !
//***************************************************************************************************************
uint8_t fat_writeSector(uint32_t sec){

	fat.bufferDirty = FALSE;						// buffer kann nicht dirty sein weil wird geschrieben
	//printf("\nw_Sec = %lu",sec);
	return (mmc_write_sector(sec,fat.sector));		// schreiben von sektor puffer
}
#endif


//***************************************************************************************************************
// laed sektor:sec auf puffer:sector zum bearbeiten im ram !
// setzt currentSectorNr auf richtigen wert (also den sektor der gepuffert ist). es wird geprueft
// ob der gepufferte sektor geändert wurde, wenn ja muss erst geschrieben werden, um diese daten nicht zu verlieren !
//***************************************************************************************************************
uint8_t fat_loadSector(uint32_t sec){

	if( sec != file.currentSectorNr){			// nachladen noetig
		#if (MMC_WRITE==TRUE)
			if( fat.bufferDirty == TRUE ) {
				fat.bufferDirty = FALSE;		// buffer kann nicht dirty sein weil wird geschrieben
				mmc_write_sector( file.currentSectorNr,fat.sector );			// schreiben von sektor puffer
			}
		#endif
		fat.lastSector = file.currentSectorNr;	// den alten sektor sichern
		mmc_read_sector( sec,fat.sector );		// neuen sektor laden
		file.currentSectorNr = sec;				// aktualisiert sektor nummer (nummer des gepufferten sektors)
		return TRUE;
	}

	else return TRUE;							// alles ok, daten sind schon da (sec==fat.currentSectorNr)

}


// ***************************************************************************************************************
// umrechnung cluster auf 1.sektor des clusters (möglicherweise mehrere sektoren/cluster) !
// ***************************************************************************************************************
uint32_t fat_clustToSec(uint32_t clust){

	return fat.dataDirSec + ( (clust - 2) * fat.secPerClust );		// errechnet den 1. sektor der sektoren des clusters
}


// ***************************************************************************************************************
// umrechnung sektor auf cluster (nicht die position im cluster selber!!)
// ***************************************************************************************************************
uint32_t fat_secToClust(uint32_t sec){

  return ( (sec - fat.dataDirSec) / fat.secPerClust ) + 2;			// umkerhrfunktion von fat_clustToSec
}



// datei lesen funktionen:


//***************************************************************************************************************
// laed die reihe:row des gepufferten sektors auf das struct:file. dort stehen dann
// alle wichgigen daten wie: 1.cluster,länge bei dateien, name des eintrags, reihen nummer (im sektor), attribut use...
//***************************************************************************************************************
void fat_loadRowOfSector(uint16_t row){

	#if (MMC_ENDIANNESS_LITTLE==TRUE)
		void *vsector;									// void-pointer, damit man schoen umbiegen kann :)

		vsector=&fat.sector[row+20];					// row ist byteoffset einer reihe

		file.firstCluster=*(uint16_t*)vsector;	// high word von first.cluster (20,2)
		file.firstCluster=file.firstCluster<<16;

		vsector=&fat.sector[row+26];
		file.firstCluster|=*(uint16_t*)vsector;	// low word von first.cluster (26,2)

		vsector=&fat.sector[row+28];
		file.length=*(uint32_t*)vsector;		// 4 byte von file.length (28,4)
	#else
		uint8_t *psector =& fat.sector[row+31];

		file.length =  *psector--;		// 31		hoechstes byte
		file.length <<= 8;
		file.length |=  *psector--;		// 30
		file.length <<= 8;
		file.length |=  *psector--;		// 29
		file.length <<= 8;
		file.length |=  *psector;		// 28

		psector-=7;
		file.firstCluster =  *psector--;					// 21	hoechstes byte
		file.firstCluster <<= 8;
		file.firstCluster = file.firstCluster | *psector;	// 20
		file.firstCluster <<= 8;

		psector+=7;
		file.firstCluster = file.firstCluster | *psector--;	// 27
		file.firstCluster <<= 8;
		file.firstCluster = file.firstCluster | *psector;	// 26
	#endif
}


#if (MMC_LFN_SUPPORT==TRUE)
//***************************************************************************************************************
// checksumme fuer den kurzen dateieintrag
//***************************************************************************************************************
static uint8_t fat_lfn_checksum(uint8_t name[])
{
	uint8_t cnt;
	uint8_t sum;

	cnt=11;
	sum=0;
	do{
		sum = ((sum & 1) << 7) + (sum >> 1) + *name++;
	}while(--cnt);

	return sum;
}


//***************************************************************************************************************
// prueft auf lange dateinamen (lfn), ermittelt anhand des langen dateinamens den kurzen. nur der kurze
// enthaelt die noetigen informationen um die datei lesen zu koennen. ist der gesuchte dateiname gefunden
// aber im naechsten sektor/cluster ermittelt die funktion selbstaendig diese und holt die informationen !
//***************************************************************************************************************
static uint8_t fat_loadFileDataFromCluster(uint32_t sec , uint8_t name []){

    uint16_t row;    // um durch zeilen zu gehen, eigentlich erstes byte einer reihe...
    uint8_t sectors;  // um durch sektoren zu zaehlen, sectors+sec = tatsaechliche sektornummer (absoluter sektor)
    uint8_t i;      // um durch eine zeile/reihe zu laufen
    uint8_t j;      // laufvariable fuer sfn

    // diese variablen muessen statisch sein, weil ein lfn eintrag auf 2 cluter aufgeteilt sein kann !
    static uint8_t checksum = 0;  // die kurze dateinamen checksumme, die ausgelesene.
    static enum flags { wait=0,start,readout } lfn_state;
    static uint8_t match = 0;    // treffer bei datei namen vergleich zaehlen

    const uint8_t map[13]={1,3,5,7,9,14,16,18,20,22,24,28,30};    // index von map ist index des zu pruefenden bytes und inhalt ist index des zu pruefenden bytes in der reihe
    const uint8_t name_length = strlen((char *)name)-1;      // es wird die laenge des dateinamen zum vergleichen benoetigt!

    sectors = 0;

    // 5 moegliche zustaende im inneren der beiden schleifen...
    do{                      // sektoren des clusters pruefen
    row=0;                    // neuer sektor, dann reihen von 0 an.
    mmc_read_sector(sec+sectors,fat.sector);  // laed den sektor sec auf den puffer fat.sector
    fat.lastSector = file.currentSectorNr;    // sichern der alten sektor nummer
    file.currentSectorNr = sec + sectors;    // setzen des aktuellen sektors
    do{                      // reihen des sektors pruefen
      // 1.) nach einem eintrag mit 0x00 kommt nix mehr => restlicher cluster leer!
      if( fat.sector[row]==0x00 ){
        return FALSE;
      }
      // 2.1) ist eintrag ein lfn eintrag?   siehe: http://en.wikipedia.org/wiki/File_Allocation_Table#Long_file_names
      if( (fat.sector[row]&0x40) == 0x40 && fat.sector[row+11] == 0x0F && fat.sector[row] != 0xE5 ){                                      // ist lfn eintrag, jetzt pruefen...
        lfn_state = start;
      }
      // 2.2) ist eintrag ein sfn eintrag?
      if( (fat.sector[row+11] == 0x10 || fat.sector[row+11] == 0x20) && fat.sector[row] != 0xE5 && lfn_state != readout){
        // vergleich von sfn dateinamen
        i = 0; j = 0;
        do{
          if( fat.sector[row+i] == 0x20 ){     // ueberspringen von leerzeichen auf der karte
            i++; continue;
          }
          if( name[j] == '.' ){           // ueberspringen von punkt im dateinamen
            if( i <= 7) break;
            j++; continue;
          }
          if( fat.sector[row+i] != toupper(name[j]) )  break;
          i++; j++;
        }while(i<11);
        // datei gefunden
        if( i == 11 && j != 11){
          lfn_state = readout;                                      // ist sfn eintrag jetzt auslesen
          checksum = fat_lfn_checksum( &fat.sector[row] );
        }
      }
      // 3.) lfn gefunden, jetzt verarbeiten. raussuchen der richtigen bytes und direkter vergleich mit dem original.
      if( lfn_state == start || match !=0 ){
        i=12;
        do{
          if( fat.sector[row+map[i]]!=0x00 && fat.sector[row+map[i]]!=0xFF ){  // gueltiger buchstabe. ist gueltig wenn, es nicht 0 oder ff ist.
            if( fat.sector[row+map[i]] == name[name_length-match] ){    // vergleich mit original, ist treffer?
              match += 1;                          // bei treffer index zaehler fuer vergleich hochzaehlen
            }
            else {            // wenn ein gueltiger buchstabe keinen treffer verursacht, ist es die falsche datei !
              lfn_state = wait;     // zurueck zu ausgangszustand
              match = 0;        // treffer zurueck setzen
              break;          // weitere pruefung nicht noetig da eintraege ungleich!
            }
          }
        }while(i--);            // zeichen index einer zeile/reihe abarbeiten
        // komplette uebereinstimmung und sequenz id 1 ==> datei gefunden !!
        if( (name_length-match) == -1 && (fat.sector[row]&0x01) == 0x01){
          lfn_state = readout;      // langer dateiname stimmt ueberein, jetzt zustand wechseln.
          match = 0;            // vergleich index zurueck setzen, damit nicht wieder in diesen zustand gewechselt wird.
          checksum = fat.sector[row+13];  // checksumme sichern zum vergleich mit der errechneten anhand des kurzen dateinamens. in naechstem zustand vergleich.
        }
      }
      // 4.) gesuchte reihe auf stuct file laden zum weiter verarbeiten. das ist der sfn eintrag, mit dem firstCluster, der laenge usw...
      if( lfn_state == readout && fat.sector[row+11] != 0x0F ){
        lfn_state = wait;
        fat_loadRowOfSector(row);
        file.row = row>>5;                  // reihe sichern, ist fuer ffrm wichtig
        if(checksum==fat_lfn_checksum(&fat.sector[row])){  // pruefen ob checksumme stimmt...wenn ja alles klar :)
          file.entrySector = file.currentSectorNr;    // sichern des sektors in dem der sfn dateieintrag steht.
          return TRUE;
        }
      }

      }while( (row+=32) < 512 );      // mit row+32 springt man immer an die 1. stelle einer reihe (zeile) und das durch einen ganzen sektor
    }while( ++sectors < fat.secPerClust );  // geht durch sektoren des clusters

    return FALSE;              // datei nicht gefunden, zumindest in diesem cluster...
}


#else
//***************************************************************************************************************
// geht reihen weise durch sektoren des clusters mit dem startsektor:sec, und sucht nach der datei mit dem
// namen:name. es werden die einzelnen sektoren nachgeladen auf puffer:sector vor dem bearbeiten.
// wird die datei in dem cluster gefunden ist return 0 , sonst return1.
//***************************************************************************************************************
static uint8_t fat_loadFileDataFromCluster(uint32_t sec ,  uint8_t name []){

  uint16_t rows;		// um durch zeilen nummern zu zaehlen
  uint8_t sectors;		// um durch sektoren zu zaehlen
  uint8_t i;			// zaehler fuer datei namen vergleich
  uint8_t j;

  sectors = 0;
  do{										// sektoren des clusters pruefen
	rows = 0;								// neuer sektor, dann reihen von 0 an.
	mmc_read_sector( sec+sectors , fat.sector );	// laed den sektor sec auf den puffer fat.sector
	file.currentSectorNr = sec + sectors;	// setzen des aktuellen sektors
	do{										// reihen des sektors pruefen

		if( fat.sector[rows] == 0 ){		// wenn man auf erste 0 stoesst muesste der rest auch leer sein!
			return FALSE;
			}
		// normaler eintrag, ordner oder datei.
		if( (fat.sector[rows+11] == 0x10 || fat.sector[rows+11] == 0x20) && fat.sector[rows] != 0xE5 ){
			// vergleich von sfn dateinamen
			i = 0; j = 0;
			do{
				if( fat.sector[rows+i] == 0x20 ){ 		// ueberspringen von leerzeichen auf der karte
					i++; continue;
				}
				if( name[j] == '.' ){ 					// ueberspringen von punkt im dateinamen
					if( i <= 7) break;
					j++; continue;
				}
				if( fat.sector[rows+i] != toupper(name[j]) )	break;
				j++;i++;
			}while(i<11);
			// datei gefunden
			if( i == 11 ){
				file.row = rows>>5;								// zeile sichern.
				fat_loadRowOfSector(rows);						// datei infos auf struct laden
				file.entrySector = file.currentSectorNr;		   	// sektor in dem die datei infos stehen sichern
		  		return TRUE;
		  	}																			// ist lfn eintrag, jetzt pruefen...
		}


		}while( (rows+=32) < 512 );			// springt immer auf zeilenanfang eines 32 byte eintrags im sektor
	}while(++sectors<fat.secPerClust);		// geht durch sektoren des clusters

	return FALSE;							// fehler (datei nicht gefunden, oder fehler beim lesen)
}
#endif

//***************************************************************************************************************
// wenn dir == 0 dann wird das root direktory durchsucht, wenn nicht wird der ordner cluster-chain gefolgt, um
// die datei zu finden. es wird das komplette directory in dem man sich befindet durchsucht.
// bei fat16 wird der rootDir berreich durchsucht, bei fat32 die cluster chain des rootDir.
//***************************************************************************************************************
uint8_t fat_loadFileDataFromDir( uint8_t name []){

	uint8_t	 	sectors;	// variable um durch root-dir sektoren zu zaehlen bei fat16
	uint32_t 	clusters;	// variable um durch cluster des verzeichnisses zu gehen

	// root-dir fat16 nach eintrag durchsuchen. es bleiben noch 3 moeglichkeiten: nicht root-dir fat16, nicht root-dir fat32 und root-dir fat32
	if(fat.dir==0 && fat.fatType==16){
		sectors = 0;
		do{
			// eintrag gefunden?
			if(TRUE==fat_loadFileDataFromCluster( fat.rootDir+sectors , name)) return TRUE;
			sectors += fat.secPerClust;
		}while( sectors < (uint8_t)32 );
	}

	// root-dir fat32 oder nicht root-dir fat32/16, nach eintrag durchsuchen
	else {
		// bestimmen ab welchem cluster nach eintrag gesucht werden soll
		clusters = fat.dir==0?fat.rootDir:fat.dir;

		// durch cluster des verzeichnisses gehen und ueberpruefen ob eintrag vorhanden
		while(!((clusters>=0x0ffffff8&&fat.fatType==32)||(clusters>=0xfff8&&fat.fatType==16))){// prueft ob weitere sektoren zum lesen da sind (fat32||fat16)
			if(TRUE==fat_loadFileDataFromCluster( fat_clustToSec(clusters) , name)) return TRUE;		 // daten der datei auf struct:file. datei gefunden (umrechnung auf absoluten sektor)
			clusters = fat_getNextCluster(clusters);									// liest naechsten cluster des dir-eintrags (unterverzeichniss groeßer 16 einträge)
		}
	}

	return FALSE;																// datei/verzeichniss nicht gefunden
}




// datei anlegen funktionen :


#if (MMC_WRITE==TRUE)
//***************************************************************************************************************
// macht den datei eintrag im jetzigen verzeichniss (fat.dir).
// file.row enthaelt die reihen nummer des leeren eintrags, der vorher gesucht wurde, auf puffer:sector ist der gewuenschte
// sektor gepuffert. fuer fat16 im root dir muss andere funktion genutzt werden, als fat_getFreeRowOfDir (durchsucht nur dirs).
// fat.rootDir enthält bei fat32 den start cluster des directory, bei fat16 den 1. sektor des rootDir bereichs!
//***************************************************************************************************************
void fat_makeFileEntry( uint8_t name [],uint8_t attrib){

	uint8_t sectors;				// um bei fat16 root dir sektoren zu zaehlen
	uint32_t dir;				// zum bestimmen ob im root-dir oder woanders

	#if (MMC_LFN_SUPPORT==TRUE)
		uint8_t need_rows;		// reihen die benoetigt werden um lfn und sfn zu speichern
		uint8_t name_length; 	// laenge des langen dateinamen
		name_length = strlen((char *)name);
		// berechnung wieviele reihen fuer den eintrag, also die lfn und den sfn, benoetigt werden.
		need_rows = (name_length % 13 == 0) ? name_length/13 + 1 : name_length/13  + 2;
	#endif

	// bestimmen in welchem cluster nach freien eintraegen gesucht werden soll
	dir = (fat.dir==0) ? fat.rootDir : fat.dir;

	// nach leeren eintraegen im root-dir suchen, nur wenn fat16
	if( fat.fatType == 16 && fat.dir == 0){
		sectors = 0;
		do{
			#if (MMC_LFN_SUPPORT==TRUE)
				if(TRUE==fat_getFreeRowsOfCluster( dir + sectors,need_rows) )break;	// freien eintrag im fat16 root dir gefunden?
			#else
				if(TRUE==fat_getFreeRowsOfCluster( dir + sectors,1) )break;		// freien eintrag im fat16 root dir gefunden?
			#endif
			sectors += fat.secPerClust;
		}while( sectors < (uint8_t)32  );
	}

	// wenn oben fat16 und root-dir abgehandelt wurde, bleiben noch 3 moeglichkeiten: root-dir fat32, nicht root-dir fat32 und nicht root-dir fat16
	#if (MMC_LFN_SUPPORT==TRUE)
		else fat_getFreeRowsOfDir(dir,need_rows);
	#else
		else fat_getFreeRowsOfDir(dir,1);
	#endif

	// setzten der noetigen werte der datei !
	file.entrySector = file.currentSectorNr;							// sichern des sektors in dem der sfn dateieintrag steht.
	file.firstCluster = fat_secToClust(chain.startSectors);
	chain.lastCluster = file.firstCluster;
	file.length = 0;

	// eintraege machen. sfn = short file name, lfn = long file name
	fat_makeSfnDataEntry(name,attrib,file.firstCluster,0);
	#if (MMC_LFN_SUPPORT==FALSE)
		mmc_write_sector(file.currentSectorNr,fat.sector);			// wenn nur sfn eintrag, dann jetzt hier sektor mit sfn eintrag schreiben...
	#else
		fat_makeLfnDataEntrys( name,need_rows-1);
	#endif

	// setzten des daten sektors der datei, damit jetzt darauf geschrieben werden kann
	file.currentSectorNr = chain.startSectors;
}


#if (MMC_ENDIANNESS_LITTLE==TRUE)
//***************************************************************************************************************
// erstellt 32 byte eintrag einer datei, oder verzeichnisses im puffer:sector.
// erstellt eintrag in reihe:row, mit namen:name usw... !!
// muss noch auf die karte geschrieben werden ! nicht optimiert auf geschwindigkeit.
//***************************************************************************************************************
void fat_makeSfnDataEntry(uint8_t name [],uint8_t attrib,uint32_t cluster,uint32_t length){

	uint8_t i,j; 		// byte zaehler in reihe von sektor (32byte eintrag)
	void *vsector; 				// void zeiger auf sektor, um beliebig casten zu können
	uint16_t row;		// reihe in dem sektor

	fat.bufferDirty = TRUE; 	// puffer beschrieben, also neue daten darin(vor lesen muss geschrieben werden)
	row = file.row;
	row = row<<5;				// multipliziert mit 32 um immer auf zeilen anfang zu kommen (zeile 0=0,zeile 1=32,zeile 2=62 ... zeile 15=480)
	vsector =& fat.sector[row];	// anfangs adresse holen ab der stelle auf sector geschrieben werden soll

	#if (MMC_TIME_STAMP==TRUE)
		uint8_t new=FALSE;
		// pruefung ob eintrag neu ist.
		if(fat.sector[row]==0xE5||fat.sector[row]==0x00)	 new=TRUE;
	#endif

	#if (MMC_TIME_STAMP==FALSE)
		// alle felder nullen...
		i = 20;
		do{
			*(uint8_t*)vsector++ = 0x00;
		}while( i-- );
		vsector =& fat.sector[row];
	#endif

	// namen schreiben (0,10)
	i=0; j=0;
	do{

		*(uint8_t*)vsector = 0x20;

		if( i < 8 && name[j] != '.'){
			*(uint8_t*)vsector = toupper(name[j]);
			j++;
		}

		j = ( i==8 ) ? j+1 : j;

		if( i >= 8 && name[j] != '\0'){
			*(uint8_t*)vsector = toupper(name[j]);
			j++;
		}
		vsector++;
	}while( ++i < 11);

	// attrib schreiben (11,1)
	*(uint8_t*)vsector = attrib;
	vsector++;

	#if (MMC_TIME_STAMP==TRUE)
		uint16_t time=fat_getTime();
		uint16_t date=fat_getDate();

		// reserviertes byte und milisekunden der erstellung (12,2)
		*(uint16_t*)vsector=0x0000;
		vsector+=2;

		if(new==TRUE){
			// creation time,date (14,4)
			*(uint32_t*)vsector=date;
			*(uint32_t*)vsector=*(uint32_t*)vsector<<16;
			*(uint32_t*)vsector|=time;
		}
		vsector+=4;

		// last access date (18,2)
		*(uint16_t*)vsector=date;
		vsector+=2;

		// low word von cluster (20,2)
		*(uint16_t*)vsector=(cluster&0xffff0000)>>16;
		vsector+=2;

		// last write time,date (22,4)
		*(uint32_t*)vsector=date;
		*(uint32_t*)vsector=*(uint32_t*)vsector<<16;
		*(uint32_t*)vsector|=time;
		vsector+=4;

		// high word von cluster (26,2)
		*(uint16_t*)vsector=(cluster&0x0000ffff);
		vsector+=2;

		// laenge (28,4)
		*(uint32_t*)vsector=length;
	#else
		vsector+=8;

		// low word	von cluster (20,2)
		*(uint16_t*)vsector=(cluster&0xffff0000)>>16;
		vsector+=6;

		// high word von cluster (26,2)
		*(uint16_t*)vsector=(cluster&0x0000ffff);
		vsector+=2;

		// laenge (28,4)
		*(uint32_t*)vsector=length;
	#endif
}
#else
//***************************************************************************************************************
// erstellt 32 byte eintrag einer datei, oder verzeichnisses im puffer:sector.
// erstellt eintrag in reihe:row, mit namen:name usw... !!
// muss noch auf die karte geschrieben werden ! nicht optimiert auf geschwindigkeit.
//***************************************************************************************************************
void fat_makeSfnDataEntry(uint8_t name [],uint8_t attrib,uint32_t cluster,uint32_t length){

	uint8_t i; 			// byte zaehler in reihe von sektor (32byte eintrag)
	uint8_t j;
	uint16_t row;
	uint8_t *psector;

	fat.bufferDirty = TRUE; 		// puffer beschrieben, also neue daten darin(vor lesen muss geschrieben werden)
	row = file.row;
	row = row<<5;					// multipliziert mit 32 um immer auf zeilen anfang zu kommen (zeile 0=0,zeile 1=32,zeile 2=62 ... zeile 15=480)
	psector =& fat.sector[row];

	#if (MMC_TIME_STAMP==TRUE)
		uint8_t new = FALSE;
		// pruefung ob eintrag neu ist.
		if(*psector==0xE5||*psector==0x00)	 new=TRUE;
	#endif

	#if (MMC_TIME_STAMP==FALSE)
		// alle felder nullen...
		i = 0;
		do{
			*psector++ = 0x00;
		}while( ++i < 32);
		psector =& fat.sector[row];
	#endif

	// namen schreiben (0,10)
	i = 0; j = 0;
	do{
		*psector = 0x20;

		if( i < 8 && name[j] != '.'){
			*psector = toupper(name[j]);
			j++;
		}

		j = ( i==8 ) ? j+1 : j;

		if( i >= 8 && name[j] != '\0'){
			*psector = toupper(name[j]);
			j++;
		}
		psector++;
	}while( ++i < 11);

	// attrib schreiben (11,1)
	*psector++ = attrib;

	#if (MMC_TIME_STAMP==TRUE)
		uint16_t time=fat_getTime();
		uint16_t date=fat_getDate();

		// reserviertes byte und milisekunden der erstellung (12,2)
		*psector++=0x00;
		*psector++=0x00;

		if(new==TRUE){
			// creation time,date (14,4)
			*psector++ = time&0xFF;
			*psector++ = (time&0xFF00)>>8;

			*psector++ = date&0xFF;
			*psector++ = (date&0xFF00)>>8;
		}
		else psector+=4;

		// last access date (18,2)
		*psector++=(date&0xFF00) >> 8;
		*psector++= date&0xFF;

		// hi word	von cluster (20,2)
		*psector++=(cluster&0x00ff0000)>>16;
		*psector++=(cluster&0xff000000)>>24;

		// last write time (22,2)
		*psector++ = time&0xFF;
		*psector++ = (time&0xFF00)>>8;

		// last write date (24,2)
		*psector++ = date&0xFF;
		*psector++ = (date&0xFF00)>>8;

		// low word von cluster (26,2)
		*psector++=cluster&0x000000ff;
		*psector++=(cluster&0x0000ff00)>>8;

		// laenge (28,4)
		*psector++=length&0x000000ff;
		*psector++=(length&0x0000ff00)>>8;
		*psector++=(length&0x00ff0000)>>16;
		*psector=(length&0xff000000)>>24;

	#else
		psector += 8;

		// hi word	von cluster (20,2)
		*psector++=(cluster&0x00ff0000)>>16;
		*psector=(cluster&0xff000000)>>24;
		psector += 5;

		// low word von cluster (26,2)
		*psector++=cluster&0x000000ff;
		*psector++=(cluster&0x0000ff00)>>8;

		// laenge (28,4)
		*psector++=length&0x000000ff;
		*psector++=(length&0x0000ff00)>>8;
		*psector++=(length&0x00ff0000)>>16;
		*psector=(length&0xff000000)>>24;
	#endif
}
#endif

#if (MMC_LFN_SUPPORT==TRUE)
//***************************************************************************************************************
// macht lange dateieintraege
//***************************************************************************************************************
static void fat_makeLfnDataEntrys(uint8_t name[],uint8_t rows){

	uint8_t cnt;				// sequenz nummer des lfn eintrags
	uint8_t c;				// wird gebraucht um unnoetige felder zu bearbeiten
	uint8_t chkSum;			// sfn checksumme
	uint16_t offset;			// reihen offset im sektor
	uint8_t i;				// laufindex fuer name
	uint8_t j;				// laufindex fuer eine zeile
	const uint8_t map[13]={1,3,5,7,9,14,16,18,20,22,24,28,30};	// index von map ist index des zu pruefenden bytes und inhalt ist index des zu pruefenden bytes in der reihe
	uint8_t name_length = strlen((char *)name);					// es wird die laenge des dateinamen zum vergleichen benoetigt!

	fat.bufferDirty = TRUE;			// der puffer wird veraendert...
	offset = file.row<<5;			// jetzt auf 1. byte des letzten eintrags, das ist eigentlich der sfn, deshalt wird gleich noch was abgezogen

	// gerade vorher ist der sfn eintrag auf den sektor geschrieben worden. daraus jetzt die checksumme berechnen.
	chkSum = fat_lfn_checksum( &fat.sector[offset] );

	if( offset == 0 ){						// es muss ein eintrag abgezogen werden, geht das ohne probleme? wenn man in reihe 0 ist liegt der vorherige eintrag in einem anderen sektor/cluster
		fat_loadSector(fat.lastSector);		// fat.lastSectorNr ist in der fat_loadFileDataFromCluster gesichert worden
		offset = 512;
	}

	// die schleife zaehlt die zeichen durch, behandelt -offset, die -sequenz und die -stelle in einer reihe
	for( i=0, cnt=1 ; i<name_length ; cnt++ ){
		offset -= 32;
		fat.sector[offset]=cnt;				// sequenz nummer, fortlaufend hochzaehlend
		fat.sector[offset+11]=0x0F;			// lfn attribut
		fat.sector[offset+12]=0x00;			// immer 0
		fat.sector[offset+13]=chkSum;		// sfn check summe
		fat.sector[offset+26]=0x00;			// immer 0
		fat.sector[offset+27]=0x00;			// immer 0

		// hier wird der eigentliche name geschrieben, mit dem array map, werden die moeglichen stellen gemapt.
		j=0;
		do{
			fat.sector[ offset + map[j]    ] = name[i];		// ein zeichen des namens, ueber das array map auf die richtige stelle gemapt
			fat.sector[ offset + map[j] +1 ] = 0x00;		// hier ist das zweite ascii zeichen
			i++;											// i ist die anzahl der geschriebenen datei zeichen
			j++;
		}while( j<13 && i<name_length );					// geht durch stellen an denen der dateiname steht

		// es ist moeglich, dass die freien eintraege in verschiedenen sektoren/clustern liegen, deshalb diese verrenkungen
		if( offset == 0 && i < name_length){
			fat_loadSector(fat.lastSector);					// fat.lastSectorNr ist in der fat_loadFileDataFromCluster gesichert worden
			offset = 512;
		}
	}

	fat.sector[offset] |= 0x40;								// letzten eintag mit bit 7 gesetzt markieren, zu der sequenz nummer

	// nicht benoetigte felder bearbeiten...
	c = 0x00;
	while( j < 13 ){
		fat.sector[ offset + map[j]     ] = c;
		fat.sector[ offset + map[j] + 1 ] = c;
		c = (j<11) ? 0xFF : c;
		j++;
	}

	fat_writeSector(file.currentSectorNr);					// schreibt puffer mit den letzten lfn eintraegen auf die karte...
}
#endif

// ***************************************************************************************************************
// durchsucht cluster nach freiem platz fuer die anzahl rows.
// ***************************************************************************************************************
static uint8_t fat_getFreeRowsOfCluster(uint32_t secStart, uint8_t rows){

	uint8_t sectors;			// variable zum zaehlen der sektoren eines clusters.
	uint16_t row;			// offset auf reihen anfang

	// variable muss statisch sein, wenn die anzahl der freien reihen die gesucht werden auf 2 cluster aufgeteilt sind !
	static uint8_t match = 0;

	sectors = 0;
	do{
		fat_loadSector( secStart + sectors );
		row = 0;
		do{
			if( fat.sector[row]==0x00 || fat.sector[row]==0xE5 ){ 	// prueft auf freihen eintrag (leer oder geloescht gefunden?).
				match += 1;
				if( match == rows ){								// fertig hier, noetige anzahl gefunden!
					file.row = row>>5; 								// byteoffset umrechnen zu reihe
					match = 0;
					return TRUE;
				}
			}
			else {								// kein freier eintrag, wenn bis hier nicht schon genuegend, dann sinds zuwenige
				match = 0;
			}
		}while( (row+=32) < 512 );				// geht durch reihen/zeilen eines sektors
	}while( ++sectors < fat.secPerClust );		// geht die sektoren des clusters durch (moeglicherweise auch nur 1. sektor).

	return FALSE;
}

// ***************************************************************************************************************
// sucht leeren eintrag (zeile) im cluster mit dem startsektor:secStart.
// wird dort kein freier eintrag gefunden ist return (1).
// wird ein freier eintrag gefunden, ist die position der freien reihe auf file.row abzulesen und return (0).
// der sektor mit der freien reihe ist auf dem puffer fat.sector gepuffert.
// ****************************************************************************************************************
static void fat_getFreeRowsOfDir(uint32_t dir, uint8_t rows){

	uint32_t lastCluster;
	uint32_t lastSector;

	// verzeichnis durchsuchen und pruefen ob genuegend platz fuer anzahl der benoetigten reihen ist.
	do{
		if( TRUE == fat_getFreeRowsOfCluster( fat_clustToSec(dir), rows ) ){ 			// freien platz gefunden!!
			return;
		}
		lastSector = file.currentSectorNr;
		lastCluster = dir;
		dir = fat_getNextCluster(dir);													// dir ist parameter der funktion und der startcluster des ordners/dirs
	}while( !((dir>=0x0ffffff8&&fat.fatType==32)||(dir>=0xfff8&&fat.fatType==16)) );	// geht durch ganzes dir


	// hier verzeichnis durchsucht und nicht genuegend platz gefunden, jetzt platzt schaffen. es wird ein neuer cluster zu dem dir verkettet und dieser aufbereitet!

	fat_addClusterToDir(lastCluster);
	chain.lastCluster = fat_secToClust(lastSector);
	fat_getFreeRowsOfCluster( file.currentSectorNr, rows );

	// pruefen ob mehr als ein cluster am stueck frei ist/war. wenn nicht, muessen neue freie fuer das beschreiben der datei gesucht werden, da ja gerade einer zum dir verkettet wurde
	if( chain.cntSecs>fat.secPerClust ){
		chain.startSectors += fat.secPerClust;
		chain.cntSecs -= fat.secPerClust;
	}
	else{
		fat_getFreeClustersInRow( lastCluster );
	}
}

// ***************************************************************************************************************
// verkettet einen neuen cluster zu einem vorhandenen verzeichniss. es muss der letzte bekannte cluster des
// verzeichnisses uebergeben werden und es muessen vorher neue freie gesucht worden sein.
// ****************************************************************************************************************
static void fat_addClusterToDir(uint32_t lastClusterOfDir){

	uint32_t newCluster;
	uint16_t j;
	uint8_t i;

	newCluster = fat_secToClust(chain.startSectors);

	// verketten des dirs mit dem neuen cluster (fat bereich)
	fat_setCluster(lastClusterOfDir,newCluster);
	fat_setCluster(newCluster,0x0fffffff);
	mmc_write_sector(file.currentSectorNr,fat.sector);

	// bereitet puffer so auf, dass die einzelnen sektoren des neuen clusters mit 0x00 initialisiert werden koennen.
	j = 511;
	do{
		fat.sector[j]=0x00;
	}while(j--);

	// jetzt sektoren des neuen clusters mit vorbereitetem puffer beschreiben (daten bereich)
	i = 0;
	do{
		fat_writeSector(chain.startSectors + i );		// nullen des clusters
	}while( i++ < fat.secPerClust);					// bis cluster komplett genullt wurde.

	// auf ersten sektor des clusters setzen
	file.currentSectorNr = chain.startSectors;
}
#endif





// fat funktionen:

//***************************************************************************************************************
// sucht folge Cluster aus der fat !
// erster daten cluster = 2, ende einer cluster chain 0xFFFF (fat16) oder 0xFFFFFFF (fat32),
// stelle des clusters in der fat, hat als wert, den nächsten cluster. (1:1 gemapt)!
//***************************************************************************************************************
uint32_t fat_getNextCluster(uint32_t oneCluster){

	uint32_t sector;

	#if (MMC_ENDIANNESS_LITTLE==TRUE)
		void *bytesOfSec;
	#else
		uint8_t *bytesOfSec;
	#endif

	// FAT 16
	if(fat.fatType==16){
		oneCluster = oneCluster << 1;
		sector = fat.fatSec + (oneCluster >> 9);
		fat_loadSector(sector);
		bytesOfSec =& fat.sector[oneCluster % 512];

		#if (MMC_ENDIANNESS_LITTLE==TRUE)
			return *(uint16_t*)bytesOfSec;
		#else
			bytesOfSec += 1;
			sector = *bytesOfSec--;
			sector <<= 8;
			sector |= *bytesOfSec;
			return sector;
		#endif
	}
	// FAT 32
	else{
		oneCluster = oneCluster << 2;
		sector = fat.fatSec + (oneCluster >> 9);
		fat_loadSector(sector);
		bytesOfSec =& fat.sector[oneCluster % 512];

		#if (MMC_ENDIANNESS_LITTLE==TRUE)
			return *(uint32_t*)bytesOfSec;
		#else
			bytesOfSec += 3;
			sector = *bytesOfSec--;
			sector <<= 8;
			sector |= *bytesOfSec--;
			sector <<= 8;
			sector |= *bytesOfSec--;
			sector <<= 8;
			sector |= *bytesOfSec;
			return sector;
		#endif
	}

}


//***************************************************************************************************************
// sucht verkettete cluster einer datei, die in einer reihe liegen. worst case: nur ein cluster.
// sieht in der fat ab dem cluster offsetCluster nach. sucht die anzahl von MAX_CLUSTERS_IN_ROW,
// am stueck,falls möglich. prueft ob der cluster neben offsetCluster dazu gehört...
// setzt dann fat.endSectors und fat.startSectors. das -1 weil z.b. [95,98] = {95,96,97,98} = 4 sektoren
//***************************************************************************************************************
void fat_getFatChainClustersInRow( uint32_t offsetCluster){

	uint16_t cnt = 0;

	chain.startSectors = fat_clustToSec(offsetCluster);		// setzen des 1. sektors der datei
	chain.cntSecs = fat.secPerClust;

	while( cnt<MMC_MAX_CLUSTERS_IN_ROW ){
		if( (offsetCluster+cnt+1)==fat_getNextCluster(offsetCluster+cnt) )		// zaehlen der zusammenhaengenden sektoren
			chain.cntSecs += fat.secPerClust;
		else {
			chain.lastCluster = offsetCluster+cnt;	// hier landet man, wenn es nicht MAX_CLUSTERS_IN_ROW am stueck gibt, also vorher ein nicht passender cluster gefunden wurde.
			return;
		}
		cnt+=1;
	}

	chain.lastCluster = offsetCluster+cnt;			// hier landet man, wenn MAX_CLUSTERS_IN_ROW gefunden wurden
}


#if (MMC_WRITE==TRUE)
//***************************************************************************************************************
// sucht freie zusammenhaengende cluster aus der fat. maximal MAX_CLUSTERS_IN_ROW am stueck.
// erst wir der erste frei cluster gesucht, ab offsetCluster(iklusive) und dann wird geschaut, ob der
// daneben auch frei ist. setzt dann fat.endSectors und chain.startSectors. das -1 weil z.b. [95,98] = {95,96,97,98} = 4 sektoren
//***************************************************************************************************************
void fat_getFreeClustersInRow(uint32_t offsetCluster){

	uint16_t cnt=1; 							// variable fuer anzahl der zu suchenden cluster

	while(fat_getNextCluster(offsetCluster)){		// suche des 1. freien clusters
		offsetCluster++;
	}

	chain.startSectors = fat_clustToSec(offsetCluster);	// setzen des startsektors der freien sektoren (umrechnen von cluster zu sektoren)
	chain.cntSecs = fat.secPerClust;					// da schonmal mindestens einer gefunden wurde kann hier auch schon cntSecs damit gesetzt werden

	do{													// suche der naechsten freien
		if(0==fat_getNextCluster(offsetCluster+cnt) )	// zaehlen der zusammenhängenden sektoren
			chain.cntSecs += fat.secPerClust;
		else{
			return;									// cluster daneben ist nicht frei
		}
		cnt++;
	}while( cnt<MMC_MAX_CLUSTERS_IN_ROW );			// wenn man hier raus rasselt, gibt es mehr freie zusammenhaengende als MAX_CLUSTERS_IN_ROW


}


//***************************************************************************************************************
// verkettet ab startCluster bis einschließlich endClu
// es ist wegen der fragmentierung der fat nötig, sich den letzten bekannten cluster zu merken,
// damit man bei weiteren cluster in einer reihe die alten cluster noch dazu verketten kann (so sind luecken im verketten möglich).
//***************************************************************************************************************
void fat_setClusterChain(uint32_t startCluster, uint32_t endCluster){

  fat_setCluster( chain.lastCluster ,startCluster );	// ende der chain setzen, bzw verketten der ketten

  while( startCluster != endCluster ){
	 startCluster++;
	 fat_setCluster( startCluster-1 ,startCluster );// verketten der cluster der neuen kette
	 }
	
  fat_setCluster( startCluster,0xfffffff );			// ende der chain setzen
  chain.lastCluster = endCluster;					// ende cluster der kette updaten
  fat.bufferDirty = FALSE;
  mmc_write_sector (file.currentSectorNr,fat.sector);
}


//***************************************************************************************************************
// setzt den cluster inhalt. errechnet den sektor der fat in dem cluster ist, errechnet das low byte von
// cluster und setzt dann byteweise den inhalt:content.
// prueft ob buffer dirty (zu setztender cluster nicht in jetzt gepuffertem).
// pruefung erfolgt in fat_loadSector, dann wird alter vorher geschrieben, sonst gehen dort daten verloren !!
//***************************************************************************************************************
void fat_setCluster( uint32_t cluster, uint32_t content){

	uint32_t sector;

	#if (MMC_ENDIANNESS_LITTLE==TRUE)
		void *bytesOfSec;
	#else
		uint8_t *bytesOfSec;
	#endif

	// FAT 16
	if(fat.fatType==16){
		cluster = cluster << 1;
		sector = fat.fatSec + (cluster >> 9);
		fat_loadSector(sector);
		bytesOfSec =& fat.sector[cluster % 512];

		#if (MMC_ENDIANNESS_LITTLE==TRUE)
			*(uint16_t*)bytesOfSec = content;
		#else
			*bytesOfSec++ = content;
			*bytesOfSec = content >> 8;
		#endif
	}
	// FAT 32
	else{
		cluster = cluster << 2;
		sector = fat.fatSec + (cluster >> 9);
		fat_loadSector(sector);
		bytesOfSec =& fat.sector[cluster % 512];

		#if (MMC_ENDIANNESS_LITTLE==TRUE)
			*(uint32_t*)bytesOfSec = content;
		#else
			*bytesOfSec++ = content;
			*bytesOfSec++ = content >> 8;
			*bytesOfSec++ = content >> 16;
			*bytesOfSec = content >> 24;
		#endif
	}

	fat.bufferDirty = TRUE;						// zeigt an, dass im aktuellen sector geschrieben wurde
}


//***************************************************************************************************************
// löscht cluster chain, beginnend ab dem startCluster.
// sucht cluster, setzt inhalt usw.. abschließend noch den cluster-chain ende markierten cluster löschen.
//***************************************************************************************************************
void fat_delClusterChain(uint32_t startCluster){

  uint32_t nextCluster = startCluster;		// tmp variable, wegen verketteter cluster..
	
  do{
	 startCluster = nextCluster;
	 nextCluster = fat_getNextCluster(startCluster);
	 fat_setCluster(startCluster,0x00000000);  	
  }while(!((nextCluster>=0x0ffffff8&&fat.fatType==32)||(nextCluster>=0xfff8&&fat.fatType==16)));

  fat_writeSector(file.currentSectorNr);
}
#endif


//***************************************************************************************************************
// Initialisiert die Fat(16/32) daten, wie: root directory sektor, daten sektor, fat sektor...
// siehe auch Fatgen103.pdf. ist NICHT auf performance optimiert!
// byte/sector, byte/cluster, anzahl der fats, sector/fat ... (halt alle wichtigen daten zum lesen ders datei systems!)
//*****************************************************************<**********************************************
uint8_t fat_loadFatData(void){
										// offset,size
	uint16_t  	rootEntCnt;		// 17,2				groesse die eine fat belegt
	uint16_t  	fatSz16;		// 22,2				sectors occupied by one fat16
	uint32_t 	fatSz32;		// 36,4				sectors occupied by one fat32
	uint32_t 	secOfFirstPartition;				// ist 1. sektor der 1. partition aus dem MBR
	#if (MMC_ENDIANNESS_LITTLE==TRUE)
		void *vsector;
	#endif

	if(TRUE==mmc_read_sector(0,fat.sector)){				//startsektor bestimmen
		secOfFirstPartition = 0;
		if( fat.sector[457] == 0 ){
			#if (MMC_ENDIANNESS_LITTLE==TRUE)
				vsector =& fat.sector[454];
				secOfFirstPartition = *(uint32_t*)vsector;
			#else
				secOfFirstPartition |= fat.sector[456];
				secOfFirstPartition <<= 8;
	
				secOfFirstPartition |= fat.sector[455];
				secOfFirstPartition <<= 8;

				secOfFirstPartition |= fat.sector[454];
			#endif
			mmc_read_sector(secOfFirstPartition,fat.sector);		// ist kein superfloppy gewesen
		}
					
		fat.secPerClust=fat.sector[13];		// fat.secPerClust, 13 only (power of 2)

		#if (MMC_ENDIANNESS_LITTLE==TRUE)
			vsector =& fat.sector[14];
			fat.fatSec=*(uint16_t*)vsector;

			vsector=&fat.sector[17];
			rootEntCnt=*(uint16_t*)vsector;

			vsector=&fat.sector[22];
			fatSz16=*(uint16_t*)vsector;
		#else
			fat.fatSec = fat.sector[15];
			fat.fatSec <<= 8;
			fat.fatSec |= fat.sector[14];

			rootEntCnt = fat.sector[18];
			rootEntCnt <<= 8;
			rootEntCnt |= fat.sector[17];

			fatSz16 = fat.sector[23];
			fatSz16 <<= 8;
			fatSz16 |= fat.sector[22];
		#endif

		fat.rootDir	 = ( (rootEntCnt <<5) + 511 ) /512;	// ist 0 bei fat 32, sonst der root dir sektor

		if(fat.rootDir==0){									// FAT32 spezifisch (die pruefung so, ist nicht spezifikation konform !).
			#if (MMC_ENDIANNESS_LITTLE==TRUE)
				vsector=&fat.sector[36];
				fatSz32=*(uint32_t *)vsector;

				vsector=&fat.sector[44];
				fat.rootDir=*(uint32_t *)vsector;
			#else
				fatSz32 = fat.sector[39];
				fatSz32 <<= 8;
				fatSz32 |= fat.sector[38];
				fatSz32 <<= 8;
				fatSz32 |= fat.sector[37];
				fatSz32 <<= 8;
				fatSz32 |= fat.sector[36];

				fat.rootDir = fat.sector[47];
				fat.rootDir <<= 8;
				fat.rootDir |= fat.sector[46];
				fat.rootDir <<= 8;
				fat.rootDir |= fat.sector[45];
				fat.rootDir <<= 8;
				fat.rootDir |= fat.sector[44];
			#endif

			fat.dataDirSec = fat.fatSec + (fatSz32 * fat.sector[16]);	// data sector (beginnt mit cluster 2)
			fat.fatType=32;									// fat typ
			}

		else{												// FAT16	spezifisch
			fat.dataDirSec = fat.fatSec + (fatSz16 * fat.sector[16]) + fat.rootDir;		// data sektor (beginnt mit cluster 2)
			fat.rootDir=fat.dataDirSec-fat.rootDir;			// root dir sektor, da nicht im datenbereich (cluster)
			fat.rootDir+=secOfFirstPartition;				// addiert den startsektor auf 	"
			fat.fatType=16;									// fat typ
			}

		fat.fatSec+=secOfFirstPartition;					// addiert den startsektor auf
		fat.dataDirSec+=secOfFirstPartition;				// addiert den startsektor auf (umrechnung von absolut auf real)
		fat.dir=0;											// dir auf '0'==root dir, sonst 1.Cluster des dir
		return TRUE;
		}

return FALSE;			// sector nicht gelesen, fat nicht initialisiert!!
}


#if (MMC_TIME_STAMP==TRUE)
// *****************************************************************************************************************
// gibt akuelles datum zurueck
// *****************************************************************************************************************
static uint16_t fat_getDate(void){
	// rueckgabe wert in folgendem format:
	// bits [0-4]  => tag es monats, gueltige werte: 1-31
	// bits [5-8]  => monat des jahres, gueltige werte: 1-12
	// bits [9-15] => anzahl jahre seit 1980, gueltige werte: 0-127 (moegliche jahre demnach: 1980-2107)
	// macht insgesammt 16 bit also eine 2 byte variable !

	uint16_t date=0;
	//    Tag Monat   Jahr (29=2009-1980)
	date|=22|(12<<5)|(29<<9);
	// bei diesem beispiel => 22.12.2009 	(29=2009-1980)

	// hier muss code hin, der richtige werte auf date erstellt, so wie beim beispiel !!

	return date;
}


// *****************************************************************************************************************
// gibt aktuelle zeit zurueck
// *****************************************************************************************************************
static uint16_t fat_getTime(void){
	// rueckgabe wert in folgendem format:
	// bits [0-4]   => sekunde, gueltige werte: 0-29, laut spezifikation wird diese zahl beim anzeigen mit 2 multipliziert, womit man dann auf eine anzeige von 0-58 sekunden kommt !
	// bits [5-10]  => minuten, gueltige werte: 0-59
	// bits [11-15] => stunden, gueltige werte: 0-23
	// macht insgesammt 16 bit also eine 2 byte variable !


	uint16_t time=0;
	//  Sek. Minute  Stunde		Sek wird beim anzeigen noch mit 2 multipliziert.
	time|=10|(58<<5)|(16<<11);
	// bei diesem Beispiel=> 16:58 und 20 sekunden (20=10*2 muss so laut spezifikation !)

	// hier muss code hin, der richtige werte auf time erstellt, so wie beim beispiel gezeigt !!

	return time;
}
#endif


#if (MMC_GET_FREE_BYTES==TRUE)
// *****************************************************************************************************************
// zaehlt die freien cluster in der fat. gibt die anzahl der freien bytes zurueck !
// vorsicht, kann lange dauern !
// ist nicht 100% exakt, es werden bis zu ein paar hundert bytes nicht mit gezaehlt, da man lieber ein paar bytes verschwenden
// sollte, als zu versuchen auf bytes zu schreiben die nicht da sind ;)
// *****************************************************************************************************************
uint64_t fat_getFreeBytes(void){
	uint64_t bytes;							// die tatsaechliche anzahl an bytes
	uint32_t count;								// zaehler fuer cluster in der fat
	uint32_t fatSz;								// zuerst groeße der fat in sektoren, dann letzter sektor der fat in clustern
	uint16_t tmp;									// hilfsvariable zum zaehlen
	void *vsector;

	// bestimmen des 1. sektors der 1. partition (ist nach der initialisierung nicht mehr bekannt...)
	fat_loadSector(0);
	vsector=&fat.sector[454];
	fatSz=*(uint32_t*)vsector;
	if(fatSz!=0x6964654d){
		fat_loadSector(fatSz);
		}

	// anzahl sektoren bestimmen die von einer fat belegt werden
	if(fat.fatType==32){
		vsector=&fat.sector[36];
		fatSz=(*(uint32_t *)vsector)-1;
		fatSz*=128;										// multipliziert auf cluster/sektor in der fat, also ein sektor der fat beinhaltet 128 cluster nummern
		}
	else{
		vsector=&fat.sector[22];
		fatSz=(*(uint16_t*)vsector)-1;
		fatSz*=256;										// multipliziert auf cluster/sektor in der fat, also ein sektor der fat beinhaltet 256 cluster nummern
		}

	// variablen initialisieren
	tmp=0;
	bytes=0;
	count=0;

	// zaehlen der freien cluster in der fat, mit hilfsvariable um nicht eine 64 bit variable pro freien cluster hochzaehlen zu muessen
	do{
		if(0==fat_getNextCluster(count)) tmp++;
		if(tmp==254){
			bytes+=254;
			tmp=0;
		}
		count++;
	}while(count<fatSz);
	bytes+=tmp;

	// multiplizieren um auf bytes zu kommen
	return (bytes*fat.secPerClust)<<9;   // 2^9 = 512
}
#endif

// Definitions for MMC/SDC command
#define CMD0	(0)			// GO_IDLE_STATE
#define CMD1	(1)			// SEND_OP_COND (MMC)
#define	ACMD41	(41)		// SEND_OP_COND (SDC)
#define CMD8	(8)			// SEND_IF_COND
#define CMD9	(9)			// SEND_CSD
#define CMD10	(10)		// SEND_CID
#define CMD12	(12)		// STOP_TRANSMISSION
#define ACMD13	(0x80+13)	// SD_STATUS (SDC)
#define CMD16	(16)		// SET_BLOCKLEN
#define CMD17	(17)		// READ_SINGLE_BLOCK
#define CMD18	(18)		// READ_MULTIPLE_BLOCK
#define CMD23	(23)		// SET_BLOCK_COUNT (MMC)
#define	ACMD23	(0x80+23)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24	(24)		// WRITE_BLOCK
#define CMD25	(25)		// WRITE_MULTIPLE_BLOCK
#define CMD55	(55)		// APP_CMD
#define CMD58	(58)		// READ_OCR

// Card type flags (CardType)
#define CT_MMC		0x01			// MMC ver 3
#define CT_SD1		0x02			// SD ver 1
#define CT_SD2		0x04			// SD ver 2
#define CT_SDC		(CT_SD1|CT_SD2)	// SD
#define CT_BLOCK	0x08			// Block addressing

// **********************************************************************************************************************************
// funktionsprototypen von funktionen die nur in dieser datei benutzt werden !

static uint8_t 	mmc_enable(void);
static void 			mmc_disable(void);
static uint8_t 	mmc_wait_ready (void);
static uint8_t 	mmc_send_cmd (	uint8_t cmd,	uint32_t arg);

// beginn -> hardware abhaengiger teil !
#define MMC_CS_LOW 		MMC_Write &= ~(1<<SPI_SS)		// Set pin B2 to 0
#define MMC_CS_HIGH		MMC_Write |= (1<<SPI_SS)		// Set pin B2 to 1

static void 			spi_init(void);
static void 			spi_maxSpeed(void);
static void 			spi_write_byte(uint8_t byte);
static uint8_t 	spi_read_byte(void);


// *****************************************************************************
static void spi_init(void){

	// port configuration der mmc/sd/sdhc karte
	MMC_Direction_REG &=~(1<<SPI_MISO);             // miso auf input
	MMC_Direction_REG |= (1<<SPI_Clock);            // clock auf output
	MMC_Direction_REG |= (1<<SPI_MOSI);             // mosi auf output
	MMC_Direction_REG |= (1<<SPI_SS);               // chip select auf output

	// hardware spi: bus clock = idle low, spi clock / 128 , spi master mode
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);

	MMC_Write |= (1<<SPI_SS);                       // chip selet auf high, karte anwaehlen
}



#if (MMC_MAX_SPEED==TRUE)
// *****************************************************************************
static void spi_maxSpeed(){

	//SPI Bus auf max Geschwindigkeit

//      SPCR &= ~((1<<SPR0) | (1<<SPR1));               // sck_spi= f_osc / 4
        SPCR |= ( (1<<SPR0) );
        SPCR &= ~( (1<<SPR1) );                         // sck_spi= f_osc / 16
        SPSR |= (1<<SPI2X);                             // sck_spi= sck_spi * 2  ( f_osc / 2)

                                                        // somit nun sck_spi = f_osc / 8

}
#endif

// *****************************************************************************
static void spi_write_byte(uint8_t byte){


	#if (MMC_SOFT_SPI==TRUE)
		uint8_t a;
	#endif

	// mmc/sd in hardware spi
	#if (MMC_SOFT_SPI==FALSE)
		SPDR = byte;    						//Sendet ein Byte
		loop_until_bit_is_set(SPSR,SPIF);

	// mmc/sd in software spi
	#else
		for (a=8; a>0; a--){					//das Byte wird Bitweise nacheinander Gesendet MSB First
			if (bit_is_set(byte,(a-1))>0){		//Ist Bit a in Byte gesetzt
				MMC_Write |= (1<<SPI_MOSI); 	//Set Output High
			}
			else{
				MMC_Write &= ~(1<<SPI_MOSI); 	//Set Output Low
			}
			MMC_Write |= (1<<SPI_Clock); 		//setzt Clock Impuls wieder auf (High)
			MMC_Write &= ~(1<<SPI_Clock);		//erzeugt ein Clock Impuls (LOW)
		}
		MMC_Write |= (1<<SPI_MOSI);				//setzt Output wieder auf High
	#endif
}


// *****************************************************************************
static uint8_t spi_read_byte(void){

	// mmc/sd in hardware spi
	#if (MMC_SOFT_SPI==FALSE)
	  SPDR = 0xff;
	  loop_until_bit_is_set(SPSR,SPIF);
	  return (SPDR);

	// mmc/sd in software spi
	#else
	    uint8_t Byte=0;
	    uint8_t a;
		for (a=8; a>0; a--){							//das Byte wird Bitweise nacheinander Empangen MSB First
			MMC_Write |=(1<<SPI_Clock);					//setzt Clock Impuls wieder auf (High)
			if (bit_is_set(MMC_Read,SPI_MISO) > 0){ 	//Lesen des Pegels von MMC_MISO
				Byte |= (1<<(a-1));
			}
			else{
				Byte &=~(1<<(a-1));
			}
			MMC_Write &=~(1<<SPI_Clock); 				//erzeugt ein Clock Impuls (Low)
		}
		return (Byte);
	#endif
}



// ende <- hardware abhaengiger teil !








// **********************************************************************************************************************************
uint8_t mmc_init (void){

	uint8_t cmd, ty, ocr[4];
	uint16_t n, j;

	spi_init();
	mmc_disable();

	for (n = 100; n; n--) spi_read_byte();    					// 80+ dummy clocks

	ty = 0;
	j=100;
	do {
		if (mmc_send_cmd(CMD0, 0) == 1) {      					// Enter Idle state
			j=0;
			TimingDelay = 100;            						// Initialization timeout of 1000 msec

			if (mmc_send_cmd(CMD8, 0x1AA) == 1) {  				// SDv2?
				for (n = 0; n < 4; n++){
					ocr[n] = spi_read_byte();    				// Get trailing return value of R7 resp
				}
				if (ocr[2] == 0x01 && ocr[3] == 0xAA) {         // The card can work at vdd range of 2.7-3.6V
					while (TimingDelay) {  						// Wait for leaving idle state (ACMD41 with HCS bit)
						mmc_send_cmd(CMD55, 0);
						if(!mmc_send_cmd(ACMD41, 1UL << 30))
							break;
					}

					while(TimingDelay) {
						if (mmc_send_cmd(CMD58, 0) == 0x00) {    // Check CCS bit in the OCR
							for (n = 0; n < 4; n++){
								ocr[n] = spi_read_byte();
							}
							ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  // SDv2
							break;
						}
					}
				}
			} else {        									// SDv1 or MMCv3
				if (mmc_send_cmd(ACMD41, 0) <= 1)   {
					ty = CT_SD1;
					cmd = ACMD41;  								// SDv1
				} else {
					ty = CT_MMC;
					cmd = CMD1;    								// MMCv3
				}
				while (TimingDelay && mmc_send_cmd(cmd, 0));    // Wait for leaving idle state
			}
			if(ty != (CT_SD2 | CT_BLOCK)) {
				while(TimingDelay && (mmc_send_cmd(CMD16, 512) != 0));
			}
			if(!TimingDelay) ty = 0;
		} else { j--; }
	}while(j>0);

	fat.card_type = ty;
	mmc_disable();

	if( fat.card_type == 0 ){
		return FALSE;
	}
	#if (MMC_MAX_SPEED==TRUE)
		spi_maxSpeed();
	#endif

	return TRUE;
}

// **********************************************************************************************************************************
static uint8_t mmc_send_cmd (	uint8_t cmd,	uint32_t arg){

	uint8_t n, res;
	// Select the card and wait for ready
	mmc_disable();
	if ( FALSE == mmc_enable() ){
		return 0xFF;
	}
	// Send command packet
	spi_write_byte(0x40 | cmd);						// Start + Command index
	spi_write_byte( (uint8_t)(arg >> 24) );	// Argument[31..24]
	spi_write_byte( (uint8_t)(arg >> 16) );	// Argument[23..16]
	spi_write_byte( (uint8_t)(arg >> 8) );	// Argument[15..8]
	spi_write_byte( (uint8_t)arg );			// Argument[7..0]
	n = 0x01;										// Dummy CRC + Stop
	if (cmd == CMD0) n = 0x95;						// Valid CRC for CMD0(0)
	if (cmd == CMD8) n = 0x87;						// Valid CRC for CMD8(0x1AA)
	spi_write_byte(n);

	// Receive command response
	if (cmd == CMD12) spi_read_byte();				// Skip a stuff byte when stop reading
	n = 10;											// Wait for a valid response in timeout of 10 attempts
	do
		res = spi_read_byte();
	while ( (res & 0x80) && --n );

	return res;										// Return with the response value
}





// **********************************************************************************************************************************
static uint8_t mmc_enable(){

   MMC_CS_LOW;
   if( !mmc_wait_ready() ){
   	  mmc_disable();
	  return FALSE;
   }

   return TRUE;
}

// **********************************************************************************************************************************
static void mmc_disable(){

   MMC_CS_HIGH;
   spi_read_byte();
}


#if (MMC_MULTI_BLOCK==TRUE && MMC_OVER_WRITE == FALSE)
// **********************************************************************************************************************************
// stopt multiblock lesen
// **********************************************************************************************************************************
uint8_t mmc_multi_block_stop_read (void){

	uint8_t cmd[] = {0x40+12,0x00,0x00,0x00,0x00,0xFF};	// CMD12 (stop_transmission), response R1b (kein fehler, dann 0)
	uint8_t response;

	response = mmc_write_command (cmd);		// r1 antwort auf cmd12

	response = mmc_read_byte();				// dummy byte nach cmd12

	mmc_disable();
	return response;
}


// **********************************************************************************************************************************
// stop multiblock schreiben
// **********************************************************************************************************************************
uint8_t mmc_multi_block_stop_write (void){

	uint8_t cmd[] = {0x40+13,0x00,0x00,0x00,0x00,0xFF};	// CMD13 (send_status), response R2
	uint8_t response;

	mmc_write_byte(0xFD);					// stop token

	mmc_wait_ready();

	response=mmc_write_command (cmd);		// cmd13, alles ok?

	mmc_wait_ready();

	mmc_disable();
	return response;
}


// **********************************************************************************************************************************
// starten von multi block read. ab sektor addr wird der reihe nach gelesen. also addr++ usw...
// **********************************************************************************************************************************
uint8_t mmc_multi_block_start_read (uint32_t int addr){

	uint8_t cmd[] = {0x40+18,0x00,0x00,0x00,0x00,0xFF};	// CMD18 (read_multiple_block), response R1
	uint8_t response;

	mmc_enable();

	// addressiertung bei mmc und sd (standart < 2.0) in bytes, also muss sektor auf byte adresse umgerechnet werden.
	// sd standart > 2.0, adressierung in sektoren, also 512 byte bloecke
	if(card_type==0) addr = addr << 9; //addr = addr * 512, nur wenn mmc/sd karte vorliegt

	cmd[1] = ((addr & 0xFF000000) >>24 );
	cmd[2] = ((addr & 0x00FF0000) >>16 );
	cmd[3] = ((addr & 0x0000FF00) >>8 );
	cmd[4] = (addr &  0x000000FF);

	mmc_wait_ready ();

	response=mmc_write_command (cmd);		// commando senden und response speichern

	while (mmc_read_byte() != 0xFE){		// warten auf start byte
		nop();
	};

	return response;
}


// **********************************************************************************************************************************
//multi block lesen von sektoren. bei aufruf wird immer ein sektor gelesen und immer der reihe nach
// **********************************************************************************************************************************
void mmc_multi_block_read_sector (uint8_t *Buffer){

	uint16_t a; 							// einfacher zähler fuer bytes eines sektors

	// mmc/sd in hardware spi, block lesen
	#if (MMC_SOFT_SPI==FALSE)
	   uint8_t tmp; 						// hilfs variable zur optimierung
	   a=512;
	   SPDR = 0xff;								// dummy byte
		do{										// 512er block lesen
			loop_until_bit_is_set(SPSR,SPIF);
			tmp=SPDR;
			SPDR = 0xff;						// dummy byte
			*Buffer=tmp;
			Buffer++;
		}while(--a);

	// mmc/sd/sdhc in software spi, block lesen
	#else
		a=512;
		do{
			*Buffer++ = mmc_read_byte();
		}while(--a);
	#endif

	mmc_read_byte();						// crc byte
	mmc_read_byte();						// crc byte

	while (mmc_read_byte() != 0xFE){		// warten auf start byte 0xFE, damit fängt jede datenuebertragung an...
		nop();
		}
}


// **********************************************************************************************************************************
// starten von multi block write. ab sektor addr wird der reihe nach geschrieben. also addr++ usw...
// **********************************************************************************************************************************
uint8_t mmc_multi_block_start_write (uint32_t int addr){

	uint8_t cmd[] = {0x40+25,0x00,0x00,0x00,0x00,0xFF};	// CMD25 (write_multiple_block),response R1
	uint8_t response;

	mmc_enable();

	// addressiertung bei mmc und sd (standart < 2.0) in bytes, also muss sektor auf byte adresse umgerechnet werden.
	// sd standart > 2.0, adressierung in sektoren, also 512 byte bloecke
	if(card_type==0) addr = addr << 9; //addr = addr * 512

	cmd[1] = ((addr & 0xFF000000) >>24 );
	cmd[2] = ((addr & 0x00FF0000) >>16 );
	cmd[3] = ((addr & 0x0000FF00) >>8 );
	cmd[4] = (addr &  0x000000FF);

	response=mmc_write_command (cmd);		// commando senden und response speichern

	return response;
}


// **********************************************************************************************************************************
//multi block schreiben von sektoren. bei aufruf wird immer ein sektor geschrieben immer der reihe nach
// **********************************************************************************************************************************
uint8_t mmc_multi_block_write_sector (uint8_t *Buffer){

	uint16_t a;			// einfacher zaehler fuer bytes eines sektors
	uint8_t response;

	mmc_write_byte(0xFC);

	// mmc/sd in hardware spi, block schreiben
	#if (MMC_SOFT_SPI==FALSE)
		uint8_t tmp;			// hilfs variable zur optimierung
		a=512;				// do while konstrukt weils schneller geht
		tmp=*Buffer;			// holt neues byte aus ram in register
		Buffer++;			// zeigt auf naechstes byte
		do{
			SPDR = tmp;    //Sendet ein Byte
			tmp=*Buffer;	// holt schonmal neues aus ram in register
			Buffer++;
			loop_until_bit_is_set(SPSR,SPIF);
		}while(--a);

	// mmc/sd in software spi, block schreiben
	#else
		a=512;
		do{
			mmc_write_byte(*Buffer++);
		}while(--a);
	#endif

	//CRC-Bytes schreiben
	mmc_write_byte(0xFF); //Schreibt Dummy CRC
	mmc_write_byte(0xFF); //CRC Code wird nicht benutzt

	response=mmc_read_byte();

	mmc_wait_ready();

	if ((response&0x1F) == 0x05 ){			// daten von der karte angenommen, alles ok.
		return TRUE;
	}

	return FALSE;							// daten nicht angenommen... hiernach muss stop token gesendet werden !

}



#endif

// **********************************************************************************************************************************
// wartet darauf, dass die mmc karte in idle geht
// **********************************************************************************************************************************
static uint8_t mmc_wait_ready (void){

	TimingDelay = 50;

	do{
		if(	 spi_read_byte() == 0xFF ) return TRUE;
	}while ( TimingDelay );

	return FALSE;
}




// **********************************************************************************************************************************
// Routine zum schreiben eines Blocks(512Byte) auf die MMC/SD-Karte
// **********************************************************************************************************************************
uint8_t mmc_write_sector (uint32_t addr,uint8_t *buffer){

	uint8_t resp;
	uint8_t retrys;
	uint16_t count;

	if ( !(fat.card_type & CT_BLOCK) ){
		addr *= 512;				// Convert to byte address if needed
	}

	if ( mmc_send_cmd(CMD24, addr) != 0){ 	// enables card
		return FALSE;
	}

	if ( FALSE == mmc_wait_ready() ){
		return FALSE;
	}

	spi_write_byte(0xFE);			// Xmit data token

	count = 512;
	do {							// Xmit the 512 byte data block to MMC
		spi_write_byte(*buffer++);
	} while (--count);

	spi_write_byte(0xFF);			// CRC (Dummy)
	spi_write_byte(0xFF);

	retrys = 20;
	do{
		resp = spi_read_byte();		// Reveive data response, 20 retrys if not acepted
	}while( (resp & 0x1F) != 0x05 && --retrys);

	if ( retrys == 0){				// If not accepted, return with error
		return FALSE;
	}

	mmc_disable();

	return TRUE;
}


// **********************************************************************************************************************************
// Routine zum lesen eines Blocks(512Byte) von der MMC/SD-Karte
// **********************************************************************************************************************************
uint8_t mmc_read_sector (uint32_t addr,uint8_t *buffer){

	uint8_t token;
	uint16_t count;

	if ( !(fat.card_type & CT_BLOCK) ) addr *= 512;	// Convert to byte address if needed

	if ( mmc_send_cmd(CMD17, addr) != 0 ){
		return FALSE;
	}

	TimingDelay = 20;
	do {							// Wait for data packet in timeout of 200ms
		token = spi_read_byte();
	} while ( (token == 0xFF) && TimingDelay );

	if(token != 0xFE){
		return FALSE;				// If not valid data token, retutn with error
	}

	count = 512;
	do {							// Receive the data block into buffer
		*buffer++ = spi_read_byte();
	} while (--count);

	spi_read_byte();				// Discard CRC
	spi_read_byte();

	mmc_disable();

	return TRUE;					// Return with success
}



// **********************************************************************************************************************************
#if(MMC_STATUS_INFO == TRUE)
uint8_t mmc_present(void) {
	  return get_pin_present() == 0x00;
  }
#endif


// **********************************************************************************************************************************
#if(MMC_STATUS_INFO == TRUE && MMC_WRITE == TRUE)
uint8_t mmc_protected(void) {
	  return get_pin_protected() != 0x00;
  }
#endif

#if (MMC_LS == TRUE)
// *****************************************************************************************************************
int8_t *ltostr(int32_t num, int8_t *string, uint16_t max_chars, uint8_t base)
{
      int8_t remainder;
      int sign = 0;   /* number of digits occupied by the sign. */

      if (base < 2 || base > 36)
            return FALSE;

      if (num < 0)
      {
            sign = 1;
            num = -num;
      }

      string[--max_chars] = '\0';

      for (max_chars--; max_chars > sign && num!=0; max_chars --)
      {
            remainder = (int8_t) (num % base);
            if ( remainder <= 9 )
                  string[max_chars] = remainder + '0';
            else  string[max_chars] = remainder - 10 + 'A';
            num /= base;
      }

      if (sign)
            string[--max_chars] = '-';

      if ( max_chars > 0 )
            memset(string, ' ', max_chars+1);

      return string + max_chars;
}
#endif



// *****************************************************************************************************************
ISR (TIMER0_COMPA_vect)
{
	TimingDelay = (TimingDelay==0) ? 0 : TimingDelay-1;
}



// *****************************************************************************************************************
void timer0_init(){

	TimingDelay = 0;		// initialisierung der zaehl variable

	TCCR0A = 1<<WGM01; 		// timer0 im ctc mode
	TIMSK0 = 1<<OCIE0A;		// compare interrupt an

	TCNT0 = START_TCNT;		// ab wo hochgezaehlt wird,
	OCR0A = TOP_OCR;		// maximum bis wo gezaehlt wird bevor compare match

	TCCR0B = PRESCALER;		// wenn prescaler gesetzt wird, lauft timer los
	sei();					// interrupts anschalten, wegen compare match
}

// *****************************************************************************************************************

