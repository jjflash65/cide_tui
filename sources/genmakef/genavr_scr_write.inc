{ -----------------------------------------------------------------------------------
    schreibt das Makefile und eine vom Benutzer zu bearbeitende Projektdatei
    auf Datentraeger
  ----------------------------------------------------------------------------------- }
function mydesk.write_avrmakefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;
  
  addputchar          : boolean;

const
  programmerstxt  : array[0..8] of string =
    ('usbtiny', 'usbasp', 'stk500v2', 'ponyser', 'arduino',
     'avrisp', 'stk500v2', 'wiring', '');

  serporttxt : array[0..8] of string =
    ('', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2',
     '/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2',
     '/dev/ttyS0', '/dev/ttyS1');

  bratetxt : array[0..4] of string =
    ('9600', '19200', '38400', '57600', '115200');

  sclktxt : array[0..2] of string=
    ('-B15', '-B5', '-B1');

begin
  writetoscreen:= toscreen;
  addputchar:= false;

  if (not(toscreen)) then
  begin
    // Makefile in Datei schreiben
    write_avrmakefile:= 0;
    dname:= expandfilename(avrpath);
    dname:= dname + '/' + avrrec.proj_dir;
    fname:= dname + '/' + avrrec.proj_nam;
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
  addaline(tdat,'PROJECT    = '+avrRec.proj_nam);
  addaline(tdat,'');
  addaline(tdat,'');
  addaline(tdat,'INC_DIR    = -I./ -I../include');
  addaline(tdat,'');
  addaline(tdat,'# hier alle zusaetzlichen Softwaremodule angegeben');
  addaline(tdat,'');
  if (lnkanz> 0) then
  begin
    for i:= 1 to lnkanz do
    begin
      addaline(tdat,avr_lnkfiles[i-1]);
    end;
  end;

  addaline(tdat,'');
  if ((avrRec.float_check and $01)= 1) then
    addaline(tdat,'PRINT_FL   = 1')
  else
    addaline(tdat,'PRINT_FL   = 0');

  if ((avrRec.float_check and $02)=  2) then
    addaline(tdat,'SCAN_FL    = 1')
  else
    addaline(tdat,'SCAN_FL    = 0');

  if ((avrRec.float_check and $04)= 4) then
    addaline(tdat,'MATH       = 1')
  else
    addaline(tdat,'MATH       = 0');

   addaline(tdat,'');
  addaline(tdat,'# fuer Compiler / Linker');
  addaline(tdat,'FREQ       = '+avrRec.freq_str+'ul');
  addaline(tdat,'MCU        = '+ avrgccname[avrlistwahl]);
  addaline(tdat,'');    
  addaline(tdat,'# fuer AVRDUDE');
  addaline(tdat,'PROGRAMMER = '+ programmerstxt[avrRec.programmers]);
  addaline(tdat,'PROGPORT   = '+ serporttxt[avrRec.progport]);
  addaline(tdat,'BRATE      = '+ bratetxt[avrRec.baud]);
  addaline(tdat,'DUDEOPTS   = '+ sclktxt[avrRec.isp_clk]);
  addaline(tdat,'');
  addaline(tdat,'# bei manchen Mainboards muss ein CH340G (USB zu seriell Chip) evtl. geresetet werden!');
  if (avrRec.reset340 = 1) then
    addaline(tdat,'CH340RESET = 1')
  else
    addaline(tdat,'CH340RESET = 0'); 
  addaline(tdat,'');
  
  addaline(tdat,'include ../makefile.mk');
  addaline(tdat,'');

  write_avrmakefile:= linecount;

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
    rewrite(tdat);
    writeln(tdat, avr_templ[0]);
    writeln(tdat, '     ' + avrRec.proj_nam + '.c');
    writeln(tdat, '');
    writeln(tdat, '     MCU     : ' + avrgccname[avrlistwahl]);
    writeln(tdat, '     F_CPU   : ' + avrRec.freq_str);
    writeln(tdat, '');
    writeln(tdat, format('     %d.%d.%d',[dd,mm,yy]));
    writeln(tdat, '');
    for i:= 1 to 7 do
    begin
      writeln(tdat, avr_templ[i]);
    end;
    // #include Header fuer zu linkende Files
    for i:= 1 to lnkanz do
    begin
      writeln(tdat, '#include "' + lnkfiles[i-1] + '.h"');
      if (lnkfiles[i-1]= 'my_printf') or (lnkfiles[i-1]= 'my_printf_float' ) then
        addputchar:= true;
    end;   
    
    if addputchar= true then addsrc_putchar(tdat);
    
    // den "Rest" schreiben
    for i:= 8 to avr_src_zeilanz do
    begin
      writeln(tdat, avr_templ[i]);
    end;
    close(tdat);

  end;
end;


{ -----------------------------------------------------------------------------------
    Bildschirmmaske und damit Auswahlmenue fuer ein zu erstellendes Makefile
    fuer AVR - Controller
  ----------------------------------------------------------------------------------- }
  
procedure mydesk.avr_make_gen;
var
  avr_dlg     : PDialog;
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
  if (lnkanz> 0) then
  begin
    for i:= 1 to lnkanz do
    begin
      lines[i-1]:= newstr(avr_lnkfiles[i-1]);
    end;
    linecount:= lnkanz;
  end;
  genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
  DeskTop^.Insert(genwindow);

  R.Assign(45, 3, 116, 38);
  New(avr_dlg, Init(R, 'Makefile generator for AVR MCU'));

  // die Devicelist muss als erstes angefuehrt werden, da ansonsten beim 64-Bit Compiler
  // aus mir nicht erklaerbaren Gruenden das Programm abstuerzt !!!!

  R.Assign(3, 7, 11, 8);
  Control := New(PStaticText, Init(R, 'Devices'));
  avr_dlg^.Insert(Control);

  R.Assign(18, 8, 19, 19);
  scroll := New(PScrollBar, Init(R));
  avr_dlg^.Insert(scroll);

  R.Assign(3, 8, 18, 19);
  lb := New(PListBox, Init(R, 1, PScrollbar(scroll)));
  avr_dlg^.Insert(lb);

  R.Assign(3, 22, 14, 23);
  Control := New(PStaticText, Init(R, 'Programmer'));
  avr_dlg^.Insert(Control);

  R.Assign(3, 24, 21, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('USBtinyisp',
    NewSItem('USBasp',
    NewSItem('Diamex-ISP',
    NewSItem('PonySer',
    NewSItem('Arduino Boot',
    NewSItem('AVRISP',
    NewSItem('STK500v2',
    NewSItem('wiring',
    NewSItem('reserved', Nil)))))))))));
  avr_dlg^.Insert(Control);

  R.Assign(23, 22, 40, 23);
  Control := New(PStaticText, Init(R, 'Programmer port'));
  avr_dlg^.Insert(Control);

  R.Assign(23, 24, 41, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('USBasp-tiny ',
    NewSItem('/dev/ttyUSB0',
    NewSItem('/dev/ttyUSB1',
    NewSItem('/dev/ttyUSB2',
    NewSItem('/dev/ttyACM0',
    NewSItem('/dev/ttyACM1',
    NewSItem('/dev/ttyACM2',
    NewSItem('/dev/ttyS0',
    NewSItem('/dev/ttyS1', Nil)))))))))));
  avr_dlg^.Insert(Control);

  R.Assign(58, 22, 68, 23);
  Control := New(PStaticText, Init(R, 'ISP-clock'));
  avr_dlg^.Insert(Control);

  R.Assign(58, 24, 68, 27);
  Control := New(PRadioButtons, Init(R,
    NewSItem('Low',
    NewSItem('Med',
    NewSItem('High', Nil)))));
  avr_dlg^.Insert(Control);

  R.Assign(43, 22, 54, 23);
  Control := New(PStaticText, Init(R, 'Baudrate'));
  avr_dlg^.Insert(Control);

  R.Assign(43, 24, 56, 29);
  Control := New(PRadioButtons, Init(R,
    NewSItem('  9600',
    NewSItem(' 19200',
    NewSItem(' 38400',
    NewSItem(' 57600',
    NewSItem('115200', Nil)))))));
  avr_dlg^.Insert(Control);

  R.Assign(42, 15, 67, 17);
  Control := New(PStaticText, Init(R, 'Additional source files'^M+
       'to link'));
  avr_dlg^.Insert(Control);

  R.Assign(41, 9, 68, 16);

  R.Assign(22, 7, 37, 9);
  Control := New(PStaticText, Init(R, 'Support for'^M+
       'floating point'));
  avr_dlg^.Insert(Control);

  R.Assign(22, 9, 34, 12);
  Control := New(PCheckboxes, Init(R,
    NewSItem('printf',
    NewSItem('scanf',
    NewSItem('math',Nil)))));
  avr_dlg^.Insert(Control);
  
  R.Assign(22, 14, 37, 15);
  Control := New(PStaticText, Init(R, 'CH340 reset'^M+
       'floating point'));
  avr_dlg^.Insert(Control);
  
  R.Assign(22, 15, 34, 16);
  Control := New(PCheckboxes, Init(R,
    NewSItem('enable',Nil)));
  avr_dlg^.Insert(Control);

  R.Assign(3, 2, 33, 3);
  Control := New(PStaticText, Init(R, 'Path to AVR projects'));
  avr_dlg^.Insert(Control);
  R.Assign(3, 3, 69, 4);							// Projekte - Heimverzeichnis
  Control := New(PStaticText, Init(R, avrpath));
  avr_dlg^.Insert(Control);

  R.Assign(22, 17, 38, 18);
  Control := New(PStaticText, Init(R, 'clock frequency'));
  avr_dlg^.Insert(Control);
  R.Assign(34, 18, 39, 19);
  Control := New(PStaticText, Init(R, 'Hz'));
  avr_dlg^.Insert(Control);

  R.Assign(39, 10, 66, 11);
  Control := New(PStaticText, Init(R, 'Project name'));
  avr_dlg^.Insert(Control);
  R.Assign(39, 11, 66, 12);
  Control := New(PStaticText, Init(R, '(without .c extension)'));
  avr_dlg^.Insert(Control);

  R.Assign(39, 7, 66, 8);
  Control := New(PStaticText, Init(R, 'Project directory'));
  avr_dlg^.Insert(Control);

  R.Assign(39, 8, 68, 9);					        // Projektdirectory
  Control := New(PInputLine, Init(R, 255));
  avr_dlg^.Insert(Control);

  R.Assign(39, 12, 68, 13);					        // Projektname
  Control := New(PInputLine, Init(R, 38));
  avr_dlg^.Insert(Control);

  R.Assign(22, 18, 33, 19);				            // Frequenzeingabe
  Control := New(PInputLine, Init(R, 8));
  avr_dlg^.Insert(Control);


  R.Assign(57, 32, 69, 34);
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfNormal));
  avr_dlg^.Insert(Control);

  R.Assign(57, 29, 69, 31);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfNormal));
  avr_dlg^.Insert(Control);

  R.Assign(41, 18, 53, 20);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfNormal));
  avr_dlg^.Insert(Control);

  R.Assign(54, 18, 68, 20);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfNormal));
  avr_dlg^.Insert(Control);

  R.Assign(3, 20, 68, 21);
  Control := New(PStaticText, Init(R, '-------------------------------------'+
       '----------------------------'));
  avr_dlg^.Insert(Control);

  avr_dlg^.SelectNext(False);
  avr_dlg^.setdata(AvrRec);               // Einstellungen holen

  // die Liste mit AVR-Controllern der Listbox "laden"
  list := New(PStrCollection, Init(partanz_avr-1, 5));
  for i:= 0 to partanz_avr-1 do
  begin
    list^.AtInsert(i, NewStr(avrgccname[i]));
  end;
  lb^.Newlist(list);

  repeat
    lb^.focusitem(avrlistwahl);             // alten Focus der Listbox setzen
    c:= desktop^.execview(avr_dlg);         // kompletten Dialog anzeigen
    begin
      avr_dlg^.getdata(AvrRec);             // Einstellungen speichern
      avrlistwahl:= lb^.focused;
    end;

    // eine Datei zum Linken hinzufuegen
    if c= cmNo then
    begin
      chdir(avrpath+'/src');
      s:= openlinkfiles;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);

        lnkfiles[lnkanz]:= na;
        if (lnkanz= 0) then
          avr_lnkfiles[lnkanz]:= 'SRCS       = ../src/'+na+'.o'
        else
          avr_lnkfiles[lnkanz]:= 'SRCS      += ../src/'+na+'.o';
//        mymessage(avr_lnkfiles[lnkanz]);
        lines[linecount]:= newstr(avr_lnkfiles[lnkanz]);
        inc(lnkanz);
        linecount:= lnkanz;
        tgentxwin.draw;

        fsplit(s,sourcedir,na,ex);
      end;
      chdir(olddir);
    end;

    // Projekt und Makefile generieren
    if c=cmYes then
    begin

      if (avrrec.proj_dir= '') then
      begin
        mymessage('There is no projectdirectory given, cannot create a Makefile');
      end else
      begin

      // alten Inhalt des Anzeigefensters loeschen (dynamische Strings)
      if (lnkanz> 0) then
      begin
        for i:= 1 to lnkanz do
        begin
          disposestr(lines[i-1]);
        end;
        linecount:= 0;
        tgentxwin.draw;
      end;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      mklen:= write_avrmakefile(true);
      R.Assign(25, 1, 98, 28);
      genWindow := New(pgentxwin, Init(R, 'generated Makefile', 0));
      DeskTop^.Insert(genwindow);

      R.Assign(43,30,80,38);
      c2:= MessageBoxRect(r,'Generate a project with this makefile ?',nil,mfConfirmation or mfOkCancel);

      if (c2= 10) then
      begin
        mklen:= write_avrmakefile(false);
      end;
      for i:= 1 to mklen do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      R.Assign(1, 3, 43, 38);
      if (lnkanz> 0) then
      begin
        for i:= 1 to lnkanz do
        begin
          lines[i-1]:= newstr(avr_lnkfiles[i-1]);
        end;
        linecount:= lnkanz;
      end;

      genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
      DeskTop^.Insert(genwindow);

    end;
  end;

  if c= cmok then
  begin
    if (lnkanz> 0) then
    begin
      for i:= 1 to lnkanz do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;
      lnkanz:= 0;
      tgentxwin.draw;
    end;
  end;

  until (c= cmcancel);
  if (lnkanz> 0) then
  begin
    for i:= 1 to lnkanz do
    begin
      disposestr(lines[i-1]);
    end;
    linecount:= 0;
  end;
  tgentxwin.closewin(genwindow);
end;
