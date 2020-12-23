{
  Versuche, eine C-Datei daraufhingehend zu analysieren um eine Include-Anweisung
  zu "veraendern" in dem man den Pfad feststellt und einen eigenen Pfad dort
  einfuegt.

  Ziel ist es, eine "*.opt*" Datei in ein Projekt mit Makefile zu bringen

  21.09.2015 }

program TxtCAnalyse;

uses
  crt, dos;
  
{$i sourceanalyse01.inc}  

var
  ch   : char;

var
  sfilename : string;
  ifilename : string;
  tfilename : string;

begin
  clrscr;
  sfilename:= '    /home/avr_programs/programs/color_display/analogtest.c';
  ifilename:= '    #include   "      ../include/rs232_io.h"       // serielle Schnittstelle  ';
//  ifilename:= '    #include "test.h" ';
  tfilename:= make_abs_incfilename(sfilename, ifilename);
  writeln(sfilename,chr(10));
  writeln(ifilename,chr(10));
  writeln; writeln; writeln;
  writeln(tfilename,chr(10));
end.


{  assign(tdat,datnam);
  reset(tdat);
  while(not eof(tdat)) do
  begin
    readln(tdat,s);
    writeln(s);
  end; }
