/* --------------------------- hef4094_mega.c -------------------------

   Ein HEF4094 Schieberegister als Porterweiterung fuer ATmega MCU's
   Verwendet Hardware - SPI !!

   Hardware:
      MCU     : ATmega8, ATmegaxx8

   11.03.2015            R. Seelig

   --------------------------------------------------------------------

   PINS HEF4094

   16                               9
   ------------------------------------
  | Vdd  EO   O4   O5  O6  O7  /OS  OS |
  | STR  DAT  CLK  O0  O1  O2  O3   GND|
   ------------------------------------
   1                                10

   STR (Strobe): Impuls zur Uebernahme des Schieberegisterinhaltes
                 in das Ausgangsregister ( in Ruhezustand = 1 )
   DAT (Datum) : serielles Datenbit (das bei einem Taktimpuls
                 in das Schieberegister geschoben wird)
   CLK (Clock) : Taktleitung (Uebernahme des an DAT anliegenden
                 Bits bei positiver Taktflanke)
   O0 bis 07   : parallele Ausgaenge des Schieberegisters
   OS          : serielle Ausgang
   /OS         : negierter serieller Ausgang
   EO          : (enable Output) 1 = Ausgaenge sind verfuegbar
                                 0 = Ausgaenge hochohmig (abgeschalten)
   Vdd         : + Versorgungsspannung
   GND         : Masse


   17.06.2014  R. Seelig

   ------------------------------------------------------------------ */


#include "hef4094_mega.h"

/*
     _a_            Pinzuordnung KINGSBRIGHT SA52 7-Segment LED
    |   |
   f|   |b
     -g-            7-Segmentanzeige   Bitnummer
    |   |
   e|___|c                 a ------------  6
      d                    b ------------  5
                           c ------------  0
                           d ------------  1
                           e ------------  2
                           f ------------  4
                           g ------------  3
*/


/* Bitmuster fuer eine  LED 7-Segmentanzeige
   0 => LED an

  Beispiel:

  set4094(~(led7sbmp[5]));     // zeigt auf einer 7 Segmentanzeige die Ziffer 5 an

*/

uint8_t led7sbmp[10] = {0xf7,0x21,0xee,0x6b,0xb9,0xdb,0xdf,0xe1,0xff,0xfb};
uint8_t port4094 = 0x00;

void set4094(unsigned char wert)
{
  signed char i;
  uint8_t oldspi;

  oldspi= SPCR;
  SPCR= 0;                                  // Hardware SPI abschalten, da ansonsten der CLK
                                            // blockiert ist !!

  for (i=7;i> -1;i--)
  {
    if ((wert>>i)&1) {setb4094(datport,dat4094);} else {clrb4094(datport,dat4094);}
    setb4094(clkport,clk4094);
    clrb4094(clkport,clk4094);              // Taktimpuls erzeugen
  }
  setb4094(strport,str4094);                // Strobeimpuls : Daten Schieberegister ins Latch uebernehmen
  clrb4094(strport,str4094);
  SPCR= oldspi;                             // Status der Hardware SPI restaurieren
}

void setbit4094(char nr)
{
  port4094 |= (1 << nr);
  set4094(port4094);
}

void clrbit4094(char nr)
{
  port4094 &= ~(1 << nr);
  set4094(port4094);
}

void h4094_init(void)
{
  // Leitungen die den 4094 bedienen als Ausgaenge schalten
  setb4094(clkdir,clk4094);
  setb4094(datdir,dat4094);
  setb4094(strdir,str4094);

  clrb4094(strport,str4094);  // Strobe (fuer Latch)= 0
  clrb4094(clkport,clk4094);
  clrb4094(datport,dat4094);
}

void spi_off(void)
{
  SPCR &= ~(1 << SPE);                   // SPI-Funktionalitaet ausschalten
}

void spi_on(void)
{
  SPCR |= (1 << SPE);                   // SPI-Funktionalitaet anschalten
}
