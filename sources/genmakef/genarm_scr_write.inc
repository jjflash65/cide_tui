function mydesk.write_armmakefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;
  
  ldstring            : string;
  hs				  : string;
  
  addputchar		  : boolean;
  
  f4mcu, lpcmcu, l0mcu,
  f0mcu, f1mcu, f3mcu  : boolean;

const
  nonstm32 = 7;						// Ab diesem Index kommen "NON STM32" MCUs in der Liste
  

const
  serporttxt : array[0..8] of string =
    ('', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2',
     '/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2',
     '/dev/ttyS0', '/dev/ttyS1');

begin   
  f4mcu:= false; f1mcu:= false; f3mcu:= false;
  f0mcu:= false; l0mcu:= true;
  lpcmcu:= false;
  writetoscreen:= toscreen;
  addputchar:= false;

  if (not(toscreen)) then
  begin
    // Makefile in Datei schreiben
    write_armmakefile:= 0;
    dname:= expandfilename(armpath);
    dname:= dname + '/' + arm32rec.proj_dir;
    fname:= dname + '/' + arm32rec.proj_nam;
    mname:= dname + '/Makefile';
    
    lastpath:= dname;
    

    {$i-} mkdir(dname); {$i+}
    ior:= ioresult;

    if (fileexists(mname)) then
    begin
      c:= messagebox('There is an existing Makefile, do you want to overwrite it?',nil, mfConfirmation or mfOkCancel);
      if (c <> 10) then exit;
    end;

    assign(tdat,mname);
    {$i-} rewrite(tdat); {$i+}
    ior:= ioresult;
    if (ior <> 0) then
    begin
      mymessage('Cannot generate a makefile in this directory');
      myintmessage(ior);
      exit;
    end;

  end;

  linecount:= 0;
  addaline(tdat,'############################################################');
  addaline(tdat,'#');
  addaline(tdat,'#                         Makefile');
  addaline(tdat,'#');
  addaline(tdat,'############################################################');
  addaline(tdat,'');
  addaline(tdat,'PROJECT       = '+arm32Rec.proj_nam);
  addaline(tdat,'');
  addaline(tdat,'# hier alle zusaetzlichen Softwaremodule angegeben');
  if (lnk32anz> 0) then
  begin
    for i:= 1 to lnk32anz do
    begin
      addaline(tdat,arm32_lnkfiles[i-1]);
    end;
  end;
  addaline(tdat,'');
  
  ldstring:= '';
  case arm32listwahl of
    0 : begin
          ldstring:= 'stm32f030x6.ld';					// STM32F030   (16kb)
          f0mcu:= true;
        end;  
    1 : begin
    		  ldstring:= 'stm32f103c8.ld';					// STM32F103   (64kb) 
		      f1mcu:= true;
		    end;  
    2 : begin
          ldstring:= 'stm32f103cb.ld';					// STM32F103   (128kb) 
          f1mcu:= true;
        end; 
    3 : begin
    		  ldstring:= 'stm32f303cb.ld';					// STM32F103   (128kb) 
		      f3mcu:= true;
		    end;  
    4 : begin
          ldstring:= 'stm32f303cc.ld';					// STM32F303   (128kb) 
          f3mcu:= true;
        end; 
    5 : begin
		      ldstring:= 'stm32f401re.ld';					// STM32F401 
		      f4mcu:= true;
		    end;  
    6 : begin
          ldstring:= 'stm32f407.ld';		 			  // STM32F407 
          f4mcu:= true;
        end;  
    7 : begin
          ldstring:= 'stm32f411re.ld';				  // STM32F411
          f4mcu:= true;
        end;  
    8 : begin
          ldstring:= 'stm32l053.ld';					  // STM32L053   (32kb)
          l0mcu:= true;
        end;  
    9 : begin
          ldstring:= 'lpc11cx4.ld';						  // LPC11Cx4
          lpcmcu:= true;
        end;  
  end;
  addaline(tdat,'');
  addaline(tdat,'INC_DIR       = -I./ -I../include');
  addaline(tdat,'');
  
  addaline(tdat,'LSCRIPT       = '+ldstring);
  
  addaline(tdat,'');       
  addaline(tdat,'# FLASHERPROG Auswahl fuer STM32:');
  addaline(tdat,'# 0 : STLINK-V2, 1 : 1 : stm32flash_rts  2 : stm32chflash 3 : DFU_UTIL');
  addaline(tdat,'# FLASHERPROG Auswahl fuer LPC');
  addaline(tdat,'# 4 : flash1114_rts');
  addaline(tdat,'');       
  addaline(tdat,'PROGPORT      = '+ serporttxt[arm32Rec.progport]);
  if ((arm32Rec.before_flash and $01)= $01) then
    addaline(tdat,'CH340RESET    = 1')
  else  
    addaline(tdat,'CH340RESET    = 0');
  if ((arm32Rec.before_flash and $02)= $02) then
    addaline(tdat,'ERASEFLASH    = 1')
  else  
    addaline(tdat,'ERASEFLASH    = 0');
  str(arm32Rec.programmers, hs);
  addaline(tdat,'FLASHERPROG   = '+hs);
  addaline(tdat,'');       
  addaline(tdat,'');    
  if (arm32listwahl < nonstm32) then				   
    addaline(tdat,'include ../lib/libopencm3.mk')			// stm32 & libopencm3
  else  
    addaline(tdat,'include ../lib/makefile.mk');			// Projekt ohne libopencm3
  write_armmakefile:= linecount;

  if (not(toscreen)) then
  begin
    close(tdat);
    fsplit(fname,sourcedir,na,ex);
    fname:= sourcedir+na+'.c';
    if (fileexists(fname)) then
    begin
      c:= messagebox('There is an existing file: '+ fname + ', do you want to overwrite it?',nil, mfConfirmation or mfOkCancel);
      if (c <> 10) then exit;
    end;

    decodedate (date, yy, mm, dd);
    assign(tdat, fname);
    for i:= 0 to arm_src_zeilanz do
    begin
      if (f4mcu) then      
        arm_templ[i]:= arm_templ_f4[i]
      else if (f3mcu) then
        arm_templ[i]:= arm_templ_f3[i]
      else  
        arm_templ[i]:= arm_templ_std[i]
    end;
    
    rewrite(tdat);
    writeln(tdat, arm_templ[0]);
    writeln(tdat, '     ' + arm32Rec.proj_nam + '.c');
    writeln(tdat, '');
    writeln(tdat, format('     %d.%d.%d',[dd,mm,yy]));
    writeln(tdat, '');
    for i:= 1 to 2 do
    begin
      writeln(tdat, arm_templ[i]);
    end;
    
    // #include Header fuer zu linkende Files
    for i:= 1 to lnk32anz do
    begin
      writeln(tdat, '#include "' + lnk32files[i-1] + '.h"');
      if (lnk32files[i-1]= 'my_printf') or (lnk32files[i-1]= 'my_printf_float' ) then
        addputchar:= true;
    end;
    
    if addputchar= true then addsrc_putchar(tdat);
    
    // den "Rest" schreiben
    for i:= 3 to arm_src_zeilanz do
    begin
      writeln(tdat, arm_templ[i]);
    end;
    close(tdat);

  end;
end;



procedure mydesk.arm32_make_gen;
var
  arm32_dlg   : PDialog;
  R           : TRect;
  Control     : PView;
  scroll      : PScrollBar;
  lb          : PListBox;
  list        : PStrCollection;
  i			  : integer;
  c, c2       : word;
  mklen		  : word;

  sourcedir,
  olddir, na, ex, s   : string;

begin
  olddir:= getcurrentdir;
  R.Assign(1, 3, 43, 38);
  if (lnk32anz> 0) then
  begin
    for i:= 1 to lnk32anz do
    begin
      lines[i-1]:= newstr(arm32_lnkfiles[i-1]);
    end;
    linecount:= lnk32anz;
  end;
  genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
  DeskTop^.Insert(genwindow);

  R.Assign(45, 3, 116, 38);
  New(arm32_dlg, Init(R, 'Makefile generator for arm32 MCU'));

  // die Devicelist muss als erstes angefuehrt werden, da ansonsten beim 64-Bit Compiler
  // aus mir nicht erklaerbaren Gruenden das Programm abstuerzt !!!!
  // Die dynamische Liste selbst muss am Ende der Fensterinhalte eingefuegt werden

  R.Assign(3, 6, 11, 7);
  Control := New(PStaticText, Init(R, 'Devices'));
  arm32_dlg^.Insert(Control);

  // Scrollbar der Deviceliste
  R.Assign(21, 8, 22, 19);
  scroll := New(PScrollBar, Init(R));
  arm32_dlg^.Insert(scroll);

  // die Deviceliste mit Scrollbar
  R.Assign(3, 8, 21, 19);
  lb := New(PListBox, Init(R, 1, PScrollbar(scroll)));
  arm32_dlg^.Insert(lb);

  R.Assign(3, 22, 14, 23);
  Control := New(PStaticText, Init(R, 'Programmer'));
  arm32_dlg^.Insert(Control);

  R.Assign(3, 24, 27, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('ST-Link v2',
    NewSItem('stm32flash (rts)',
    NewSItem('stm32flash (ch340)',
    NewSItem('DFU-UTIL',
    NewSItem('flash11cx4 (rts)',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved', Nil)))))))))));
  arm32_dlg^.Insert(Control);

  R.Assign(29, 22, 49, 23);
  Control := New(PStaticText, Init(R, 'Programmer port'));
  arm32_dlg^.Insert(Control);

  R.Assign(29, 24, 49, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('USB (ST-Link)',
    NewSItem('/dev/ttyUSB0',
    NewSItem('/dev/ttyUSB1',
    NewSItem('/dev/ttyUSB2',
    NewSItem('/dev/ttyACM0',
    NewSItem('/dev/ttyACM1',
    NewSItem('/dev/ttyACM2',
    NewSItem('/dev/ttyS0',
    NewSItem('/dev/ttyS1', Nil)))))))))));
  arm32_dlg^.Insert(Control);

  R.Assign(42, 15, 67, 17);
  Control := New(PStaticText, Init(R, 'Additional source files'^M+
       'to link'));
  arm32_dlg^.Insert(Control);
  
  R.Assign(52, 22, 69, 24);
  Control := New(PStaticText, Init(R, 'before flashing'));
  arm32_dlg^.Insert(Control);

  R.Assign(52, 24, 69, 26);
  Control := New(PCheckboxes, Init(R,
    NewSItem('CH340-reset',
    NewSItem('erase',Nil))));
  arm32_dlg^.Insert(Control);  

  R.Assign(3, 2, 33, 3);
  Control := New(PStaticText, Init(R, 'Path to ARM projects'));
  arm32_dlg^.Insert(Control);
  R.Assign(3, 3, 69, 4);							// Projekte - Heimverzeichnis
  Control := New(PStaticText, Init(R, armpath));
  arm32_dlg^.Insert(Control);

  R.Assign(39, 10, 66, 11);
  Control := New(PStaticText, Init(R, 'Project name'));
  arm32_dlg^.Insert(Control);
  R.Assign(39, 11, 66, 12);
  Control := New(PStaticText, Init(R, '(without .c extension)'));
  arm32_dlg^.Insert(Control);

  R.Assign(39, 7, 66, 8);
  Control := New(PStaticText, Init(R, 'Project directory'));
  arm32_dlg^.Insert(Control);

  R.Assign(39, 8, 68, 9);					        // Projektdirectory
  Control := New(PInputLine, Init(R, 255));
  arm32_dlg^.Insert(Control);

  R.Assign(39, 12, 68, 13);					        // Projektname
  Control := New(PInputLine, Init(R, 38));
  arm32_dlg^.Insert(Control);

  R.Assign(57, 32, 69, 34);
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfNormal));
  arm32_dlg^.Insert(Control);

  R.Assign(57, 29, 69, 31);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfNormal));
  arm32_dlg^.Insert(Control);

  R.Assign(41, 18, 53, 20);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfNormal));
  arm32_dlg^.Insert(Control);

  R.Assign(54, 18, 68, 20);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfNormal));
  arm32_dlg^.Insert(Control);

  R.Assign(3, 20, 68, 21);
  Control := New(PStaticText, Init(R, '-------------------------------------'+
       '----------------------------'));
  arm32_dlg^.Insert(Control);

  arm32_dlg^.SelectNext(False);
  arm32_dlg^.setdata(arm32Rec);               // Einstellungen holen

  // die Liste mit arm32-Controllern der Listbox "laden"
  // muss leider hier am Ende des Fensters angegeben, weil, warum auch immer
  // unter einem 64-Bit System sich das System aufhaengt ! Warum ?????
  list := New(PStrCollection, Init(partanz_arm32-1, 5));
  for i:= 0 to partanz_arm32-1 do
  begin
    list^.AtInsert(i, NewStr(arm32name[i]));
  end;
  lb^.Newlist(list);

  repeat
    lb^.focusitem(arm32listwahl);             // alten Focus der Listbox setzen
    c:= desktop^.execview(arm32_dlg);         // kompletten Dialog anzeigen
    begin
      arm32_dlg^.getdata(arm32Rec);           // Einstellungen speichern
      arm32listwahl:= lb^.focused;
    end;

    // eine Datei zum Linken hinzufuegen
    if c= cmNo then
    begin
      chdir(armpath+'/src');
      s:= openlinkfiles;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);

        lnk32files[lnk32anz]:= na;
        if (lnk32anz= 0) then
          arm32_lnkfiles[lnk32anz]:= 'SRCS          = ../src/'+na+'.o'
        else
          arm32_lnkfiles[lnk32anz]:= 'SRCS         += ../src/'+na+'.o';
//        mymessage(arm32_lnkfiles[lnkanz]);
        lines[linecount]:= newstr(arm32_lnkfiles[lnk32anz]);
        inc(lnk32anz);
        linecount:= lnk32anz;
        tgentxwin.draw;

        fsplit(s,sourcedir,na,ex);
      end;
      chdir(olddir);
    end;

    // Projekt und Makefile generieren
    if c=cmYes then
    begin

      if (arm32rec.proj_dir= '') then
      begin
        mymessage('There is no projectdirectory given, cannot create a Makefile');
      end else
      begin

      // alten Inhalt des Anzeigefensters loeschen (dynamische Strings)
      if (lnk32anz> 0) then
      begin
        for i:= 1 to lnk32anz do
        begin
          disposestr(lines[i-1]);
        end;
        linecount:= 0;
        tgentxwin.draw;
      end;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      mklen:= write_armmakefile(true);
      R.Assign(25, 1, 98, 28);
      genWindow := New(pgentxwin, Init(R, 'generated Makefile', 0));
      DeskTop^.Insert(genwindow);

      R.Assign(43,30,80,38);
      c2:= MessageBoxRect(r,'Generate a project with this makefile ?',nil,mfConfirmation or mfOkCancel);

      if (c2= 10) then
      begin
        mklen:= write_armmakefile(false);
      end;
      for i:= 1 to mklen do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      R.Assign(1, 3, 43, 38);
      if (lnk32anz> 0) then
      begin
        for i:= 1 to lnk32anz do
        begin
          lines[i-1]:= newstr(arm32_lnkfiles[i-1]);
        end;
        linecount:= lnk32anz;
      end;

      genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
      DeskTop^.Insert(genwindow);

    end;
  end;

  if c= cmok then
  begin
    if (lnk32anz> 0) then
    begin
      for i:= 1 to lnk32anz do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;
      lnk32anz:= 0;
      tgentxwin.draw;
    end;
  end;

  until (c= cmcancel);
  if (lnk32anz> 0) then
  begin
    for i:= 1 to lnk32anz do
    begin
      disposestr(lines[i-1]);
    end;
    linecount:= 0;
  end;
  tgentxwin.closewin(genwindow);
end;
