function mydesk.write_mcs51makefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;
  
  hs				  : string;
  addputchar          : boolean;

const
  serporttxt : array[0..8] of string =
    ('', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2',
     '/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2',
     '/dev/ttyS0', '/dev/ttyS1');

begin
  writetoscreen:= toscreen;
  addputchar:= false;

  if (not(toscreen)) then
  begin
    // Makefile in Datei schreiben
    write_mcs51makefile:= 0;
    dname:= expandfilename(mcs51path);
    dname:= dname + '/' + mcsRec.proj_dir;
    fname:= dname + '/' + mcsRec.proj_nam;
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
  addaline(tdat,'PROJECT       = '+mcsRec.proj_nam);
  addaline(tdat,'');
  addaline(tdat,'MCU           = '+ mcsname[mcslistwahl]);  
  addaline(tdat,'');
  addaline(tdat,'# hier alle zusaetzlichen Softwaremodule angegeben');
  if (lnkmcsanz> 0) then
  begin
    for i:= 1 to lnkmcsanz do
    begin
      addaline(tdat,mcs_lnkfiles[i-1]);
    end;
  end;
  addaline(tdat,'');
  
  addaline(tdat,'');
  addaline(tdat,'INC_DIR       = -I./ -I../include');
  addaline(tdat,'');
  addaline(tdat,'XTAL          = '+ mcsRec.freq_str);
   
  addaline(tdat,'');       
  addaline(tdat,'# FLASHERPROG Auswahl fuer MCS51:');
  addaline(tdat,'# 0 : FLASH51 (propietaerer SPI Programmer ohne Wartezeit beim Start)');
  addaline(tdat,'# 1 : FLASH51 (mit Wartezeit = Arduino als Programmer)');
  addaline(tdat,'# 2 : DFU-PROGRAMMER');
  addaline(tdat,'');       
  addaline(tdat,'PROGPORT      = '+ serporttxt[mcsRec.progport]);
  if ((mcsRec.before_flash and $01)= $01) then
    addaline(tdat,'CH340RESET    = 1')
  else  
    addaline(tdat,'CH340RESET    = 0');
  str(mcsRec.programmers, hs);
  addaline(tdat,'FLASHERPROG   = '+hs);
  addaline(tdat,'');       
  addaline(tdat,'');    
  
  addaline(tdat,'include ../makefile.mk');
  
  write_mcs51makefile:= linecount;

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
    writeln(tdat, mcs51_templ[0]);
    writeln(tdat, '     ' + mcsRec.proj_nam + '.c');
    writeln(tdat, '');
    writeln(tdat, format('     %d.%d.%d',[dd,mm,yy]));
    writeln(tdat, '');
    for i:= 1 to 3 do
    begin
      writeln(tdat, mcs51_templ[i]);
    end;
    
    // #include Header fuer zu linkende Files
    for i:= 1 to lnkmcsanz do
    begin
      writeln(tdat, '#include "' + lnkmcsfiles[i-1] + '.h"');
      if (lnkmcsfiles[i-1]= 'my_printf') or (lnkmcsfiles[i-1]= 'my_printf_float' ) then
        addputchar:= true;
    end;
    
    if addputchar= true then addsrc_putchar(tdat);

    // den "Rest" schreiben
    for i:= 4 to mcs51_src_zeilanz do
    begin
      writeln(tdat, mcs51_templ[i]);
    end;
    close(tdat);

  end;
end;


procedure mydesk.mcs51_make_gen;
var
  mcs_dlg     : PDialog2;
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
  if (lnkmcsanz> 0) then
  begin
    for i:= 1 to lnkmcsanz do
    begin
      lines[i-1]:= newstr(mcs_lnkfiles[i-1]);
    end;
    linecount:= lnkmcsanz;
  end;
  genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
  DeskTop^.Insert(genwindow);

  R.Assign(45, 3, 116, 38);
  New(mcs_dlg, Init(R, 'Makefile generator for MCS51 MCU'));

  // die Devicelist muss als erstes angefuehrt werden, da ansonsten beim 64-Bit Compiler
  // aus mir nicht erklaerbaren Gruenden das Programm abstuerzt !!!!
  // Die dynamische Liste selbst muss am Ende der Fensterinhalte eingefuegt werden

  R.Assign(3, 6, 11, 7);
  Control := New(PStaticText, Init(R, 'Devices'));
  mcs_dlg^.Insert(Control);

  // Scrollbar der Deviceliste
  R.Assign(18, 8, 19, 19);
  scroll := New(PScrollBar, Init(R));
  mcs_dlg^.Insert(scroll);

  // die Deviceliste mit Scrollbar
  R.Assign(3, 8, 18, 19);
  lb := New(PListBox, Init(R, 1, PScrollbar(scroll)));
  mcs_dlg^.Insert(lb);

  R.Assign(3, 22, 14, 23);
  Control := New(PStaticText, Init(R, 'Programmer'));
  mcs_dlg^.Insert(Control);

  R.Assign(3, 24, 27, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('FLASH51 (nowait)',
    NewSItem('FLASH51 (wait)',
    NewSItem('DFU-PROGRAMMER',
    NewSItem('reserved',
    NewSItem('reversed',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved', Nil)))))))))));
  mcs_dlg^.Insert(Control);

  R.Assign(29, 22, 49, 23);
  Control := New(PStaticText, Init(R, 'Programmer port'));
  mcs_dlg^.Insert(Control);

  R.Assign(29, 24, 49, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('USB (DFU-Programmer)',
    NewSItem('/dev/ttyUSB0',
    NewSItem('/dev/ttyUSB1',
    NewSItem('/dev/ttyUSB2',
    NewSItem('/dev/ttyACM0',
    NewSItem('/dev/ttyACM1',
    NewSItem('/dev/ttyACM2',
    NewSItem('/dev/ttyS0',
    NewSItem('/dev/ttyS1', Nil)))))))))));
  mcs_dlg^.Insert(Control);

  R.Assign(42, 15, 67, 17);
  Control := New(PStaticText, Init(R, 'Additional source files'^M+
       'to link'));
  mcs_dlg^.Insert(Control);
  
  R.Assign(52, 22, 69, 24);
  Control := New(PStaticText, Init(R, 'before flashing'));
  mcs_dlg^.Insert(Control);

  R.Assign(52, 24, 69, 26);
  Control := New(PCheckboxes, Init(R,
    NewSItem('CH340-reset',Nil)));
  mcs_dlg^.Insert(Control);  

  R.Assign(3, 2, 33, 3);
  Control := New(PStaticText, Init(R, 'Path to MCS51 projects'));
  mcs_dlg^.Insert(Control);
  R.Assign(3, 3, 69, 4);							// Projekte - Heimverzeichnis
  Control := New(PStaticText, Init(R, mcs51path));
  mcs_dlg^.Insert(Control);
  
  R.Assign(22, 17, 41, 18);
  Control := New(PStaticText, Init(R, 'crystal frequency'));
  mcs_dlg^.Insert(Control);
  R.Assign(34, 18, 39, 19);
  Control := New(PStaticText, Init(R, 'Hz'));
  mcs_dlg^.Insert(Control);
  

  R.Assign(39, 10, 66, 11);
  Control := New(PStaticText, Init(R, 'Project name'));
  mcs_dlg^.Insert(Control);
  R.Assign(39, 11, 66, 12);
  Control := New(PStaticText, Init(R, '(without .c extension)'));
  mcs_dlg^.Insert(Control);

  R.Assign(39, 7, 66, 8);
  Control := New(PStaticText, Init(R, 'Project directory'));
  mcs_dlg^.Insert(Control);

  R.Assign(39, 8, 68, 9);					        // Projektdirectory
  Control := New(PInputLine, Init(R, 255));
  mcs_dlg^.Insert(Control);

  R.Assign(39, 12, 68, 13);					        // Projektname
  Control := New(PInputLine, Init(R, 38));
  mcs_dlg^.Insert(Control);
  
  R.Assign(22, 18, 33, 19);				            // Frequenzeingabe
  Control := New(PInputLine, Init(R, 8));
  mcs_dlg^.Insert(Control);  

  R.Assign(57, 32, 69, 34);
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfNormal));
  mcs_dlg^.Insert(Control);

  R.Assign(57, 29, 69, 31);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfNormal));
  mcs_dlg^.Insert(Control);

  R.Assign(41, 18, 53, 20);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfNormal));
  mcs_dlg^.Insert(Control);

  R.Assign(54, 18, 68, 20);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfNormal));
  mcs_dlg^.Insert(Control);

  R.Assign(3, 20, 68, 21);
  Control := New(PStaticText, Init(R, '-------------------------------------'+
       '----------------------------'));
  mcs_dlg^.Insert(Control);

  mcs_dlg^.SelectNext(False);
  mcs_dlg^.setdata(mcsRec);               // Einstellungen holen

  // die Liste mit MCS-51-Controllern der Listbox "laden"
  // muss leider hier am Ende des Fensters angegeben, weil, warum auch immer
  // unter einem 64-Bit System sich das System aufhaengt ! Warum ?????
  list := New(PStrCollection, Init(partanz_mcs-1, 5));
  for i:= 0 to partanz_mcs-1 do
  begin
    list^.AtInsert(i, NewStr(mcsname[i]));
  end;
  lb^.Newlist(list);

  repeat
    lb^.focusitem(mcslistwahl);             // alten Focus der Listbox setzen
    c:= desktop^.execview(mcs_dlg);         // kompletten Dialog anzeigen
    begin
      mcs_dlg^.getdata(mcsRec);           // Einstellungen speichern
      mcslistwahl:= lb^.focused;
    end;

    // eine Datei zum Linken hinzufuegen
    if c= cmNo then
    begin
      chdir(mcs51path+'/src');
      s:= openlinkfiles;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);

        lnkmcsfiles[lnkmcsanz]:= na;
        if (lnkmcsanz= 0) then
          mcs_lnkfiles[lnkmcsanz]:= 'SRCS          = ../src/'+na+'.rel'
        else
          mcs_lnkfiles[lnkmcsanz]:= 'SRCS         += ../src/'+na+'.rel';
//        mymessage(mcs_lnkfiles[lnkanz]);
        lines[linecount]:= newstr(mcs_lnkfiles[lnkmcsanz]);
        inc(lnkmcsanz);
        linecount:= lnkmcsanz;
        tgentxwin.draw;

        fsplit(s,sourcedir,na,ex);
      end;
      chdir(olddir);
    end;

    // Projekt und Makefile generieren
    if c=cmYes then
    begin

      if (mcsRec.proj_dir= '') then
      begin
        mymessage('There is no projectdirectory given, cannot create a Makefile');
      end else
      begin

      // alten Inhalt des Anzeigefensters loeschen (dynamische Strings)
      if (lnkmcsanz> 0) then
      begin
        for i:= 1 to lnkmcsanz do
        begin
          disposestr(lines[i-1]);
        end;
        linecount:= 0;
        tgentxwin.draw;
      end;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      mklen:= write_mcs51makefile(true);
      R.Assign(25, 1, 98, 28);
      genWindow := New(pgentxwin, Init(R, 'generated Makefile', 0));
      DeskTop^.Insert(genwindow);

      R.Assign(43,30,80,38);
      c2:= MessageBoxRect(r,'Generate a project with this makefile ?',nil,mfConfirmation or mfOkCancel);

      if (c2= 10) then
      begin
        mklen:= write_mcs51makefile(false);
      end;
      for i:= 1 to mklen do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      R.Assign(1, 3, 43, 38);
      if (lnkmcsanz> 0) then
      begin
        for i:= 1 to lnkmcsanz do
        begin
          lines[i-1]:= newstr(mcs_lnkfiles[i-1]);
        end;
        linecount:= lnkmcsanz;
      end;

      genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
      DeskTop^.Insert(genwindow);

    end;
  end;

  if c= cmok then
  begin
    if (lnkmcsanz> 0) then
    begin
      for i:= 1 to lnkmcsanz do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;
      lnkmcsanz:= 0;
      tgentxwin.draw;
    end;
  end;

  until (c= cmcancel);
  if (lnkmcsanz> 0) then
  begin
    for i:= 1 to lnkmcsanz do
    begin
      disposestr(lines[i-1]);
    end;
    linecount:= 0;
  end;
  tgentxwin.closewin(genwindow);
end;
