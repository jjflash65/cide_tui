{ ---------------------------------------------------------
                    utils.inc
                    
   eigenstaendige, kleine Hilsfunktionen und - prozeduren

  14.08.2018    R. Seelig
  --------------------------------------------------------- }

{ ---------------------------------------------------------
                         congotoxy
         positioniert den Textcursor in der Console
  --------------------------------------------------------- }
procedure congotoxy (x, y : byte);
begin
  write(#27,'[',y,';',x,'H');
end;

{ ---------------------------------------------------------
                         conclrscr
               loescht den Consoleninhalt
  --------------------------------------------------------- }
procedure conclrscr;
var
  i,i2 : integer;
begin
  congotoxy(1,1);
  for i:= 1 to 60 do
  begin
    for i2:= 1 to 150 do
    begin
      write(' ');
    end;
    writeln;
  end;
  congotoxy(1,1);
end;

{ ---------------------------------------------------------
                         Dez2Hex
       konvertiert einen Dezimalwert in einen String
  --------------------------------------------------------- }
function Dez2Hex(hz : longint) : string;
var
  s    : string;
  l    : longint;
  b    : byte;
  c    : char;
begin
  s:= '';
  repeat
    l:= hz shr 4; b:= hz- (l shl 4); hz:= l;
    if b< 10 then c:= chr(b+48) else c:= chr(b+55);
    s:= concat(c,s);
  until (hz= 0) and (l= 0);
  Dez2Hex:= s;
end;

{ ---------------------------------------------------------
                         Upper
       konvertiert die in einem String enthaltenen
       Buchstaben zu Grossbuchstaben
  --------------------------------------------------------- }
function upper(const s : string) : string;
var
  i  : word;
begin
  for i:=1 to length(s) do
   if s[i] in ['a'..'z'] then
    upper[i]:=char(byte(s[i])-32)
   else
    upper[i]:=s[i];
  upper[0]:=s[0];
end;

{ ---------------------------------------------------------
                      replace_utfstring
    ersetzt UTF - Zeichen durch 7-Bit ASCII Zeichen
  --------------------------------------------------------- }
procedure replace_utfstring(var s : string);
const
  utfanz = 2;
var
  utfstr   : array[0..utfanz-1] of string =
            (chr(226)+chr(128)+chr(152),
             chr(226)+chr(128)+chr(153) );
  utfrep   : array[0..utfanz-1] of string =
            (chr(39),
             chr(39));

  i   : integer;
  b,l : byte;
begin
  for i:= 0 to utfanz-1 do
  begin
    repeat
      b:= pos(utfstr[i],s);
      if b <> 0 then
      begin
        l:= length(utfstr[i]);
        delete(s,b,l);
        l:= length(utfrep[i]);
        insert(utfrep[i],s,b);
      end;
    until (b= 0);
  end;
end;

{ ---------------------------------------------------------
                      ExistFile
        testet, ob eine Datei vorhanden ist
  --------------------------------------------------------- }
function ExistFile(fname : string) : boolean;
var
  fdat   : file;
  result : boolean;
begin
  result:= false;
  assign(fdat, fname);
  {$i-} reset(fdat); {$i+}
  if (ioresult= 0) then result:= true;
  close(fdat);
  ExistFile:= result;
end;

{ ---------------------------------------------------------
                         myMessage
              erzeugt ein Ausgabefenster

    Parameter:
       s  : String, der im Fenster angezeigt werden soll
  --------------------------------------------------------- }
procedure myMessage(s : string);
begin
  s:= #3+s;
  MessageBox(s,nil, mfInformation or mfOKButton);
end;

{ ---------------------------------------------------------
                         myIntMessage
                 erzeugt ein Ausgabefenster

    Parameter:
       i  : Integer, der im Fenster angezeigt werden soll
  --------------------------------------------------------- }
procedure myIntMessage(i : longint);
var
  s : string;
begin
  str(i,s);
  s:= #3+s;
  MessageBox(s,nil, mfInformation or mfOKButton);
end;
