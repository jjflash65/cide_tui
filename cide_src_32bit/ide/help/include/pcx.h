/* ----------------------- PCX.H  -------------------------

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

#ifndef inpcx
  #define inpcx

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <stdlib.h>
  #include <stdio.h>

  #include "coldisplay.h"

  void showinc_pcx(const unsigned char* const pcxarray, unsigned int filesize, int x, int y);


#endif
