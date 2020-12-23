procedure TIDEApp.callwriteex(cname : string);
var
  anz, w          : word;
  scall1, scall2,
  scall3, scall4  : string;
  s2,hs           : string;
  rawname, oname,
  mname, datnam   : string;
  exitcode        : word;
  myprocess       : tprocess;

begin
  if (cname='') then exit;
  s2:= copy(cname,length(cname)-1,2);
  if (s2<> '.c') then
  begin
    MessageBox(#3'Wrong file or filename'#13 +
               #3'File-Extension must be .c',
               nil, mfInformation or mfOKButton);
    exit;
  end;
  rawname:= copy(cname,1,length(cname)-1);
  oname:= rawname+'o';
  datnam:= getrawfilename(cname);
  datnam:= copy(datnam,1,length(datnam)-1);

  mname:= avrgccname[mculistwahl];
  
//    avr-gcc palettentest.c -c  -Os -o palettentest.o -mmcu=atmega328p -DF_CPU=16000000ul
    scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;

  if ((mname= 'at89-mcs51') or (mname= 'at89c5131')) then     
  begin
    scall1:= 'sdcc -mmcs51 --disable-warning 197 --disable-warning 84 -c ' + cname;
  end;    

  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,scall1,'',1,1);
  docompilermessages;

end;


procedure TIDEApp.writeex;
var
  anz, w          : word;
  scall1, scall2,
  scall3, scall4  : string;
  s2,hs,s         : string;
  cname, rawname,
  oname, rawnamel,
  oname2,
  mname, datnam   : string;
  exitcode        : word;
  olddir          : string;
  sourcedir,na,ex,
  odir,tmpnam     : string;
  odirorg         : string;
  tmpnam2		  : string;
  linkanz         : byte;

  myprocess       : tprocess;

begin
  olddir:= getcurrentdir;
  mysave;
  cname:= getmainfile(ccompile);
  fsplit(cname,sourcedir,na,ex);
  if (cname='') then exit;
  chdir(sourcedir);
  s2:= copy(cname,length(cname)-1,2);
  if (s2<> '.c') then
  begin
    MessageBox(#3'Wrong file or filename'#13 +
               #3'File-Extension must be .c',
               nil, mfInformation or mfOKButton);
    exit;
  end;
  compilermessagewindow^.clearmessages;
  rawname:= copy(cname,1,length(cname)-1);
  oname:= rawname+'o';
  oname2:= oname;
  
  mname:= avrgccname[mculistwahl];     
  
  if ((mname= 'at89-mcs51') or (mname= 'at89c5131')) then   
  begin
    oname2:= cname;
  end;   

  linkanz:= 0;
  if (linkersettings.additional= 1) then
  begin
    s2:= linkersettings.lfilename;
    repeat
	  s:= splitsemistring(s2);
	  if (s<> '') then
	  begin
	    if iscfile(s) then avrcallcompile(s);
	    if (gexitcode<> 0) then
	    begin
          compilermessagewindow^.addmessage(0,'Compiling error, finished..','',1,1);
          docompilermessages;	    	     
          chdir(olddir);
	      exit;
	    end;	    
	    inc(linkanz);
	
	    fsplit(s,odir,na,ex);
	    odir:= odir+na+'.o';
	    str(linkanz,tmpnam);
	    tmpnam2:= 'avrtmp'+tmpnam+'.o'; 	    
	    tmpnam:= sourcedir+'avrtmp'+tmpnam+'.o';	    	    
	    
        if ((mname= 'at89-mcs51') or (mname= 'at89c5131')) then 
        begin
	      fsplit(s,odirorg,na,ex);	      
	      odir:= odirorg+na+'.lst';
	      str(linkanz,tmpnam);    
	      tmpnam2:= 'avrtmp'+tmpnam+'.lst';    
 	      tmpnam:= sourcedir+'avrtmp'+tmpnam+'.lst';	  
	      callcmd('cp '+odir+' '+tmpnam,0);	
 	       	      	      
	      odir:= odirorg+na+'.rel';
	      str(linkanz,tmpnam);    
	      tmpnam2:= 'avrtmp'+tmpnam+'.rel';    
 	      tmpnam:= sourcedir+'avrtmp'+tmpnam+'.rel';	    	     	      
        end;   	    
	
	    callcmd('cp '+odir+' '+tmpnam,0);	
	
	    if (gexitcode<> 0) then
	    begin
          chdir(olddir);
	      exit;
	    end;
//        rawnamel:= copy(s,1,length(s)-1);
//        oname2:= oname2+' '+rawnamel+'o';
        oname2:= oname2+' '+tmpnam2;
	  end;
	until (s= '');
  end;
  
  if ((mname= 'at89-mcs51') or (mname= 'at89c5131')) then   
  begin 
    compilermessagewindow^.addmessage(0,'--------','',1,1);
    compilermessagewindow^.addmessage(0,'sdcc -mmcs51 --disable-warning 197 --disable-warning 84'+cname+' opt. modules','',1,1);
    docompilermessages;	   
    callcmd('sdcc -mmcs51 --disable-warning 197 --disable-warning 84 '+oname2,0);	    
    if (gexitcode<> 0) then
    begin
      compilermessagewindow^.addmessage(0,'Compiling error...','',1,1);
      docompilermessages;	    	     
      chdir(olddir);
      exit;
    end;	   
    compilermessagewindow^.addmessage(0,'Compilation finished successfully','',1,1);
    compilermessagewindow^.addmessage(0,'--------','',1,1);    
    docompilermessages;	         
//    callcmd('cat '+rawname+'mem',1);
    callcmd('grep -i --after-context=7 "starts" '+rawname+'mem',1);
    callcmd('rm '+rawname+'lst',1);
    callcmd('rm '+rawname+'cdb',1);
    callcmd('rm '+rawname+'lk',1);
    callcmd('rm '+rawname+'map',1);
    callcmd('rm '+rawname+'omf',1);
    callcmd('rm '+rawname+'rel',1);
    callcmd('rm '+rawname+'rst',1);
    callcmd('rm '+rawname+'sym',1);
//    callcmd('rm '+rawname+'mem',1);
    callcmd('rm '+rawname+'asm',1);    
    
    callcmd('rm *.lst',1);
    callcmd('rm *.cdb',1);
    callcmd('rm *.lk',1);
    callcmd('rm *.map',1);
    callcmd('rm *.omf',1);
    callcmd('rm *.rel',1);
    callcmd('rm *.rst',1);
    callcmd('rm *.sym',1);
//    callcmd('rm *.mem',1);
    callcmd('rm *.asm',1);    

    compilermessagewindow^.addmessage(0,'SDCC compilation finished, thank you','',1,1);      
    docompilermessages;	    	     
    chdir(olddir);
    exit;
  end;  

  datnam:= getrawfilename(cname);
  datnam:= copy(datnam,1,length(datnam)-1);

  case linkersettings.standard of
    0 : begin
          scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall2:= 'avr-gcc '+oname2+' -o '+rawname+'elf -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall3:= 'avr-objcopy -j .text -j .data -O ihex '+rawname+'elf '+rawname+'hex';
          scall4:= 'avr-size -C '+rawname+'elf --mcu='+mname;
//          scall4:= 'avr-size -d '+rawname+'elf';
          hs:= ('Compile                : '+datnam+'c   Target-Controller: '+mname);
        end;
    1 : begin
{          scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall2:= 'avr-gcc -mmcu='+mname+' -Wl,-Map='+rawname+'map '+oname2+' -lprintf_flt -lm -o '+rawname+'elf'+' -DF_CPU='+avrfreq;
          scall3:= 'avr-objcopy -j .text -j .data -O ihex '+rawname+'elf '+rawname+'hex';
          scall4:= 'avr-size -C '+rawname+'elf --mcu='+mname; }
          scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall2:= 'avr-gcc -mmcu='+mname+' '+oname2+' -lprintf_flt -lm -o '+rawname+'elf'+' -DF_CPU='+avrfreq;
          scall3:= 'avr-objcopy -j .text -j .data -O ihex '+rawname+'elf '+rawname+'hex';
          scall4:= 'avr-size -C '+rawname+'elf --mcu='+mname;
          hs:= ('Compile                : '+datnam+'c   Target-Controller: '+mname);
        end;
    2 : begin
{          scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall2:= 'avr-gcc -mmcu=' +mname+' -Wl,-u,vfprintf -Wl,-Map='+rawname+'map '+oname2;
          scall2:= scall2+' -lprintf_flt -lm -o '+rawname+'elf'+' -DF_CPU='+avrfreq;
          scall3:= 'avr-objcopy -j .text -j .data -O ihex '+rawname+'elf '+rawname+'hex';
          scall4:= 'avr-size -C '+rawname+'elf --mcu='+mname; }
          scall1:= 'avr-gcc '+cname+' -c  -o '+oname+' -Os -mmcu='+mname+' -DF_CPU='+avrfreq;
          scall2:= 'avr-gcc -mmcu=' +mname+' -Wl,-u,vfprintf '+oname2;
          scall2:= scall2+' -lprintf_flt -lm -o '+rawname+'elf'+' -DF_CPU='+avrfreq;
          scall3:= 'avr-objcopy -j .text -j .data -O ihex '+rawname+'elf '+rawname+'hex';
          scall4:= 'avr-size -C '+rawname+'elf --mcu='+mname;
          hs:= ('Compile                : '+datnam+'c   Target-Controller: '+mname);
        end;
    3 : begin
          hs:= ('Option 3');
        end;
  end;

  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,hs,'',1,1);
//  compilermessagewindow^.addmessage(0,scall1,'',1,1);
  docompilermessages;

  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall1;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];

  // -----------------------------------------------------------------------
  //               Den Compiler aufrufen !!!! (o - Code erzeugen)
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.stderr);
  anz:= mystringlist.count;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      compilermessagewindow^.addmessage(0,hs,'',1,1);
    end;
  end;

  if (exitcode<> 0) then
  begin
    chdir(olddir);
    exit;
  end;

//  compilermessagewindow^.addmessage(0,scall2,'',1,1);
  hs:= ('Link to .elf file from : '+datnam+'o');

  if (linkersettings.additional= 1) then
  begin
    s2:= linkersettings.lfilename;
    repeat
	  s:= splitsemistring(s2);
	  if (s<> '') then
	  begin
	
	    fsplit(s,odir,na,ex);
	    hs:= hs+'; '+na+'.o';
	  end;
	until (s= '');
  end;

  compilermessagewindow^.addmessage(0,hs,'',1,1);

  myprocess.commandline:= scall2;                   // SCALL2 fuer die elf-Dateien
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  //               Den Compiler aufrufen !!!! (elf - Code erzeugen)
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.stderr);
  anz:= mystringlist.count;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      compilermessagewindow^.addmessage(0,hs,'',1,1);
    end;
  end;

  if (linkersettings.additional= 1) then
  begin
    for w:= 1 to linkanz do
    begin
      str(w,tmpnam);
	  tmpnam2:= 'avrtmp'+tmpnam+'.o';
      callcmd('rm '+tmpnam2,0);
    end;
  end;

  if (exitcode<> 0) then
  begin
    compilermessagewindow^.addmessage(0,'Linker error','',1,1);
    chdir(olddir);
    exit;
  end;

  hs:= ('Generate .hex file from: '+datnam+'elf');
  compilermessagewindow^.addmessage(0,hs,'',1,1);

  myprocess.commandline:= scall3;                   // SCALL3 fuer die hex-Dateien
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  //               Den Compiler aufrufen !!!! (hex - Code erzeugen)
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.stderr);
  anz:= mystringlist.count;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      compilermessagewindow^.addmessage(0,hs,'',1,1);
    end;
  end;
  if (gexitcode<> 0) then
  begin
    chdir(olddir);
    exit;
  end;

  hs:= ('----------------');
  compilermessagewindow^.addmessage(0,hs,'',1,1);

  myprocess.commandline:= scall4;                   // SCALL4 fuer die Zusammenfassung
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  //          Den Compiler aufrufen !!!! (Zusammenfassung anzeigen)
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      if (hs<>'') then compilermessagewindow^.addmessage(0,hs,'',1,1);
    end;
  end;
  if (gexitcode<> 0) then
  begin
    chdir(olddir);
    exit;
  end;

  hs:= 'Compilation finished successfully';
  compilermessagewindow^.addmessage(0,hs,'',1,1);
  chdir(olddir);
end;
