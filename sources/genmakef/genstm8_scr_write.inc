function mydesk.write_stm8makefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;
  
  hs				  : string;
  addputchar          : boolean;
  stlinkname		  : string;

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
    write_stm8makefile:= 0;
    dname:= expandfilename(stm8path);
    dname:= dname + '/' + stm8rec.proj_dir;
    fname:= dname + '/' + stm8rec.proj_nam;
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
  addaline(tdat,'PROJECT       = '+stm8Rec.proj_nam);
  addaline(tdat,'');
  
  //  STM8S003F3,  STM8S103F3,  STM8S105K4,  STM8S105K6
  
  stlinkname:= '';
  hs:= stm8name[stm8listwahl];
  if (hs= 'STM8S003F3') then stlinkname:= 'stm8s003?3';
  if (hs= 'STM8S103F3') then stlinkname:= 'stm8s103f3';
  if (hs= 'STM8S105K4') then stlinkname:= 'stm8s105k4';
  if (hs= 'STM8S105K6') then stlinkname:= 'stm8s105k6';
    
  addaline(tdat,'MCU           = '+ stlinkname);   
  addaline(tdat,'');
  addaline(tdat,'# hier alle zusaetzlichen Softwaremodule angegeben');
  if (lnks8anz> 0) then
  begin
    for i:= 1 to lnks8anz do
    begin
      addaline(tdat,stm8_lnkfiles[i-1]);
    end;
  end;
  addaline(tdat,'');
  
  addaline(tdat,'');
  addaline(tdat,'INC_DIR       = -I./ -I../include');
  addaline(tdat,'');
  addaline(tdat,'# FLASHERPROG Auswahl fuer STM8:');
  addaline(tdat,'# 0 : stm8flash (ueber ST-Link Adapter)  1 : stm8_bootlash (Bootloader)');
  addaline(tdat,'');       
  addaline(tdat,'PROGPORT      = '+ serporttxt[stm8Rec.progport]);
  if ((stm8Rec.before_flash and $01)= $01) then
    addaline(tdat,'CH340RESET    = 1')
  else  
    addaline(tdat,'CH340RESET    = 0');
  str(stm8Rec.programmers, hs);
  addaline(tdat,'FLASHERPROG   = '+hs);
  addaline(tdat,'');       
  addaline(tdat,'');    
  
  addaline(tdat,'include ../makefile.mk');
  
  write_stm8makefile:= linecount;

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
    writeln(tdat, stm8_templ[0]);
    writeln(tdat, '     ' + stm8Rec.proj_nam + '.c');
    writeln(tdat, '');   
    writeln(tdat, '     MCU: ' + stm8name[stm8listwahl]);
    writeln(tdat, format('     %d.%d.%d',[dd,mm,yy]));
    writeln(tdat, '');
    for i:= 1 to 2 do
    begin
      writeln(tdat, stm8_templ[i]);
    end;
    
    // #include Header fuer zu linkende Files
    for i:= 1 to lnks8anz do
    begin
      writeln(tdat, '#include "' + lnks8files[i-1] + '.h"');
      if (lnks8files[i-1]= 'my_printf') or (lnks8files[i-1]= 'my_printf_float' ) then
        addputchar:= true;
    end;
    
    if addputchar= true then addsrc_putchar(tdat);
    
    
    // den "Rest" schreiben
    for i:= 3 to stm8_src_zeilanz do
    begin
      writeln(tdat, stm8_templ[i]);
    end;
    close(tdat);

  end;
end;


procedure mydesk.stm8_make_gen;
var
  stm8_dlg    : PDialog;
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
  if (lnks8anz> 0) then
  begin
    for i:= 1 to lnks8anz do
    begin
      lines[i-1]:= newstr(stm8_lnkfiles[i-1]);
    end;
    linecount:= lnks8anz;
  end;
  genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
  DeskTop^.Insert(genwindow);

  R.Assign(45, 3, 116, 38);
  New(stm8_dlg, Init(R, 'Makefile generator for STM8 MCU'));

  // die Devicelist muss als erstes angefuehrt werden, da ansonsten beim 64-Bit Compiler
  // aus mir nicht erklaerbaren Gruenden das Programm abstuerzt !!!!
  // Die dynamische Liste selbst muss am Ende der Fensterinhalte eingefuegt werden

  R.Assign(3, 6, 11, 7);
  Control := New(PStaticText, Init(R, 'Devices'));
  stm8_dlg^.Insert(Control);

  // Scrollbar der Deviceliste
  R.Assign(21, 8, 22, 19);
  scroll := New(PScrollBar, Init(R));
  stm8_dlg^.Insert(scroll);

  // die Deviceliste mit Scrollbar
  R.Assign(3, 8, 21, 19);
  lb := New(PListBox, Init(R, 1, PScrollbar(scroll)));
  stm8_dlg^.Insert(lb);

  R.Assign(3, 22, 14, 23);
  Control := New(PStaticText, Init(R, 'Programmer'));
  stm8_dlg^.Insert(Control);

  R.Assign(3, 24, 27, 33);
  Control := New(PRadioButtons, Init(R,
    NewSItem('ST-Link SWIM',
    NewSItem('Bootloader',
    NewSItem('reversed',
    NewSItem('reserved',
    NewSItem('reversed',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved',
    NewSItem('reserved', Nil)))))))))));
  stm8_dlg^.Insert(Control);

  R.Assign(29, 22, 49, 23);
  Control := New(PStaticText, Init(R, 'Programmer port'));
  stm8_dlg^.Insert(Control);

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
  stm8_dlg^.Insert(Control);

  R.Assign(42, 15, 67, 17);
  Control := New(PStaticText, Init(R, 'Additional source files'^M+
       'to link'));
  stm8_dlg^.Insert(Control);
  
  R.Assign(52, 22, 69, 24);
  Control := New(PStaticText, Init(R, 'before flashing'));
  stm8_dlg^.Insert(Control);

  R.Assign(52, 24, 69, 26);
  Control := New(PCheckboxes, Init(R,
    NewSItem('CH340-reset',Nil)));
  stm8_dlg^.Insert(Control);  

  R.Assign(3, 2, 33, 3);
  Control := New(PStaticText, Init(R, 'Path to STM8 projects'));
  stm8_dlg^.Insert(Control);
  R.Assign(3, 3, 69, 4);							// Projekte - Heimverzeichnis
  Control := New(PStaticText, Init(R, stm8path));
  stm8_dlg^.Insert(Control);

  R.Assign(39, 10, 66, 11);
  Control := New(PStaticText, Init(R, 'Project name'));
  stm8_dlg^.Insert(Control);
  R.Assign(39, 11, 66, 12);
  Control := New(PStaticText, Init(R, '(without .c extension)'));
  stm8_dlg^.Insert(Control);

  R.Assign(39, 7, 66, 8);
  Control := New(PStaticText, Init(R, 'Project directory'));
  stm8_dlg^.Insert(Control);

  R.Assign(39, 8, 68, 9);					        // Projektdirectory
  Control := New(PInputLine, Init(R, 255));
  stm8_dlg^.Insert(Control);

  R.Assign(39, 12, 68, 13);					        // Projektname
  Control := New(PInputLine, Init(R, 38));
  stm8_dlg^.Insert(Control);

  R.Assign(57, 32, 69, 34);
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfNormal));
  stm8_dlg^.Insert(Control);

  R.Assign(57, 29, 69, 31);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfNormal));
  stm8_dlg^.Insert(Control);

  R.Assign(41, 18, 53, 20);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfNormal));
  stm8_dlg^.Insert(Control);

  R.Assign(54, 18, 68, 20);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfNormal));
  stm8_dlg^.Insert(Control);

  R.Assign(3, 20, 68, 21);
  Control := New(PStaticText, Init(R, '-------------------------------------'+
       '----------------------------'));
  stm8_dlg^.Insert(Control);

  stm8_dlg^.SelectNext(False);
  stm8_dlg^.setdata(stm8rec);               // Einstellungen holen

  // die Liste mit STM8-Controllern der Listbox "laden"
  // muss leider hier am Ende des Fensters angegeben, weil, warum auch immer
  // unter einem 64-Bit System sich das System aufhaengt ! Warum ?????
  list := New(PStrCollection, Init(partanz_stm8-1, 5));
  for i:= 0 to partanz_stm8-1 do
  begin
    list^.AtInsert(i, NewStr(stm8name[i]));
  end;
  lb^.Newlist(list);

  repeat
    lb^.focusitem(stm8listwahl);             // alten Focus der Listbox setzen
    c:= desktop^.execview(stm8_dlg);         // kompletten Dialog anzeigen
    begin
      stm8_dlg^.getdata(stm8rec);           // Einstellungen speichern
      stm8listwahl:= lb^.focused;
    end;

    // eine Datei zum Linken hinzufuegen
    if c= cmNo then
    begin
      chdir(stm8path+'/src');
      s:= openlinkfiles;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);

        lnks8files[lnks8anz]:= na;
        if (lnks8anz= 0) then
          stm8_lnkfiles[lnks8anz]:= 'SRCS          = ../src/'+na+'.rel'
        else
          stm8_lnkfiles[lnks8anz]:= 'SRCS         += ../src/'+na+'.rel';
//        mymessage(stm8_lnkfiles[lnkanz]);
        lines[linecount]:= newstr(stm8_lnkfiles[lnks8anz]);
        inc(lnks8anz);
        linecount:= lnks8anz;
        tgentxwin.draw;

        fsplit(s,sourcedir,na,ex);
      end;
      chdir(olddir);
    end;

    // Projekt und Makefile generieren
    if c=cmYes then
    begin

      if (stm8rec.proj_dir= '') then
      begin
        mymessage('There is no projectdirectory given, cannot create a Makefile');
      end else
      begin

      // alten Inhalt des Anzeigefensters loeschen (dynamische Strings)
      if (lnks8anz> 0) then
      begin
        for i:= 1 to lnks8anz do
        begin
          disposestr(lines[i-1]);
        end;
        linecount:= 0;
        tgentxwin.draw;
      end;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      mklen:= write_stm8makefile(true);
      R.Assign(25, 1, 98, 28);
      genWindow := New(pgentxwin, Init(R, 'generated Makefile', 0));
      DeskTop^.Insert(genwindow);

      R.Assign(43,30,80,38);
      c2:= MessageBoxRect(r,'Generate a project with this makefile ?',nil,mfConfirmation or mfOkCancel);

      if (c2= 10) then
      begin
        mklen:= write_stm8makefile(false);
      end;
      for i:= 1 to mklen do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      R.Assign(1, 3, 43, 38);
      if (lnks8anz> 0) then
      begin
        for i:= 1 to lnks8anz do
        begin
          lines[i-1]:= newstr(stm8_lnkfiles[i-1]);
        end;
        linecount:= lnks8anz;
      end;

      genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
      DeskTop^.Insert(genwindow);

    end;
  end;

  if c= cmok then
  begin
    if (lnks8anz> 0) then
    begin
      for i:= 1 to lnks8anz do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;
      lnks8anz:= 0;
      tgentxwin.draw;
    end;
  end;

  until (c= cmcancel);
  if (lnks8anz> 0) then
  begin
    for i:= 1 to lnks8anz do
    begin
      disposestr(lines[i-1]);
    end;
    linecount:= 0;
  end;
  tgentxwin.closewin(genwindow);
end;
