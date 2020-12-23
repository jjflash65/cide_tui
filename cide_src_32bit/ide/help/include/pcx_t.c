/* ----------------------- pcx_t.c    ------------------------

     zeigt eine auf 8 Bit Farben reduziertes PCX Grafik auf
     einem Farbdisplay an.

     Im Gegensatz zu pcx.c kann ein Farbindex als trans-
     parent angegeben werden, so dass dieser bei der Aus-
     gabe NICHT gezeichnet wird.

     MCU:              ATmegaxx8

     Compiler:         AVR-GCC
     IDE:              RSIDE

     11.03.2015        R. Seelig

   --------------------------------------------------------*/

#include "pcx_t.h"

// --------------------------------------------------
//                   SHOWINC_PCX_T
//   zeigt ein in einem Array liegende PCX-Grafik
//   ab den Koordinaten  x / y (linke obere Ecke)
//   auf dem Bildschirm an. PCX-Grafik muss zwingend
//   eine 256 Farben Grafik beinhalten
//
//   t beinhaltet den Paletteneintrag, der als
//   "transparente Farbe" behandelt werden soll !
// --------------------------------------------------
void showinc_pcx_t(const unsigned char* const pcxarray, unsigned int filesize, int x, int y, int t)
{

  #define  pcx(nr)         (pgm_read_byte(&pcxarray[nr]))

  int anz, pos, c, w, h, e, pack;
  uint8_t b;
  int c2;

  uint16_t frompcxpal(unsigned int nr)                       // gibt aus einem Bytearray dass die RGB
                                                            // Farbpalette beinhaltet einen Integerwert
                                                            // zurueck
  {
    unsigned int palofs;
    unsigned char r,g,b;
    uint16_t pw;

    palofs= filesize - (3*256)-1;

     // --------------------------------------------------
     //       Farbumrechnung in einen 8-Bit RGB-Wert
     //       Bitbelegung:
     //
     //               |   rot      |   gruen    |  blau  |
     //       Bits:   | D7  D6  D5 | D4  D3  D2 | D1  D0 |
     // --------------------------------------------------

    nr = ((nr)*3)+1;
    nr= nr+palofs;
    r= pcx(nr);
    g= pcx(nr+1);
    b= pcx(nr+2);
    pw= rgbfromvalue(r,g,b);

    return pw;
  }

  if ((pcx(0) != 10) | (pcx(3) != 8))                  // Identity Bytes abfragen
  {
                                                       // "Datei ist ein nicht darstellbares Format !!"
    return;                                            // Function mit Fehlercode beenden
  }

                                                       // Bildformat berechnen,
                                                       // w = Pixel in X-Achse / h = Pixel in Y-Achse
  w= ((pcx(9) - pcx(5))*256 + pcx(8) - pcx(4))+1;
  h= ((pcx(11) - pcx(7))*256 + pcx(10) - pcx(6))+1;

  pack= 0; c= 0; e= y+h;

  pos= 128;
  while (y <e)
  {
    if (pack != 0)
    {
      for (c2= 0; c2< (pack); c2++)
      {
        if (pcx(pos) != t)
        {
          putpixel(x+c,y,frompcxpal(pcx(pos)));
        }
        if (c== w)
        {
          c= 0;
          y++;
        }
        else
        {
          c++;
        }
      }
      pack= 0;
    }
    else
    {
      if ((pcx(pos) & 0xc0)== 0xc0)
      {
        pack= pcx(pos) & 0x3f;
      }
      else
      {
        if (pcx(pos) != t)
        {
          putpixel(x+c,y,frompcxpal(pcx(pos)));
        }
        c++;
      }
    }
    pos++;
    if (c== w)
    {
      c= 0;
      y++;
    }
  }
}
