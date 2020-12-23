{
    Copyright (c) 1998-2002 by Florian Klaempfl, Pierre Muller

    Tokens used by the compiler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ****************************************************************************
}
unit mytokens;

{$i fpcdefs.inc}

interface

uses
  globtype;

type
  ttoken=(NOTOKEN,
    { operators, which can also be overloaded }
    _UNSIG,    
    _SIG,
    _CHAR,
    _INT,
    _CONST,
    _LONG,
    _FLOAT,
    _DOUBLE,
    _STRUCT,
    _TYPEDEF,
    _UNION,
    _ENUM,
    _VOID,
    _FOR,
    _WHILE,
    _DO,
    _IF,
    _THEN,
    _ELSE,
    _SWITCH,
    _CASE,
    _BREAK,
    _DEFAULT,
    _CINCLUDE,
    _CDEFINE,
    _IFDEF,
    _DEF,
    _IFNDEF,
    _ENDIF,
    _EXTERN,
    _UNDEF,
    _FILE,
    _RETURN,
    _UINT8_T,
    _UINT16_T,
    _UINT32_T,
    _INT8_T,
    _INT16_T,
    _INT32_T,
    _ELIF
  );

const
  tokenlenmin = 1;
  tokenlenmax = 18;

  { last operator which can be overloaded, the first_overloaded should
    be declared directly after NOTOKEN }
  first_overloaded = succ(NOTOKEN);
  last_overloaded  = _UNSIG;
  last_operator = _CDEFINE;

type
  tokenrec=record
    str     : string[tokenlenmax];
    special : boolean;
    keyword : tmodeswitch;
    op      : ttoken;
  end;

  ttokenarray=array[ttoken] of tokenrec;
  ptokenarray=^ttokenarray;

  tokenidxrec=record
    first,last : ttoken;
  end;

  ptokenidx=^ttokenidx;
  ttokenidx=array[tokenlenmin..tokenlenmax,'A'..'Z'] of tokenidxrec;

const
  arraytokeninfo : ttokenarray =(
      (str:''              ;special:true ;keyword:m_none;op:NOTOKEN),
    { Operators which can be overloaded }
    
      (str:'UNSIGNED' ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'SIGNED'   ;special:false;keyword:m_all;op:NOTOKEN),      
      (str:'CHAR'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'INT'      ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'CONST'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'LONG'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'FLOAT'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'DOUBLE'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'STRUCT'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'TYPEDEF'  ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'UNION'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'ENUM'     ;special:false;keyword:m_all;op:NOTOKEN),
      
      (str:'VOID'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'FOR'      ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'WHILE'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'DO'       ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'IF'       ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'THEN'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'ELSE'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'SWITCH'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'CASE'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'BREAK'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'DEFAULT'  ;special:false;keyword:m_all;op:NOTOKEN),

      (str:'INCLUDE'  ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'DEFINE'   ;special:false;keyword:m_all;op:NOTOKEN),      
      (str:'IFDEF'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'DEFINED'  ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'IFNDEF'   ;special:false;keyword:m_all;op:NOTOKEN),      
      (str:'ENDIF'    ;special:false;keyword:m_all;op:NOTOKEN), 
      (str:'EXTERN'   ;special:false;keyword:m_all;op:NOTOKEN),      
      (str:'UNDEF'    ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'FILE'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'RETURN'   ;special:false;keyword:m_all;op:NOTOKEN),             
      (str:'UINT8_T'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'UINT16_T'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'UINT32_T'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'INT8_T'     ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'INT16_T'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'INT32_T'   ;special:false;keyword:m_all;op:NOTOKEN),
      (str:'ELIF'   ;special:false;keyword:m_all;op:NOTOKEN)
  );


var
  tokeninfo:ptokenarray;
  tokenidx:ptokenidx;

procedure inittokens;
procedure donetokens;
procedure create_tokenidx;


implementation

procedure create_tokenidx;
{ create an index with the first and last token for every possible token
  length, so a search only will be done in that small part }
var
  t : ttoken;
  i : longint;
  c : char;
begin
  fillchar(tokenidx^,sizeof(tokenidx^),0);
  for t:=low(ttoken) to high(ttoken) do
   begin
     if not arraytokeninfo[t].special then
      begin
        i:=length(arraytokeninfo[t].str);
        c:=arraytokeninfo[t].str[1];
        if ord(tokenidx^[i,c].first)=0 then
         tokenidx^[i,c].first:=t;
        tokenidx^[i,c].last:=t;
      end;
   end;
end;


procedure inittokens;
begin
  if tokenidx = nil then
  begin
    tokeninfo:=@arraytokeninfo;
    new(tokenidx);
    create_tokenidx;
  end;
end;


procedure donetokens;
begin
  if tokenidx <> nil then
  begin
    tokeninfo:=nil;
    dispose(tokenidx);
    tokenidx:=nil;
  end;
end;

end.
