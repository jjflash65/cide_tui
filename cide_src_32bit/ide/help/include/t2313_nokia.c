/* -------------------- T2313_NOKIA.H -------------------------

   Headerdatei fuer die Anbindung eines s/w Nokia LCD an
   einen ATtiny. Abgespeckte LCD-Bibliothek, keine Grafik-
   routinen

   02.03.2015 R. Seelig
   ------------------------------------------------------------ */

#include "t2313_nokia.h"

/*

  Die Prototypen aus t2313_nokia.h

  void spi_init(void);
  unsigned char spi_out(uint8_t data);
  void wrcmd(uint8_t command);                            // sende ein Kommando
  void lcd_init();                                        // initialisiere das Display
  void wrdata(uint8_t dat);                               // sende ein Datum
  void clrscr();                                          // loesche das Display
  void gotoxy(char x,char y);                             // positioniere Ausgabeposition
  void lcd_putchar_d(char ch);                            // setze Zeichen auf das Display
  void putramstring(char *c);                             // schreibe String aus dem RAM
  void putromstring(const unsigned char *dataPtr);        // dto. ROM

  #define prints(tx)     (putromstring(PSTR(tx)))         // Anzeige String aus Flashrom: prints("Text");
  #define printa(tx)     (putramstring(tx))               // Anzeige eines Strings der in einem Array im RAM liegt
*/

char wherex= 0;
char wherey= 0;
char invchar= 0;               // = 1 fuer inversive Textausgabe

/* -------------------- SPI Interface -------------------------- */

#ifndef softspi

  /* -------------------------------------------------------------
     SPI_INIT

     SPI-Hardwareinterface der AVR MCU konfigurieren
     ------------------------------------------------------------- */
  void spi_init(void)
  // SPI Hardware initialisieren
  {
    SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_CLK_PIN) | (1 << SPI_SS_PIN);

  }

  /* -------------------------------------------------------------
     SPI_OUT

        Byte ueber Software SPI senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  unsigned char spi_out(uint8_t data)
  {
    char a;

    for (a=0;a<8;a++)
    {
      if((data & 0x80)> 0) { LCD_PORT |= (1 << SPI_MOSI_PIN); }
                      else { LCD_PORT &= ~(1 << SPI_MOSI_PIN); }

      LCD_PORT |= (1 << SPI_CLK_PIN);                           // Taktleitung auf 1
      LCD_PORT &= ~(1 << SPI_CLK_PIN);                          // und wieder auf 0

      data <<= 1;
    }

  }

#else
  /* -------------------------------------------------------------
     SPI_INIT

     SPI-Softwareinterface der AVR MCU konfigurieren
     ------------------------------------------------------------- */

  void spi_init(void)
  // Pins der SPI Funktionen als Ausgang setzen
  {
    SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_CLK_PIN) | (1 << SPI_SS_PIN);

  }

  /* -------------------------------------------------------------
     SPI_OUT

        Byte ueber Software SPI senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  unsigned char spi_out(uint8_t data)
  {
    char a;

    for (a=0;a<8;a++)
    {
      if((data & 0x80)> 0) { LCD_PORT |= (1 << SPI_MOSI_PIN); }
                      else { LCD_PORT &= ~(1 << SPI_MOSI_PIN); }

      LCD_PORT |= (1 << SPI_CLK_PIN);                           // Taktleitung auf 1
      LCD_PORT &= ~(1 << SPI_CLK_PIN);                          // und wieder auf 0

      data <<= 1;
    }

  }

#endif


/* -------------------------------------------------------------
   WRCMD

   sendet Kommando via SPI an das LCD
   ------------------------------------------------------------- */
void wrcmd(uint8_t cmd)
{
  LCD_PORT &= ~(1 << LCD_DC_PIN);                    // C/D = 0 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(cmd);                                      // senden
  LCD_PORT |= (1 << LCD_CE_PIN );                    // Disable Controller
}

/* -------------------------------------------------------------
   WRDATA

   sendet Datum via SPI an das LCD
   ------------------------------------------------------------- */
void wrdata(uint8_t data)
{
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(data);                                     // senden
  LCD_PORT |= (1 <<  LCD_CE_PIN );                   // Disable Controller
}

/* -------------------------------------------------------------
   DIRECT3410_INIT

   initialisiert das Display
   ------------------------------------------------------------- */
void lcd_init()
{
  spi_init();

  LCD_DDR |= (1 << LCD_RST_PIN) | (1 << LCD_DC_PIN);     // Set LCD Output pins
  LCD_PORT &= ~(1 << LCD_RST_PIN);                       // Resets LCD controler
  _delay_ms(1);
  LCD_PORT |= (1 << LCD_RST_PIN);                        // Set LCD CE = 1 (Disabled)
  LCD_PORT |= (1 << LCD_CE_PIN);

  // LCD Controller Kommandos  (eigentliches initialisieren)

  wrcmd(0x21);                            // Erweiterter Kommandomodus
  wrcmd(0x09);                            // Set int. HV Generator (ca. 7V an Pin7)
  wrcmd(0xff);                            // VOP max
  wrcmd(0x16);                            // BIAS = 2
  wrcmd(0x06);                            // Temp. Koeffizient = 2
  wrcmd(0x20);                            // Standart Kommandomodus
  wrcmd(0x0c);                            // normale Ausgabe (normal = 0Ch, invertiert = 0Dh)

  clrscr();
}


/* -----------------------------------------------------
   CLRSCR

   loescht den Displayinhalt
   ----------------------------------------------------- */
void clrscr()
{
  int  i=0;

  wrcmd(0x80);             // Anfangsadresse des Displays
  wrcmd(0x40);
  for(i=1;i<(LCD_REAL_X_RES * LCD_REAL_Y_RES/8);i++) { wrdata(0x00); }
  gotoxy(0,0);
}

/* -----------------------------------------------------
   GOTOXY

   positioniert die Textausgabeposition auf X/Y
   ----------------------------------------------------- */
void gotoxy(char x,char y)
{
 wrcmd(0x80+(x*6));
 wrcmd(0x40+y);
 wherex= x; wherey= y;
}

/* -----------------------------------------------------
   PUTCHAR_D

   gibt ein Zeichen auf dem Display aus
   (die Steuerkommandos \n und \r fuer <printf> sind
   implementiert)
   ----------------------------------------------------- */
void lcd_putchar_d(char ch)
{
  int b,rb;

  if (ch== 13)
  {
    gotoxy(0,wherey);
    return;
  }
  if (ch== 10)
  {
    wherey++;
    gotoxy(wherex,wherey);
    return;
  }

  if (ch== 8)
  {
    if ((wherex> 0))
    {
      wherex--;
      gotoxy(wherex,wherey);
      for (b= 0;b<6;b++)
      {
        if (invchar) {wrdata(0xff);} else {wrdata(0);}
      }
      gotoxy(wherex,wherey);
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
  gotoxy(wherex,wherey);
}

/* ---------------------------------------------------
   PUTRAMSTRING
   gibt einen Text aus dem RAM an der aktuellen Position aus
   ---------------------------------------------------*/
void putramstring(char *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    lcd_putchar_d(*c++);
  }
}

/* ---------------------------------------------------
   PUTROMSTRING
   gibt einen Text aus dem ROM an der aktuellen Position aus
   ---------------------------------------------------*/
void putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar_d(c);
}

