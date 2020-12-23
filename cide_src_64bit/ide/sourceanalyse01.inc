function make_abs_incfilename(sourcefile : string; incfile : string) : string;
{ erzeugt aus einem String (sourcefile) der einen Dateinamen INCLUSIVE absolutem
  Pfad darstellt einen absoluten Dateinamensstring fuer (incfile).

  Bsp.:
  sourcefile:  /home/avr_programs/demos/programm1.c
  incfile   : ../include/rs232_io.h

  Rueckgabe : /home/avr_programs/include/rs232_io.h

  Ist fuer incfile kein relativer Pfadangegeben, so ist der Rueckgabestring gleich
  dem incfile
}

type
  str80     = string[80];
  patharray = array[0..50] of str80;


var
  s, exttxt            : string;
  ch                   : char;
  targetzeil           : string;

  spathparts           : patharray;
  tpathparts           : patharray;

  ssdir, ssname, ssext : string;
  stdir, stname, stext : string;

  spanz                : integer;
  tpanz                : integer;
  incpos               : integer;
  i,steppdwn           : integer;

  function KillSpaces(s : string) : string;
  begin
    while ((s[1]< chr(32)) or (s[1]= ' ')) do
    begin
      delete(s,1,1);
    end;
    KillSpaces:= s;
  end;

  procedure splitpath(dname : string; var anz : integer; var pparts : patharray);
  var
    slashpos             : integer;
    i                    : integer;
    ssdir, ssname, ssext : string;

  begin

    for i:= 0 to 50 do
    begin
      pparts[i]:= '';
    end;

    anz:= 0;

    slashpos:= pos('/',dname);
    if (slashpos = 0) then
    begin
      exit;
    end
    else begin
      if (slashpos= 1) then
      begin
        delete(dname,1,1);
        slashpos:= pos('/',dname);
        pparts[anz]:= '/'+copy(dname,1,slashpos-1);
        delete(dname,1,slashpos);
        inc(anz);
      end;
      while ((pos('/',dname)> 0)) do
      begin
        slashpos:= pos('/',dname);
        pparts[anz]:= '/'+copy(dname,1,slashpos-1);
        delete(dname,1,slashpos);
        inc(anz);
      end;
    end;
  end;

begin

  targetzeil:= incfile;
  sourcefile:= killspaces(sourcefile);
  incfile:= killspaces(incfile);
  fsplit(sourcefile,ssdir,ssname,ssext);

  splitpath(ssdir,spanz,spathparts);

  incpos:= pos('#include ',incfile);

  if (incpos <> 1) then
  begin
    make_abs_incfilename:= targetzeil;                       // keine Include - Zeile
    exit;
  end;
  delete(incfile,1,9);
  incfile:= killspaces(incfile);
  if (incfile[1] <> '"') then
  begin
    make_abs_incfilename:= targetzeil;                      // keine Include - Datei
    exit;
  end;
  delete(incfile,1,1);
  incfile:= killspaces(incfile);
  incpos:= pos('"',incfile);
  exttxt:= copy(incfile,incpos+1,length(incfile)-incpos);
  delete(incfile,incpos,length(incfile)-incpos+1);

  fsplit(incfile,stdir,stname,stext);
  if ((pos('../',stdir) = 0)) then
  begin
    make_abs_incfilename:= targetzeil;                     // kein relativer Pfad
    exit;
  end;

  while (pos('../',stdir) = 1) do
  begin
    delete(stdir,1,3);
    if (spanz > 0) then dec(spanz);
  end;

  targetzeil:= '';
  if (spanz> 0) then
  begin
    for i:= 1 to spanz do
      targetzeil:= targetzeil+spathparts[i-1];
  end;
  targetzeil:= '#include "'+targetzeil+'/'+stdir+stname+stext+'"'+exttxt;
  make_abs_incfilename:= targetzeil;

end;

