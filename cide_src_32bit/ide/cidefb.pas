{
    This file is part of the Free Pascal Integrated Development Environment
    Copyright (c) 1998 by Berczi Gabor

    Main IDEApp object

    See the file COPYING.FPC, included in this distribution,menu_compile_compilermessages,menu_key_compile_compilermessagesavr
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

**********************************************************************    Massive Modifikationen der originalen IDE von Freepascal damit
    diese somit als IDE fuer den AVR-GCC verwendet werden kann !


    Modifikationen 03.01.2015   by R. Seelig

********************************************************************* }
unit cidefb;

{$define Linux}
{$define USEGDBLIBINC}


{2.0 compatibility}
{$ifdef VER2_0}
  {$macro on}
  {$define resourcestring := const}
{$endif}

interface

{$i globdir.inc}

uses

  classes, SysUtils, Process, Synaser,

  objects, Drivers,Views,App,Gadgets,MsgBox,Tabs,
  WEditor,WCEdit,
  Comphook,Browcol,mytokens,
  WHTMLScn,
  FPViews,FPSymbol;

type
    TExecType = (exNormal,exNoSwap,exDosShell);
    Tdisplaymode = (dmIDE,dmUser);

    TIDEApp = object(TApplication)
      IsRunning   : boolean;
      displaymode : Tdisplaymode;
      clock       : pClockView;

      constructor Init;
      procedure   InitDesktop; virtual;
      procedure   LoadMenuBar;
      procedure   InitMenuBar; virtual;
      procedure   reload_menubar;
      procedure   InitStatusLine; virtual;
      procedure   Open(FileName: string;FileDir:string);
      function    OpenSearch(FileName: string) : boolean;
      function    AskSaveAll: boolean;
      function    SaveAll: boolean;
      function    AutoSave: boolean;
      procedure   Idle; virtual;
      procedure   Update;
      procedure   UpdateMode;
      procedure   UpdateRunMenu(DebuggeeRunning : boolean);
      procedure   UpdateTarget;
      procedure   GetEvent(var Event: TEvent); virtual;
      procedure   HandleEvent(var Event: TEvent); virtual;
      procedure   GetTileRect(var R: TRect); virtual;
      function    GetPalette: PPalette; virtual;
      procedure   DosShell; {virtual;}
      procedure   ShowReadme;
      destructor  Done; virtual;
      procedure   ShowUserScreen;
      procedure   ShowIDEScreen;
      function    IsClosing : boolean;

	  procedure   mysettings;
      procedure   settermpara;
      procedure   callcmd(cmdnam : string; ochannel : byte);
      procedure   callmake(cmdnam : string);
          
      procedure   calendar;

      function	  GetMaxPorts : byte;
      procedure   Testfunktion;
      procedure   ListPorts;
      procedure   ListUSB;
      procedure   StartUart;
      procedure   CallMidnight;
      procedure   getutcdate;
      procedure   rshelp;
      procedure   About;
      procedure   showcompwindow;

    private
      procedure NewEditor;
      procedure NewFromTemplate;
      procedure OpenRecentFile(RecentIndex: integer);
      procedure ChangeDir;
      procedure Print;
      procedure PrinterSetup;
      procedure ShowClipboard;
      procedure FindProcedure;
      procedure Objects;
      procedure Modules;
      procedure Globals;
      procedure SearchSymbol;
      procedure RunDir;
      procedure Parameters;
      procedure DoStepOver;
      procedure DoTraceInto;
      procedure DoRun;
      procedure DoResetDebugger;
      procedure DoContToCursor;
      procedure DoContUntilReturn;
      procedure Target;
      procedure DoCompilerMessages;
      procedure DoPrimaryFile;
      procedure DoClearPrimary;
      procedure DoUserScreenWindow;
      procedure DoCloseUserScreenWindow;
      procedure DoUserScreen;
      procedure DoOpenGDBWindow;
      procedure DoToggleBreak;
      procedure DoShowCallStack;
      procedure DoShowDisassembly;
      procedure DoShowBreakpointList;
      procedure DoShowWatches;
      procedure DoAddWatch;
      procedure do_evaluate;
      procedure DoShowRegisters;
      procedure DoShowFPU;
      procedure DoShowVector;
      function  AskRecompileIfModified:boolean;
      procedure Messages;
      procedure Calculator;
      procedure DoAsciiTable;
      procedure ExecuteTool(Idx: integer);
      procedure SetSwitchesMode;
      procedure DoCompilerSwitch;
      procedure MemorySizes;
      procedure DoLinkerSwitch;
      procedure DoDebuggerSwitch;
{$ifdef SUPPORT_REMOTE}
      procedure DoRemote;
      procedure TransferRemote;
{$endif SUPPORT_REMOTE}
      procedure Directories;
      procedure Tools;
      procedure DoGrep;
      procedure Preferences;
      procedure EditorOptions(Editor: PEditor);
      procedure CodeComplete;
      procedure CodeTemplates;
      procedure BrowserOptions(Browser: PBrowserWindow);
      procedure DesktopOptions;
      procedure ResizeApplication(x, y : longint);
      procedure Mouse;
      procedure StartUp;
      procedure Colors;
      procedure OpenINI;
      procedure SaveINI;
      procedure SaveAsINI;
      procedure CloseAll;
      procedure WindowList;
      procedure HelpContents;
      procedure HelpHelpIndex;
      procedure HelpTopicSearch;
      procedure HelpPrevTopic;
      procedure HelpUsingHelp;
      procedure HelpFiles;
      procedure CreateAnsiFile;
    public
      procedure SourceWindowClosed;
      function  DoExecute(ProgramPath, Params, InFile, OutFile, ErrFile: string; ExecType: TExecType): boolean;
    private
      SaveCancelled: boolean;
      InsideDone : boolean;
      LastEvent: longint;
      procedure AddRecentFile(AFileName: string; CurX, CurY: sw_integer);
      function  SearchRecentFile(AFileName: string): integer;
      procedure RemoveRecentFile(Index: integer);
      procedure CurDirChanged;
      procedure UpdatePrimaryFile;
      procedure UpdateINIFile;
      procedure UpdateRecentFileList;
      procedure UpdateTools;
    end;

const
  cmcalendar          = 3006;
  cmsetmcu            = 3008;
  cmsetport           = 3009;
  cmsetfuse           = 3010;
  cmsettermset        = 3011;
  cmmakeall           = 3012;
  cmmakeflash         = 3013;
  cmmakeclean         = 3014;
  cmmakereset		  = 3015;
  cmclearlist         = 3018;
  cmrshelp            = 3020;
  cmavrversion		  = 3021;
  cmsetfreq           = 3022;
  cmdudeversion       = 3023;
  cmcallmc			  = 3024;
  cmgccversion        = 3027;
  cmdatetime		  = 3028;
  cmsetflasher        = 3029;
  cmsetportbaud       = 3032;

  cmnothing           = 3121;
  cmsizemove          = 3122;
  
  cmlistusb			  = 3197;
  cmlistports         = 3198;
  cmtestfunction      = 3199;

  cmrs232win          = 3031;
  cmrs232winclose     = 3032;

  cmrcvrs232          = 3035;
  cmrsparam           = 3036;
  cmrsstart           = 3037;
  cmrsstop            = 3038;
  cmrsclear           = 3039;
  cmrsupdate          = 3040;
  cmAbout             = 3060;
  
procedure PutEvent(TargetView: PView; E: TEvent);
procedure PutCommand(TargetView: PView; What, Command: Word; InfoPtr: Pointer);

var
  IDEApp: TIDEApp;

implementation

uses
{$ifdef HasSignal}
  fpcatch,
{$endif HasSignal}
{$ifdef WinClipSupported}
  WinClip,
{$endif WinClipSupported}
{$ifdef Unix}
  fpKeys,
{$endif Unix}
  FpDpAnsi,WConsts,
  Video,Mouse,Keyboard,
  Compiler,Version,
  FVConsts,
  Dos{,Memory},Menus,StdDlg,dialogs,timeddlg,
  Systems,
  WUtils,WHlpView,WViews,WHTMLHlp,WHelp,WConsole,
  FPConst,FPVars,FPUtils,FPSwitch,FPIni,FPIntf,FPCompil,FPHelp,
  FPTemplt,FPCalc,FPUsrScr,FPTools,  calendar2,
{$ifndef NODEBUG}
  FPDebug,FPRegs,
{$endif}
  FPRedir,
  FPDesk,FPCodCmp,FPCodTmp;

{$i fpmresstrings.inc }

const
  rshelpfilename : string = '/usr/share/cide/rshelp.ide';
  serprogbaud    : string = '115200';
  infozeils      = 18;
  firstinfo      : array[0..infozeils] of string =

  ('                                                 ',
   '  ----------------- Hinweis -------------------  ',
   '   CIDE 0.1                                      ',
   '   c 2016 / 2017 by R. Seelig                    ',
   '  ---------------------------------------------  ',
   '                                                 ',
   '  Hallo   Benutzer,   dies  hier ist ein   ein-  ',
   '  facher Texteditor  mit der  Moeglichkeit  des  ',
   '  aufrufens von "make". Vordergruendig soll mit  ',
   '  ihm  einfache  Assemblerprogramme fuer Mikro-  ',
   '  controller  erstellt werden.funden in der die  ',
   '  UIDE hat eine Konfigurationsdatei unterdiesem  ',
   '                                                 ',
   '       ~/.config/cide/cide.conf            ',
   '                                                 ',
   '  angelegt !                                     ',
   '                                                 ',
   '          ENTER fuer den Start von CIDE ...      ',
   '                                                 ');

     
  bratestrings : array[0..11] of string =
              ('150','300','600','1200','2400','4800','9600','19200','28800','38400','57600','115200');


// ................. Typen der Dialogfelder ...............

type
  mcudataval = record
        mcutyp     : longint;
        list       : PStrCollection;
        selection  : longint;
  end;

  fusedatval = record
        mcutyp     : longint;
        clock      : longint;
        flasher    : longint;
  end;

  termsetpara = record
        port       : longint;
        baud       : longint;
        datab      : longint;
        stopb      : longint;
        parity     : longint;
        misc       : longint;
        rkey       : longint;
   end;

   linkallsettings = record
        standard     : longint;
        additional   : longint;
        lfilename    : string[255];
   end;

   linkallpcsettings = record
        standard     : longint;
        libnames     : string[255];
        additional   : longint;
        lfilename    : string[255];
        arguments    : string[255];
   end;
   
   fusebox1settings = record
     checkfuse  : longint;
     lofuse     : string[2];
     hifuse     : string[2];
     exfuse     : string[2];
     lockbits   : string[2];
     programmer : longint;
     devices    : PStrCollection;
     selection  : longint;
   end;   

   flashhexsettings = record
     programmer : longint;
     devices    : PStrCollection;     
     selection  : longint;  
     ispspeed   : longint;          
     hexnam     : string[255];             
   end;

   sumsettings = record
        set1       : mcudataval;
        set2       : fusedatval;
        set3       : termsetpara;
        set4       : longint;			   	   // Settings Programmer Port
        set5       : linkallsettings;
        set6       : string;                   // MCU Frequenz
        set7       : linkallpcsettings;
        set8       : integer;                  // Auswahl MCU in Listbox
        set9       : integer;                  // gewaehlter Programmer
        set10      : fusebox1settings;
        set11      : flashhexsettings;
        set12      : longint;					// Settings Programmer Baudrate
   end;

   strarray = array[1..25] of string;

// -----------------------------------------------------
//                  Globale Variable
// -----------------------------------------------------


var
  conffile      : string = '~/.config/cide/cide.conf';  
  confdir       : string = '~/.config/cide/';
  confarmdir    : string = '~/.config/cide/cide2.conf';
  arm_make_path : string = '/usr/local/arm/';
  rside_headers : string = '/home/arm_programs/include/';
  fcname        : string;
  dcname        : string;
  gexitcode     : integer;


// Speichervariable der Dialoginhalte
  mcusettings      : mcudataval;
  fusesettings     : fusedatval;
  termparams       : termsetpara;
  progportsetting  : longint;
  progportbaud     : longint;
  linkersettings   : linkallsettings;
  linkerpcsettings : linkallpcsettings;
  avrfreq          : string;
  mculistwahl      : integer;
  selectedflasher  : longint;
  fusebox1values   : fusebox1settings;
  flashhexvalues   : flashhexsettings;
  allsettings      : sumsettings;
  dat              : file of sumsettings;                // Dateityp fuer die Dialoginhalte

//  myprocess        : tprocess;
  mystringlist     : classes.tstringlist;
  errcode          : integer;

// --------------------------------------------------------
//          globales fuer den seriellen Monitor
// --------------------------------------------------------

type
  str132 = string[132];

const
  zeilcnt          : integer = 0;
  spalcnt          : integer = 1;
  feedback         : boolean = false;
  locecho          : boolean = false;
  crlfflag         : boolean = false;
  maxanz           = 150;               // max. 150 Zeilen Buffer
  chanz            = 132;               // max. 132 Zeichen per Zeile
  

var
  owvalue          : array[0..maxanz] of str132;
  lastvalue        : char;
  spalmax          : integer;

  cud1,cud2        : boolean;           // Cursorflags
  sermonactive     : boolean;

  outwinp          : pgroup;            // Zeiger auf das Outwin-Fenster
  ser              : TBlockSerial;
  activeport       : string;


type
   TTargetedEvent = record
     Target: PView;
     Event: TEvent;
   end;

const
     TargetedEventHead   : integer = 0;
     TargetedEventTail   : integer = 0;
var
     TargetedEvents      : array[0..10] of TTargetedEvent;


// -----------------------------------------------------
//                 ausserhalb der Objekte
// -----------------------------------------------------

const
  axofs     : byte = 0;
  ayofs		: byte = 0;

//                Textfarbpalette EGA-Farben
//                                   sw  bl  gn  cy  rt  mg  br  gr
  txpal     : array[0..7] of byte = (30, 34, 32, 36, 31, 35, 33, 37);
  bkpal     : array[0..7] of byte = (40, 44, 42, 46, 41, 45, 41, 47);


var
  axwhere   : byte;
  aywhere	: byte;

  i         : Integer;
  notexist  : boolean;
  ch        : char;

procedure myshellexecute(cmdstr : string);
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;
  anz,w 		  : integer;
  scall,s         : string;
begin
  scall:= cmdstr;
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.stderr);
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  myprocess.free;
end;


function byte2string(b : byte) : string;
var
  s : string;
begin
  str(b,s);
  byte2string:= s;
end;

function Dez2Hex(hz : longint) : string;
var
  s,s2 : string;
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

procedure congotoxy (x, y : byte);
begin
  x:= x+axofs;
  y:= y+ayofs;
  axwhere:= x; aywhere:= y;
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

function conreadkey : char;
var
  k : tkeyevent;
  s  : string[5];
  ch : char;
begin
  k:= getkeyevent;
  k:= translatekeyevent(k);
  s:= keyeventtostring(k);
  if (GetKeyEventFlags(K)= kbascii) then
  begin
    ch:= s[1];
  end else
  begin
    ch:= chr(0);
  end;
  conreadkey:= ch;
end;

// Aus einem Dateistring nur den Dateinamen extrahieren

function getrawfilename(s : string) : string;
var
  p : integer;
begin
  p:= pos('/',s);
  while (p>0) do
  begin
    delete(s,1,p);
    p:= pos('/',s);
  end;
  getrawfilename:= s;
end;

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

// speariert aus einem Stringdateiliste, bei der Dateinamen mit einem
// Semikolon getrennt sind. Entfernt eine Datei und gibt diese als
// Funktionsergebnis zurueck

function splitsemistring(var srcstr : string) : string;
var
  s : string;
  i : integer;
begin
  s:= '';
  repeat
    i:= pos(' ',srcstr);
    delete(srcstr,i,1);
  until (i= 0);
  i:= pos(';',srcstr);
  if i= 0 then
  begin
    splitsemistring:= srcstr;
    srcstr:= '';
    exit;
  end else
  begin
    s:= copy(srcstr,1,i-1);
    delete(srcstr,1,i);
  end;
  splitsemistring:= s;
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


function iscfile(s : string) : boolean;
var
  hs,hs2  : string;
begin
  hs:= upper(copy(s,length(s)-1,2));
  hs2:= upper(copy(s,length(s)-3,4));
  if ((hs= '.C') or (hs2= '.CPP')) then iscfile:= true else iscfile:= false;  
end;


procedure replace_utfstring(var s : string);
const
  utfanz = 3;
var
  utfstr   : array[0..utfanz-1] of string =
            (chr(226)+chr(128)+chr(152),
             chr(226)+chr(128)+chr(153),
             chr(9) );
  utfrep   : array[0..utfanz-1] of string =
            (chr(39),
             chr(39),
             chr(32));

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

function splitstring(tmpstr : string; var strlist : strarray) : byte;
var
  p, sanz  : byte;
begin
  sanz:= 0;
  p:= 0;
  repeat
    p:= pos(' ',tmpstr);
    if (p> 0) then
    begin
      delete(tmpstr,p,1);                      // gefundene Leerzeichen loeschen
    end;
  until (p= 0);
  repeat
    p:= pos(';',tmpstr);
    if (p> 0) then
    begin
      inc(sanz);
      strlist[sanz]:= copy(tmpstr,1,p-1);
      delete(tmpstr,1,p);
    end;
  until (p= 0);
  if (tmpstr<> '') then
  begin
    inc(sanz);
    strlist[sanz]:= tmpstr;
  end;
  splitstring:= sanz;
end;


function makelinkstr(s : string) : string;
var
  anz,i   : byte;
  linkstr : strarray;
  s2      : string;
begin
  anz:= splitstring(s,linkstr);
  s2:= '';
  if anz> 0 then
  begin
    for i:= 1 to anz do
    begin
      s2:= s2+'-l'+linkstr[i]+' ';
    end;
  end;
  makelinkstr:= s2;
end;


// den Puffer des RS232-Fensters loeschen

procedure Clearbuffer;
var
  i, spal : integer;
begin
  for i:= 0 to maxanz do
  begin
    owvalue[i][0]:= chr(chanz);
    for spal:= 1 to chanz do owvalue[i][spal]:= ' ';
  end;
  owvalue[0][1]:= '_';
  lastvalue:= ' ';
end;

procedure mySave;
var
  p   : pSourceWindow;
begin

  P:=Message(Desktop,evBroadcast,cmSearchWindow,nil);
  if (PrimaryFileMain='') and (P=nil) then
  begin
    mymessage('Keine Datei zum Speichern');
  end else
  begin
    if p^.Editor^.IsClipboard=false then
      if (p^.Editor^.FileName='') then
        p^.Editor^.SaveAs
      else
        p^.Editor^.Save;
  end;
end;

function IncTargetedEventPtr(I: integer): integer;
begin
  Inc(I);
  if I>High(TargetedEvents) then I:=Low(TargetedEvents);
  IncTargetedEventPtr:=I;
end;

procedure PutEvent(TargetView: PView; E: TEvent);
begin
  if IncTargetedEventPtr(TargetedEventHead)=TargetedEventTail then Exit;
  with TargetedEvents[TargetedEventHead] do
  begin
    Target:=TargetView;
    Event:=E;
  end;
  TargetedEventHead:=IncTargetedEventPtr(TargetedEventHead);
end;

procedure PutCommand(TargetView: PView; What, Command: Word; InfoPtr: Pointer);
var E: TEvent;
begin
  FillChar(E,Sizeof(E),0);
  E.What:=What;
  E.Command:=Command;
  E.InfoPtr:=InfoPtr;
  PutEvent(TargetView,E);
end;

function GetTargetedEvent(var P: PView; var E: TEvent): boolean;
var OK: boolean;
begin
  OK:=TargetedEventHead<>TargetedEventTail;
  if OK then
  begin
    with TargetedEvents[TargetedEventTail] do
    begin
      P:=Target;
      E:=Event;
    end;
    TargetedEventTail:=IncTargetedEventPtr(TargetedEventTail);
  end;
  GetTargetedEvent:=OK;
end;

function IDEUseSyntaxHighlight(Editor: PFileEditor): boolean;
begin
  IDEUseSyntaxHighlight:=(Editor^.IsFlagSet(efSyntaxHighlight)) and ((Editor^.FileName='') or MatchesFileList(NameAndExtOf(Editor^.FileName),HighlightExts));
end;

function IDEUseTabsPattern(Editor: PFileEditor): boolean;
begin
  { the commented code lead all new files
    to become with TAB use enabled which is wrong in my opinion PM }
  IDEUseTabsPattern:={(Editor^.FileName='') or }MatchesFileList(NameAndExtOf(Editor^.FileName),TabsPattern);
end;

constructor TIDEApp.Init;
var R: TRect;
begin

  mysettings;
  displaymode:=dmIDE;
  UseSyntaxHighlight:=@IDEUseSyntaxHighlight;
  UseTabsPattern:=@IDEUseTabsPattern;
  inherited Init;
  InitAdvMsgBox;
  InsideDone:=false;
  IsRunning:=true;

  MenuBar^.GetBounds(R); R.A.X:=R.B.X-8;
  New(ClockView, Init(R));
  ClockView^.GrowMode:=gfGrowLoX+gfGrowHiX;
  Application^.Insert(ClockView);

  New(ClipboardWindow, Init);
  Desktop^.Insert(ClipboardWindow);
  New(CalcWindow, Init); CalcWindow^.Hide;
  Desktop^.Insert(CalcWindow);
  New(CompilerMessageWindow, Init);
  CompilerMessageWindow^.Hide;
  Desktop^.Insert(CompilerMessageWindow);
  Message(@Self,evBroadcast,cmUpdate,nil);
  CurDirChanged;
  { heap viewer }
  GetExtent(R); Dec(R.B.X); R.A.X:=R.B.X-9; R.A.Y:=R.B.Y-1;
  New(HeapView, InitKb(R));
  if (StartupOptions and soHeapMonitor)=0 then HeapView^.Hide;
  Insert(HeapView);
  Drivers.ShowMouse;
{$ifdef Windows}
  // WindowsShowMouse;
{$endif Windows}
  // about;

end;

procedure TIDEApp.mysettings;
begin
  termparams.port:= 0;		//   /dev/ttyUSB0
  termparams.baud:= 7; 		//   19200 Bd
  termparams.datab:= 1;		//	 8 Datenbits
  termparams.stopb:= 0;		//	 1 Stopbit
  termparams.parity:= 0;	//   keine Paritaet
  termparams.misc:= 0;
  termparams.rkey:= 1;		//   CR - only (kein LF)
end;

procedure TIDEApp.InitDesktop;
var
  R: TRect;
begin
  GetExtent(R);
  Inc(R.A.Y);
  Dec(R.B.Y);
  Desktop:=New(PFPDesktop, Init(R));
end;

procedure TIDEApp.LoadMenuBar;

var R: TRect;

begin
  GetExtent(R); R.B.Y:=R.A.Y+1;
{$ifdef WinClipSupported}
  if WinClipboardSupported then
    WinPMI:=NewLine(
      NewItem(menu_edit_copywin,'', kbNoKey, cmCopyWin, hcCopyWin,
      NewItem(menu_edit_pastewin,'', kbNoKey, cmPasteWin, hcPasteWin,
      nil)));
{$endif WinClipSupported}
  MenuBar:=New(PAdvancedMenuBar, Init(R, NewMenu(

    NewSubMenu(menu_file,hcFileMenu, NewMenu(
      NewItem('~N~ew','F4',kbF4,cmNew,hcNew,
      NewItem(menu_file_template,'',kbNoKey,cmNewFromTemplate,hcNewFromTemplate,
      NewItem(menu_file_open,menu_key_file_open,kbF3,cmOpen,hcOpen,
      NewItem(menu_file_reload,'',kbNoKey,cmDoReload,hcDoReload,
      NewItem(menu_file_save,menu_key_file_save,kbF2,cmSave,hcSave,
      NewItem(menu_file_saveas,'',kbNoKey,cmSaveAs,hcSaveAs,
      NewItem(menu_file_saveall,'',kbNoKey,cmSaveAll,hcSaveAll,
      NewLine(
      NewItem(menu_file_print,'',kbNoKey,cmPrint,hcPrint,
      NewItem(menu_file_printsetup,'',kbNoKey,cmPrinterSetup,hcPrinterSetup,
      NewLine(
      NewItem(menu_file_changedir,'',kbNoKey,cmChangeDir,hcChangeDir,
      NewItem(menu_file_dosshell,'',kbNoKey,cmDOSShell,hcDOSShell,
      NewItem(menu_file_exit,menu_key_file_exit,kbNoKey,cmQuit,hcQuit,
      nil))))))))))))))),

    NewSubMenu(menu_edit,hcEditMenu, NewMenu(
      NewItem(menu_edit_undo,menu_key_edit_undo, kbAltBack, cmUndo, hcUndo,
      NewItem(menu_edit_redo,'', kbNoKey, cmRedo, hcRedo,
      NewLine(
      NewItem(menu_edit_cut,menu_key_edit_cut, cut_key, cmCut, hcCut,
      NewItem(menu_edit_copy,menu_key_edit_copy, copy_key, cmCopy, hcCut,
      NewItem(menu_edit_paste,menu_key_edit_paste, paste_key, cmPaste, hcPaste,
      NewItem(menu_edit_clear,menu_key_edit_clear, kbCtrlDel, cmClear, hcClear,
      NewItem(menu_edit_selectall,'', kbNoKey, cmSelectAll, hcSelectAll,
      NewItem(menu_edit_unselect,'', kbNoKey, cmUnselect, hcUnselect,
      NewLine(
      NewItem(menu_edit_showclipboard,'', kbNoKey, cmShowClipboard, hcShowClipboard,
      NewLine(
      NewSubMenu('Options', hcEnvironmentMenu, NewMenu(
        NewItem(menu_options_env_preferences,'', kbNoKey, cmPreferences, hcPreferences,
        NewItem(menu_options_env_editor,'', kbNoKey, cmEditor, hcEditor,
        NewItem(menu_options_env_keybmouse,'', kbNoKey, cmMouse, hcMouse,
        NIL)))),
      NIL)))))))))
      ))))),

    NewSubMenu(menu_search,hcSearchMenu, NewMenu(
      NewItem(menu_search_find,'', kbNoKey, cmFind, hcFind,
      NewItem(menu_search_replace,'', kbNoKey, cmReplace, hcReplace,
      NewItem(menu_search_searchagain,'', kbNoKey, cmSearchAgain, hcSearchAgain,
      NewLine(
      NewItem(menu_search_jumpline,'', kbNoKey, cmJumpLine, hcGotoLine,
      nil)))))),

    NewSubMenu('~C~all Make', 0, NewMenu(              
      NewItem('make ~a~ll', 'F9', kbF9, cmmakeall, hcNoContext,
      NewItem('make ~f~lash','F8', kbF8, cmmakeflash, hcNoContext,
      NewItem('make clea~n~', '', kbnokey, cmmakeclean, hcNoContext,
      NewLine(
      NewItem('make ~r~eset', '', kbnokey, cmmakereset, hcNoContext,
      NewLine(
      NewItem('Compiler me~s~sages','F12', kbf12, cmCompilerMessages, hcCompilerMessages,
      nil)))))))),

    NewSubMenu('~R~S232-Monitor',0,NewMenu(
      NewItem('~O~pen Monitor','',kbNoKey,cmrsstart,hcNoContext,
      NewLine(
      NewItem('S~e~ttings','',kbNoKey,cmsettermset,hcNoContext,
      nil)))),

    NewSubMenu(menu_window, hcWindowMenu, NewMenu(
      NewItem(menu_window_tile,'', kbNoKey, cmTile, hcTile,
      NewItem(menu_window_cascade,'', kbNoKey, cmCascade, hcCascade,
      NewItem(menu_window_closeall,'', kbNoKey, cmCloseAll, hcCloseAll,
      NewLine(
      NewItem(menu_window_resize,menu_key_window_resize, kbCtrlF5, cmResize, hcResize,
      NewItem(menu_window_zoom,menu_key_window_zoom, kbF5, cmZoom, hcZoom,
      NewItem(menu_window_next,menu_key_window_next, kbF6, cmNext, hcNext,
      NewItem(menu_window_previous,menu_key_window_previous, kbShiftF6, cmPrev, hcPrev,
      NewItem(menu_window_hide,menu_key_window_hide, kbCtrlF6, cmHide, hcHide,
      NewItem(menu_window_close,menu_key_window_close, kbAltF3, cmClose, hcClose,
      NewLine(
      NewItem(menu_window_list,menu_key_window_list, kbAlt0, cmWindowList, hcWindowList,
      NewItem(menu_window_update,'', kbNoKey, cmUpdate, hcUpdate,
      nil)))))))))))))),

    NewSubMenu('~U~tils', hcToolsMenu, NewMenu(
      NewItem(menu_tools_calculator, '', kbNoKey, cmCalculator, hcCalculator,
      NewItem(menu_tools_asciitable, '', kbNoKey, cmAsciiTable, hcAsciiTable,
      NewItem('Ca~l~endar', '', kbNoKey, cmcalendar, hcnocontext,
      NewLine(
      NewItem('~M~idnight Commander', '', kbf7, cmcallmc, hcnocontext,
      NewLine(
      NewItem('L~i~st Com-Ports', '', kbNoKey, cmlistports, hcnocontext,
      NewItem('List ~U~SB-Devices', '', kbNoKey, cmlistusb, hcnocontext,
      NewItem('Date & Time', '', kbNoKey, cmdatetime, hcnocontext,
      nil)))))))))),

    NewSubMenu(menu_help, hcHelpMenu, NewMenu(
      NewItem(menu_help_contents,'', kbNoKey, cmHelpContents, hcHelpContents,
      NewItem(menu_help_index,menu_key_help_helpindex, kbShiftF1, cmHelpIndex, hcHelpIndex,
      NewItem(menu_help_topicsearch,menu_key_help_topicsearch, kbCtrlF1, cmHelpTopicSearch, hcHelpTopicSearch,
      NewItem(menu_help_prevtopic,menu_key_help_prevtopic, kbAltF1, cmHelpPrevTopic, hcHelpPrevTopic,
      NewItem(menu_help_using,'',kbNoKey, cmHelpUsingHelp, hcHelpUsingHelp,
      NewItem(menu_help_files,'',kbNoKey, cmHelpFiles, hcHelpFiles,
      NewLine(
      NewItem(menu_help_about,'',kbNoKey, cmAbout, hcAbout,
      nil))))))))),

    nil)))))))))));

   SetCmdState(ToClipCmds+FromClipCmds+NulClipCmds+UndoCmd+RedoCmd,false);
end;

procedure TIDEApp.InitMenuBar;

begin
  LoadMenuBar;
  DisableCommands(EditorCmds+SourceCmds+CompileCmds);
  // Update; Desktop is still nil at that point ...
end;

procedure TIDEApp.reload_menubar;

begin
   delete(menubar);
   dispose(menubar,done);
   case EditKeys of
     ekm_microsoft:
       begin
         menu_key_edit_cut:=menu_key_edit_cut_microsoft;
         menu_key_edit_copy:=menu_key_edit_copy_microsoft;
         menu_key_edit_paste:=menu_key_edit_paste_microsoft;
         menu_key_hlplocal_copy:=menu_key_hlplocal_copy_microsoft;
         cut_key:=kbCtrlX;
         copy_key:=kbCtrlC;
         paste_key:=kbCtrlV;
       end;
     ekm_borland:
       begin
         menu_key_edit_cut:=menu_key_edit_cut_borland;
         menu_key_edit_copy:=menu_key_edit_copy_borland;
         menu_key_edit_paste:=menu_key_edit_paste_borland;
         menu_key_hlplocal_copy:=menu_key_hlplocal_copy_borland;
         cut_key:=kbShiftDel;
         copy_key:=kbCtrlIns;
         paste_key:=kbShiftIns;
       end;
   end;
   loadmenubar;
   insert(menubar);
end;

procedure TIDEApp.InitStatusLine;
var
  R: TRect;
begin
  GetExtent(R);
  R.A.Y := R.B.Y - 1;
  StatusLine:=New(PIDEStatusLine, Init(R,
      NewStatusDef(0, $EFFF,
        NewStatusKey('~F1~ Help', kbF1,cmrshelp,
        NewStatusKey('~F3~ Open', kbF3, cmOpen,
        NewStatusKey('~F4~ New', kbF4, cmNew,
        NewStatusKey('~Alt+F3~ Close', kbAltF3, cmClose,
        NewStatusKey('~ALT+F7~ M.Commander', kbaltF7, cmcallmc,
        NewStatusKey('~F10~ Menu', kbF10,cmmenu,
        NewStatusKey('~F12~ Messages', kbF12,cmcompilermessages,
        NewStatusKey('~Alt+X~ exit', kbAltx, cmquit,
{$ifdef mytest}
        NewStatusKey('Test', kbnokey, cmtestfunction,
{$endif}
        stdstatuskeys(nil)
{$ifdef mytest}
        )
{$endif}
        )))))))),nil)
      )
    );

end;

procedure TIDEApp.settermpara;
var
  Dlg     : PDialog;
  R       : TRect;
  Control : PView;
  c       : word;

  begin
//  if TOutWindow.getstate then exit;

  R.Assign(12, 1, 80, 17);
  New(Dlg, Init(R, 'RS-232 Settings'));

  R.Assign(3, 2, 23, 14);
  Control := New(PRadioButtons, Init(R,
    NewSItem('ttyUSB0',
    NewSItem('ttyUSB1',
    NewSItem('ttyACM0',    
    NewSItem('ttyACM1',    
    NewSItem('ttyS0 | COM1 ',
    NewSItem('ttyS1 | COM2 ',
    NewSItem('ttyS2 | COM3 ',
    NewSItem('ttyS3 | COM4 ',
    NewSItem('ttyS4 | COM5 ',
    NewSItem('ttyS5 | COM6 ',
    NewSItem('ttyS6 | COM7 ',
    NewSItem('ttyS7 | COM8 ', Nil))))))))))))));
  Dlg^.Insert(Control);
    R.Assign(4, 1, 16, 2);
    Dlg^.Insert(New(PLabel, Init(R, 'Serial-Port', Control)));

  R.Assign(25, 2, 37, 14);
  Control := New(PRadioButtons, Init(R,
    NewSItem('150',
    NewSItem('300',
    NewSItem('600',
    NewSItem('1200',
    NewSItem('2400',
    NewSItem('4800',
    NewSItem('9600',
    NewSItem('19200',
    NewSItem('28800',
    NewSItem('38400',
    NewSItem('57600',
    NewSItem('115200', Nil))))))))))))));
  Dlg^.Insert(Control);
    R.Assign(24, 1, 33, 2);
    Dlg^.Insert(New(PLabel, Init(R, 'Baudrate', Control)));

  R.Assign(39, 2, 49, 4);
  Control := New(PRadioButtons, Init(R,
    NewSItem('7',
    NewSItem('8', Nil))));
  Dlg^.Insert(Control);
    R.Assign(38, 1, 47, 2);
    Dlg^.Insert(New(PLabel, Init(R, 'Databits', Control)));

  R.Assign(39, 6, 49, 8);
  Control := New(PRadioButtons, Init(R,
    NewSItem('1',
    NewSItem('2', Nil))));
  Dlg^.Insert(Control);
    R.Assign(38, 5, 47, 6);
    Dlg^.Insert(New(PLabel, Init(R, 'Stopbits', Control)));

  R.Assign(39, 11, 49, 14);
  Control := New(PRadioButtons, Init(R,
    NewSItem('none',
    NewSItem('even',
    NewSItem('odd', Nil)))));
  Dlg^.Insert(Control);
    R.Assign(38, 10, 45, 11);
    Dlg^.Insert(New(PLabel, Init(R, 'Parity', Control)));

  R.Assign(51, 2, 65, 4);
  Control := New(PCheckboxes, Init(R,
    NewSItem('ECHO',
    NewSItem('FEEDBACK', Nil))));
  Dlg^.Insert(Control);
    R.Assign(50, 1, 55, 2);
    Dlg^.Insert(New(PLabel, Init(R, 'Misc', Control)));

  R.Assign(51, 6, 65, 8);
  Control := New(PRadioButtons, Init(R,
    NewSItem('CR + LF',
    NewSItem('CR', Nil))));
  Dlg^.Insert(Control);
    R.Assign(50, 5, 61, 6);
    Dlg^.Insert(New(PLabel, Init(R, 'Return key', Control)));

  R.Assign(53, 10, 63, 12);
  Control := New(PButton, Init(R, 'Cancel', cmcancel, bfnormal));
    Dlg^.Insert(Control);


  R.Assign(53, 13, 63, 15);
  Control := New(PButton, Init(R, 'O~K~', cmOK, bfDefault));
    Dlg^.Insert(Control);

  Dlg^.SelectNext(False);

  dlg^.setdata(termparams);
  c:= desktop^.execview(dlg);
  if (c= cmok) then
  begin
    dlg^.getdata(termparams);      // Einstellungen speichern
    if (termparams.port-3 > getmaxports) then
    begin
      mymessage('The selected Port is not'#13+
                #3'available on this computer.');
    end;
    if termparams.port= 0 then
	  if not(existsfile('/dev/ttyUSB0')) then
	  begin
	    mymessage('The selected Port is not'#13+
	              #3'available on this computer.');	    
	  end;
	if termparams.port= 1 then
	  if not(existsfile('/dev/ttyUSB1')) then
	  begin
	    mymessage('The selected Port is not'#13+
	              #3'available on this computer.');	     
      end;            
    if termparams.port= 2 then
	  if not(existsfile('/dev/ttyACM0')) then
	  begin
	    mymessage('The selected Port is not'#13+
	              #3'available on this computer.');	    
	  end;
	if termparams.port= 3 then
	  if not(existsfile('/dev/ttyACM1')) then
	  begin
	    mymessage('The selected Port is not'#13+
	              #3'available on this computer.');	     
      end;        
  end;
end;


procedure TIDEApp.GetEvent(var Event: TEvent);
var P: PView;
begin
  { first of all dispatch queued targeted events }
  while GetTargetedEvent(P,Event) do
    P^.HandleEvent(Event);
  { Handle System events directly }
  Drivers.GetSystemEvent(Event);         { Load system event }
  If (Event.What <> evNothing) Then
    HandleEvent(Event);

  inherited GetEvent(Event);
{$ifdef DEBUG}
  if (Event.What=evKeyDown) and (Event.KeyCode=kbAltF11) then
    begin
{$ifdef HasSignal}
      Generate_SIGSEGV;
{$else}
      Halt(1);
{$endif}
    end;
  if (Event.What=evKeyDown) and (Event.KeyCode=kbCtrlF11) then
    begin
      RunError(250);
    end;
{$endif DEBUG}
  if (Event.What=evKeyDown) and (Event.KeyCode=kbAltF12) then
    begin
      CreateAnsiFile;
      ClearEvent(Event);
    end;
  if Event.What<>evNothing then
    LastEvent:=GetDosTicks
  else
    begin
      if abs(GetDosTicks-LastEvent)>SleepTimeOut then
        GiveUpTimeSlice;
    end;
end;

procedure TIDEApp.HandleEvent(var Event: TEvent);
var 
  DontClear: boolean;
  TempS    : string;
  ForceDlg : boolean;
  W        : PSourceWindow;
  DS       : DirStr;
  NS       : NameStr;
  ES       : ExtStr;
  hs       : string;
  p        : psourcewindow;
  olddir   : string;

{$ifdef HasSignal}
    CtrlCCatched : boolean;
{$endif HasSignal}
begin
{$ifdef HasSignal}
  if (Event.What=evKeyDown) and (Event.keyCode=kbCtrlC) and
     (CtrlCPressed) then
    begin
      CtrlCCatched:=true;
{$ifdef DEBUG}
      Writeln(stderr,'One Ctrl-C caught');
{$endif DEBUG}
    end
  else
    CtrlCCatched:=false;
{$endif HasSignal}
  case Event.What of
       evKeyDown :
         begin
           DontClear:=true;
           { just for debugging purposes }
         end;
       evCommand :
         begin
           DontClear:=false;
           case Event.Command of
             cmUpdate        : Message(Application,evBroadcast,cmUpdate,nil);
           { -- File menu -- }
             cmNew           : NewEditor;
             cmNewFromTemplate: NewFromTemplate;
             cmOpen          : begin
							     olddir:= getcurrentdir;
                                 ForceDlg:=false;
                                 if (OpenFileName<>'') and
                                    ((DirOf(OpenFileName)='') or (Pos(ListSeparator,OpenFileName)<>0)) then
                                   begin
                                     TempS:=LocateSourceFile(OpenFileName,false);
                                     if TempS='' then
                                       ForceDlg:=true
                                     else
                                       OpenFileName:=TempS;
                                   end;
                                 if ForceDlg then
                                   OpenSearch(OpenFileName)
                                 else
                                   begin
                                     W:=LastSourceEditor;
                                     if assigned(W) then
                                       FSplit(W^.Editor^.FileName,DS,NS,ES)
                                     else
                                       DS:='';
                                     Open(OpenFileName,DS);
                                   end;
                                 OpenFileName:='';
                                 chdir(olddir);
                               end;
             cmPrint         : Print;
             cmPrinterSetup  : PrinterSetup;
             cmSaveAll       : SaveAll;
             cmChangeDir     : ChangeDir;
             cmDOSShell      : DOSShell;
             cmRecentFileBase..
             cmRecentFileBase+10
                             : OpenRecentFile(Event.Command-cmRecentFileBase);
           { -- Edit menu -- }
             cmShowClipboard : ShowClipboard;
           { -- Search menu -- }
             cmFindProcedure : FindProcedure;
             cmObjects       : Objects;
             cmModules       : Modules;
             cmGlobals       : Globals;
             cmSymbol        : SearchSymbol;
           { -- Run menu -- }
             cmRunDir        : RunDir;
             cmParameters    : Parameters;
             cmStepOver      : DoStepOver;
             cmTraceInto     : DoTraceInto;
             cmRun,
             cmContinue      : DoRun;
             cmResetDebugger : DoResetDebugger;
             cmContToCursor  : DoContToCursor;
             cmUntilReturn   : DoContUntilReturn;
           { -- Compile menu -- }
             cmCompile       : DoCompile(cCompile);
             cmBuild         : DoCompile(cBuild);
             cmMake          : DoCompile(cMake);
             cmTarget        : Target;
             cmPrimaryFile   : DoPrimaryFile;
             cmClearPrimary  : DoClearPrimary;
             cmCompilerMessages : showcompwindow;

           { -- AVR compile -- }
             cmmakeall       : callmake('make all');
             cmmakeclean     : callmake('make clean');
             cmmakeflash     : callmake('make flash');
             cmmakereset     : callmake('make reset');
             cmsetmcu        : testfunktion;

             cmsettermset    : settermpara;
            
			 { -- RS232 Terminal -- }
             cmrsstart      : StartUart;
             cmrsstop       : begin
                              end;

             cmtestfunction : testfunktion;


           { -- Options menu -- }
             cmSwitchesMode  : SetSwitchesMode;
             cmCompiler      : DoCompilerSwitch;
             cmMemorySizes   : MemorySizes;
             cmLinker        : DoLinkerSwitch;
             cmDebugger      : DoDebuggerSwitch;
{$ifdef SUPPORT_REMOTE}
             cmRemoteDialog  : DoRemote;
             cmTransferRemote: TransferRemote;
{$endif SUPPORT_REMOTE}
             cmDirectories   : Directories;
             cmTools         : Tools;
             cmPreferences   : Preferences;
             cmEditor        : EditorOptions(nil);
             cmEditorOptions : EditorOptions(Event.InfoPtr);
             cmCodeTemplateOptions: CodeTemplates;
             cmCodeCompleteOptions: CodeComplete;
             cmBrowser       : BrowserOptions(nil);
             cmBrowserOptions : BrowserOptions(Event.InfoPtr);
             cmMouse         : Mouse;
             cmStartup       : StartUp;
             cmDesktopOptions: DesktopOptions;
             cmColors        : Colors;
{$ifdef Unix}
             cmKeys          : LearnKeysDialog;
{$endif Unix}
             cmOpenINI       : OpenINI;
             cmSaveINI       : SaveINI;
             cmSaveAsINI     : SaveAsINI;
           { -- Tools menu -- }
             cmToolsMessages : Messages;
             cmCalculator    : Calculator;
             cmcalendar		 : calendar;
             cmAsciiTable    : DoAsciiTable;
             cmListPorts     : ListPorts;
             cmListUsb		 : ListUSB;
             cmcallmc		 : callmidnight;
             cmdatetime      : getutcdate;

           { -- Window menu -- }
             cmCloseAll      : CloseAll;
             cmWindowList    : WindowList;
             cmUserScreenWindow: DoUserScreenWindow;

           { -- Help menu -- }
             cmHelp          : HelpContents;
             cmHelpContents  : HelpContents;
             cmHelpIndex     : HelpHelpIndex;
             cmHelpDebug     : HelpDebugInfos;
             cmHelpTopicSearch: HelpTopicSearch;
             cmHelpPrevTopic : HelpPrevTopic;
             cmHelpUsingHelp : HelpUsingHelp;
             cmHelpFiles     : HelpFiles;
             cmAbout         : About;
             cmShowReadme    : ShowReadme;
             cmResizeApp     : ResizeApplication(Event.Id, Event.InfoWord);
             cmQuitApp       : Message(@Self, evCommand, cmQuit, nil);
//             cmrshelp	     : rshelp;
             cmrshelp	     : helpcontents;

           else DontClear:=true;
           end;
           if DontClear=false then ClearEvent(Event);
         end;
       evBroadcast :
         case Event.Command of
           cmSaveCancelled :
             SaveCancelled:=true;
           cmUpdateTools :
             UpdateTools;
           cmCommandSetChanged :
             UpdateMenu(MenuBar^.Menu);
           cmUpdate              :
             Update;
           cmSourceWndClosing :
             begin
               with PSourceWindow(Event.InfoPtr)^ do
                 if Editor^.FileName<>'' then
                   AddRecentFile(Editor^.FileName,Editor^.CurPos.X,Editor^.CurPos.Y);
               {$ifndef NODEBUG}
               if assigned(Debugger) and (PView(Event.InfoPtr)=Debugger^.LastSource) then
                 Debugger^.LastSource:=nil;
               {$endif}
             end;

         end;
  end;
  inherited HandleEvent(Event);
{$ifdef HasSignal}
  { Reset flag if CrtlC was handled }
  if CtrlCCatched and (Event.What=evNothing) then
    begin
      CtrlCPressed:=false;
{$ifdef DEBUG}
      Writeln(stderr,'One CtrlC handled');
{$endif DEBUG}
    end;
{$endif HasSignal}
end;


procedure TIDEApp.GetTileRect(var R: TRect);
begin
  Desktop^.GetExtent(R);
{ Leave the compiler messages window in the bottom }
  if assigned(CompilerMessageWindow) and (CompilerMessageWindow^.GetState(sfVisible)) then
   R.B.Y:=Min(CompilerMessageWindow^.Origin.Y,R.B.Y);
{ Leave the messages window in the bottom }
  if assigned(MessagesWindow) and (MessagesWindow^.GetState(sfVisible)) then
   R.B.Y:=Min(MessagesWindow^.Origin.Y,R.B.Y);
{$ifndef NODEBUG}
{ Leave the watch window in the bottom }
  if assigned(WatchesWindow) and (WatchesWindow^.GetState(sfVisible)) then
   R.B.Y:=Min(WatchesWindow^.Origin.Y,R.B.Y);
{$endif NODEBUG}
end;


{****************************************************************************
                                 Switch Screens
****************************************************************************}

procedure TIDEApp.ShowUserScreen;
begin
  displaymode:=dmUser;
  if Assigned(UserScreen) then
    UserScreen^.SaveIDEScreen;
  DoneSysError;
  DoneEvents;
  { DoneKeyboard should be called last to
    restore the keyboard correctly PM }
{$ifndef go32v2}
  donevideo;
{$endif ndef go32v2}
  DoneKeyboard;
  If UseMouse then
    DoneMouse
  else
    ButtonCount:=0;
{  DoneDosMem;}

  if Assigned(UserScreen) then
    UserScreen^.SwitchToConsoleScreen;
end;


procedure TIDEApp.ShowIDEScreen;
begin
  if Assigned(UserScreen) then
    UserScreen^.SaveConsoleScreen;
{  InitDosMem;}
  InitKeyboard;
  If UseMouse then
    InitMouse
  else
    ButtonCount:=0;
{$ifndef go32v2}
  initvideo;
{$endif ndef go32v2}
  {Videobuffer has been reallocated, need passive video situation detection
   again.}
  initscreen;
{$ifdef Windows}
  { write the empty screen to dummy console handle }
  UpdateScreen(true);
{$endif ndef Windows}
  InitEvents;
  InitSysError;
  CurDirChanged;
{$ifndef Windows}
  Message(Application,evBroadcast,cmUpdate,nil);
{$endif Windows}
{$ifdef Windows}
  // WindowsShowMouse;
{$endif Windows}

  if Assigned(UserScreen) then
    UserScreen^.SwitchBackToIDEScreen;
  Video.SetCursorType(crHidden);
{$ifdef Windows}
  { This message was sent when the VideoBuffer was smaller
    than was the IdeApp thought => writes to random memory and random crashes... PM }
  Message(Application,evBroadcast,cmUpdate,nil);
{$endif Windows}
{$ifdef Unix}
  SetKnownKeys;
{$endif Unix}
 {$ifndef Windows}
{$ifndef go32v2}
  UpdateScreen(true);
{$endif go32v2}
{$endif Windows}
  displaymode:=dmIDE;
end;

function TIDEApp.AutoSave: boolean;
var IOK,SOK,DOK: boolean;
begin
  IOK:=true; SOK:=true; DOK:=true;
  if (AutoSaveOptions and asEnvironment)<>0 then
    begin
      IOK:=WriteINIFile(false);
      if IOK=false then
        ErrorBox(error_saving_cfg_file,nil);
    end;
  if (AutoSaveOptions and asEditorFiles)<>0 then { was a typo here ("=0") - Gabor }
      SOK:=SaveAll;
  if (AutoSaveOptions and asDesktop)<>0 then
    begin
      { destory all help & browser windows - we don't want to store them }
      { UserScreenWindow is also not registered PM }
      DoCloseUserScreenWindow;
      {$IFNDEF NODEBUG}
      DoneDisassemblyWindow;
      {$ENDIF}
      CloseHelpWindows;
      CloseAllBrowsers;
      DOK:=SaveDesktop;
      if DOK=false then
        ErrorBox(error_saving_dsk_file,nil);
    end;
  AutoSave:=IOK and SOK and DOK;
end;

function TIDEApp.DoExecute(ProgramPath, Params, InFile,OutFile,ErrFile: string; ExecType: TExecType): boolean;
var CanRun: boolean;
    ConsoleMode : TConsoleMode;
{$ifndef Unix}
    PosExe: sw_integer;
{$endif Unix}
begin
  SaveCancelled:=false;
  CanRun:=AutoSave;
  if (CanRun=false) and (SaveCancelled=false) then
    CanRun:=true; { do not care about .DSK or .INI saving problems - just like TP }
  if CanRun then
  begin
    if UserScreen=nil then
     begin
       ErrorBox(error_user_screen_not_avail,nil);
       Exit;
     end;

    if ExecType<>exNoSwap then
      ShowUserScreen;
    SaveConsoleMode(ConsoleMode);

    if ExecType=exDosShell then
      WriteShellMsg
    else if ExecType<>exNoSwap then
      Writeln('Running "'+ProgramPath+' '+Params+'"');
     { DO NOT use COMSPEC for exe files as the
      ExitCode is lost in those cases PM }

{$ifndef Unix}
    posexe:=Pos('.EXE',UpCaseStr(ProgramPath));
    { if programpath was three char long => bug }
    if (posexe>0) and (posexe=Length(ProgramPath)-3) then
      begin
{$endif Unix}
        if (InFile='') and (OutFile='') and (ErrFile='') then
          DosExecute(ProgramPath,Params)
        else
          begin
            if ErrFile='' then
              ErrFile:='stderr';
            ExecuteRedir(ProgramPath,Params,InFile,OutFile,ErrFile);
          end;
{$ifndef Unix}
      end
    else if (InFile='') and (OutFile='') and (ErrFile='') then
      DosExecute(GetEnv('COMSPEC'),'/C '+ProgramPath+' '+Params)
    else
      begin
        if ErrFile='' then
          ErrFile:='stderr';
        ExecuteRedir(GetEnv('COMSPEC'),'/C '+ProgramPath+' '+Params,
          InFile,OutFile,ErrFile);
     end;
{$endif Unix}

{$ifdef Unix}
    if (DebuggeeTTY='') and (OutFile='') and (ExecType<>exDosShell) then
      begin
        Write(' Press any key to return to IDE');
        InitKeyBoard;
        Keyboard.GetKeyEvent;
        while (Keyboard.PollKeyEvent<>0) do
         Keyboard.GetKeyEvent;
        DoneKeyboard;
      end;
{$endif}
    RestoreConsoleMode(ConsoleMode);
    if ExecType<>exNoSwap then
      ShowIDEScreen;
  end;
  DoExecute:=CanRun;
end;


procedure TIDEApp.Update;
begin
  SetCmdState([cmSaveAll],IsThereAnyEditor);
  SetCmdState([cmCloseAll,cmWindowList],IsThereAnyWindow);
  SetCmdState([cmTile,cmCascade],IsThereAnyVisibleWindow);
  SetCmdState([cmFindProcedure,cmObjects,cmModules,cmGlobals,cmSymbol],IsSymbolInfoAvailable);
{$ifndef NODEBUG}
  SetCmdState([cmResetDebugger,cmUntilReturn],assigned(debugger) and debugger^.debuggee_started);
{$endif}
  SetCmdState([cmToolsMsgNext,cmToolsMsgPrev],MessagesWindow<>nil);
  UpdateTools;
  UpdateRecentFileList;
  UpdatePrimaryFile;
  UpdateINIFile;
  Message(Application,evBroadcast,cmCommandSetChanged,nil);
  application^.redraw;
end;

procedure TIDEApp.SourceWindowClosed;
begin
  if not IsClosing then
    Update;
end;

procedure TIDEApp.CurDirChanged;
begin
  Message(Application,evBroadcast,cmUpdateTitle,nil);
  UpdatePrimaryFile;
  UpdateINIFile;
  UpdateMenu(MenuBar^.Menu);
end;


procedure TIDEApp.UpdatePrimaryFile;
begin
  SetMenuItemParam(SearchMenuItem(MenuBar^.Menu,cmPrimaryFile),SmartPath(PrimaryFile));
  SetCmdState([cmClearPrimary],PrimaryFile<>'');
  if PrimaryFile<>'' then
     SetCmdState(CompileCmds,true);
  UpdateMenu(MenuBar^.Menu);
end;

procedure TIDEApp.UpdateINIFile;
begin
  SetMenuItemParam(SearchMenuItem(MenuBar^.Menu,cmSaveINI),SmartPath(IniFileName));
end;

procedure TIDEApp.UpdateRecentFileList;
var P: PMenuItem;
    {ID,}I: word;
    FileMenu: PMenuItem;
begin
{  ID:=cmRecentFileBase;}
  FileMenu:=SearchSubMenu(MenuBar^.Menu,menuFile);
  repeat
{    Inc(ID);
    P:=SearchMenuItem(FileMenu^.SubMenu,ID);
    if FileMenu^.SubMenu^.Default=P then
      FileMenu^.SubMenu^.Default:=FileMenu^.SubMenu^.Items;
    if P<>nil then RemoveMenuItem(FileMenu^.SubMenu,P);}
    P:=GetMenuItemBefore(FileMenu^.SubMenu,nil);
    if (P<>nil) then
    begin
      if (cmRecentFileBase<P^.Command) and (P^.Command<=cmRecentFileBase+MaxRecentFileCount) then
        begin
          RemoveMenuItem(FileMenu^.SubMenu,P);
          if FileMenu^.SubMenu^.Default=P then
            FileMenu^.SubMenu^.Default:=FileMenu^.SubMenu^.Items;
        end
      else
        P:=nil;
    end;
  until P=nil;
  P:=GetMenuItemBefore(FileMenu^.SubMenu,nil);
  if (P<>nil) and IsSeparator(P) then
     RemoveMenuItem(FileMenu^.SubMenu,P);

  if RecentFileCount>0 then
     AppendMenuItem(FileMenu^.SubMenu,NewLine(nil));
  for I:=1 to RecentFileCount do
  begin
    P:=NewItem('~'+IntToStr(I)+'~ '+ShrinkPath(SmartPath(RecentFiles[I].FileName),27),' ',
        kbNoKey,cmRecentFileBase+I,hcRecentFileBase+I,nil);
    AppendMenuItem(FileMenu^.SubMenu,P);
  end;
end;

procedure TIDEApp.UpdateTools;
var P: PMenuItem;
{    ID,}I: word;
    ToolsMenu: PMenuItem;
    S1,S2,S3: string;
    W: word;
begin
{  ID:=cmToolsBase;}
  ToolsMenu:=SearchSubMenu(MenuBar^.Menu,menuTools);
  repeat
    P:=GetMenuItemBefore(ToolsMenu^.SubMenu,nil);
    if (P<>nil) then
    begin
      if (cmToolsBase<P^.Command) and (P^.Command<=cmToolsBase+MaxToolCount) then
        begin
          RemoveMenuItem(ToolsMenu^.SubMenu,P);
          if ToolsMenu^.SubMenu^.Default=P then
            ToolsMenu^.SubMenu^.Default:=ToolsMenu^.SubMenu^.Items;
        end
      else
        P:=nil;
    end;
  until P=nil;
  P:=GetMenuItemBefore(ToolsMenu^.SubMenu,nil);
  if (P<>nil) and IsSeparator(P) then
     RemoveMenuItem(ToolsMenu^.SubMenu,P);

  if GetToolCount>0 then
     AppendMenuItem(ToolsMenu^.SubMenu,NewLine(nil));
  for I:=1 to GetToolCount do
  begin
    GetToolParams(I-1,S1,S2,S3,W);
    P:=NewItem(S1,KillTilde(GetHotKeyName(W)),W,cmToolsBase+I,hcToolsBase+I,nil);
    AppendMenuItem(ToolsMenu^.SubMenu,P);
  end;
end;

procedure TIDEApp.DosShell;
var
  s : string;
begin
{$ifdef Unix}
  s:=GetEnv('SHELL');
  if s='' then
    if ExistsFile('/bin/sh') then
      s:='/bin/sh';
{$else}
  s:=GetEnv('COMSPEC');
  if s='' then
    if ExistsFile('c:\command.com') then
      s:='c:\command.com'
    else
      begin
        s:='command.com';
        if Not LocateExeFile(s) then
          s:='';
      end;
{$endif}
  if s='' then
    ErrorBox(msg_errorexecutingshell,nil)
  else
    DoExecute(s, '', '', '', '', exDosShell);
  { In case we have something that the compiler touched }
  AskToReloadAllModifiedFiles;
end;

procedure TIDEApp.rshelp;
var R,R2: TRect;
    D: PCenterDialog;
    M: PFPMemo;
    VSB: PScrollBar;
    S: PFastBufStream;
begin
  New(S, Init(rshelpfilename, stOpenRead, 4096));
  if S^.Status=stOK then
  begin
    R.Assign(0,0,63,18);
    New(D, Init(R, 'Help'));
    with D^ do
    begin
      GetExtent(R);
      R.Grow(-2,-2); Inc(R.B.Y);
      R2.Copy(R); R2.Move(1,0); R2.A.X:=R2.B.X-1;
      New(VSB, Init(R2)); VSB^.GrowMode:=0; Insert(VSB);
      New(M, Init(R,nil,VSB,nil));
      M^.LoadFromStream(S);
      M^.ReadOnly:=true;
      Insert(M);
    end;
    InsertOK(D);
    ExecuteDialog(D,nil);
  end;
  Dispose(S, Done);
end;

procedure TIDEApp.ShowReadme;
var R,R2: TRect;
    D: PCenterDialog;
    M: PFPMemo;
    VSB: PScrollBar;
    S: PFastBufStream;
begin
  New(S, Init(ReadmeName, stOpenRead, 4096));
  if S^.Status=stOK then
  begin
    R.Assign(0,0,63,18);
    New(D, Init(R, 'AVR IDE'));
    with D^ do
    begin
      GetExtent(R);
      R.Grow(-2,-2); Inc(R.B.Y);
      R2.Copy(R); R2.Move(1,0); R2.A.X:=R2.B.X-1;
      New(VSB, Init(R2)); VSB^.GrowMode:=0; Insert(VSB);
      New(M, Init(R,nil,VSB,nil));
      M^.LoadFromStream(S);
      M^.ReadOnly:=true;
      Insert(M);
    end;
    InsertOK(D);
    ExecuteDialog(D,nil);
  end;
  Dispose(S, Done);
end;

{$I FPMFILE.INC}

{$I FPMEDIT.INC}

{$I FPMSRCH.INC}

{$I FPMRUN.INC}

{$I FPMCOMP.INC}

{$I FPMDEBUG.INC}

{$I FPMTOOLS.INC}

{$I FPMOPTS.INC}

{$I FPMWND.INC}

{$I FPMHELP.INC}

{$I fpmansi.inc}

procedure TIDEApp.AddRecentFile(AFileName: string; CurX, CurY: sw_integer);
begin
  if SearchRecentFile(AFileName)<>-1 then Exit;
  if RecentFileCount>0 then
   Move(RecentFiles[1],RecentFiles[2],SizeOf(RecentFiles[1])*Min(RecentFileCount,High(RecentFiles)-1));
  if RecentFileCount<High(RecentFiles) then Inc(RecentFileCount);
  with RecentFiles[1] do
  begin
    FileName:=AFileName;
    LastPos.X:=CurX; LastPos.Y:=CurY;
  end;
  UpdateRecentFileList;
end;

function TIDEApp.SearchRecentFile(AFileName: string): integer;
var Idx,I: integer;
begin
  Idx:=-1;
  for I:=1 to RecentFileCount do
    if UpcaseStr(AFileName)=UpcaseStr(RecentFiles[I].FileName) then
      begin Idx:=I; Break; end;
  SearchRecentFile:=Idx;
end;

procedure TIDEApp.RemoveRecentFile(Index: integer);
begin
  if Index<RecentFileCount then
     Move(RecentFiles[Index+1],RecentFiles[Index],SizeOf(RecentFiles[1])*(RecentFileCount-Index));
  Dec(RecentFileCount);
  UpdateRecentFileList;
end;

function TIDEApp.GetPalette: PPalette;
begin
  GetPalette:=@AppPalette;
end;

function TIDEApp.IsClosing: Boolean;
begin
  IsClosing:=InsideDone;
end;

procedure TIDEApp.calendar;
var
  p : Pcalendarwindow;
begin
  p:= new(pcalendarwindow, init);
  desktop^.insert(validview(p));
end;

procedure TIDEApp.Idle;

const
  cspeed = 35;
var
  e          : tevent;
  h,m,s,s100 : word;

  function IsTileable(P: PView): Boolean;
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
    
  Message(Application,evIdle,0,nil);
end;

function TIDEApp.getmaxports : byte;
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;
  anz,w,cx		  : integer;
  scall,s         : string;

begin
  scall:= 'dmesg';

  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.stderror);
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  cx:= 0;
  for w:= 0 to (anz-1) do
  begin
    s:= mystringlist.strings[w];
    if (pos('TTYS',uppercasestr(s))> 0) then
    begin
      inc(cx);
    end;
  end;
  getmaxports:= cx;
  myprocess.free;  
end;

procedure TIDEApp.listUSB;
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;
  anz,w 		  : integer;
  scall,s         : string;
begin
  scall:= 'lsusb';
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.output);
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'--------','',1,1);
  for w:= 0 to (anz-1) do
  begin
    s:= mystringlist.strings[w];
    if (pos('ROOT HUB',uppercasestr(s))= 0) then
    begin
      compilermessagewindow^.addmessage(0,s,'',1,1);
    end;
  end;
  DoCompilerMessages;
  myprocess.free;  
end;

procedure TIDEApp.listports;
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;
  anz,w 		  : integer;
  scall,s         : string;
begin
  scall:= 'dmesg';
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.output);
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'Native serial ports:','',1,1);
  compilermessagewindow^.addmessage(0,'--------------------','',1,1);
  
  for w:= 0 to (anz-1) do
  begin
    s:= mystringlist.strings[w];
    if (pos('TTYS',uppercasestr(s))> 0) then
    begin
      compilermessagewindow^.addmessage(0,s,'',1,1);
    end;
  end;
  DoCompilerMessages;
  myprocess.free;    
end;

procedure TIDEApp.getutcdate;
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;
  anz,w 		  : integer;
  scall,s         : string;
begin
  scall:= 'date -u';
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.output);
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;
  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'--------','',1,1);
  for w:= 0 to (anz-1) do
  begin
    s:= 'DATE: ' + mystringlist.strings[w];
    compilermessagewindow^.addmessage(0,s,'',1,1);
  end;
  DoCompilerMessages;
  myprocess.free;  
end;

procedure TIDEApp.callmidnight;
var
  s, cname, sourcedir,
  olddir, na, ex        : string;

begin
  olddir:= getcurrentdir;
  cname:= getmainfile(ccompile);
  fsplit(cname,sourcedir,na,ex);
  chdir(sourcedir);
{$ifdef Unix}
  s:=GetEnv('SHELL');
  if s='' then
    if ExistsFile('/bin/sh') then
      s:='/bin/sh';
{$else}
  s:= 'Midnight commander in Windows not available';
{$endif}
  if s='' then
    ErrorBox(msg_errorexecutingshell,nil)
  else
  DoExecute('mc', '', '', '', '', exDosShell);
  { In case we have something that the compiler touched }
  AskToReloadAllModifiedFiles;

  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,'Midnight Commander closed...','',1,1);
  DoCompilerMessages;
  chdir(olddir);
end;

procedure TIDEApp.StartUart;
const
  brates : array[0..11] of longint =
              (150,300,600,1200,2400,4800,9600,19200,28800,38400,57600,115200);

var
  s, cname, sourcedir,
  olddir, na, ex, s2    : string;

  sport          : string;
  sparity        : char;
  sdbit, ssbit   : byte;
  sbaud          : longint;  
  picoopt        : string;  

begin
  with termparams do
  begin
(*
        baud       : longint;
        datab      : longint;
        stopb      : longint;
        parity     : longint;
        port       : longint;
        misc       : longint;
        rkey       : longint; *)

    if (port> 3) then
    begin
      sport:= '/dev/ttyS'+chr(port-4 + 48);     
    end;
    if (port= 0) then 
    begin
      sport:= '/dev/ttyUSB0';
    end;  
    if (port= 1) then 
    begin
      sport:= '/dev/ttyUSB1';
    end;  
    if (port= 2) then 
    begin
      sport:= '/dev/ttyACM0';
    end;  
    if (port= 3) then 
    begin
      sport:= '/dev/ttyACM1';
    end;      
    
    sbaud:= brates[baud];
    ssbit:= stopb+1;
    sdbit:= datab+7;
    case parity of
      0 : sparity:= 'n';
      1 : sparity:= 'e';
      2 : sparity:= 'o';
    end;    
  end;    
  str(sbaud,s2);
  // b= baudrate , d= databits , p= stopbits y= parity
  picoopt:= '-b '+ s2 +' -d '+chr(sdbit+48) +' -p '+chr(ssbit+48)+' -y '+sparity+' '+sport;

  olddir:= getcurrentdir;
  cname:= getmainfile(ccompile);
  fsplit(cname,sourcedir,na,ex);
  chdir(sourcedir);
{$ifdef Unix}
  s:=GetEnv('SHELL');
  if s='' then
    if ExistsFile('/bin/sh') then
      s:='/bin/sh';
{$else}
  s:= 'PicoCom in Windows not available';
{$endif}
  if s='' then
    ErrorBox(msg_errorexecutingshell,nil)
  else
  DoExecute('picocom', picoopt, '', '', '', exDosShell);
  { In case we have something that the compiler touched }
  AskToReloadAllModifiedFiles;

  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,'PicoCom closed...','',1,1);
  DoCompilerMessages;
  chdir(olddir);
end;

procedure TIDEApp.testfunktion;
var
  D       : PChDirDialog;
  olddir  : string;
  code	  : integer;
  f       : longint;
begin
  getutcdate;
end;

procedure TIDEApp.callcmd(cmdnam : string; ochannel : byte);
var
  anz, w          : word;
  hs			  : string;
  exitcode        : word;
  myprocess       : tprocess;  

begin
{  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,cmdnam,'',1,1);
  docompilermessages; }

  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= cmdnam;                   // cmdnam beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes,poStderrToOutput];

  // -----------------------------------------------------------------------
  //               Kommando des Betriebssystem aufrufen
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  gexitcode:= exitcode;
  if (ochannel= 0) then
  begin
    mystringlist.loadfromstream(myprocess.stderr);
  end else
  begin
    mystringlist.loadfromstream(myprocess.output);
  end;  
  anz:= mystringlist.count;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      if (ochannel<> 2) then
        compilermessagewindow^.addmessage(0,hs,'',1,1);
    end;
  end;

  myprocess.free;
end;

procedure TIDEApp.callmake(cmdnam : string);
var
  anz, w          : word;
  hs			  : string;
  exitcode        : word;
  myprocess       : tprocess;  

begin
  mysave;
  
{  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,cmdnam,'',1,1);
  docompilermessages; }
  compilermessagewindow^.clearmessages;
  compilermessagewindow^.addmessage(0,'--------','',1,1);
  compilermessagewindow^.addmessage(0,'executing: '+cmdnam,'',1,1);
  docompilermessages; 

  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= cmdnam;                   // cmdnam beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];

  // -----------------------------------------------------------------------
  //               Kommando des Betriebssystem aufrufen
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  gexitcode:= exitcode;
  mystringlist.loadfromstream(myprocess.stderr);
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

  myprocess.free;
  compilermessagewindow^.addmessage(0,cmdnam+': done, thank you','',1,1);
  docompilermessages; 
  
end;


destructor TIDEApp.Done;
begin
  InsideDone:=true;
  IsRunning:=false;
  inherited Done;
  Desktop:=nil;
  RemoveBrowsersCollection;
  DoneHelpSystem;
end;

procedure TIDEApp.showcompwindow;
var R: TRect;

begin
  GetExtent(R);
  R.B.Y:= R.B.Y-2;
  R.A.Y:= R.B.Y-8;
  compilermessagewindow^.locate(r);
  DoCompilerMessages;
end;

   
end.   
