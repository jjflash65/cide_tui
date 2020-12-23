/* ----------------------- PCX.C    ------------------------

     zeigt eine auf 8 Bit Farben reduziertes PCX Grafik auf
     einem Farbdisplay an. Hierfuer muss in einem Haupt-
     programm eine Funktion namens "putpixel" vorhanden sein
     so dass diese Include - Datei erst NACH einer
     Grafikdatei eingebunden werden kann!!

     MCU:              ATmegaxx8
     Quarz:            16MHz

     Compiler:         AVR-GCC
     IDE:              Geany

     17.03.2014        R. Seelig

   --------------------------------------------------------*/

#include "pcx.h"


// --------------------------------------------------
//                   SHOWINC_PCX
//   zeigt ein in einem Array liegende PCX-Grafik
//   ab den Koordinaten  x / y (linke obere Ecke)
//   auf dem Bildschirm an. PCX-Grafik muss zwingend
//   eine 256 Farben Grafik beinhalten
// --------------------------------------------------
void showinc_pcx(const unsigned char* const pcxarray, unsigned int filesize, int x, int y)
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
        putpixel(x+c,y,frompcxpal(pcx(pos)));
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
        putpixel(x+c,y,frompcxpal(pcx(pos)));
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
