program scratch;

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
    b:= pos(utfstr[i],s);
    if b <> 0 then
    begin
      l:= length(utfstr[i]);
      delete(s,b,l); 
      l:= length(utfrep[i]);
      insert(utfrep[i],s,b);
    end;
  end;
end;  
  
var
  s    : string;  
  
begin
  s:= ' Das ist ein Text mit ' + chr(226) + chr(128) + chr(152) + 'Hyroglyphen' + chr(226)+chr(128)+chr(153) + ' = Fehler';
  writeln(s);
  replace_utfstring(s);
  writeln(s);
end.  
