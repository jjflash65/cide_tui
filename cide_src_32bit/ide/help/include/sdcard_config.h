/*
 * 	Doku, siehe http://www.mikrocontroller.net/articles/AVR_FAT32
 * 	Neuste Version: http://www.mikrocontroller.net/svnbrowser/avr-fat32/
 * 	Autor: Daniel R.
 */


#ifndef CONFIG_H_
  #define CONFIG_H_

  #include <ctype.h>
  #include <stdint.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <string.h>



  //#####################################################################################
  // BOOL DEFINITIONEN

  #define TRUE 	0x01
  #define FALSE	0x00

  //#####################################################################################
  // FLAGS

  // flags fuer ffopen (sind die rueckgabewerte von ffopen, auf diese wird geprueft..)
  #define MMC_FILE_OPENED 	0	// datei existierte und wurde geoeffnet.
  #define MMC_FILE_CREATED 	1	// datei angelegt.
  #define MMC_FILE_ERROR	2	// datei existiert nicht/nicht angelegt.


  //#####################################################################################
  // WICHTIGE SCHALTER

  // schalter die den funktionsumfang aendern bzw, den funktionsumfang einiger funktionen :)

  #define MMC_WRITE               TRUE	  // TRUE, dann mit write unterstuetzung, wenn FALSE dann read only !
  #define MMC_OVER_WRITE          TRUE    // TRUE und MMC_WRITE TRUE, dann kann ffwrite dateien ueberschreiben, wenn FALSE dann nur normales schreiben. um an eine datei anzuhaengen ist MMC_OVER_WRITE TRUE nicht noetig!
  #define MMC_MULTI_BLOCK         FALSE   // TRUE und MMC_OVER_WRITE FALSE, dann werden multiblock schreib/lese funktionen benutzt. ist schneller, wird aber moeglicherweise nicht von allen karten unterstuetzt. wenn FALSE ist normale operation
  #define MMC_ENDIANNESS_LITTLE   TRUE    // TRUE, dann ist der code auf littleendian ausgelegt. AVR ist littleendian. code ist auf littleendian optimiert!! siehe: http://de.wikipedia.org/wiki/Endianness
  #define MMC_LFN_SUPPORT         FALSE   // TRUE, dann mit unterstuetzung fuer lange dateinamen. kostet wenn read und write benutzt wird um die 800 bytes flash...
  #define MMC_RM_FILES_ONLY       FALSE   // TRUE ,MMC_WRITE TRUE und MMC_RM TRUE, dann wird die funktion ffrm so mit kompiliert, dass sie nur dateien loeschen kann. wenn FALSE und MMC_WRITE TRUE, dann kann die funktion dateien und ordner rekursiv loesche

  // schalter die explizit funktionen mit kompilieren oder nicht!
  #define MMC_TIME_STAMP          TRUE    // TRUE, dann werden die funktionen fat_getTime und fat_getFreeBytes mit kompiliert. siehe auch abschnitt: ZEIT FUNKTIONEN, weiter unten
  #define MMC_RM                  TRUE    // TRUE und MMC_WRITE TRUE, dann wird die funktion ffrm mit kompiliert.
  #define MMC_SEEK                TRUE    // TRUE,dann wird die funktion ffseek mit kompiliert. mit dieser funktion kann man in einer geoeffneten datei vor und zurueck spulen. nur in kombination mit MMC_OVER_WRITE TRUE kann in einer datei ueberschrieben w
  #define MMC_MKDIR               TRUE    // TRUE und MMC_WRITE TRUE, dann wird die funktion ffmkdir mit kompiliert. mit dieser funktion kann man ordner anlegen.
  #define MMC_GET_FREE_BYTES      TRUE    // TRUE, dann wird die funkton fat_getFreeBytes mit kompiliert. mit dieser funktion kann der freie platzt auf der karte ermittelt werden
  #define MMC_LS                  TRUE    // TRUE, dann wird die funktion ffls mit kompiliert. mit dieser funkion kann man die dateien auf der karte anzeigen lassen
  #define MMC_CD                  TRUE    // TRUE, dann werden die funktionen ffcd und ffcdLower mit kompiliert. mit diesen funktionen kann man in ein verzeichnis wechseln oder aus einem verzeichnis ein verzeichnis hoeher wechseln.
  #define MMC_FILE_EXSISTS        TRUE    // TRUE, dann wird die funktion ffileExsists mit kompiliert. mit dieser funktion kann geprueft werden, ob es die datei im aktuellen verzeinis gibt !
  #define MMC_WRITE_STRING        TRUE    // TRUE und MMC_WRITE TRUE, dann wird die funktion ffwrites mit kompiliert. mit dieser funktion koennen strings auf die karte geschrieben werden.
  #define MMC_WRITEN              TRUE    // TRUE	und MMC_WRITE TRUE, dann wird die funktion ffwriten mit kompiliert.	mit dieser funktion koennen mehrere zeichen auf die karte geschrieben werden!

  // vorsicht, da die variable die die sektoren zaehlt ein short ist (MMC_MAX_CLUSTERS_IN_ROW*fat.secPerClust) !!
  #define MMC_MAX_CLUSTERS_IN_ROW 256 	// gibt an wie viele cluster am stueck ohne fat-lookup geschrieben bzw gelesen werden können, wenn die fat nicht fragmentiert ist !


  //#####################################################################################
  // SPI EINSTELLUNGEN

  // wenn MAX_SPEED FALSE dann wird die SPI bus geschwindigkeit nicht hoch gesetzt. ist zum testen von hardware die nicht richtig läuft
  // momentan ist bei MAX_SPEED F_CPU / 8 eingetragen !!!
  #define MMC_MAX_SPEED 	TRUE
  #define MMC_STATUS_INFO 	FALSE

  // software spi? wenn TRUE muessen in der mmc.h noch die pins eingestellt werden ueber die die spi kommunikation laufen soll
  // es werden nur pins an einem port unterstuetzt !
  #define MMC_SOFT_SPI 	FALSE


  //#####################################################################################
  // TYPDEFINITIONEN

  // pointer typ: fptr, auf eine funktion mit einem arugment: arg und ohne rueckgabewert. arg ist vom typ: "uint8_t *" !
  // wird fuer die ausgabe benutzt, so kann in der main auch einfach eine eigene
  // uart routine verwendet werden! beispiel aufruf von ffls: "ffls(uputs);" wobei uputs eine funktion
  // der form "uputs(uint8_t *s)" ist
  typedef void(*fptr_t)(uint8_t* arg);


  // in main.c definiert. konvertiert einen long zu string, wird nur von ls benoetigt
  #if (MMC_LS == TRUE)
  //              extern char *ltostr(long num, char *string, uint16_t max_chars, unsigned base);
    int8_t *ltostr(int32_t num, int8_t *string, uint16_t max_chars, uint8_t base);
  #endif


  //#########################################################################################################################################
  // DATENSTRUKTUR - DEKLARATIONEN (urspruenglich waren die in fat.c)

  extern struct Fat_t
  {					// fat daten (1.cluster, root-dir, dir usw.)
    uint32_t  lastSector;		// nicht der aktuell geladene sektor, sondern der davor
    uint8_t   bufferDirty;              // puffer wurde beschrieben, sector muss geschrieben werden bevor er neu geladen wird
    uint32_t  dir; 		  	// Direktory zeiger rootDir=='0' sonst(1.Cluster des dir; start auf root)
    uint32_t  rootDir;			// Sektor(f16)/Cluster(f32) nr root directory
    uint32_t  dataDirSec;		// Sektor nr data area
    uint32_t  fatSec;	 		// Sektor nr fat area
    uint8_t   secPerClust;              // anzahl der sektoren pro cluster
    uint8_t   fatType;                  // fat16 oder fat32 (16 oder 32)
    uint8_t   card_type;		// der SD/MMC karten typ. siehe auch mmc.c
    uint8_t   sector[512];              // der puffer fuer sektoren !
  } fat;

  extern struct File_t			// datei infos usw.
  {
    uint16_t  cntOfBytes;		// seek + cntOfBytes ist position in datei
    uint32_t  seek;			// seek + cntOfBytes ist position in datei
    uint32_t  currentSectorNr;          // aktuell zu bearbeitender sektor bei einer geoeffneten datei
    uint32_t  length;			// 28,4  datei Laenge (4-byte)
    uint8_t   *name;			// zeiger auf langen dateinamen (lfn)...ueber den wird auch im ordner nach der datei gesucht. der kurze dateiname (sfn) leitet sich aus dem lfn ab!
    uint8_t   row;			// reihe im sektor in der die datei infos stehen (reihe 0-15)
    uint32_t  firstCluster;             // 20,2 /26,2	datei 1.cluster hi,low(moeglicherweise der einzige)	(4-byte)
    uint32_t  entrySector;              // der sektor in dem der sfn dateieintrag ist.
  } file;

  extern struct Chain_t
  {
    uint32_t  startSectors;             // erster freier/verketteter sektor
    uint16_t  cntSecs;                  // anzahl der freien/verketteten in einer reihe
    uint32_t  lastCluster;              // letzter bekannter cluster der teilkette davor !
  } chain;


#endif
