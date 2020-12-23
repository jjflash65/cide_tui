/* ---------------------- coldisplay.c ------------------------

   Anbindung TFT Display 160x120 Pixel fuer Displaycontroller:

                            S6D02A1
                            ST7735R

   fuer AVR-Mikrocontroller  (ATMega 48 / 88 / 168 / 328)

   15.01.2015 R. Seelig
   ------------------------------------------------------------ */

#include "coldisplay.h"


uint16_t egapalette [] =
    { 0x0000, 0x0015, 0x0540, 0x0555,
      0xa800, 0xa815, 0xaaa0, 0xad55,
      0x52aa, 0x52bf, 0x57ea, 0x57ff,
      0xfaaa, 0xfabf, 0xffea, 0xffff };


// --------------------------------------------------------------
// SPI - Funktionen
// --------------------------------------------------------------

/* ----------------------------------------------------------
   SPI_INIT

     initialisiert die SPI Hardware des AVR Controllers
   ---------------------------------------------------------- */
void spi_init(void)
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

      sended ein Kommandobyte via SPI an das Display
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

   sendet einzelnes Datum via SPI an das Display
   ---------------------------------------------------------- */

void wrdata(uint8_t data)
{
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(data);                                     // senden
  LCD_PORT |= (1 <<  LCD_CE_PIN );                   // Disable Controller
}

// --------------------------------------------------------------
// Funktionen fuer Farbwertgenerierung
// --------------------------------------------------------------

uint8_t aktxp;                    // Beinhaltet die aktuelle Position des Textcursors in X-Achse
uint8_t aktyp;                    // dto. fuer die Y-Achse
uint16_t textcolor;               // Beinhaltet die Farbwahl fuer die Vordergrundfarbe
uint16_t bkcolor;                 // dto. fuer die Hintergrundfarbe
uint8_t outmode;
uint8_t textsize = 0;             // Skalierung der Ausgabeschriftgroesse



/* ----------------------------------------------------------
   RGBFROMVALUE

     Setzt einen 16-Bitfarbwert aus 3 einzelnen Farbwerten
     fuer (r)ot, (g)ruen und (b)lau zusammen.
   ---------------------------------------------------------- */
uint16_t rgbfromvalue(uint8_t r, uint8_t g, uint8_t b)
{
  uint16_t value;

  r= r >> 3;
  g= g >> 2;
  b= b >> 3;
  value= b;
  value |= (g << 5);
  value |= (r << 11);
  return value;
}

/* ----------------------------------------------------------
   RGBFROMEGA

     liefert den 16-Bit Farbwert, der in der Ega-Farbpalette
     definiert ist.
   ---------------------------------------------------------- */
uint16_t rgbfromega(uint8_t entry)
{
  return egapalette[entry];
}

// --------------------------------------------------------------
// Displayfunktionen
// --------------------------------------------------------------

void lcd160_init(const uint8_t *tabseq)
{
  uint8_t  cmd_anz;
  uint8_t  arg_anz;
  uint16_t ms;
  uint16_t i;

  spi_init();
  LCD_DDR |= (1 << LCD_RST_PIN) | (1 << LCD_DC_PIN);     // Set LCD Output pins
  LCD_PORT &= ~(1 << LCD_CE_PIN);

  LCD_PORT &= ~(1 << LCD_RST_PIN);                       // Resets LCD controler
  _delay_ms(1);
  LCD_PORT |= (1 << LCD_RST_PIN);                        // Set LCD CE = 1 (Disabled)


  // ein einzelnes Kommando besteht aus mehreren Datenbytes. Zuerst wird ein Kommandobyte
  // auf dem SPI geschickt, anschliessend die zu diesem Kommandobytes dazugehoerigen Datenbytes
  // abschliessend wird evtl. ein Timingwait eingefuegt. Dieses wird fuer alle vorhandenen
  // Kommandos durchgefuehrt

  cmd_anz = pgm_read_byte(tabseq++);               // Anzahl Gesamtkommandos

  while(cmd_anz--)                                 // alle Kommandos auf SPI schicken
  {
    wrcmd(pgm_read_byte(tabseq++));                // Kommando lesen
    arg_anz= pgm_read_byte(tabseq++);              // Anzahl zugehoeriger Datenbytes lesen
    ms= arg_anz & delay_flag;                      // bei gesetztem Flag folgt ein Timingbyte
    arg_anz &= ~delay_flag;                        // delay_flag aus Anzahl Argumenten loeschen
    while(arg_anz--)                               // jedes Datenbyte des Kommandos
    {
      wrdata(pgm_read_byte(tabseq++));             // senden
    }
    if(ms)                                         // wenn eine Timingangabe vorhanden ist
    {
      ms= pgm_read_byte(tabseq++);                 // Timingzeit lesen
      if(ms == 255) ms = 500;
      for (i= 0; i< ms; i++) _delay_ms(1);         // und entsprechend "nichts" tun
    }
  }
}

/* ----------------------------------------------------------
   LCD_SETRAMADDR

     setzt Adresszeiger des S6D02A1 in Abhaengigkeit der
     gewuenschten x / y Koordinaten
   ---------------------------------------------------------- */
void lcd_setramaddr(uint8_t x0, uint8_t y0)
{

  wrcmd(0x2A);                // Column addr
  wrdata(0x00);
  wrdata(x0);

  wrcmd(0x2b);                 // Row addr
  wrdata(0x00);

  #ifndef yres128
     wrdata(y0);
  #else
     wrdata(y0+32);
  #endif

  wrcmd(0x2c);                // write to RAM
}

/* ----------------------------------------------------------
   FASTXLINE

     zeichnet eine Linie in X-Achse mit den X Punkten
     x1 und x2 auf der Y-Achse y1
   ---------------------------------------------------------- */

void fastxline(uint8_t x1, uint8_t y1, uint8_t x2, uint16_t color)
{
  uint8_t i;
  int16_t x;

  if (outmode) { return ; }                // fastxline2 nur im Outmode 0 verfuegbar
                                           // (Display Hochkant)
  if (x1> x2)
  {
    x= x1;
    x1= x2;
    x2= x;
  }
  lcd_setramaddr(x1,y1);
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Datenmodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  for (i=x1; i< x2+1; i++)
  {
    spi_out(color >> 8);
    spi_out(color);
  }
  LCD_PORT |= (1 << LCD_CE_PIN);                    // Display Controller enable

}

/* ----------------------------------------------------------
   FILLRECT

     zeichnet ein ausgefuelltes Rechteck mit den
     Koordinatenpaaren x1/y1 (linke obere Ecke) und
     x2/y2 (rechte untere Ecke);
   ---------------------------------------------------------- */

void fillrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
  uint8_t x, y;

  if (y1> y2)
  {
    y= y1;
    y1= y2;
    y2= y;
  }

  for (y= y1; y< y2+1; y++)
  {
    fastxline(x1,y,x2,color);
  }
}

/* ----------------------------------------------------------
   CLRSCR

     loescht den Displayinhalt mit der in BKCOLOR angegebenen
     Farbe
   ---------------------------------------------------------- */
void clrscr()
{
  uint8_t x,y;
  uint8_t hi,lo;

  lcd_setramaddr(0, 0);

  hi = bkcolor >> 8; lo = bkcolor;
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Datenmodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable

  for(y=160; y>0; y--)
  {
    for(x=128; x>0; x--)
    {
      spi_out(hi);
      spi_out(lo);
    }
  }

  LCD_PORT |= (1 << LCD_CE_PIN);                    // Display Controller enable
}


/* ----------------------------------------------------------
   PUTPIXEL

     setzt einen Bildpunkt auf das Display an den
     Koordinaten x/y mit dem Farbwert color
   ---------------------------------------------------------- */
void putpixel(int16_t x, int16_t y, uint16_t color)
{

  if (outmode== 0)
  {
    lcd_setramaddr(x,y);
  }
  else
  {
    lcd_setramaddr(y,_height-1-x);
  }

  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Datenmodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable

  spi_out(color >> 8);
  spi_out(color);

  LCD_PORT |= (1 << LCD_CE_PIN);                    // Display Controller enable

}
/* --------------------------------------------------
   GOTOXY
     Setzt den Textcursor (NICHT Pixelkoordinate) an
     die angegebene Textkoordinate.

     Parameter:
        x = X-Koordinate
        y = Y-Koordinate
   -------------------------------------------------- */
void gotoxy(unsigned char x, unsigned char y)
{
  aktxp= x*(fontsizex+(textsize*fontsizex));
  aktyp= y*(fontsizey+(textsize*fontsizey));
}

/* --------------------------------------------------
   LCD_PUTCHAR
     gibt das in ch angegebene Zeichen auf dem
     Display aus
   -------------------------------------------------- */
void lcd_putchar(unsigned char ch)
{
  uint8_t i,i2;
  uint8_t b;
  uint8_t oldx,oldy;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    aktxp= 0;
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    aktyp= aktyp+fontsizey+(fontsizey*textsize);
    return;
  }

  oldx= aktxp;
  oldy= aktyp;
  for (i=0; i<fontsizey; i++)
  {
    b=pgm_read_byte(&(font[(ch-32)][i]));
    for (i2= 0; i2<fontsizex;i2++)
    {
      if (0x80 & b)
      {
        putpixel(oldx,oldy,textcolor);
        if ((textsize==1))
        {
          putpixel(oldx+1,oldy,textcolor);
          putpixel(oldx,oldy+1,textcolor);
          putpixel(oldx+1,oldy+1,textcolor);
        }
      }
      else
      {
        putpixel(oldx,oldy,bkcolor);
        if ((textsize==1))
        {
          putpixel(oldx+1,oldy,bkcolor);
          putpixel(oldx,oldy+1,bkcolor);
          putpixel(oldx+1,oldy+1,bkcolor);
        }
      }
      b= b<<1;
      oldx++;
      if ((textsize==1)) {oldx++; }
    }
    oldy++;
    if ((textsize==1)) {oldy++; }
    oldx= aktxp;
  }
  aktxp= aktxp+fontsizex+(fontsizex*textsize);
}

void lcd_putcharxy(uint8_t oldx, uint8_t oldy, unsigned char ch)
{
  uint8_t i,i2;
  uint8_t b;
  uint8_t orgx;

  orgx= oldx;
  for (i=0; i<fontsizey; i++)
  {
    b=pgm_read_byte(&(font[(ch-32)][i]));
    for (i2= 0; i2<fontsizex;i2++)
    {
      if (0x80 & b)
      {
        putpixel(oldx,oldy,textcolor);
        if ((textsize==1))
        {
          putpixel(oldx+1,oldy,textcolor);
          putpixel(oldx,oldy+1,textcolor);
          putpixel(oldx+1,oldy+1,textcolor);
        }
      }
      b= b<<1;
      oldx++;
      if ((textsize==1)) {oldx++; }
    }
    oldy++;
    if ((textsize==1)) {oldy++; }
    oldx= orgx;
  }
}

void outtextxy(int x, int y, const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr))
  {
    lcd_putcharxy(x,y,c);
    x += fontsizex;
  }
}


/* -------------------------------------------------------------
   LINE

     Zeichnet eine Linie von den Koordinaten x0,y0 zu x1,y1
     mit der angegebenen Farbe

     Parameter:
        x0,y0 = Koordinate linke obere Ecke
        x1,y1 = Koordinate rechte untere Ecke
        color = Zeichenfarbe
     Linienalgorithmus nach Bresenham (www.wikipedia.org)

   ------------------------------------------------------------- */
void line(int x0, int y0, int x1, int y1, uint16_t color)
{

  //    Linienalgorithmus nach Bresenham (www.wikipedia.org)

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                     /* error value e_xy */

  for(;;)
  {

    putpixel(x0,y0,color);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; }                  /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; }                  /* e_xy+e_y < 0 */
  }
}

/* -------------------------------------------------------------
   RECTANGLE

   Zeichnet ein Rechteck von den Koordinaten x0,y0 zu x1,y1
   mit der angegebenen Farbe

   Parameter:
      x0,y0 = Koordinate linke obere Ecke
      x1,y1 = Koordinate rechte untere Ecke
      color = Zeichenfarbe
   ------------------------------------------------------------- */
void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
  line(x1,y1,x2,y1, color);
  line(x2,y1,x2,y2, color);
  line(x1,y2,x2,y2, color);
  line(x1,y1,x1,y2, color);
}

/* -------------------------------------------------------------
   ELLIPSE

   Zeichnet eine Ellipse mit Mittelpunt an der Koordinate
   xm,ym mit den Hoehen- Breitenverhaeltnis a:b
   mit der angegebenen Farbe

   Parameter:
      xm,ym = Koordinate des Mittelpunktes der Ellipse
      a,b   = Hoehen- Breitenverhaeltnis
      color = Zeichenfarbe

   Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)
   ------------------------------------------------------------- */
void ellipse(int xm, int ym, int a, int b, uint16_t color )
{
  // Algorithmus nach Bresenham (www.wikipedia.org)

  int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
  long a2 = a*a, b2 = b*b;
  long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

  do
  {
    putpixel(xm+dx, ym+dy,color);            // I.   Quadrant
    putpixel(xm-dx, ym+dy,color);            // II.  Quadrant
    putpixel(xm-dx, ym-dy,color);            // III. Quadrant
    putpixel(xm+dx, ym-dy,color);            // IV.  Quadrant

    e2 = 2*err;
    if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
    if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
  } while (dy >= 0);

  while (dx++ < a)                        // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
  {
    putpixel(xm+dx, ym,color);             // -> Spitze der Ellipse vollenden
    putpixel(xm-dx, ym,color);
  }
}

/* -------------------------------------------------------------
   CIRCLE

   Zeichnet einen Kreis mit Mittelpunt an der Koordinate xm,ym
   und dem Radius r mit der angegebenen Farbe

   Parameter:
      xm,ym = Koordinate des Mittelpunktes der Ellipse
      r     = Radius des Kreises
      color = Zeichenfarbe
   ------------------------------------------------------------- */
void circle(int x, int y, int r, uint16_t color )
{
  ellipse(x,y,r,r,color);
}

/* -------------------------------------------------------------
   FILLELLIPSE

   Zeichnet eine ausgefuellte Ellipse mit Mittelpunt an der
   Koordinate xm,ym mit den Hoehen- Breitenverhaeltnis a:b
   mit der angegebenen Farbe

   Parameter:
      xm,ym = Koordinate des Mittelpunktes der Ellipse
      a,b   = Hoehen- Breitenverhaeltnis
      color = Zeichenfarbe

   Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)
   ------------------------------------------------------------- */
void fillellipse(int xm, int ym, int a, int b, uint16_t color )
{
  // Algorithmus nach Bresenham (www.wikipedia.org)

  int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
  long a2 = a*a, b2 = b*b;
  long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

  do
  {
    fastxline(xm+dx, ym+dy,xm-dx, color);            // I. und II.   Quadrant
    fastxline(xm-dx, ym-dy,xm+dx, color);            // III. und IV. Quadrant

    e2 = 2*err;
    if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
    if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
  } while (dy >= 0);

  while (dx++ < a)                        // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
  {
    putpixel(xm+dx, ym,color);             // -> Spitze der Ellipse vollenden
    putpixel(xm-dx, ym,color);
  }
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
   fwert        => Farbwert mit der das Pixel gezeichnet wird

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


   ---------------------------------------------------------- */

void showimage(char ox, char oy, const unsigned char* const image, uint16_t fwert)
{
  int x,y;
  uint8_t b,bp;
  uint8_t resX, resY;

  resX= pgm_read_byte(&(image[0]));
  resY= pgm_read_byte(&(image[1]));
  if ((resX % 8) == 0) { resX= resX / 8; }
                 else  { resX= (resX / 8)+1; }

  for (y=0;y< resY;y++)
  {
    for (x= 0;x<resX;x++)
    {
      b= pgm_read_byte(&(image[y *resX +x+2]));
      for (bp=8;bp>0;bp--)
      {
        if (b& 1<<bp-1) {putpixel(ox+(x*8)+8-bp,oy+y,fwert);}
      }
    }
  }
}


/* -------------------------------------------------------------
   FILLCIRCLE

   Zeichnet einen ausgefuellten Kreis mit Mittelpunt an der
   Koordinate xm,ym und dem Radius r mit der angegebenen Farbe

   Parameter:
      xm,ym = Koordinate des Mittelpunktes der Ellipse
      r     = Radius des Kreises
      color = Zeichenfarbe
   ------------------------------------------------------------- */
void fillcircle(int x, int y, int r, uint16_t color )
{
  fillellipse(x,y,r,r,color);
}

/* ----------------------------------------------------------
   PUTRAMSTRING

   gibt einen Text aus dem RAM an der aktuellen
   Position aus
   ----------------------------------------------------------*/
void putramstring(char *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    lcd_putchar(*c++);
  }
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

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar(c);
}

int lcd_fileout(char ch, FILE *stream)
{
  lcd_putchar(ch);
}


