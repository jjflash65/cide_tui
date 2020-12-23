{ ---------------------------------------------------------
                     tmsg_window.inc

       Objekt und Methoden eines Messagewindows

  13.08.2018    R. Seelig
  --------------------------------------------------------- }

{ ---------------------------------------------------------
                   Textbuffer des Messagewindows
  --------------------------------------------------------- }
const
  MsgWindow_MaxLines      = 300;

var
  MsgWindow_LineCnt       : integer;
  MsgWindow_Lines         : array[0..MsgWindow_MaxLines - 1] of PString;
  MsgWindow_active        : boolean;

  MsgWindow_ScrollToEnd   : boolean;
  MsgWindow_ScrollToBegin : boolean;


{ ---------------------------------------------------------
                     MsgWindow_clear
    loescht den Inhalt des MsgWindow Buffers und gibt
    den belegten Speicher wieder frei
  --------------------------------------------------------- }
procedure MsgWindow_clear;
var
  i  : integer;
begin
  if MsgWindow_LineCnt> 0 then
  begin
  for i := 0 to MsgWindow_LineCnt - 1 do
    if MsgWindow_Lines[I] <> nil then DisposeStr(MsgWindow_Lines[i]);
  end;
  MsgWindow_LineCnt:= 0;
end;

{ ---------------------------------------------------------
                     MsgWindow_addstring
    fuegt dem Messagewindow eine weitere Textzeile hin-
    zu

    Parameter:
       s  : String, der im Fenster angezeigt werden soll
  --------------------------------------------------------- }
procedure MsgWindow_addstring(s : string);
begin
  inc(MsgWindow_LineCnt);
  MsgWindow_Lines[MsgWindow_LineCnt-1]:= newStr(s);
end;

{ ----------------------------------------------------------------------------------
                           Deklaration der Objekte (type)
  ---------------------------------------------------------------------------------- }
{ ---------------------------------------------------------
                        Interior_MSG
          Object fuer den Inhalt des Messagefensters
  --------------------------------------------------------- }
type

  PInterior_MSG = ^TInterior_MSG;
  TInterior_MSG = object(TScroller)

    constructor Init(var Bounds: TRect; AHScrollBar,
      AVScrollBar: PScrollBar);
    procedure Draw; virtual;
end;

{ ---------------------------------------------------------
                        TMsg_Window
               Object des Message Fensters
  --------------------------------------------------------- }
  PMsg_window = ^TMsg_window;
  TMsg_window = object(TWindow)

    var
      Interior_MSG : PInterior_MSG;

    constructor Init(Bounds: TRect; WinTitle: String);
    procedure MakeInterior(Bounds: TRect);
    procedure close; virtual;
  end;

{ ----------------------------------------------------------------------------------
                               Methoden der Objekte
  ---------------------------------------------------------------------------------- }
 { ----------------------------------------------------------------------------------
                                TInterior_MSG
  ---------------------------------------------------------------------------------- }

{ ---------------------------------------------------------
                     TInterior_MSG.init
  --------------------------------------------------------- }
constructor TInterior_MSG.Init(var Bounds: TRect; AHScrollBar,
  AVScrollBar: PScrollBar);
begin
  TScroller.Init(Bounds, AHScrollBar, AVScrollBar);
  GrowMode := gfGrowHiX + gfGrowHiY;
  Options := Options or ofFramed;
  SetLimit(128, MsgWindow_MaxLines);
  MsgWindow_ScrollToEnd:= false;
  MsgWindow_ScrollToBegin:= false;
end;

{ ---------------------------------------------------------
                     TInterior_MSG.draw
  --------------------------------------------------------- }
procedure TInterior_MSG.Draw;
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
    if (I < MsgWindow_LineCnt) and (MsgWindow_Lines[I] <> nil) then
      MoveStr(B, Copy(MsgWindow_Lines[I]^, Delta.X + 1, Size.X), Color);
    WriteLine(0, Y, Size.X, 1, B);
  end;
  
  if (MsgWindow_ScrollToEnd= true) then
  begin
    MsgWindow_ScrollToEnd:= false;
    ScrollTo(0, MsgWindow_LineCnt - 7);
  end;
  if (MsgWindow_ScrollToBegin= true) then
  begin
    MsgWindow_ScrollToBegin:= false;
    ScrollTo(0, 0);
  end;
end;

{ ----------------------------------------------------------------------------------
                                TMsg_Window
  ---------------------------------------------------------------------------------- }
{ ---------------------------------------------------------
                     TMsg_Window.init
  --------------------------------------------------------- }
constructor TMsg_window.Init(Bounds: TRect; WinTitle: string);
begin
  TWindow.Init(Bounds, WinTitle + ' ', wnNoNumber);
  GetClipRect(Bounds);
  Bounds.Grow(-1, -1);
  MakeInterior(Bounds);
end;

{ ---------------------------------------------------------
                     TMsg_Window.makeinterior
  --------------------------------------------------------- }
  procedure TMsg_window.makeinterior(Bounds: TRect);
var
  HScrollBar, VScrollBar: PScrollBar;
begin
  VScrollBar := StandardScrollBar(sbVertical + sbHandleKeyboard);
  HScrollBar := StandardScrollBar(sbHorizontal + sbHandleKeyboard);

  GetExtent(Bounds);
  Bounds.Grow(-1,-1);
  Interior_MSG := New(PInterior_MSG, Init(Bounds, HScrollBar, VScrollBar));
  Insert(Interior_MSG);
end;

{ ---------------------------------------------------------
                     TMsg_Window.close
  --------------------------------------------------------- }
procedure TMsg_window.Close;
begin
  MsgWindow_active:= false;
//  mymessage('Close');
  inherited close;
end;




