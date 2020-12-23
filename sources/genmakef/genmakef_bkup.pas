program genmakef;

uses
  SysUtils, dos, Objects, Drivers, Views, Menus, Dialogs, App,
  FVConsts, MsgBox, StdDlg;

const
//                Textfarbpalette EGA-Farben
//                                   sw  bl  gn  cy  rt  mg  br  gr
  txpal     : array[0..7] of byte = (30, 34, 32, 36, 31, 35, 33, 37);
  bkpal     : array[0..7] of byte = (40, 44, 42, 46, 41, 45, 41, 47);

const
  partanz_avr        = 75;

const
  avrgccname   : array[0..partanz_avr-1] of string =
    ('at90s1200','at90s2313','at90s2333','at90s2343','at90s4414','at90s4433','at90s4434','at90s8515',
     'at90s8535','at90can128','at90can32','at90can64','atmega103','atmega128','atmega1280','atmega1281',
     'atmega1284p','atmega128rfa1','atmega16','atmega161','atmega162','atmega163','atmega164p','atmega168',
     'atmega169','atmega2560','atmega2561','atmega32','atmega324p','atmega325','atmega3250','atmega328p',
     'atmega329','atmega3290','atmega3290p','atmega329p','atmega48','atmega64','atmega640','atmega644',
     'atmega644p','atmega645','atmega6450','atmega649','atmega6490','atmega8','atmega8515','atmega8535',
     'atmega88','at90pwm2','at90pwm2b','at90pwm3','at90pwm3b','attiny12','attiny13','attiny15',
     'attiny2313','attiny24','attiny25','attiny26','attiny261','attiny4313','attiny44','attiny45',
     'attiny461','attiny84','attiny85','attiny861','attiny88','at90usb1286','at90usb1287','at90usb162',
     'at90usb646','at90usb647','at90usb82');
     
  partanz_stm32      = 3;
  
  stm32name    : array[0..partanz_stm32-1] of string =
    ('stm32f030','stm32f103','stm32f401');

{$i genmake_tmpl.inc}

const
  configpath    = '~/.config/genmakefile/';
  configname    = '~/.config/genmakefile/genmakefile.conf';

var
//  avrpath       : string = '/home/mcu/avr_programs';
  avrpath       : string = '/home/atmel_avr';
  f030path      : string = '/home/mcu/stm32f030';
  f103path      : string = '/home/mcu/stm32f103c8';
  f401path      : string = '/home/mcu/stm32f401';
  lpcpath       : string = '/home/mcu/lpc11cx4';
  mcs51path     : string = '/home/mcu/at89C52';
  stm8path      : string = '/home/mcu/stm8';
  pcgccpath     : string = '/home/mcu/pc_projects';

const
  hismygetdialog   = 2000;


type
  avr_box = record
    device_box  : PStringCollection;
    selection   : longint;
    programmers : longint;
    progport    : longint;
    isp_clk     : longint;
    baud        : longint;
    float_check : longint;
    proj_dir    : string[255];
    proj_nam    : string[38];
    freq_str    : string[8];
  end;


const
  avr_mkcommentanz   = 5;
  maxlnkfiles        = 100;

var
  lnkanz           : longint;
  avr_lnkfiles     : array[0..maxlnkfiles] of string;
  lnkfiles         : array[0..maxlnkfiles] of string;

var
  AvrRec   		   : avr_box;
  avrlistwahl      : integer;

  writetoscreen    : boolean = true;
  yy, mm, dd       : word;


procedure AvrRec_setdefaults;
begin
  with AvrRec do
  begin
    programmers:= 1;
    progport:= 1;
    baud:= 4;
    isp_clk:= 2;
    avrlistwahl:= 31;
    float_check:= 0;
    proj_dir:= '';
    proj_nam:= '';
    freq_str:= '16000000';
  end;
end;

procedure congotoxy (x, y : byte);
begin
  write(#27,'[',y,';',x,'H');
end;

procedure contxcolor(col : byte);
var
  intensity : byte;
begin
  intensity:= col div 8;
  if intensity= 0 then intensity:= 21 else intensity:= 1;
  write(#27,'[',intensity,';',txpal[col mod 8],'m');
end;

procedure conbkcolor(col : byte);
var
  intensity : byte;
begin
  intensity:= col div 8;
  if intensity= 0 then intensity:= 21 else intensity:= 1;
  write(#27,'[',intensity,';',bkpal[col mod 8],'m');
end;

procedure contxattr(col : byte);
begin
  contxcolor(col and $0f);
  conbkcolor(col shr 4);
end;

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

procedure myMessage(s : string);
begin
  s:= #3+s;
  MessageBox(s,nil, mfInformation or mfOKButton);
end;

procedure myintMessage(i : longint);
var
  s : string;
begin
  str(i,s);
  s:= #3+s;
  MessageBox(s,nil, mfInformation or mfOKButton);
end;


procedure readln_nospaces(var fh : text; var t : string);
begin
  readln(fh, t);
  while (t[1]= ' ') do delete (t,1,1);
  while (t[length(t)]= ' ') do delete(t, length(t), 1);
end;

procedure init_global;
var
  tdat         : text;
  dname, fname : string;
  zeil		   : string;

begin
  dname:= expandfilename(configpath);
  fname:= expandfilename(configname);

  if (not(fileexists(fname))) then
  begin
    writeln(dname);
    if CreateDir (dname) then
    begin
      assign(tdat, fname);
      {$i-}  rewrite(tdat); {$i+}
      if ioresult = 0 then
      begin

        // Pfade schreiben wenn nicht vorhanden war
        writeln(tdat, '<avr>');
        writeln(tdat, avrpath +chr(10));

        writeln(tdat, '<stm8>');
        writeln(tdat, stm8path +chr(10));

        writeln(tdat, '<stm32f030>');
        writeln(tdat, f030path +chr(10));

        writeln(tdat, '<stm32f103>');
        writeln(tdat, f103path +chr(10));

        writeln(tdat, '<stm32f401>');
        writeln(tdat, f401path +chr(10));

        writeln(tdat, '<lpc1114>');
        writeln(tdat, lpcpath +chr(10));

        writeln(tdat, '<mcs51>');
        writeln(tdat, mcs51path +chr(10));

        writeln(tdat, '<pc_textmode>');
        writeln(tdat, pcgccpath +chr(10));
        close(tdat);
      end else
      begin
        writeln(^j'File I/O error: can not create :'+fname);
        halt(3);
      end;
    end;
  end
  else begin
    assign(tdat, fname);
    reset(tdat);

    while (not(eof(tdat))) do
    begin
      readln_nospaces(tdat, zeil);
      if (zeil= '<avr>') then readln(tdat, avrpath);
      if (zeil= '<stm8>') then readln(tdat, stm8path);
      if (zeil= '<stm32f030>') then readln(tdat, f030path);
      if (zeil= '<stm32f103>') then readln(tdat, f103path);
      if (zeil= '<stm32f401>') then readln(tdat, f401path);
      if (zeil= '<lpc1114>') then readln(tdat, lpcpath);
      if (zeil= '<mcs51>') then readln(tdat, mcs51path);
    end;

    close(tdat);
  end;
end;

procedure save_pathes;
var
  tdat  : text;
  fname : string;
begin
  fname:= expandfilename(configname);

  assign(tdat, fname);
  {$i-}  rewrite(tdat); {$i+}
  if ioresult = 0 then
  begin

	// Pfade schreiben wenn nicht vorhanden war
	writeln(tdat, '<avr>');
	writeln(tdat, avrpath +chr(10));
	
	writeln(tdat, '<stm8>');
	writeln(tdat, stm8path +chr(10));

	writeln(tdat, '<stm32f030>');
	writeln(tdat, f030path +chr(10));

	writeln(tdat, '<stm32f103>');
	writeln(tdat, f103path +chr(10));

	writeln(tdat, '<stm32f401>');
	writeln(tdat, f401path +chr(10));
	
	writeln(tdat, '<lpc1114>');
	writeln(tdat, lpcpath +chr(10));

	writeln(tdat, '<mcs51>');
	writeln(tdat, mcs51path +chr(10));
	close(tdat);
  end;
end;


const
  cmAppToolbar      = 1000;
  cmgenwindow       = 1001;
  cmtestwindow      = 1002;
  cm_avrmkgen       = 1003;
  cmclosegenwindow  = 1101;
  cmclosetestwindow = 1101;

  cmselavrpath      = 1150;

const
  MaxLines          = 100;

var
  LineCount         : Integer;
  Lines             : array[0..MaxLines - 1] of PString;

{---------------------------------------------------------------------------}
{          Tinterior object                                                 }
{---------------------------------------------------------------------------}
type
  PInterior = ^TInterior;
  TInterior = object(TScroller)
    constructor Init(var Bounds: TRect; AHScrollBar,
      AVScrollBar: PScrollBar);
    procedure Draw; virtual;
  end;

constructor TInterior.Init(var Bounds: TRect; AHScrollBar,
  AVScrollBar: PScrollBar);
begin
  TScroller.Init(Bounds, AHScrollBar, AVScrollBar);
  GrowMode := gfGrowHiX + gfGrowHiY;
  Options := Options or ofFramed;
  SetLimit(128, LineCount);
end;

procedure TInterior.Draw;
var
  Color: Byte;
  I, Y: Integer;
  B: TDrawBuffer;
begin
  Color := GetColor(1);
  for Y := 0 to Size.Y - 1 do
  begin
    MoveChar(B, ' ', Color, Size.X);
    i := Delta.Y + Y;
    if (I < LineCount) and (Lines[I] <> nil) then
      MoveStr(B, Copy(Lines[I]^, Delta.X + 1, Size.X), Color);
    WriteLine(0, Y, Size.X, 1, B);
  end;
end;

{---------------------------------------------------------------------------}
{          gentxwin object                                                  }
{---------------------------------------------------------------------------}

type
  Pgentxwin = ^Tgentxwin;
  Tgentxwin = object(TWindow)
    constructor Init(Bounds: TRect; WinTitle: String; WindowNo: Word);
    procedure MakeInterior(Bounds: TRect);
    procedure closewin(var P : PGroup);
  end;

{ Tgentxwin }
constructor Tgentxwin.Init(Bounds: TRect; WinTitle: String;
  WindowNo: Word);
var
  S: string[3];
begin
  Str(WindowNo, S);
  TWindow.Init(Bounds, WinTitle + ' ' + S, wnNoNumber);
  MakeInterior(Bounds);
end;

procedure Tgentxwin.MakeInterior(Bounds: TRect);
var
  HScrollBar, VScrollBar: PScrollBar;
  Interior: PInterior;
  R: TRect;
begin
  VScrollBar := StandardScrollBar(sbVertical + sbHandleKeyboard);
  HScrollBar := StandardScrollBar(sbHorizontal + sbHandleKeyboard);
  GetExtent(Bounds);
  Bounds.Grow(-1,-1);
  Interior := New(PInterior, Init(Bounds, HScrollBar, VScrollBar));
  Insert(Interior);
end;

procedure Tgentxwin.closewin(var P : PGroup);
begin
  if assigned(P) then
  begin
    Desktop^.Delete(P);
    Dispose(P,Done);
    P:=Nil;
  end;
end;
{---------------------------------------------------------------------------}
{          mydesk object                                                    }
{---------------------------------------------------------------------------}
type
  p_mydesk = ^mydesk;

  { mydesk }

  mydesk = object (TApplication)
    genwindow : PGroup;

    constructor Init;
    procedure Idle; Virtual;
    procedure HandleEvent(var Event : TEvent);virtual;
    function openlinkfiles : string;
    function SelDir(startpath : string) : string;
    procedure InitMenuBar; Virtual;
    procedure InitDeskTop; Virtual;
    procedure InitStatusLine; Virtual;
    procedure avr_make_gen;
    procedure CloseWindow(var P : PGroup);

    private
      function write_avrmakefile(toscreen : boolean) : word;
  end;

{+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++}
{                           mydesk object start                          }
{+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++}

constructor mydesk.Init;
var R: TRect;
begin
  Inherited Init;

  GetExtent(R);
  R.A.X := R.B.X - 9; R.B.Y := R.A.Y + 1;

  GetExtent(R);

  avrrec_setdefaults;

  linecount:= 0;
  lnkanz:= 0;
end;

procedure mydesk.Idle;

  function IsTileable(P: PView): Boolean; far;
  begin
    IsTileable := (P^.Options and ofTileable <> 0) and
      (P^.State and sfVisible <> 0);
  end;

begin
  inherited Idle;
  if Desktop^.FirstThat(@IsTileable) <> nil then
    EnableCommands([cmTile, cmCascade])
  else
    DisableCommands([cmTile, cmCascade]);
end;

procedure mydesk.HandleEvent(var Event : TEvent);
begin
  Inherited HandleEvent(Event);                      { Call ancestor }
  if (Event.What = evCommand) then
  begin
    Case Event.Command Of
      cm_avrmkgen       : begin
                            avr_make_gen;
//                            myintmessage(avrlistwahl);
                          end;
      cmselavrpath      : begin
                            avrpath:= seldir(avrpath);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+avrpath);
						  end;
      cmClosegenwindow  : CloseWindow(genwindow);
      else
      begin
        exit;                                     { Unhandled exit }
      end;
    end;
  end;
  ClearEvent(Event);
end;

function mydesk.openlinkfiles : string;
var
  FileDialog   : PFileDialog;
  FileName     : string;
  orgdir	   : string;
const
  FDOptions: Word = fdOKButton or fdOpenButton;
begin
  orgdir:= getcurrentdir;
  FileName := '*.c';
  New(FileDialog, Init(FileName, 'Additional file to link', 'File name', FDOptions, 1));
  if not(ExecuteDialog(FileDialog, @FileName) <> cmCancel) then
  begin
    filename:= '';
  end;
  openlinkfiles:= filename;
  setcurrentdir(orgdir);
end;

function mydesk.SelDir(startpath : string) : string;
var
  D       : PChDirDialog;
  olddir  : string;
  selpath : string;

begin
  chdir(startpath);
  New(D, Init(cdNormal, hismygetDialog));
  ExecuteDialog(D,nil);

  GetDir(0,selpath);
  seldir:= selpath;
end;

procedure mydesk.InitMenuBar;
var R: TRect;
begin
  GetExtent(R);                                      { Get view extents }
  R.B.Y := R.A.Y + 1;                                { One line high  }
  MenuBar := New(PMenuBar, Init(R, NewMenu(
   NewSubMenu('~F~ile', 0, NewMenu(
     NewItem('E~x~it','Alt+x',kbAltX,cmquit,hcNoContext,
     Nil)),

   NewSubMenu('~A~tmel AVR', 0, NewMenu(
     NewItem('~S~elect path to AVR projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   NewSubMenu('~S~TM32', 0, NewMenu(
     NewItem('~S~elect path to STM32 projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   NewSubMenu('~L~PC11cx4', 0, NewMenu(
     NewItem('~S~elect path to LPC projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   NewSubMenu('STM~8~', 0, NewMenu(
     NewItem('~S~elect path to STM8 projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   NewSubMenu('M~C~S51', 0, NewMenu(
     NewItem('~S~elect path to MCS51 projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   nil))))))) //end NewSubMenus
  )); //end MenuBar
end;

procedure mydesk.InitDesktop;
var R: TRect; {ToolBar: PToolBar;}
begin
  GetExtent(R);                                      { Get app extents }
  Inc(R.A.Y);               { Adjust top down }
  Dec(R.B.Y);            { Adjust bottom up }
  Desktop := New(PDeskTop, Init(R));
end;

procedure mydesk.InitStatusLine;
var
   R: TRect;
begin
  GetExtent(R);
  R.A.Y := R.B.Y - 1;
  R.B.X := R.B.X;
  New(StatusLine,
    Init(R,
      NewStatusDef(0, $EFFF,
        NewStatusKey('~Alt+X~ Close', kbAltX, cmquit,
        StdStatusKeys(nil
        )),nil
      )
    )
  );

  GetExtent(R);
  R.A.X := R.B.X - 12; R.A.Y := R.B.Y - 1;
end;

procedure mydesk.CloseWindow(var P : PGroup);
begin
  if Assigned(P) then
  begin
    Desktop^.Delete(P);
    Dispose(P,done);
    P:=Nil;
  end;
end;

{
    programmers:= 1;
    progport:= 1;
    baud:= 4;
    isp_clk:= 2;
    avrlistwahl:= 31;
    float_check:= 0;
    proj_dir:= '';
    proj_nam:= '';
    freq_str:= '16000000';
 }

procedure addaline(var fh : text; s : string);
begin
  // im Textfenster ausgeben
  if (writetoscreen) then
  begin
    lines[linecount]:= newstr(s);
  end else
  // in Datei schreiben
  begin
    writeln(fh, s);
  end;
  inc(linecount);
end;


function mydesk.write_avrmakefile(toscreen : boolean) : word;
var
  i, c, ior           : word;
  tdat                : text;
  dname, fname, mname : string;
  sourcedir, na, ex   : string;

const
  programmerstxt  : array[0..8] of string =
    ('usbtiny', 'usbasp', 'stk500v2', 'ponyser', 'arduino',
     'avrisp', 'stk500v2', '', '');

  serporttxt : array[0..8] of string =
    ('', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2',
     '/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2',
     '/dev/ttyS0', '/dev/ttyS1');

  bratetxt : array[0..4] of string =
    ('9600', '19200', '38400', '57600', '115200');

  sclktxt : array[0..2] of string=
    ('-B15', '-B5', '-B0');

begin
  writetoscreen:= toscreen;

  if (not(toscreen)) then
  begin
    // Makefile in Datei schreiben
    write_avrmakefile:= 0;
    dname:= expandfilename(avrpath);
    dname:= dname + '/' + avrrec.proj_dir;
    fname:= dname + '/' + avrrec.proj_nam;
    mname:= dname + '/Makefile';

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
  addaline(tdat,'SERPORT    = '+ serporttxt[avrRec.progport]);
  addaline(tdat,'BRATE      = '+ bratetxt[avrRec.baud]);
  addaline(tdat,'DUDEOPTS   = '+ sclktxt[avrRec.isp_clk]);
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
    end;
    // den "Rest" schreiben
    for i:= 8 to avr_src_zeilanz do
    begin
      writeln(tdat, avr_templ[i]);
    end;
    close(tdat);

  end;
end;

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

  R.Assign(3, 6, 11, 7);
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
    NewSItem('reserved',
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

  R.Assign(22, 6, 37, 8);
  Control := New(PStaticText, Init(R, 'Support for'^M+
       'floating point'));
  avr_dlg^.Insert(Control);

  R.Assign(22, 9, 34, 12);
  Control := New(PCheckboxes, Init(R,
    NewSItem('printf',
    NewSItem('scanf',
    NewSItem('math',Nil)))));
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
  Control := New(PButton, Init(R, '~E~nd', cmCancel, bfDefault));
  avr_dlg^.Insert(Control);

  R.Assign(57, 29, 69, 31);
  Control := New(PButton, Init(R, '~G~enerate', cmYes, bfDefault));
  avr_dlg^.Insert(Control);

  R.Assign(41, 18, 53, 20);
  Control := New(PButton, Init(R, '~A~dd file', cmNo, bfDefault));
  avr_dlg^.Insert(Control);

  R.Assign(54, 18, 68, 20);
  Control := New(PButton, Init(R, '~C~lear list', cmOk, bfDefault));
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

var
  MyApp: mydesk;

begin
  init_global;
  MyApp.init;
  MyApp.run;
  MyApp.done;

  save_pathes;
  conclrscr;
  writeln('Type');
  writeln('cd /home/mcu/stm32f103');
  writeln('to enter your new project directory');
  writeln;
end.
