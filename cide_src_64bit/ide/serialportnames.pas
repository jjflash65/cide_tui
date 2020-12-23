{$mode objfpc}

program serialportnames;

uses Objects, Classes, SysUtils, BaseUnix;

function GetSerialPortNames: string;
type
TSerialStruct = packed record
  typ: Integer;
  line: Integer;
  port: Cardinal;
  irq: Integer;
  flags: Integer;
  xmit_fifo_size: Integer;
  custom_divisor: Integer;
  baud_base: Integer;
  close_delay: Word;
  io_type: Char;
  reserved_char: Char;
  hub6: Integer;
  closing_wait: Word; // time to wait before closing
  closing_wait2: Word; // no longer used...
  iomem_base: ^Char;
  iomem_reg_shift: Word;
  port_high: Cardinal;
  iomap_base: LongWord; // cookie passed into ioremap
end;

var
  i: Integer;
  sr : TSearchRec;
  sl: TStringList;
  st: stat;
  s: String;
  fd: PtrInt;
  Ser : TSerialStruct;
const TIOCGSERIAL = $541E;
  PORT_UNKNOWN = 0;
begin
  Result := '';
  sl := TStringList.Create;
  try
    // 1. Alle möglichen Ports finden
    if FindFirst('/sys/class/tty/*', LongInt($FFFFFFFF), sr) = 0 then
    begin
      repeat
        if (sr.Name <> '.') and (sr.Name <> '..') Then
          if (sr.Attr and LongInt($FFFFFFFF)) = Sr.Attr then
            sl.Add(sr.Name);
      until FindNext(sr) <> 0;
    end;
    FindClose(sr);
    // 2. heraussuchen ob ./device/driver vorhanden ist
    for i := sl.Count - 1 Downto 0 Do
    Begin
      If Not DirectoryExists('/sys/class/tty/' + sl[i] + '/device/driver') Then
        sl.Delete(i); // Nicht vorhanden >> Port existiert nicht
    end;
    // 3. Herausfinden welcher Treiber
    st.st_mode := 0;
    for i := sl.Count - 1 Downto 0 Do
    Begin
      IF fpLstat('/sys/class/tty/' + sl[i] + '/device', st) = 0 Then
      Begin
        if fpS_ISLNK(st.st_mode) Then
        Begin
          s := fpReadLink('/sys/class/tty/' + sl[i] + '/device/driver');
          s := ExtractFileName(s);
          // 4. Bei serial8250 Treiber muss der Port geprüft werden
          If s = 'serial8250' Then
          Begin
            sl.Objects[i] := TObject(PtrInt(1));
            fd := FpOpen('/dev/' + sl[i], O_RDWR Or O_NONBLOCK Or O_NOCTTY);
            If fd > 0 Then
            Begin
              If FpIOCtl(fd, TIOCGSERIAL, @Ser) = 0 Then
              Begin
                If Ser.typ = PORT_UNKNOWN Then // PORT_UNKNOWN
                  sl.Delete(i);
              end;
              FpClose(fd);
            end else sl.Delete(i); // Port kann nicht geöffnet werden
          end;
        End;
      end;
    end;
    // 5. Dev anhängen
    for i := 0 To sl.Count - 1 Do
      sl[i] := '/dev/' + sl[i];
   Result := sl.CommaText;
  finally
    sl.Free;
  end;
end;

begin
  writeln();
  writeln(getserialportnames);
  writeln();
end.
