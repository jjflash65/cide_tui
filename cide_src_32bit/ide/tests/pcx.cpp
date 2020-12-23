/* ----------------------- PCX.CPP ------------------------

     zeigt eine auf 8 Bit Farben reduziertes PCX Grafik auf
     einem Farbdisplay an. Hierfuer muss in einem Haupt-
     programm eine Funktion namens "putpixel" vorhanden sein
     so dass diese Include - Datei erst NACH einer
     Grafikdatei eingebunden werden kann!!

     17.03.2014        R. Seelig

   --------------------------------------------------------*/

#include "pcx.h"

#define  pcx(nr)         ( pcxarray[nr] )


void putpixel(uint16_t x, uint16_t y, uint16_t f)
{
}

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


// --------------------------------------------------
//                   SHOWINC_PCX
//   zeigt ein in einem Array liegende PCX-Grafik
//   ab den Koordinaten  x / y (linke obere Ecke)
//   auf dem Bildschirm an. PCX-Grafik muss zwingend
//   eine 256 Farben Grafik beinhalten
// --------------------------------------------------
void showinc_pcx(const uint8_t *pcxarray, unsigned int filesize, int x, int y)
{


  int16_t   anz, pos, c, w, h, e, pack;
  int16_t   nr;
  uint8_t   b;
  int16_t   c2;

  uint16_t  palofs;
  uint8_t   ro,ge,bl;
  uint16_t  pw;


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
      
        palofs= filesize - (3*256)-1;
        nr = ((pos)*3)+1;
        nr= nr+palofs;
        ro= pcx(nr);
        ge= pcx(nr+1);
        bl= pcx(nr+2);
        pw= rgbfromvalue(ro,ge,bl);

      
        putpixel(x+c,y,pw);
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

        palofs= filesize - (3*256)-1;
        nr = ((pos)*3)+1;
        nr= nr+palofs;
        ro= pcx(nr);
        ge= pcx(nr+1);
        bl= pcx(nr+2);
        pw= rgbfromvalue(ro,ge,bl);
      
        putpixel(x+c,y, pw);
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
