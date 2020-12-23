{ ---------------------------------------------------------
                    tuidude_objects.inc

  Deklarationen der Objekte von tuidude.pas (type)

  15.08.2018    R. Seelig
  --------------------------------------------------------- }

// ---------------------------------------------------------
//          TButtonEvents Objekt
//          fuer Erweiterung von Buttonevents
// ---------------------------------------------------------
type
  pButtonEvents = ^ TButtonEvents;

  TButtonEvents = object (TDialog)

    procedure HandleEvent(var Event : TEvent); virtual;
  end;

// ---------------------------------------------------------
//          TMain Objekt
//          Hauptobjekt des Programms (Bedienerdesktop)
// ---------------------------------------------------------
type
  p_TMain = ^TMain;

  TMain = object (TApplication)
    genwindow : PGroup;

    constructor Init;
    procedure Idle; Virtual;
    procedure HandleEvent(var Event : TEvent);virtual;
    function SelDir(startpath : string) : string;
    procedure InitMenuBar; Virtual;
    procedure InitDeskTop; Virtual;
    procedure InitStatusLine; Virtual;

    procedure CallExtProg(scall1 : string);
    procedure fusebox; Virtual;
    procedure DudeSettingBox; Virtual;
    procedure Readfuses(var lo : byte; var hi : byte; var ex : byte; var lockb : byte; var errcode : integer);
    procedure WriteFuses(lo : string; hi : string; ex : string; lockb : string; mask : byte);
    procedure AvrdudeVersion;
    function OpenHexfiles : string;    
    procedure FlashBox;    

    procedure CloseWindow(var P : PGroup);
    procedure NewMsgwindow;    
  end;
