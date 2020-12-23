/* --------------------------- hef4094_mega.h -------------------------

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


// Zwingend notwendig, <avr/io.h> weiss, welche Adressen fuer
// die Ports zu verwenden sind !!!!

#ifndef in_hef4094_mega
  #define in_hef4094_mega


  #include <avr/io.h>
  #include <util/delay.h>

  #define clkdir           DDRB
  #define clkport          PORTB
  #define clk4094          5          // Taktleitung Schieberegister HEF4094, Datenuebernahme pos. Flanke
  #define datdir           DDRB
  #define datport          PORTB
  #define dat4094          3          // Datenleitung Schieberegister HEF4094
  #define strdir           DDRD
  #define strport          PORTD
  #define str4094          7          // Strobeleitung Schieberegister, Datenuebernahme pos. Flanke
                                      // (Impuls übernimmt Register auf die Tri-State Ausgaenge)

  #define setb4094(myport,nr)  (myport |= (1<<nr))
  #define clrb4094(myport,nr)  (myport &= (~(1<<nr)))

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


  extern uint8_t led7sbmp[10];
  extern uint8_t port4094;

  void h4094_init(void);
  void set4094(unsigned char wert);
  void setbit4094(char nr);
  void clrbit4094(char nr);

#endif
