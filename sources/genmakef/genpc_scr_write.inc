function mydesk.write_pcmakefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;

  ldstring            : string;
  hs				  : string;

  addputchar		  : boolean;

begin
  writetoscreen:= toscreen;
  addputchar:= false;

  if (not(toscreen)) then
  begin
    // Makefile in Datei schreiben
    write_pcmakefile:= 0;
    dname:= expandfilename(pcgccpath);
    dname:= dname + '/' + pcrec.proj_dir;
    fname:= dname + '/' + pcrec.proj_nam;
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
  addaline(tdat,'PROJECT       = '+pcRec.proj_nam);
  addaline(tdat,'');
  addaline(tdat,'# hier alle zusaetzlichen Softwaremodule angegeben');
  if (lnkpcanz> 0) then
  begin
    for i:= 1 to lnkpcanz do
    begin
      addaline(tdat,pc_lnkfiles[i-1]);
    end;
  end;
  addaline(tdat,'');

  ldstring:= '';
  addaline(tdat,'');
  addaline(tdat,'INCLUDE_PATHS = -I./ -I../include');
  addaline(tdat,'');
  
  if ((pcrec.linkselect and $01)> 0) then
    addaline(tdat,'MATH          = 1')
  else  
    addaline(tdat,'MATH          = 0');
    
  if ((pcrec.linkselect and $02)> 0) then
    addaline(tdat,'NCURSES       = 1')
  else  
    addaline(tdat,'NCURSES       = 0');
    
  if ((pcrec.linkselect and $04)> 0) then
    addaline(tdat,'SDL           = 1')
  else  
    addaline(tdat,'SDL           = 0');
    
  addaline(tdat,'');
  addaline(tdat,'ADD_LIBS        = '+pcrec.liblist);
  addaline(tdat,'');
  addaline(tdat,'');
  
  addaline(tdat,'include ../makefile.mk');
  
  write_pcmakefile:= linecount;
  
  
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
    writeln(tdat, pc_templ[0]);
    writeln(tdat, '     ' + PcRec.proj_nam + '.c');
    writeln(tdat, '');
    writeln(tdat, format('     %d.%d.%d',[dd,mm,yy]));
    writeln(tdat, '');
    for i:= 1 to 4 do
    begin
      writeln(tdat, pc_templ[i]);
    end;

    // #include Header fuer zu linkende Files
    for i:= 1 to lnkpcanz do
    begin
      writeln(tdat, '#include "' + lnkpcfiles[i-1] + '.h"');
      if (lnkpcfiles[i-1]= 'my_printf') or (lnkpcfiles[i-1]= 'my_printf_float' ) then
        addputchar:= true;
    end;

    if addputchar= true then addsrc_putchar(tdat);

    // den "Rest" schreiben
    for i:= 5 to pc_src_zeilanz do
    begin
      writeln(tdat, pc_templ[i]);
    end;
    close(tdat);

  end;
end;



procedure mydesk.pc_make_gen;
var
  pc_dlg      : PDialog2;
  R           : TRect;
  Control     : PView;
  i			  : integer;
  c, c2       : word;
  mklen		  : word;

  sourcedir,
  olddir, na, ex, s   : string;

begin
  olddir:= getcurrentdir;
  R.Assign(1, 3, 43, 38);
  if (lnkpcanz> 0) then
  begin
    for i:= 1 to lnkpcanz do
    begin
      lines[i-1]:= newstr(pc_lnkfiles[i-1]);
    end;
    linecount:= lnkpcanz;
  end;
  genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));     // externes Anzeigefenster
  DeskTop^.Insert(genwindow);

  R.Assign(45, 3, 116, 28);
  New(pc_dlg, Init(R, 'Makefile generator for Linux console programs'));   // Dialogtitel
  
  R.Assign(3, 2, 33, 3);
  Control := New(PStaticText, Init(R, 'Path to Linux console projects'));
  pc_dlg^.Insert(Control);
  R.Assign(3, 3, 69, 4);							// Projekte - Heimverzeichnis
  Control := New(PStaticText, Init(R, pcgccpath));
  pc_dlg^.Insert(Control);

  R.Assign(3, 11, 30, 12);
  Control := New(PStaticText, Init(R, 'Add libraries'));
  pc_dlg^.Insert(Control);

  R.Assign(3, 12, 17, 15);
  Control := New(PCheckBoxes, Init(R,
    NewSItem('MATH',
    NewSItem('NCURSES',
    NewSItem('SDL', Nil)))));
  pc_dlg^.Insert(Control);  

  R.Assign(35, 11, 60, 13);
  Control := New(PStaticText, Init(R, 'Additional source files'^M+
       'to compile and link'));
  pc_dlg^.Insert(Control); 

  R.Assign(35, 14, 48, 16);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfNormal));
  pc_dlg^.Insert(Control);

  R.Assign(51, 14, 65, 16);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfNormal));
  pc_dlg^.Insert(Control);   
  
  R.Assign(3, 5, 30, 6);
  Control := New(PStaticText, Init(R, 'Project directory'));
  pc_dlg^.Insert(Control);

  R.Assign(3, 6, 32, 7);					        // Projektdirectory
  Control := New(PInputLine, Init(R, 255));
  pc_dlg^.Insert(Control);

  R.Assign(3, 8, 34, 9);
  Control := New(PStaticText, Init(R, 'Project name (without .c)'));
  pc_dlg^.Insert(Control);
  
  R.Assign(3, 9, 32, 10);					        // Projektname
  Control := New(PInputLine, Init(R, 38));
  pc_dlg^.Insert(Control);    
  
  R.Assign(3, 17, 60, 18);
  Control := New(PStaticText, Init(R, 'Enter additional libraries to link manually'));
  pc_dlg^.Insert(Control); 
  
  R.Assign(3, 18, 65, 19);					        // zusaetzliche Linkangaben
  Control := New(PInputLine, Init(R, 38));
  pc_dlg^.Insert(Control);    
  
  R.Assign(3, 20, 68, 21);
  Control := New(PStaticText, Init(R, '-------------------------------------'+
       '----------------------------'));
  pc_dlg^.Insert(Control);

  R.Assign(35, 22, 48, 24);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfNormal));
  pc_dlg^.Insert(Control);      
       
  R.Assign(55, 22, 65, 24);
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfNormal));
  pc_dlg^.Insert(Control);

  pc_dlg^.SelectNext(False);
  pc_dlg^.setdata(PcRec);               // Einstellungen holen

  repeat
    c:= desktop^.execview(pc_dlg);      // kompletten Dialog anzeigen und Buttonwert mit dem
										// der Dialog beendet wurde nach c
    
    pc_dlg^.getdata(PcRec);             // Einstellungen speichern
    
    // eine Datei zum Linken hinzufuegen
    if c= cmNo then
    begin
      chdir(pcgccpath+'/src');
      s:= openlinkfiles;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);

        lnkpcfiles[lnkpcanz]:= na;
        if (lnkpcanz= 0) then
          pc_lnkfiles[lnkpcanz]:= 'SRCS          = ../src/'+na+'.o'
        else
          pc_lnkfiles[lnkpcanz]:= 'SRCS         += ../src/'+na+'.o';
//        mymessage(pc_lnkfiles[lnkanz]);
        lines[linecount]:= newstr(pc_lnkfiles[lnkpcanz]);
        inc(lnkpcanz);
        linecount:= lnkpcanz;
        tgentxwin.draw;

        fsplit(s,sourcedir,na,ex);
      end;
      chdir(olddir);
    end;

    // Projekt und Makefile generieren
    if c=cmYes then
    begin

      if (PcRec.proj_dir= '') then
      begin
        mymessage('There is no projectdirectory given, cannot create a Makefile');
      end else
      begin

      // alten Inhalt des Anzeigefensters loeschen (dynamische Strings)
      if (lnkpcanz> 0) then
      begin
        for i:= 1 to lnkpcanz do
        begin
          disposestr(lines[i-1]);
        end;
        linecount:= 0;
        tgentxwin.draw;
      end;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      mklen:= write_pcmakefile(true);
      R.Assign(25, 1, 98, 28);
      genWindow := New(pgentxwin, Init(R, 'generated Makefile', 0));
      DeskTop^.Insert(genwindow);

      R.Assign(43,30,80,38);
      c2:= MessageBoxRect(r,'Generate a project with this makefile ?',nil,mfConfirmation or mfOkCancel);

      if (c2= 10) then
      begin
        mklen:= write_pcmakefile(false);
      end;
      for i:= 1 to mklen do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;

      tgentxwin.closewin(genwindow);               // und Fenster entfernen

      R.Assign(1, 3, 43, 38);
      if (lnkpcanz> 0) then
      begin
        for i:= 1 to lnkpcanz do
        begin
          lines[i-1]:= newstr(pc_lnkfiles[i-1]);
        end;
        linecount:= lnkpcanz;
      end;

      genWindow := New(pgentxwin, Init(R, 'Additional files to link', 0));
      DeskTop^.Insert(genwindow);

    end;
  end;

  if c= cmok then
  begin
    if (lnkpcanz> 0) then
    begin
      for i:= 1 to lnkpcanz do
      begin
        disposestr(lines[i-1]);
      end;
      linecount:= 0;
      lnkpcanz:= 0;
      tgentxwin.draw;
    end;
  end;

  until (c= cmcancel);
  if (lnkpcanz> 0) then
  begin
    for i:= 1 to lnkpcanz do
    begin
      disposestr(lines[i-1]);
    end;
    linecount:= 0;
  end;
  tgentxwin.closewin(genwindow);
end;
