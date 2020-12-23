/* --------------------     TXLCD.C    --------------------

     grundlegende Funktionen zum Betrieb eines
     LC-Textdisplays an einem AVR-Microcontroller.

     Hier am ATtiny2313

     10.02.2015        R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss am Bsp. Pollin-Display C0802-04 an
      einen ATtiny2313
      Pinkonfiguration fuer - lcdpindef1 - (in txlcd.h)
      ---------------------------------------------------
         o +5V
         |                            Display                 Controller
         _                        Funktion   PIN            PIN    Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------   19    PB7
        \ /                          GND      5 ------------
        -'-                    (Takt) E       6 ------------   18    PB6
         |                           D4       7 ------------   14    PB2
        --- GND                      D5       8 ------------   15    PB3
                                     D6       9 ------------   16    PB4
                                     D7      10 ------------   17    PB5
*/

#include "txlcd.h"

char wherex,wherey;


/* -------------------------------------------------------
      DELAY

      Zeitverzoegerung ist WERT * 0,1s
   ------------------------------------------------------- */

void delay(char wert)
{
   char i;
   for (i=0;i< wert;i++)
   {
     _delay_ms(100);
   }
}

/* -------------------------------------------------------
     NIBBLEOUT

     sendet ein Halbbyte an das LC-Display an die in
     den Defines angegebenen Pins

         WERT= gesamtes Byte
         HILO= 1 => oberen 4 Bits werden gesendet
         HILO= 0 => untere 4 Bits werden gesendet
   ------------------------------------------------------- */

void nibbleout(unsigned char wert, unsigned char hilo)
{
  if (hilo)
  {
     if (testbit(wert,7)) { setbit(txlcd_db,txlcd_d7); }
                    else  { clrbit(txlcd_db,txlcd_d7); }
     if (testbit(wert,6)) { setbit(txlcd_db,txlcd_d6); }
                    else  { clrbit(txlcd_db,txlcd_d6); }
     if (testbit(wert,5)) { setbit(txlcd_db,txlcd_d5); }
                    else  { clrbit(txlcd_db,txlcd_d5); }
     if (testbit(wert,4)) { setbit(txlcd_db,txlcd_d4); }
                    else  { clrbit(txlcd_db,txlcd_d4); }
  }
  else
  {
     if (testbit(wert,3)) { setbit(txlcd_db,txlcd_d7); }
                    else  { clrbit(txlcd_db,txlcd_d7); }
     if (testbit(wert,2)) { setbit(txlcd_db,txlcd_d6); }
                    else  { clrbit(txlcd_db,txlcd_d6); }
     if (testbit(wert,1)) { setbit(txlcd_db,txlcd_d5); }
                    else  { clrbit(txlcd_db,txlcd_d5); }
     if (testbit(wert,0)) { setbit(txlcd_db,txlcd_d4); }
                    else  { clrbit(txlcd_db,txlcd_d4); }
  }
}

/* -------------------------------------------------------
      txlcd_TAKT

      gibt einen Clockimpuls an das Display
   ------------------------------------------------------- */

void txlcd_takt(void)
{
  setbit(txlcd_ctrl,txlcd_e);
  _delay_us(60);
  clrbit(txlcd_ctrl,txlcd_e);
  _delay_us(60);
}

/* -------------------------------------------------------
      txlcd_IO

      sendet ein Byte an das Display an die in den
      Defines angegebenen Pins
              Wert = zu sendendes Byte
   ------------------------------------------------------- */

void txlcd_io(char wert)
{
  nibbleout(wert,1);
  txlcd_takt();
  nibbleout(wert,0);
  txlcd_takt();
}

/* -------------------------------------------------------
     txlcd_INIT

     bereitet das Display fuer den Betrieb vor. Die in
     den Defines gegebenen Anschluesse des Displays
     werden auf Ausgang geschaltet.
     Es wird 4-Bit Datenuebertragung verwendet.

   ------------------------------------------------------- */

void txlcd_init(void)
{
  char i;

  txlcd_dbio |= 1<<txlcd_d7 | 1<<txlcd_d6 | 1<<txlcd_d5 | 1<<txlcd_d4;
  txlcd_ctrlio |= 1<<txlcd_rs | 1<<txlcd_e;

  clrbit(txlcd_ctrl,txlcd_rs);
  for (i= 0; i< 3; i++)
  {
     txlcd_io(0x20);
	 _delay_ms(6);
  }
  txlcd_io(0x28);
  _delay_ms(6);
  txlcd_io(0x0c);
  _delay_ms(6);
  txlcd_io(0x01);
  _delay_ms(6);
  wherex= 0; wherey= 0;
}

/* -------------------------------------------------------
     GOTOXY

     setzt den Textcursor an eine Stelle im Display. Die
     obere linke Ecke hat die Koordinate (1,1)
   ------------------------------------------------------- */

void gotoxy(char x, char y)
{
  unsigned char txlcd_adr;

  txlcd_adr=(0x80+((y-1)*0x40))+x-1;
  clrbit(txlcd_ctrl,txlcd_rs);
  txlcd_io(txlcd_adr);
  wherex= x;
  wherey= y;
}

/* -------------------------------------------------------
     txlcd_setuserchar

     kopiert die Bitmap eines benutzerdefiniertes Zeichen
     in den Charactergenerator des Displaycontrollers

               nr : Position im Ram des Displays, an
                    der die Bitmap hinterlegt werden
                    soll.
        *userchar : Zeiger auf die Bitmap des Zeichens

   Bsp.:  txlcd_setuserchar(3,&meinezeichen[0]);
          txlcd_putchar(3);

   ------------------------------------------------------- */


void txlcd_setuserchar(char nr, const char *userchar)
{
  char b;

  clrbit(txlcd_ctrl,txlcd_rs);
  txlcd_io(0x40+(nr << 3));                         // CG-Ram Adresse fuer eigenes Zeichen
  setbit(txlcd_ctrl,txlcd_rs);
  for (b= 0; b< 8; b++) txlcd_io(pgm_read_byte(userchar++));
  clrbit(txlcd_ctrl,txlcd_rs);
}


/* -------------------------------------------------------
     txlcd_PUTCHAR

     platziert ein Zeichen auf dem Display.

               CH = auszugebendes Zeichen
   ------------------------------------------------------- */

void txlcd_putchar(char ch)
{
  setbit(txlcd_ctrl,txlcd_rs);
  txlcd_io(ch);
  wherex++;
}

/* -------------------------------------------------------
      txlcd_PUTRAMSTRING

      gibt einen AsciiZ Text der im RAM gespeichert ist
      auf dem Display aus.

      Bsp.:

      char strbuf[] = "H. Welt";

      putramstring(strbuf);

   ------------------------------------------------------- */

void txlcd_putramstring(char *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    txlcd_putchar(*c++);
  }
}

/* -------------------------------------------------------
     txlcd_PUTROMSTRING

     gibt einen Text, der im Flashrom gespeichert ist
     auf dem LC-Display aus (Wird vom Macro <prints>
     aufgerufen .

     Bsp.:

       static const uint8_t mytext[] PROGMEM = "Text";

       putromstring(PSTR("H. Welt"));
       putromstring(&mytext[0]);

       prints("Hallo");

   ------------------------------------------------------- */

void txlcd_putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) txlcd_putchar(c);
}

/* -------------------------------------------------------
     TXLCD_PUTINT

     gibt eine Integer-Variable auf dem Display aus

   ------------------------------------------------------- */

void txlcd_putint(int i)
{
  int  z,z2;
  char b, first;

  first= 1;
  if (i==0) {txlcd_putchar('0'); return;}
  if (i< 0) { txlcd_putchar('-'); }
  i= abs(i);
  z= 10000;
  for (b= 5; b; b--)
  {
    z2= i / z;
    if (! ((first) && (z2 == 0)) )
    {
      txlcd_putchar(z2 + 48);
      first= 0;
    }
    z2 = z2 * z;
    i -= z2;
    z = z / 10;
  }
}
