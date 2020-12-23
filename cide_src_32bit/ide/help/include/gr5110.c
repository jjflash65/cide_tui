/* ----------------------- GR5110.H -------------------------

   Includedatei fuer Anbindung Nokia 5510 Display's
   (China-Display) an einen AVR-Mikrocontroller.
   Funktionstest mit einem Nokia 3310 Display (aus Handy
   entommen): erfolgreich.

   (ATMega 48 / 88 / 168 / 328)

   23.07.2013 R. Seelig
   ---------------------------------------------------------- */

//-------------------------------------------------------------
// Hardwareanbindung
//-------------------------------------------------------------

// SPI Interface
// die PIN-Definitionen des SPI duerfen nicht geaendert werden,
// weil die besonderen Funktionen der Pin's (MOSI,SCK und SS
// verwendet werden !

#define SPI_DDR				DDRB
#define SPI_MOSI_PIN		        3  //PB3
#define SPI_CLK_PIN			5  //PB5
#define SPI_SS_PIN			2  //PB2

// Nokia 5510 LCD ( Chinadisplay )
#define LCD_PORT			PORTB
#define LCD_DDR				DDRB
#define LCD_RST_PIN			0  //PB0
#define LCD_DC_PIN			1  //PB1
#define LCD_CE_PIN			2  //PB2		// Already SS SPI pin
#define LCD_VISIBLE_X_RES	        84
#define LCD_VISIBLE_Y_RES	        48
#define LCD_REAL_X_RES		        84
#define LCD_REAL_Y_RES		        48
#define OK				0
#define OUT_OF_BORDER		        1

#ifdef noscreen_cache
  #define LCD_CACHE_SIZE                2
  #warning LCD 5510 kann nur im Directwrite-Modus genutzt werden
#else
  #define LCD_CACHE_SIZE	        ((LCD_VISIBLE_X_RES*LCD_VISIBLE_Y_RES)/8)
#endif

typedef enum {LCD_CMD=0,LCD_DATA=1} LcdCmdData;
typedef enum {PIXEL_OFF=0,PIXEL_ON=1,PIXEL_XOR=2} PixelMode;
static unsigned char signal[LCD_VISIBLE_X_RES];
unsigned char LcdCache[LCD_CACHE_SIZE];


// --------------------------------------------------------------
// SPI Prototypen
// --------------------------------------------------------------

void spi_init(void);
unsigned char spi_out(unsigned char data);

// --------------------------------------------------------------
// LCD Prototypen
// --------------------------------------------------------------

void wrcmd(uint8_t command);                            // sende ein Kommando
void wrdata(uint8_t dat);                               // sende ein Datum
void lcd_init(void);
void clrscr(void);
void gotoxy(unsigned char x, unsigned char y);
void lcd_putchar (uint8_t ch);
void lcd_putchar_d(uint8_t ch);
void putramstring(unsigned char dataArray[]);
void putromstring(const unsigned char *dataPtr);
void putpixel(unsigned char x, unsigned char y, PixelMode mode );
void line(int x0, int y0, int x1, int y1, PixelMode mode);
void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, PixelMode mode);
void ellipse(int xm, int ym, int a, int b, PixelMode mode );
void circle(int x, int y, int r, PixelMode mode );
void showimage(char ox, char oy, const unsigned char* const image,uint8_t resX,uint8_t resY, PixelMode mode);
void scr_update(void);

// --------------------------------------------------------------
//  LCD-Prototypen, die direkt in das Displayram schreiben.
//  Hiermit sind keine Pixel des Displays einzeln ansprechbar,
//  sondern immer nur 8 zusammen (1 Byte). Eignet sich für
//  schnelle Ausgaben. Außerdem kann hierfür dann der
//  Schattenspeicher ausser Betrieb genommen werden.
// --------------------------------------------------------------

void clrscr_d(void);
void gotoxy_d(unsigned char x, unsigned char y);
void lcd_putchar_d(uint8_t ch);
void yline(char x, char y1, char y2);                   // zeichnet eine Linie in Y-Achse
                                                        // zeige Bitmap direkt an
void showimage_d(uint8_t ox, uint8_t oy, const unsigned char* const image,uint8_t resX,uint8_t resY, char mode);

#define prints(tx)     (putromstring(PSTR(tx)))      // Anzeige String aus Flashrom: prints("Text");
#define printa(tx)     (putramstring(tx))            // Anzeige eines Strings der in einem Array im RAM liegt


// ----------------- fuer PrintF --------------------------------

static int lcd_fileout(char ch, FILE *stream);

static FILE lcdout = FDEV_SETUP_STREAM(lcd_fileout,NULL,_FDEV_SETUP_WRITE);		// einen Stream zuordnen

// --------------------------------------------------------------

// LCD Variable

unsigned int LcdCacheIdx;
char directwrite= 0;
char wherex=0;
char wherey=0;
char invchar= 0;               // = 1 fuer inversive Textausgabe  (nur im Directwrite-Modus)


// --------------------------------------------------------------
// Zeichensatz
// --------------------------------------------------------------


/* Bitmaps des Ascii-Zeichensatzes
   ein Smily wuerde so aussehen:
      { 0x36, 0x46, 0x40, 0x46, 0x36 }  // Smiley

   ein grosses A ist folgendermassen definiert:

   { 0x7E, 0x11, 0x11, 0x11, 0x7E }

   . x x x x x x .
   . . . x . . . x
   . . . x . . . x
   . . . x . . . x
   . x x x x x x .

*/


#define lastascii 125                   // letztes angegebenes Asciizeichen

static const unsigned char fonttab [][5] PROGMEM={
    { 0x00, 0x00, 0x00, 0x00, 0x00 },   // space
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0xc4, 0xc8, 0x10, 0x26, 0x46 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // "Yen"
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    // Zeichen vom Ascii-Satz abweichend
    { 0x3E, 0x7F, 0x7F, 0x3E, 0x00 },   // Zeichen 123 : ausgefuelltes Oval
    { 0x06, 0x09, 0x09, 0x06, 0x00 },   // Zeichen 124 : hochgestelltes kleines o (fuer Gradzeichen);
    { 0x01, 0x01, 0x01, 0x01, 0x01 }    // Zeichen 125 : Strich in der obersten Reihe
};


void spi_init(void)
// SPI Hardware initialisieren
{
  SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_CLK_PIN) | (1 << SPI_SS_PIN);

  // Enable SPI Port: Keine Interrupts, MSB first, Master Modus
  SPCR = (1 << SPE) | (1 << MSTR);
  // Taktrate F_CPU/2
  SPSR = (1 << SPI2X);
}

/* ----------------------------------------------------------
   SPI_OUT

      Byte ueber SPI senden / empfangen
      data ==> zu sendendes Datum
   ---------------------------------------------------------- */

unsigned char spi_out(unsigned char data)
{
  SPDR=data;                                // Datum senden
  while( ( SPSR & 0x80 ) != 0x80 );         // ... und warten bis es gesendet ist
  return SPDR;                              // empfangenes Byte ist Rueckgabewert
}


/* ----------------------------------------------------------
   WRCMD

   sendet Kommando via SPI an das LCD
   ---------------------------------------------------------- */

void wrcmd(uint8_t cmd)
{
  LCD_PORT &= ~(1 << LCD_DC_PIN);                    // C/D = 0 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(cmd);                                      // senden
  LCD_PORT |= (1 << LCD_CE_PIN );                    // Disable Controller
}

/* ----------------------------------------------------------
   WRDATA

   sendet Datum via SPI an das LCD
   ---------------------------------------------------------- */

void wrdata(uint8_t data)
{
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(data);                                     // senden
  LCD_PORT |= (1 <<  LCD_CE_PIN );                   // Disable Controller
}


/* ----------------------------------------------------------
   LCD Interface
   ----------------------------------------------------------

/* ----------------------------------------------------------
   INIT_LCD

   initialisert LCD
   ---------------------------------------------------------- */

void lcd_init(void)

{
  int n;

  spi_init();

  LCD_DDR |= (1 << LCD_RST_PIN) | (1 << LCD_DC_PIN);     // Set LCD Output pins
  LCD_PORT &= ~(1 << LCD_RST_PIN);                       // Resets LCD controler
  _delay_ms(1);
  LCD_PORT |= (1 << LCD_RST_PIN);                        // Set LCD CE = 1 (Disabled)
  LCD_PORT |= (1 << LCD_CE_PIN);

  // LCD Controller Kommandos  (eigentliches initialisieren)

  wrcmd(0x21);                            // Erweiterter Kommandomodus
  wrcmd(0xb6);                            // VOP max
  wrcmd(0x04);                            // Temp.Koeffizient
  wrcmd(0x14);                            // BIAS = 2
  wrcmd(0x0c);
  wrcmd(0x20);                            // Standart Kommandomodus
  wrcmd(0x0c);                            // normale Ausgabe (normal = 0Ch, invertiert = 0Dh)


  memset(LcdCache,0x00,LCD_CACHE_SIZE);              // LCD Cache loeschen

  // LCD loeschen

  wrcmd( 0x80);
  wrcmd( 0x40);
  for(n=0; n<(LCD_REAL_X_RES*LCD_REAL_Y_RES); n++) wrdata(0x00);
}

/* -----------------------------------------------------
   CLRSCR_D

   loescht den Displayinhalt
   ----------------------------------------------------- */
void clrscr_d()
{
  int  i=0;

  wrcmd(0x80);             // Anfangsadresse des Displays
  wrcmd(0x40);
  for(i=1;i<(LCD_REAL_X_RES*LCD_REAL_Y_RES/8);i++) { wrdata(0x00); }
  gotoxy_d(0,0);
}

/* ----------------------------------------------------------
   CLRSCR

   loescht den Screenspeicher
   ---------------------------------------------------------- */

void clrscr(void)
{
  int n;

  if (directwrite==1)
  {
    clrscr_d();
    return;
  }

  memset(LcdCache,0x00,LCD_CACHE_SIZE);              // LCD Cache loeschen
  wrcmd( 0x80);
  wrcmd( 0x40);
  for(n=0; n<(LCD_REAL_X_RES*LCD_REAL_Y_RES); n++) wrdata(0x00);
  gotoxy(0,0);
}

/* --------------------------------------------------------
   GOTOXY_D

   positioniert den Textcursor direkt im Displayspeicher.
   Nachfolgende Textausgaben werden an diese Stelle ge-
   schrieben.

   Position x,y (Textkoordinaten x= 0..15 / y= 0..7 )
   -------------------------------------------------------- */

void gotoxy_d(unsigned char x, unsigned char y)
{
  wrcmd(0x80+(x*6));
  wrcmd(0x40+y);
  wherex= x; wherey= y;
}

/* --------------------------------------------------------
   GOTOXY

   positioniert den Textcursor im Screenspeicher an

   Position x,y (Textkoordinaten x= 0..15 / y= 0..7 )
   -------------------------------------------------------- */

void gotoxy(unsigned char x, unsigned char y)
{

  if (directwrite==1)
  {
    gotoxy_d(x,y);
    return;
  }
  if(x>((LCD_VISIBLE_X_RES/6)-1)) return;               // Funktion verlassen wenn X-Koordinate ausserhalb des Displays
  if(y>((LCD_VISIBLE_Y_RES/8)-1)) return;               // dto. Y Koordinate

  LcdCacheIdx = ( x * 6) + ( y * LCD_VISIBLE_X_RES);    // Adresse im LCD-Cache berechnen
  wherex= x;
  wherey= y;
}

/* ----------------------------------------------------------
   LCD_PUTCHAR_D

   setzt ein Zeichen direkt auf das Display und setzt den
   Cursor auf die naechste Position

   (die Steuerkommandos \n , \r  und Backspace fuer <printf>
   sind implementiert)

   ch ==> zu setzenes Zeichen
   ---------------------------------------------------------- */


void lcd_putchar_d(uint8_t ch)
{
  int b,rb;

  if (ch== 13)
  {
    gotoxy_d(0,wherey);
    return;
  }
  if (ch== 10)
  {
    wherey++;
    gotoxy_d(wherex,wherey);
    return;
  }

  if (ch== 8)
  {
    if ((wherex> 0))
    {
      wherex--;
      gotoxy_d(wherex,wherey);
      for (b= 0;b<6;b++)
      {
        if (invchar) {wrdata(0xff);} else {wrdata(0);}
      }
      gotoxy_d(wherex,wherey);
    }
    return;
  }

  if ((ch<0x20)||(ch>lastascii)) ch = 92;               // ASCII Zeichen umrechnen

  // Kopiere Daten eines Zeichens aus dem Zeichenarray in den LCD-Screenspeicher

  for (b= 0;b<5;b++)
  {
    rb= pgm_read_byte(&(fonttab[ch-32][b]));
    if (invchar) {rb= ~rb;}
    wrdata(rb);
  }
  if (invchar) {wrdata(0xff);} else {wrdata(0);}
  wherex++;
  if (wherex> 15)
  {
    wherex= 0;
    wherey++;
  }
  gotoxy_d(wherex,wherey);
}


/* ----------------------------------------------------------
   LCD_PUTCHAR

   setzt ein Zeichen im Screenspeicher und postioniert den
   Cursor auf die naechste Position

   ch ==> zu setzenes Zeichen
   ---------------------------------------------------------- */

void lcd_putchar (uint8_t ch)
{
  unsigned char i;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    gotoxy(0,wherey);
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    gotoxy(wherex,wherey+1);
    return;
  }
  if ((ch<0x20)||(ch>lastascii)) ch = 92;               // ASCII Zeichen umrechnen

  // Kopiere Daten eines Zeichens aus dem Zeichenarray in den LCD-Cache

  for (i=0; i<5; i++) LcdCache[LcdCacheIdx++]= pgm_read_byte(&(fonttab[ch-32][i]))<<1;

  LcdCache[LcdCacheIdx]=0x00;

  if(LcdCacheIdx==(LCD_CACHE_SIZE-1))
  {
    LcdCacheIdx = 0;
    wherex= 0;
    wherey++;
  }
  else
  {
    LcdCacheIdx++;
    wherex++;
  }
}


/* ----------------------------------------------------------
   PUTRAMSTRING

   kopiert einen String aus dem RAM (Array) in den
   Screenspeicher ab der aktuellen Cursorposition

          dataArray ==> zu setzender String (innerhalb eines
                                             Arrays)
   ---------------------------------------------------------- */

void putramstring(unsigned char dataArray[])
{
  unsigned char tmpIdx=0;

  if (directwrite==0)
    { while(dataArray[tmpIdx]!='\0') lcd_putchar(dataArray[tmpIdx++]);}
  else
    { while(dataArray[tmpIdx]!='\0') lcd_putchar_d(dataArray[tmpIdx++]);}
}

/* ----------------------------------------------------------
   PUTROMSTRING

   kopiert einen String aus dem ROM in den Screenspeicher
   ab der aktuellen Cursorposition.

          *dataPtr ==> Zeiger auf den String im ROM

   ---------------------------------------------------------- */

void putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  if (directwrite==0)
    { for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar(c); }
  else
    { for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar_d(c); }
}


/* ----------------------------------------------------------
   PUTPIXEL

   Setzt Pixel an Position X,Y im Screenspeicher
   PixelMode 0 = loeschen
             1 = setzen
             2 = Pixelpositon im XOR-Modus verknuepfen

   ---------------------------------------------------------- */

void putpixel(unsigned char x, unsigned char y, PixelMode mode )
{
  unsigned int index;
  unsigned char offset;
  unsigned char data;

  index=((y/8)*LCD_VISIBLE_X_RES)+x;
  offset=y-((y/8)*8);

 // Pixel im entsprechenden PixelMode setzen

  data=LcdCache[index];
  if (mode==PIXEL_ON) data |= (0x01<<offset);                          // Pixel setzen
  else if (mode==PIXEL_OFF) data &= (~(0x01<<offset));                 // Pixel loeschen
  else if (mode==PIXEL_XOR) data ^= (0x01<<offset);                    // Pixel im XOR-Mode setzen
  LcdCache[index]=data;
}


/* ----------------------------------------------------------
   LINE

   Zeichnet eine Linie von den Koordinaten x0,y0 zu x1,y1
   im Screenspeicher.

   Linienalgorithmus nach Bresenham (www.wikipedia.org)

   PixelMode 0 = loeschen
             1 = setzen
             2 = Pixelpositon im XOR-Modus verknuepfen

   ---------------------------------------------------------- */

void line(int x0, int y0, int x1, int y1, PixelMode mode)
{


  //    Linienalgorithmus nach Bresenham (www.wikipedia.org)

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;

  for(;;)
  {
    putpixel(x0,y0,mode);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; }
    if (e2 < dx) { err += dx; y0 += sy; }
  }
}

/* ----------------------------------------------------------
   RECTANGLE

   Zeichnet ein Rechteck von den Koordinaten x0,y0 zu x1,y1
   im Screenspeicher.

   Linienalgorithmus nach Bresenham (www.wikipedia.org)

   PixelMode 0 = loeschen
             1 = setzen
             2 = Pixelpositon im XOR-Modus verknuepfen

   ---------------------------------------------------------- */

void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, PixelMode mode)
{
  line(x1,y1,x2,y1, mode);
  line(x2,y1,x2,y2, mode);
  line(x1,y2,x2,y2, mode);
  line(x1,y1,x1,y2, mode);
}

/* ----------------------------------------------------------
   ELLIPSE

   Zeichnet eine Ellipse mit Mittelpunt an der Koordinate xm,ym
   mit den Hoehen- Breitenverhaeltnis a:b
   im Screenspeicher.

   Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)

   PixelMode 0 = loeschen
             1 = setzen
             2 = Pixelpositon im XOR-Modus verknuepfen

   ---------------------------------------------------------- */

void ellipse(int xm, int ym, int a, int b, PixelMode mode )
{
  // Algorithmus nach Bresenham (www.wikipedia.org)

  int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
  long a2 = a*a, b2 = b*b;
  long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

  do
  {
    putpixel(xm+dx, ym+dy,mode);            // I.   Quadrant
    putpixel(xm-dx, ym+dy,mode);            // II.  Quadrant
    putpixel(xm-dx, ym-dy,mode);            // III. Quadrant
    putpixel(xm+dx, ym-dy,mode);            // IV.  Quadrant

    e2 = 2*err;
    if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
    if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
  } while (dy >= 0);

  while (dx++ < a)                         // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
  {
    putpixel(xm+dx, ym,mode);              // -> Spitze der Ellipse vollenden
    putpixel(xm-dx, ym,mode);
  }
}

/* ----------------------------------------------------------
   CIRCLE

   Zeichnet einen Kreis mit Mittelpunt an der Koordinate xm,ym
   und dem Radius r im Screenspeicher.

   PixelMode 0 = loeschen
             1 = setzen
             2 = Pixelpositon im XOR-Modus verknuepfen

   ---------------------------------------------------------- */

void circle(int x, int y, int r, PixelMode mode )
{
  ellipse(x,y,r,r,mode);
}



/* ----------------------------------------------------------
   SHOWIMAGE

   Kopiert ein im Flash abgelegtes Bitmap in den Screens-
   peicher. Bitmap muss byteweise in Zeilen gespeichert
   vorliegen Hierbei entspricht 1 Byte 8 Pixel.
   Bsp.: eine Reihe mit 6 Bytes entsprechen 48 Pixel
         in X-Achse

   ox,oy        => linke obere Ecke an der das Bitmap
                   angezeigt wird
   image        => das anzuzeigende Bitmap
   resX         => Anzahl der Bytes in X-Achse
   LcdPixelMode => Ausgabemode
      0 = Bitmappixel loeschen
      1 = Bitmappixel setzen
      2 = Bitmappixel im XOR-Mode mit Hintergrund verknuepfen

   ---------------------------------------------------------- */

void showimage(char ox, char oy, const unsigned char* const image,uint8_t resX,uint8_t resY, PixelMode mode)
{
  int x,y;
  char b,bp;
  for (y=0;y< resY;y++)
  {
    for (x= 0;x<resX;x++)
    {
      b= pgm_read_byte(&(image[y *resX +x]));
      for (bp=8;bp>0;bp--)
      {
        if (b& 1<<bp-1) {putpixel(ox+(x*8)+8-bp,oy+y,mode);}
      }
    }
  }
}

/* -----------------------------------------------------
   SHOWIMAGE_D

   Parameter

   mode: Zeichenmodus
            1 = Bitmap wird gezeichnet
            0 = Bitmap wird geloescht

   zeigt ein im ROM abgelegtes Bitmap an. Bitmap hat das-
   selbe Format wie in <showimage>.

   Speicherorganisation des Displays

         X-Koordinate
           0     1     2
         ----------------------------
   Y     Byte0 Byte1 Byte2 ...
   |
   K  0   D0    D0    D0
   o
   o  1   D1    D1    D1
   r
   d  2   D2    D2    D2
   i
   n  3   D3    D3    D3
   a
   t  4   D4    D4    D4
   e      .     .     .
          .     .     .

   Speicherorganisation des Bitmaps:

   Y       X-Koordinate
   |        0  1  2  3  4  5  6  7    8  9 10 11 12 13 14 15
   K               Byte 0                    Byte 1
   o  0     D7 D6 D5 D4 D3 D2 D1 D0   D7 D6 D5 D4 D3 D2 D1 D0
   o
   r         Byte (Y*XBytebreite)     Byte (Y*XBytebreite)+1
   d  1     D7 D6 D5 D4 D3 D2 D1 D0   D7 D6 D5 D4 D3 D2 D1 D0
   i
   n
   a
   t
   e
   ----------------------------------------------------- */
void showimage_d(uint8_t ox, uint8_t oy, const unsigned char* const image,uint8_t resX,uint8_t resY, char mode)
{
  uint8_t x,y,xp;
  uint8_t hb,b;
  char i;

  for (y=0;y< (resY>>3);y++)
  {
    xp= 0;
    for (x= 0;x<(resX<<3);x++)
    {
      wrcmd(0x80+ox+x);                                        // Displayadresse fuer x-Koordinate
      wrcmd(0x40+(y)+(oy>>3));                                 // dto y-Koordinate
      if (mode==1)
      {

        b= 0;
        for (i= 0; i<8; i++)
        {
          hb = pgm_read_byte(&(image[(((y*8)+i) *resX) +(x>>3)]));
          hb &= 1<<(7-xp);
          if (hb != 0)
          {
            b|= (1<<i);
          }
        }
        xp++;
        xp= xp % 8;
        wrdata(b);                                               // Byte des Bitmaps an Display schicken
      }
      else
      {
        wrdata(0xff);
      }
    }
  }
}

void yline(char x, char y1, char y2)
{
  unsigned char y,yd1,yd2,i,b,b2,b3;

  if (y2< y1)
  {
     y= y2; y2= y1; y1= y;
  }
  yd1= y1>>3; yd2= y2>>3;
  b= y1 % 8;
  b2= 1<< b;
  for (i=b; i<7; i++)
  {
    b2+= 1<<(i+1);
  }
  wrcmd(0x80+x);
  wrcmd(0x40+yd1);
  wrdata(b2);
  if (yd1 == yd2)
  {
    b= y2%8;
    b3=0;
    for (i=8; i> b+1;i--)
    {
      b3= 0x80+(b3>>1);
    }
    b3=~b3;
    b2 &= b3;
    wrcmd(0x80+x);
    wrcmd(0x40+yd1);
    wrdata(b2);
  }
  else
  {
    for (y= (yd1+1); y< yd2; y++)
    {
      wrcmd(0x80+x);
      wrcmd(0x40+y);
      wrdata(0xff);
    }
    b= y2%8;
    b2=0;
    for (i= 0; i< b+1;i++)
    {
      b2 = b2+(1<<i);
    }
    wrcmd(0x80+x);
    wrcmd(0x40+yd2);
    wrdata(b2);
  }
  wrcmd(0);
}


/* ----------------------------------------------------------
   SCR_UPDATE

   bringt den Screenspeicher (abgelegt im ATmega) zur Anzeige
   auf dem LCD

   ---------------------------------------------------------- */

void scr_update(void)
{
  unsigned int i=0;
  unsigned char row;
  unsigned char col;

  for (row=0; row<(LCD_VISIBLE_Y_RES / 8); row++)
  {
    wrcmd( 0x80);
    wrcmd( 0x40 | row);
    for (col=0; col<LCD_VISIBLE_X_RES; col++) wrdata(LcdCache[i++]);
  }
}

/* ----------------------------------------------------------
   LCD_FILEOUT

   Dateiausgabe. Ein Zeichen wird auf dem angegebenen File aus-
   gegeben!!!

   Wird die Standardausgabe hierhin umgeleitet sendet bspw.
   <printf> jedes einzelne Zeichen hin

   ---------------------------------------------------------- */

static int lcd_fileout(char ch, FILE *stream)
{
  if (directwrite==0) {lcd_putchar(ch);}
                 else {lcd_putchar_d(ch);}
}

