/*   ------------------- TIMUHR_INT.H ---------------------
     Library Datei fuer eine interruptgesteuerte
     Softwareuhr.

     benoetigt zwingend das DEFINE F_CPU zur Berechnung
     des Reloadfaktors des Timers

     Controller: ATmega8 / ATmegaxx8 / ATtiny2313

     IDE-Platform:  AVR-Context mit WINAVR
     17.06.2014

     ------------------------------------------------------ */

#ifndef in_timuhr
  #define in_timuhr

  #include <avr/interrupt.h>


  #ifndef OCR1A
    #define OCR1A OCR1	                // ATtiny2313
  #endif

  #ifndef WGM12
    #define WGM12 CTC1	                // ATtiny2313
  #endif

  #ifndef TIMSK1
    #define TIMSK1 TIMSK
  #endif

  #define t1reload 256L

  //  (F_CPU / t1reload)-1  => Wert Vergleichsregister (nach deren Impulse wird Interrupt ausgeloest
  //   dieser Wert * Zykluszeit (16MHz) Takt ergibt einze Zeit von 3906,25 us
  //  Diese Zeit Zaehlt die Variable count256. 3609,25 us * 256 = 1 s !!!!!

  extern uint8_t volatile count256;
  extern uint8_t volatile std,min,sek,vsek;

  ISR (TIMER1_COMPA_vect);
  void timer1_init(void);

#endif