{ ---------------------------------------------------------
                     tuidude.pas

         Textuserinterface fuer AVRDUDE

  15.08.2018    R. Seelig
  --------------------------------------------------------- }
program TuiDude;

uses
  Classes, SysUtils, Process,
  Objects, Drivers, Views,
  Menus, Dialogs, App,
  StdDlg, MsgBox, Dos;

const
  cmAppToolbar        = 1000;
  cmgenwindow         = 1001;
  cmtestwindow        = 1002;
  cmRunFusebox        = 1003;
  cmdummy             = 1004;

  cmclosegenwindow    = 1101;
  cmclosetestwindow   = 1101;

  // Events fuer Buttons
  cmRead              = 1200;
  cmFlash             = 1201;
  cmSelectFile        = 1202;

  cmNewMsgWin         = 101;
  cmMsgWindowClose    = 102;
  cmMsgWindowClear    = 103;

  cmAvrReadWrite      = 201;
  cmAvrProgrammertype = 202;
  cmDudeVersion       = 203;

const
  hismygetdialog    = 2000;


const
  partanz        = 75;

  dudename   : array[0..partanz-1] of string =
    ('1200','2313','2333','2343','4414','4433','4434','8515',
     '8535','c128','c32','c64','m103','m128','m1280','m1281',
     'm1284p','m128rfa1','m16','m161','m162','m163','m164p','m168',
     'm169','m2560','m2561','m32','m324p','m325','m3250','m328p',
     'm329','m3290','m3290p','m329p','m48','m64','m640','m644',
     'm644p','m645','m6450','m649','m6490','m8','m8515','m8535',
     'm88','pwm2','pwm2b','pwm3','pwm3b','t12','t13','t15',
     't2313','t24','t25','t26','t261','t4313','t44','t45',
     't461','t84','t85','t861','t88','usb1286','usb1287','usb162',
     'usb646','usb647','usb82');

  avrgccname   : array[0..partanz-1] of string =
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
  confdir       = '~/.config/tuidude/';
  conffile      = '~/.config/tuidude/tuidude.conf';

var
  fcname        : string;
  dcname        : string;

type
   fusebox1settings = record
     devices      : PStrCollection;
     selection    : longint;
     checkfuse    : longint;
     lofuse       : string[2];
     hifuse       : string[2];
     exfuse       : string[2];
     lockbits     : string[2];
   end;

   flashhexsettings = record
     devices      : PStrCollection;
     selection    : longint;
     hexnam       : string[255];
   end;

   dudesetup = record             //  Beispielinhalte
     programmer   : string;       //   "-c avrisp"
     device       : string;       //   "-p m328p"
     baud         : string;       //   "-b 19200"
     port         : string;       //   "-P /dev/ttyUSB0"
     ispclk       : string;       //   "-B 10"
   end;

   dudeboxsettings = record
     programmer   : longint;
     baud         : longint;
     port         : longint;
     ispclk       : longint;
   end;

   sumsettings = record
     fbox         : fusebox1settings;
     fhex         : flashhexsettings;
     dset         : dudesetup;
     dbox         : dudeboxsettings;
   end;

var
  fusebox1values   : fusebox1settings;
  flashhexvalues   : flashhexsettings;
  dudeoptions      : dudesetup;
  dudeboxvalues    : dudeboxsettings;

  allsettings      : sumsettings;

var
  mystringlist     : classes.tstringlist;

{$i tmsg_window.inc}
{$i utils.inc}

{ ----------------------------------------------------------------------------------
                            ausserhalb der Objects
  ---------------------------------------------------------------------------------- }


{ ---------------------------------------------------------
                      initdevicelist
    weist dem Record der Fusebox die Stringliste der
    AVR-Controller zu
  --------------------------------------------------------- }
procedure initdevicelist;
var
  i : integer;
begin

  fusebox1values.devices := New(PStrCollection, Init(partanz-1, 5));
  for i:= 0 to partanz-1 do
  begin
    fusebox1values.devices^.AtInsert(i, NewStr(avrgccname[i]));
  end;

  flashhexvalues.devices := New(PStrCollection, Init(partanz-1, 5));
  for i:= 0 to partanz-1 do
  begin
    flashhexvalues.devices^.AtInsert(i, NewStr(avrgccname[i]));
  end;
end;

{ ---------------------------------------------------------
                      duderecordset
    setzt entsprechend der Eintraege von Dudesettingbox
    entsprechende Strings im Record dudeoptions
  --------------------------------------------------------- }
procedure duderecord_set;
begin
{
   dudesetup = record             //  Beispielinhalte
     programmer   : string;       //   "-c avrisp"
     baud         : string;       //   "-b 19200"
     port         : string;       //   "-P /dev/ttyUSB0"
     ispclk       : string;       //   "-B 10"
   end;
}
  with dudeboxvalues do
  begin
    case programmer of
      0 : dudeoptions.programmer:= ' -c usbtiny';
      1 : dudeoptions.programmer:= ' -c usbasp';
      2 : dudeoptions.programmer:= ' -c stk500v2';
      3 : dudeoptions.programmer:= ' -c ponyser';
      4 : dudeoptions.programmer:= ' -c arduino';
      5 : dudeoptions.programmer:= ' -c avrisp';
      6 : dudeoptions.programmer:= ' -c stk500v2';
      7 : dudeoptions.programmer:= ' -c wiring';
    end;

    case baud of
      0 : dudeoptions.baud:= '';
      1 : dudeoptions.baud:= ' -b 9600';
      2 : dudeoptions.baud:= ' -b 19200';
      3 : dudeoptions.baud:= ' -b 28800';
      4 : dudeoptions.baud:= ' -b 38400';
      5 : dudeoptions.baud:= ' -b 57600';
      6 : dudeoptions.baud:= ' -b 115200';
    end;

    case port of
      0 : dudeoptions.port:= ' -P /dev/ttyUSB0';
      1 : dudeoptions.port:= ' -P /dev/ttyUSB1';
      2 : dudeoptions.port:= ' -P /dev/ttyUSB2';
      3 : dudeoptions.port:= ' -P /dev/ttyACM0';
      4 : dudeoptions.port:= ' -P /dev/ttyACM1';
      5 : dudeoptions.port:= ' -P /dev/ttyACM2';
      6 : dudeoptions.port:= ' -P /dev/ttyS0';
    end;

    case ispclk of
      0 : dudeoptions.ispclk:= ' -B 1';
      1 : dudeoptions.ispclk:= ' -B 5';
      2 : dudeoptions.ispclk:= ' -B 10';
      3 : dudeoptions.ispclk:= ' -B 20';
    end;

    if (programmer< 2) then
    begin
      dudeoptions.port:= '';
      dudeoptions.baud:= '';
    end;

  end;
end;


{ ---------------------------------------------------------
                      ReadSettings
   liest Einstellungen der in conffile angegebenen Datei
  --------------------------------------------------------- }
procedure ReadSettings;
var
  dat       : file of sumsettings;                // Dateityp fuer die Dialoginhalte
  notexist  : boolean;

begin

   fcname:= expandfilename(conffile);
   dcname:= expandfilename(confdir);

   notexist:= false;
   assign(dat,fcname);

   {$i-} reset(dat) {$i+};
   if ioresult= 0 then
   begin
     read(dat,allsettings);

     with allsettings do
     begin

       fusebox1values:= fbox;
       flashhexvalues:= fhex;;
       dudeoptions:= dset;
       dudeboxvalues:= dbox;

     end;
   end else
   begin
     notexist:= true;
   end;
   close(dat);

   if notexist then
   begin

     with allsettings do
     begin
       fbox:= fusebox1values;
       fhex:= flashhexvalues;
       dset:= dudeoptions;
       dbox:= dudeboxvalues;

     end;

     if not DirectoryExists(dcname) then
     begin
       If Not CreateDir (dcname) then
       begin
       writeln;
       writeln('Unerwarteter Fehler: Kann Directory "'+dcname+'" nicht erstellen ...');
       writeln;
       halt(2);
       end;
    end;

    assign(dat,fcname);

    {$i-} rewrite(dat) {$i+};

    if ioresult<> 0 then
    begin
      writeln;
      writeln('Unerwarteter Fehler: Kann Datei "'+fcname+'" nicht schreiben ...');
      writeln;
      halt(1);
    end;
    write(dat,allsettings);
    close(dat);
  end;
end;


{ ---------------------------------------------------------
                      SaveSettings
   schreibt Einstellungen die in conffile angegebenen Datei
  --------------------------------------------------------- }
procedure SaveSettings;
var
  dat      : file of sumsettings;                // Dateityp fuer die Dialoginhalte
begin
   fcname:= expandfilename(conffile);
   dcname:= expandfilename(confdir);
   with allsettings do
   begin
     fbox:= fusebox1values;
     fhex:= flashhexvalues;
     dset:= dudeoptions;
     dbox:= dudeboxvalues;
   end;

   assign(dat,fcname);
   {$i-} rewrite(dat) {$i+};
   if ioresult<> 0 then
   begin
     writeln;
     writeln('Unerwarteter Fehler: Kann Datei "'+fcname+'" nicht schreiben ...');
     writeln;
     halt(1);
   end;
   write(dat,allsettings);
   close(dat);
end;

{ ---------------------------------------------------------
                      init_global
    initialisiert globale Variable
  --------------------------------------------------------- }
procedure init_global;
begin
  with fusebox1values do
  begin
    lofuse:= '0';
    hifuse:= '0';
    exfuse:= '0';
    lockbits:= '0';

    selection:= 31;
    checkfuse:= 3;
  end;

  with dudeboxvalues do
  begin
    programmer:= 5;
    baud:= 2;
    port:= 0;
    ispclk:= 0;
  end;

  duderecord_set;
end;


{ ----------------------------------------------------------------------------------
                           Deklaration der Objekte (type)
  ---------------------------------------------------------------------------------- }

{$i tuidude_objects.inc}

{ ----------------------------------------------------------------------------------
                               Methoden der Objekte
  ---------------------------------------------------------------------------------- }

{ ----------------------------------------------------------------------------------
                                 TButtonEvents
  ---------------------------------------------------------------------------------- }
// ---------------------------------------------------------
//          tButtonEvents Objekt
//          fuer Erweiterung von Buttonevents
// ---------------------------------------------------------
procedure TButtonEvents.HandleEvent (var Event : TEvent);
begin

  inherited HandleEvent(Event);

  case Event.What of
    evCommand:
      case Event.Command of
        cmOk, cmCancel, cmYes, cmNo,
        cmFlash, cmRead, cmSelectFile :
          if State and sfModal <> 0 then
          begin
            EndModal(Event.Command);
            ClearEvent(Event)
          end;
      end;
   end;
end;

{ ----------------------------------------------------------------------------------
                                          TMain
  ---------------------------------------------------------------------------------- }

// ---------------------------------------------------------
//                        TMain.init
// ---------------------------------------------------------
constructor TMain.Init;
var R: TRect;
begin
  Inherited Init;

  GetExtent(R);
  R.A.X := R.B.X - 9; R.B.Y := R.A.Y + 1;

  GetExtent(R);

  MsgWindow_active:= false;
  MsgWindow_LineCnt:= 0;

  NewMsgWindow;

end;


// ---------------------------------------------------------
//                        TMain.idle
// wenn momentan keine Aufgaben anstehen, wird
// diese Methode intervallmaessig aufgerufen
// ---------------------------------------------------------
procedure TMain.Idle;
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

end;

procedure TMain.HandleEvent(var Event : TEvent);
begin
  Inherited HandleEvent(Event);
  if (Event.What = evCommand) then
  begin
    Case Event.Command Of
      cmRunFusebox        : FuseBox;

      cmAvrReadWrite      : FlashBox;

      cmClosegenwindow    : CloseWindow(genwindow);

      cmNewMsgWin         : NewMsgwindow;

      cmAvrProgrammertype : DudeSettingBox;

      cmDudeVersion       : AvrdudeVersion;

      else
      begin
        exit;                                     { Unhandled exit }
      end;
    end;
  end;
  ClearEvent(Event);
end;

// ---------------------------------------------------------
//                    TMain.SelDir
//  Dialogbox zum Waehlen eines Verzeichnisses
// ---------------------------------------------------------

function TMain.SelDir(startpath : string) : string;
var
  D       : PChDirDialog;
  selpath : string;

begin
  chdir(startpath);
  New(D, Init(cdNormal, hismygetDialog));
  ExecuteDialog(D,nil);

  GetDir(0,selpath);
  seldir:= selpath;
end;

// ---------------------------------------------------------
//                  TMain.InitMenuBar
// ---------------------------------------------------------
procedure TMain.InitMenuBar;
var R: TRect;
begin
  GetExtent(R);                                      { Get view extents }
  R.B.Y := R.A.Y + 1;                                { One line high  }
  MenuBar := New(PMenuBar, Init(R, NewMenu(
   NewSubMenu('~F~ile', 0, NewMenu(
     NewItem('E~x~it','Alt+x',kbAltX,cmquit,hcNoContext,
     Nil)),

   NewSubMenu('~A~VRDUDE', 0, NewMenu(
     NewItem('F~u~ses                ','F4',kbF4,cmRunFusebox, hcNoContext,
     NewItem('~F~lash: Read/Write    ','F5',kbF5,cmAvrReadWrite, hcNoContext,
     NewLine(
     NewItem('~P~rogrammer config    ','F8',kbF8,cmAvrProgrammerType, hcNoContext,
     NewLine(
     NewItem('AVRDUDE ~V~ersion      ','F6',kbF6,cmDudeVersion, hcNoContext,
     Nil))))))),

   nil))) //end NewSubMenus
  )); //end MenuBar

end;

// ---------------------------------------------------------
//	                 TMain.InitDesktop
//   erstellt den Desktop
// ---------------------------------------------------------
procedure TMain.InitDesktop;
var R: TRect; {ToolBar: PToolBar;}
begin
  GetExtent(R);                                      { Get app extents }
  Inc(R.A.Y);               { Adjust top down }
  Dec(R.B.Y);            { Adjust bottom up }
  Desktop := New(PDeskTop, Init(R));
end;

// ---------------------------------------------------------
//                 TMain.InitStatusLine
// ---------------------------------------------------------
procedure TMain.InitStatusLine;
var
   R: TRect;
begin
  GetExtent(R);
  R.A.Y := R.B.Y - 1;
  R.B.X := R.B.X;
  New(StatusLine,
    Init(R,
      NewStatusDef(0, $EFFF,
        NewStatusKey('~F4~ Fuses', kbF4, cmRunFusebox,
        NewStatusKey('~F5~ Read/Write', kbF5, cmAvrReadWrite,
        NewStatusKey('~F6~ Version', kbF6, cmDudeVersion,
        NewStatusKey('~F10~ Menu', kbNoKey, cmMenu,
        NewStatusKey('~F12~ Messages', kbF12, cmNewMsgWin,
        NewStatusKey('~Alt+X~ Exit', kbAltX, cmquit,
        StdStatusKeys(nil
        ))))))),nil
      )
    )
  );

  GetExtent(R);
  R.A.X := R.B.X - 12; R.A.Y := R.B.Y - 1;
end;

// ---------------------------------------------------------
//                   TMain.CloseWindow
// ---------------------------------------------------------
procedure TMain.CloseWindow(var P : PGroup);
begin
  if Assigned(P) then
  begin
    Desktop^.Delete(P);
    Dispose(P,done);
    P:=Nil;
  end;
end;

{ ---------------------------------------------------------
                       TMain.CallExtProg
    ruft im String s angegebene externe Programm auf
  --------------------------------------------------------- }
procedure TMain.CallExtProg(scall1 : string);
var
  myprocess       : tprocess;
  hs              : string;
  anz, w          : word;

begin
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall1;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes,poStderrToOutPut];

  // -----------------------------------------------------------------------
  //                       externes Programm aufrufen !!!!
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;

  anz:= mystringlist.count;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      replace_utfstring(hs);
      if (hs<>'') then msgwindow_addstring(hs);
    end;
  end;
  myprocess.free;
  mystringlist.free;
end;


// ---------------------------------------------------------
//                    TMain.DudeSettingBox
//             Dialogbox fuer Einstellungen AVRDUDE
//             ( Programmer, Baudrate etc. )
// ---------------------------------------------------------
procedure  TMain.DudeSettingBox;
var
  Dlg                  : PButtonEvents;
  R                    : TRect;
  Control              : PView;

begin
  R.Assign(19, 3, 84, 24);
  New(Dlg, Init(R, 'Programmer settings'));
	
  R.Assign(3, 4, 29, 12);
  Control := New(PRadioButtons, Init(R,
  NewSItem('USBtinyisp',
  NewSItem('USBasp',
  NewSItem('Diamex - ISP',
  NewSItem('PonySer',	
  NewSItem('Arduino - Bootloader',
  NewSItem('AvrIsp',
  NewSItem('STK500v2',
  NewSItem('Wiring', Nil))))))))));
  Dlg^.Insert(Control);

  R.Assign(33, 4, 45, 12);
  Control := New(PRadioButtons, Init(R,
    NewSItem('USB',
    NewSItem('9600',
    NewSItem('19200',
    NewSItem('28800',
    NewSItem('38400',
    NewSItem('57600',
    NewSItem('115200', Nil)))))))));
  Dlg^.Insert(Control);

  R.Assign(49, 4, 62, 12);
  Control := New(PRadioButtons, Init(R,
    NewSItem('ttyUSB0',
    NewSItem('ttyUSB1',
    NewSItem('ttyUSB2',
    NewSItem('ttyACM0',
    NewSItem('ttyACM1',
    NewSItem('ttyACM2',
    NewSItem('ttyS0', Nil)))))))));
  Dlg^.Insert(Control);

  R.Assign(3, 15, 29, 19);
  Control := New(PRadioButtons, Init(R,
  NewSItem('-B 1',
  NewSItem('-B 5',
  NewSItem('-B 10',
  NewSItem('-B 20', Nil))))));
  Dlg^.Insert(Control);

  R.Assign(3, 3, 29, 4);
  Control := New(PStaticText, Init(R, 'Programmer'));
  Dlg^.Insert(Control);

  R.Assign(33, 3, 45, 4);
  Control := New(PStaticText, Init(R, 'Baudrate'));
  Dlg^.Insert(Control);

  R.Assign(49, 3, 62, 4);
  Control := New(PStaticText, Init(R, 'Port'));
  Dlg^.Insert(Control);

  R.Assign(3, 14, 29, 15);
  Control := New(PStaticText, Init(R, 'ISP -ClkSpeed'));
  Dlg^.Insert(Control);

  R.Assign(53, 18, 62, 20);
  Control := New(PButton, Init(R, '~O~K', cmOK, bfDefault));
  Dlg^.Insert(Control);

  dlg^.setdata(dudeboxvalues);
  desktop^.execview(dlg);

  dlg^.getdata(dudeboxvalues);

  duderecord_set;

  if not(MsgWindow_active) then NewMsgWindow;
  MsgWindow_ScrollToBegin:= true;

  msgwindow_clear;
  with dudeoptions do
  begin
    msgwindow_addstring('AVRDUDE Options :');
    msgwindow_addstring('-----------------------------------');
    msgwindow_addstring(' Programmer     : ' + programmer);
    msgwindow_addstring(' Programmerport : ' + port);
    msgwindow_addstring(' Baudrate       : ' + baud);
    msgwindow_addstring(' ISP-Clock      : ' + ispclk);
//    msgwindow_addstring(programmer + baud + port + ispclk)
  end;
  MsgWindow_ScrollToEnd:= true;
  TMSG_window.redraw;
end;

// ---------------------------------------------------------
//                    TMain.fusebox
//           Dialogbox zum Setzen der AVR-Fuses
// ---------------------------------------------------------
procedure  TMain.Fusebox;
var
  Dlg                  : PButtonEvents;
  R                    : TRect;
  Control              : PView;
  c,w                  : word;
  lofu,hifu,exfu,lockb : byte;
  errcode              : integer;

begin
  R.Assign(39, 3, 84, 17);
  New(Dlg, Init(R, 'Fuse Settings'));

  R.Assign(40, 4, 41, 10);
  Control := New(PScrollBar, Init(R));
  Dlg^.Insert(Control);

  R.Assign(24, 4, 40, 10);
  Control := New(PListBox, Init(R, 1, PScrollbar(Control)));
  Dlg^.Insert(Control);

  R.Assign(2, 4, 7, 8);
  control:= (New(PCheckBoxes, Init(R,
    NewSItem(' ',
    NewSItem(' ',
    NewSItem(' ',
    NewSITem(' ', Nil)))))));
  dlg^.insert(control);

  R.Assign(7, 4, 12, 5);
  Control := New(PInputLine, Init(R, 2));
  Dlg^.Insert(Control);

  R.Assign(7, 5, 12, 6);
  Control := New(PInputLine, Init(R, 2));
  Dlg^.Insert(Control);

  R.Assign(7, 6, 12, 7);
  Control := New(PInputLine, Init(R, 2));
  Dlg^.Insert(Control);

  R.Assign(7, 7, 12, 8);
  Control := New(PInputLine, Init(R, 2));
  Dlg^.Insert(Control);

  R.Assign(12, 4, 22, 5);
  Control := New(PStaticText, Init(R, 'h Low'));
  Dlg^.Insert(Control);

  R.Assign(12, 5, 22, 6);
  Control := New(PStaticText, Init(R, 'h High'));
  Dlg^.Insert(Control);

  R.Assign(12, 6, 22, 7);
  Control := New(PStaticText, Init(R, 'h Extended'));
  Dlg^.Insert(Control);

  R.Assign(12, 7, 22, 8);
  Control := New(PStaticText, Init(R, 'h Lock'));
  Dlg^.Insert(Control);

  R.Assign(7, 2, 21, 3);
  Control := New(PStaticText, Init(R, 'Fuses'));
  Dlg^.Insert(Control);

  R.Assign(25, 2, 36, 3);
  Control := New(PStaticText, Init(R, 'Devices'));
  Dlg^.Insert(Control);

  R.Assign(2, 11, 11, 13);
  Control := New(PButton, Init(R, '~R~ead', cmRead, bfNormal));
  Dlg^.Insert(Control);

  R.Assign(12, 11, 21, 13);
  Control := New(PButton, Init(R, '~F~lash', cmFlash, bfNormal));
  Dlg^.Insert(Control);

  R.Assign(32, 11, 41, 13);
  Control := New(PButton, Init(R, '~E~xit', cmOK, bfDefault));
  Dlg^.Insert(Control);

  Dlg^.SelectNext(False);
  repeat
    initdevicelist;

    dlg^.setdata(fusebox1values);
    c:= desktop^.execview(dlg);
    dlg^.getdata(fusebox1values);
    if c= cmFlash then
    begin
      w:= MessageBox(#3'Flashing the fuse with incorrect'+#13+
                     #3'parameter can damage the'+#13+
                     #3'MCU. Do you want to continue ?',
                     nil, mfWarning or mfcancelbutton or mfyesButton);

      if (w= 12) then                       // 12 = Code fuer Yes
      begin
        with fusebox1values do
        begin
          writefuses(lofuse,hifuse,exfuse,lockbits,checkfuse);
        end;
      end;

    end;
    if c= cmRead then
    begin

      readfuses(lofu,hifu,exfu,lockb,errcode);
      if (errcode= 0) then
      begin
        fusebox1values.lofuse:= dez2hex(lofu);
        fusebox1values.hifuse:= dez2hex(hifu);
        fusebox1values.exfuse:= dez2hex(exfu);
        fusebox1values.lockbits:= dez2hex(lockb);
      end;

    end;
  until ((c= cmcancel) or (c=cmok));
  if (c= cmok) then
  begin
    dlg^.getdata(fusebox1values);
  end;
  dlg^.getdata(fusebox1values);

end;

// ---------------------------------------------------------
//                    TMain.Openhexfiles
//     Waehlt ueber ein Dialogfenster eine Hex-Datei aus
// ---------------------------------------------------------
function TMain.OpenHexfiles : string;
var
  FileDialog   : PFileDialog;
  FileName     : string;
  orgdir       : string;
const
  FDOptions: Word = fdOKButton or fdOpenButton;
begin
  orgdir:= getcurrentdir;
  FileName := '*.hex;*.ihx';
  New(FileDialog, Init(FileName, 'Hex-File', 'File name', FDOptions, 1));
  if not(ExecuteDialog(FileDialog, @FileName) <> cmCancel) then
  begin
    filename:= '';
  end;
  openhexfiles:= filename;
  setcurrentdir(orgdir);
end;

// ---------------------------------------------------------
//                    TMain.FlashBox
//        Dialogbox zum Beschreiben des Flashspeichers
// ---------------------------------------------------------
procedure TMain.FlashBox;
var
  Dlg                : PButtonEvents;
  R                  : TRect;
  Control            : PView;
  s, scall1, hexname : string;
  devicetyp          : string;
  boption            : string;
  w                  : word;
  c                  : longint;

  olddir, sourcedir,
  na,ex              : string;

begin
  initdevicelist;
  R.Assign(24, 3, 85, 20);
  New(Dlg, Init(R, 'Flashing Hex-File'));

  // Scrollbar der Deviceliste
  R.Assign(18, 4, 19, 11);
  Control := New(PScrollBar, Init(R));
  Dlg^.Insert(Control);

  // Deviceliste
  R.Assign(2, 4, 18, 11);
  Control := New(PListBox, Init(R, 1, PScrollbar(Control)));
  Dlg^.Insert(Control);

  // Beschriftungstext
  R.Assign(2, 2, 13, 3);
  Control := New(PStaticText, Init(R, 'Devices'));
  Dlg^.Insert(Control);

  // Hinweistexte
  R.Assign(22, 4, 60, 5);
  Control := New(PStaticText, Init(R, 'Programmer     : ' + dudeoptions.programmer));
  Dlg^.Insert(Control);
  R.Assign(22, 5, 60, 6);
  Control := New(PStaticText, Init(R, 'Programmerport : ' + dudeoptions.port));
  Dlg^.Insert(Control);
  R.Assign(22, 6, 60, 7);
  Control := New(PStaticText, Init(R, 'Baudrate       : ' + dudeoptions.baud));
  Dlg^.Insert(Control);
  R.Assign(22, 7, 60, 8);
  Control := New(PStaticText, Init(R, 'ISP-Clock      : ' + dudeoptions.ispclk));
  Dlg^.Insert(Control);
  R.Assign(22, 9, 60, 10);
  Control := New(PStaticText, Init(R, 'To change the programmer options'));
  Dlg^.Insert(Control);
  R.Assign(22, 10, 60, 11);
  Control := New(PStaticText, Init(R, 'leave this dialog and press F8'));
  Dlg^.Insert(Control);

  // Eingabefeld fuer Dateiwahl
  R.Assign(2, 12, 59, 13);
  Control := New(PInputLine, Init(R, 255));
  Dlg^.Insert(Control);

  // Buttons
  R.Assign(50, 14, 59, 16);
  Control := New(PButton, Init(R, '~E~xit', cmOk, bfDefault));
  Dlg^.Insert(Control);

  R.Assign(2, 14, 17, 16);
  Control := New(PButton, Init(R, '~S~elect Hex', cmSelectFile, bfNormal));
  Dlg^.Insert(Control);

  R.Assign(18, 14, 32, 16);
  Control := New(PButton, Init(R, '~F~lash Hex', cmFlash, bfNormal));
  Dlg^.Insert(Control);

  R.Assign(33, 14, 48, 16);
  Control := New(PButton, Init(R, '~R~ead Flash', cmRead, bfNormal));
  Dlg^.Insert(Control);

  Dlg^.SelectNext(false);

  repeat
    initdevicelist;
    dlg^.setdata(flashhexvalues);
    c:= desktop^.execview(dlg);
    dlg^.getdata(flashhexvalues);

    // Hexfile flashen (write)
    if c= cmFlash then
    begin
      if not(MsgWindow_active) then NewMsgWindow;
      MsgWindow_ScrollToBegin:= true;

      hexname:= ' -V -U flash:w:'+flashhexvalues.hexnam;

      with dudeoptions do
      begin
        devicetyp:= ' -p '+dudename[flashhexvalues.selection];        
        if (devicetyp= ' -p m2560') or (devicetyp= ' -p m1280') then
          devicetyp:= devicetyp + ' -D';
        scall1:= 'avrdude' + programmer +devicetyp + baud + ispclk + port + hexname;
      end;
      msgwindow_clear;
      msgwindow_addstring('Flashing MCU');
      msgwindow_addstring('-----------------------------------------------------');
      msgwindow_addstring(scall1);
      MsgWindow_ScrollToBegin:= true;
      TMSG_window.redraw;

      // MCU flashen
      CallExtProg(scall1);

      MsgWindow_ScrollToEnd:= true;
      TMSG_Window.redraw;
    end;

    // MCU auslesen (read)
    if c= cmRead then
    begin
      w:= 12;
      hexname:= flashhexvalues.hexnam;
      if ExistFile(hexname) then
      begin
        fsplit(hexname, sourcedir, na, ex);
        w:= MessageBox(#3 + na + '.' + ex + #13+
                       #3'already exists. Do you want'+#13+
                       #3'to overwrite it ?',
                       nil, mfWarning or mfcancelbutton or mfyesButton);
      end;
      if (w= 12) then                       // 12 = Code fuer Yes
      begin
        if (hexname <> '') then
        begin
          if not(MsgWindow_active) then NewMsgWindow;
          MsgWindow_ScrollToBegin:= true;

          hexname:= ' -U flash:r:'+flashhexvalues.hexnam+':i';

          with dudeoptions do
          begin
            devicetyp:= ' -p '+dudename[flashhexvalues.selection];
            scall1:= 'avrdude' + programmer +devicetyp + baud + ispclk + port + hexname;
          end;
          msgwindow_clear;
          msgwindow_addstring('Reading MCU');
          msgwindow_addstring('-----------------------------------------------------');
          msgwindow_addstring(scall1);
          MsgWindow_ScrollToBegin:= true;
          TMSG_window.redraw;

          // MCU lesen
          CallExtProg(scall1);

          MsgWindow_ScrollToEnd:= true;
          TMSG_Window.redraw;
        end;
      end;
    end;

    if c= cmSelectFile then
    begin
      olddir:= getcurrentdir;

      s:= flashhexvalues.hexnam;
      if (s<> '') then
      begin
        fsplit(s,sourcedir,na,ex);
        chdir(sourcedir);
      end;
      s:= openhexfiles;
      if (s<> '') then
      begin
        flashhexvalues.hexnam:= s;
      end;
      chdir(olddir);
    end;

  until (c= cmOk);
  dlg^.getdata(flashhexvalues);

end;


{ ---------------------------------------------------------
                     TMain.NewMsgwindow
  --------------------------------------------------------- }
procedure TMain.NewMsgwindow;
var
  Window: PMsg_window;
  R: TRect;
  e: tevent;
begin
  if MsgWindow_active then
  begin
    // Fenster schliessen
    tMsg_window.focus;
    e.what:= evcommand;
    e.command:= cmclose;
    e.infoptr:= nil;
    handleEvent(e);
    // und nachfolgend neu ausrichten
  end;
  MsgWindow_active:= true;
  Desktop^.GetExtent(R);
  R.Assign(0, r.b.y-9, r.b.x, r.b.y);
  Window := New(PMsg_window, Init(R, '- Messages -'));
  window^.state:= window^.state or sfCursorVis;
  window^.palette:= wpgraywindow;
  DeskTop^.Insert(Window);
end;
{ ---------------------------------------------------------
                     TMain.WriteFuses
             schreibt ueber AVRDUDE die Fuses
  --------------------------------------------------------- }
procedure TMain.WriteFuses(lo : string; hi : string; ex : string; lockb : string; mask : byte);
var
  dummyb          : byte;
  errcode         : integer;
  scall1          : string;
  fusestring      : string;
  devicetyp       : string;

begin
  if not(MsgWindow_active) then NewMsgWindow;
  MsgWindow_ScrollToBegin:= true;

  if (mask = 0) then
  begin
    msgwindow_clear;
    msgwindow_addstring('no fuses selected, nothing do do...');
    TMSG_window.redraw;
    exit;
  end;

  if ((mask and $01)> 0) then
  begin
    val('$'+lo, dummyb, errcode);                    // auf korrekten Wert prüfen
    if (errcode> 0) then
    begin
      mymessage('Value for Lo-Fuse is not'#13+
                 #3'numerical');
      exit;
    end;
    fusestring:= ' -U lfuse:w:0x'+lo+':m ';
  end;

  if ((mask and $02)> 0) then
  begin
    val('$'+hi, dummyb, errcode);
    if (errcode> 0) then
    begin
      mymessage('Value for Hi-Fuse is not'#13+
                 #3'numerical');
      exit;
    end;
    fusestring:= fusestring +'-U hfuse:w:0x'+hi+':m ';
  end;

  if ((mask and $04)> 0) then
  begin
    val('$'+ex, dummyb, errcode);
    if (errcode> 0) then
    begin
      mymessage('Value for Ex-Fuse is not'#13+
                 #3'numerical');
      exit;
    end;
   fusestring:= fusestring +'-U efuse:w:0x'+ex+':m ';
  end;

  if ((mask and $08)> 0) then
  begin
    val('$'+lockb, dummyb, errcode);
    if (errcode> 0) then
    begin
      mymessage('Value for lockbits is not'#13+
                 #3'numerical');
      exit;
    end;
    fusestring:= fusestring +'-U lock:w:0x'+lockb+':m ';
  end;

  devicetyp:= ' -p '+dudename[fusebox1values.selection];

  msgwindow_clear;
  msgwindow_addstring('Writing fuse bytes...');
  msgwindow_addstring('--------------------------------------------------------------');

  with dudeoptions do
  begin
    devicetyp:= ' -p '+dudename[fusebox1values.selection];
    scall1:= 'avrdude' + programmer +devicetyp + baud + port + ' -B 10';
    scall1:= scall1 + fusestring;
    msgwindow_addstring(scall1);
  end;

  TMSG_window.redraw;

  CallExtProg(scall1);

  msgwindow_addstring('');
  msgwindow_addstring('Setting Fuses done...');

  MsgWindow_ScrollToEnd:= true;
  TMSG_window.redraw;
end;


{ ---------------------------------------------------------
                     TMain.ReadFuses
             liest ueber AVRDUDE die Fuses aus
  --------------------------------------------------------- }
procedure TMain.ReadFuses(var lo : byte; var hi : byte; var ex : byte; var lockb : byte; var errcode : integer);
var
  scall1           : string;
  hs               : string;
  s                : string;
  devicetyp        : string;
  anz, w, anzread  : word;

  myprocess        : tprocess;

begin
  lo:= $00;
  hi:= $00;
  ex:= $00;
  lockb:= $00;
  errcode:= 0;

  if not(MsgWindow_active) then NewMsgWindow;
  MsgWindow_ScrollToBegin:= true;

  msgwindow_clear;
  with dudeoptions do
  begin
    devicetyp:= ' -p '+dudename[fusebox1values.selection];
    scall1:= 'avrdude' + programmer +devicetyp + baud + port + ' -B 10';
    scall1:= scall1 + ' -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h';
    msgwindow_addstring(scall1);
  end;
  TMSG_window.redraw;

  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall1;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes,poStderrToOutPut];

  // -----------------------------------------------------------------------
  //                       AVRDUDE aufrufen !!!!
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.output);
  anz:= mystringlist.count;

  anz:= mystringlist.count;
  anzread:= 0; errcode:= 0;

  if (anz> 0) then
  begin
    for w:= 0 to anz-1 do
    begin
      hs:= mystringlist.strings[w];
      if (errcode = 0) then
      begin
        if (copy(upper(hs),1,2) = '0X') then
        begin
          inc(anzread);
          s:= '$'+copy(upper(hs),3,2);
          case anzread of
            1 : val(s,lo,errcode);
            2 : val(s,hi,errcode);
            3 : val(s,ex,errcode);
            4 : val(s,lockb,errcode);
          end;
        end;
      end;
      replace_utfstring(hs);
      if (hs<>'') then msgwindow_addstring(hs);
    end;
  end;
  msgwindow_addstring('------------------------------------------------');
  if anzread= 0 then errcode:= 255;
  if (errcode= 0) then msgwindow_addstring('Reading fuse bytes finished successfully')
                  else msgwindow_addstring('Failure reading fuse bytes');

  MsgWindow_ScrollToEnd:= true;
  TMSG_Window.redraw;
  mystringlist.free;
end;

{ ---------------------------------------------------------
                     TMain.AvrdudeVersion
          zeigt Avrdudeversion im Ausgabefenster an
  --------------------------------------------------------- }
procedure TMain.AvrdudeVersion;
var
  myprocess       : tprocess;
  anz,w           : integer;
  scall1,s        : string;
begin
  if not(MsgWindow_active) then NewMsgWindow;
  MsgWindow_ScrollToBegin:= true;
  msgwindow_clear;

  scall1:= 'avrdude --version';
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.commandline:= scall1;                   // SCALL1 beinhaltet den Befehl fuer die Kommandozeile / Konsole
  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  exitcode:= myprocess.exitstatus;
//  mystringlist.loadfromstream(myprocess.output);
  mystringlist.loadfromstream(myprocess.stderr);
  anz:= mystringlist.count;

  msgwindow_addstring('Version AVRDUDE:');
  msgwindow_addstring('----------------------------');
  for w:= 0 to (anz-1) do
  begin
    s:= mystringlist.strings[w];
    if (pos('AVRDUDE VERSION',upper(s))> 0) then
      msgwindow_addstring(s);
  end;
  myprocess.free;

  TMSG_Window.redraw;
  mystringlist.free;
end;
{ ----------------------------------------------------------------------------------
                                      Hauptprogramm
   ---------------------------------------------------------------------------------- }
var
  DudeInterface: TMain;

begin
  init_global;
  ReadSettings;

  DudeInterface.init;
  DudeInterface.run;
  DudeInterface.done;

  MsgWindow_clear;

  conclrscr;
  writeln('Thank you for using tuidude');
  writeln;

  SaveSettings;
end.
