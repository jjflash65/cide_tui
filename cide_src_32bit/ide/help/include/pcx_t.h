/* ----------------------- pcx_t.h ------------------------

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

#ifndef in_pcx_t
  #define in_pcx_t

  #include "coldisplay.h"

  void showinc_pcx_t(const unsigned char* const pcxarray, unsigned int filesize, int x, int y, int t);

#endif

