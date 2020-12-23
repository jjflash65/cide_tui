program genmakef;

uses
  SysUtils, dos, Objects, Drivers, Views, Menus, Dialogs, App,
  FVConsts, MsgBox, StdDlg;

const
  cmAppToolbar      = 1000;
  cmgenwindow       = 1001;
  cmtestwindow      = 1002;
  cm_avrmkgen       = 1003;
  cm_arm32mkgen     = 1004;
  cm_stm8mkgen		  = 1005;
  cm_mcs51mkgen     = 1006;
  cm_pcmkgen        = 1007;
  cm_pic16fmkgen    = 1008;
  
  cmclosegenwindow  = 1101;
  cmclosetestwindow = 1101;

  cmselavrpath      = 1150;
  cmselarmpath      = 1151;
  cmselstm8path     = 1152;
  cmselmcs51path    = 1153;
  cmselpcpath       = 1154;
  cmselpic16fpath   = 1155;
  
  cmtestbutton      = 1200;

const
//                Textfarbpalette EGA-Farben
//                                   sw  bl  gn  cy  rt  mg  br  gr
  txpal     : array[0..7] of byte = (30, 34, 32, 36, 31, 35, 33, 37);
  bkpal     : array[0..7] of byte = (40, 44, 42, 46, 41, 45, 41, 47);

const
  partanz_avr        = 75;

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

const     
  partanz_arm32      = 10;
  
  arm32name    : array[0..partanz_arm32-1] of string =
    ('STM32F030', 'STM32F103 (64k)', 'STM32F103 (128k)', 'STM32F303 (128k)', 'STM32F303 (256k)', 
     'STM32F401', 'STM32F407', 'STM32F411', 'STM32L053 (32k)', 'LPC11cx4');
     
const
  partanz_stm8       = 4;
  
  stm8name     : array[0..partanz_stm8-1] of string =
    ('STM8S003F3', 'STM8S103F3', 'STM8S105K4', 'STM8S105K6');     
    
const
  partanz_mcs        = 2;
    
  mcsname     : array[0..partanz_mcs-1] of string =
    ('at89s52', 'at89c5131');     

const
  partanz_pic16f     = 3;
    
  pic16fname     : array[0..partanz_pic16f-1] of string =
    ('16f887', '16f648', '16f676');     

{$i genavr_tmpl.inc}
{$i genarm_tmpl.inc}
{$i genstm8_tmpl.inc}
{$i genmcs51_tmpl.inc}
{$i genpic16f_tmpl.inc}
{$i genpc_tmpl.inc}

const
  configpath    = '~/.config/genmakefile/';
  configname    = '~/.config/genmakefile/genmakefile.conf';

var
  avrpath       : string = '~/';
  armpath       : string = '~/';
  mcs51path     : string = '~/';
  pic16fpath    : string = '~/';
  stm8path      : string = '~/';
  pcgccpath     : string = '~/';
  
  lastpath	    : string;

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
    reset340	: longint;
    proj_dir    : string[255];
    proj_nam    : string[38];
    freq_str    : string[8];
  end;
  
  arm32_box = record
    device_box   : PStringCollection;
    selection    : longint;
    programmers  : longint;
    progport     : longint;
    before_flash : longint;

    proj_dir     : string[255];
    proj_nam     : string[38];
  end;
  
  stm8_box = record
    device_box   : PStringCollection;
    selection    : longint;
    programmers  : longint;
    progport     : longint;
    before_flash : longint;

    proj_dir     : string[255];
    proj_nam     : string[38];
  end;
  
  mcs51_box = record
    device_box   : PStringCollection;
    selection    : longint;
    programmers  : longint;
    progport     : longint;
    before_flash : longint;

    proj_dir     : string[255];
    proj_nam     : string[38];
    freq_str     : string[8];   
  end;

  pic_box = record
    device_box   : PStringCollection;
    selection    : longint;
    programmers  : longint;
    progport     : longint;
    before_flash : longint;

    proj_dir     : string[255];
    proj_nam     : string[38];
    freq_str     : string[8];   
  end;
  
  pc_box = record
    linkselect   : longint;
    proj_dir     : string[255];
    proj_nam     : string[38];
    liblist      : string[255];
  end; 
  
  
const
  avr_mkcommentanz   = 5;
  maxlnkfiles        = 100;

var
  lnkanz           : longint;
  avr_lnkfiles     : array[0..maxlnkfiles] of string;
  lnkfiles         : array[0..maxlnkfiles] of string;

  lnk32anz         : longint;
  arm32_lnkfiles   : array[0..maxlnkfiles] of string;
  lnk32files       : array[0..maxlnkfiles] of string;

  lnks8anz         : longint;
  stm8_lnkfiles    : array[0..maxlnkfiles] of string;
  lnks8files       : array[0..maxlnkfiles] of string;
  
  lnkmcsanz        : longint;
  mcs_lnkfiles     : array[0..maxlnkfiles] of string;
  lnkmcsfiles      : array[0..maxlnkfiles] of string;

  lnkpic16fanz     : longint;
  pic16f_lnkfiles  : array[0..maxlnkfiles] of string;
  lnkpic16ffiles   : array[0..maxlnkfiles] of string;

  lnkpcanz         : longint;
  pc_lnkfiles      : array[0..maxlnkfiles] of string;
  lnkpcfiles       : array[0..maxlnkfiles] of string;

var
  AvrRec   		   : avr_box;
  arm32Rec         : arm32_box;
  stm8Rec		   : stm8_box;
  mcsRec           : mcs51_box;
  PcRec            : pc_box;
  Pic16fRec        : pic_box;

  avrlistwahl      : integer;
  arm32listwahl	   : integer;
  stm8listwahl	   : integer;
  mcslistwahl      : integer;
  pic16flistwahl      : integer;

  writetoscreen    : boolean = true;
  yy, mm, dd       : word;

const
  MaxLines          = 100;

var
  LineCount         : Integer;
  Lines             : array[0..MaxLines - 1] of PString;

procedure PcRec_setdefaults;
begin
  with PcRec do
  begin
    proj_dir:= '';
    proj_nam:= '';
    linkselect:= 2;
    liblist:= '';
  end;
end;  

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
    reset340:= 0;
    proj_dir:= '';
    proj_nam:= '';
    freq_str:= '16000000';
  end;
end;

procedure arm32Rec_setdefaults;
begin
  with arm32Rec do
  begin
    programmers:= 1;
    progport:= 1;
    before_flash:= 0;
    arm32listwahl:= 1;
    proj_dir:= '';
    proj_nam:= '';
  end;
end;

procedure stm8Rec_setdefaults;
begin
  with stm8Rec do
  begin
    programmers:= 0;
    progport:= 1;
    before_flash:= 0;
    stm8listwahl:= 1;
    proj_dir:= '';
    proj_nam:= '';
  end;
end;

procedure mcsRec_setdefaults;
begin
  with mcsRec do
  begin
    programmers:= 0;
    progport:= 1;
    before_flash:= 0;
    mcslistwahl:= 0;
    proj_dir:= '';
    proj_nam:= '';
    freq_str:= '12000000';
  end;
end;

procedure Pic16fRec_setdefaults;
begin
  with pic16fRec do
  begin
    programmers:= 0;
    progport:= 0;
    before_flash:= 1;
    pic16flistwahl:= 0;
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
        avrpath:= expandfilename(avrpath);
        stm8path:= expandfilename(stm8path);
        armpath:= expandfilename(armpath);
        mcs51path:= expandfilename(mcs51path);
        pic16fpath:= expandfilename(pic16fpath);
        pcgccpath:= expandfilename(pcgccpath);

        // Pfade schreiben wenn nicht vorhanden war
        writeln(tdat, '<avr>');
        writeln(tdat, avrpath +chr(10));

        writeln(tdat, '<stm8>');
        writeln(tdat, stm8path +chr(10));

        writeln(tdat, '<arm>');
        writeln(tdat, armpath +chr(10));

        writeln(tdat, '<mcs51>');
        writeln(tdat, mcs51path +chr(10));

        writeln(tdat, '<pic16f>');
        writeln(tdat, pic16fpath +chr(10));

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
      if (zeil= '<arm>') then readln(tdat, armpath);
      if (zeil= '<mcs51>') then readln(tdat, mcs51path);
      if (zeil= '<pic16f>') then readln(tdat, pic16fpath);
      if (zeil= '<pc_textmode>') then readln(tdat, pcgccpath);
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

	writeln(tdat, '<avr>');
	writeln(tdat, avrpath +chr(10));
	
	writeln(tdat, '<stm8>');
	writeln(tdat, stm8path +chr(10));

	writeln(tdat, '<arm>');
	writeln(tdat, armpath +chr(10));

	writeln(tdat, '<mcs51>');
	writeln(tdat, mcs51path +chr(10));

	writeln(tdat, '<pic16f>');
	writeln(tdat, pic16fpath +chr(10));

	writeln(tdat, '<pc_textmode>');
	writeln(tdat, pcgccpath +chr(10));
	close(tdat);
  end;
end;

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

type
  pdialog2 = ^ Tdialog2;
  
  Tdialog2 = object (TDialog)
  
    procedure HandleEvent(var Event : TEvent); virtual;
  end;
  
  procedure TDialog2.HandleEvent (var Event : TEvent);
  begin
  
    inherited HandleEvent(Event);
    
    case Event.What of
      evCommand:
        case Event.Command of
          cmOk, cmCancel, cmYes, cmNo, cmTestbutton :
            if State and sfModal <> 0 then
            begin
              EndModal(Event.Command);
              ClearEvent(Event)
            end;
        end;   
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
    procedure arm32_make_gen;
    procedure stm8_make_gen;
    procedure mcs51_make_gen;
    procedure pic16f_make_gen;
    procedure pc_make_gen;
    procedure CloseWindow(var P : PGroup);

    private
      function write_avrmakefile(toscreen : boolean) : word;
      function write_armmakefile(toscreen : boolean) : word;
      function write_stm8makefile(toscreen : boolean) : word;
      function write_mcs51makefile(toscreen : boolean) : word;
      function write_pic16fmakefile(toscreen : boolean) : word;
      function write_pcmakefile(toscreen : boolean) : word;
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
  arm32rec_setdefaults;
  stm8rec_setdefaults;
  mcsrec_setdefaults;
  pic16frec_setdefaults;
  pcrec_setdefaults;

  linecount:= 0;
  lnkanz:= 0;
  lnk32anz:= 0;
  lnks8anz:= 0;
  lnkmcsanz:= 0;
  lnkpcanz:= 0;
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
                          end;
      cm_arm32mkgen     : begin
                            arm32_make_gen;
                          end;                          
      cm_stm8mkgen      : begin
                            stm8_make_gen;
                          end;                          
      cm_mcs51mkgen     : begin
                            mcs51_make_gen;
                          end;                          
      cm_pic16fmkgen    : begin
                            pic16f_make_gen;
                          end;                          
      cm_pcmkgen        : begin
                            pc_make_gen;
                          end;        
                                            
      cmselavrpath      : begin
                            avrpath:= seldir(avrpath);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+avrpath);
						  end;
      cmselarmpath      : begin
                            armpath:= seldir(armpath);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+armpath);
						  end;
      cmselstm8path     : begin
                            stm8path:= seldir(stm8path);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+stm8path);
                         end;   
      cmselmcs51path    : begin
                            mcs51path:= seldir(mcs51path);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+mcs51path);
						  end;						  
      cmselpic16fpath   : begin
                            pic16fpath:= seldir(pic16fpath);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+pic16fpath);
						  end;						  
      cmselpcpath       : begin
                            pcgccpath:= seldir(pcgccpath);
                            mymessage('Selected base folder: '+chr(13)+chr(13)+pcgccpath);
                         end;   
      cmtestbutton      : mymessage('Testbutton gedrueckt');                         
						  
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

   NewSubMenu('Atmel-~A~VR', 0, NewMenu(
     NewItem('~S~elect path to AVR projects ','',kbNoKey,cmselavrpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_avrmkgen,hcNoContext,
     Nil))),

   NewSubMenu('A~R~M', 0, NewMenu(
     NewItem('~S~elect path to ARM projects ','',kbNoKey,cmselarmpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_arm32mkgen,hcNoContext,
     Nil))),

   NewSubMenu('STM~8~', 0, NewMenu(
     NewItem('~S~elect path to STM8 projects ','',kbNoKey,cmselstm8path, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_stm8mkgen,hcNoContext,
     Nil))),

   NewSubMenu('M~C~S51', 0, NewMenu(
     NewItem('~S~elect path to MCS51 projects ','',kbNoKey,cmselmcs51path, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_mcs51mkgen,hcNoContext,
     Nil))),
     
   NewSubMenu('~P~IC16F', 0, NewMenu(
     NewItem('~S~elect path to PIC16F projects ','',kbNoKey,cmselpic16fpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_pic16fmkgen,hcNoContext,
     Nil))),

   NewSubMenu('P~C~-Console', 0, NewMenu(
     NewItem('~S~elect path to consoleprojects ','',kbNoKey,cmselpcpath, hcNoContext,
     NewItem('~M~akefile generator ','',kbNoKey,cm_pcmkgen,hcNoContext,
     Nil))),     

   nil)))))))) //end NewSubMenus
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

procedure addsrc_putchar(var fh : text);
begin
  writeln(fh, '');
  writeln(fh, ' /* ------------------------------------------------- ');
  writeln(fh, '                    my_putchar');
  writeln(fh, '');
  writeln(fh, '      my_putchar is used by my_printf. Place here');
  writeln(fh, '      the function, where my_printf should send the');
  writeln(fh, '      characters');
  writeln(fh, '    ------------------------------------------------- */ ');
  writeln(fh, 'void my_putchar(char ch)');
  writeln(fh, '{');
  writeln(fh, '  // Put here the function where my_printf streams a character');
  writeln(fh, '}');
end;

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

{$i genavr_scr_write.inc}
{$i genarm_scr_write.inc}
{$i genstm8_scr_write.inc}
{$i genmcs51_scr_write.inc}
{$i genpic16f_scr_write.inc}
{$i genpc_scr_write.inc}

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
  writeln('cd '+lastpath);
  writeln('to enter your new project directory');
  writeln;
end.
