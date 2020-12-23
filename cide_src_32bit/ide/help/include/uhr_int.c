/*   --------------------- UHR_INT.C ----------------------
     Include-Datei fuer eine interruptgesteuerte 
     Softwareuhr.
     
     benoetigt zwingend das DEFINE F_CPU zur Berechnung
     des Reloadfaktors des Timers
     
     Controller: ATmega8 / ATmegaxx8 / ATtiny2313

     IDE-Platform:  AVR-Context mit WINAVR
     17.06.2014

     ------------------------------------------------------ */
     
#include <avr/interrupt.h>     


#ifndef QUARZ
  #error Keine Quarztaktangabe, kann Timerreload nicht bestimmen
#endif
     

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

//  (Quarz / t1reload)-1  => Wert Vergleichsregister (nach deren Impulse wird Interrupt ausgeloest
//   dieser Wert * Zykluszeit Takt ergibt einze Zeit von 3906,25 us
//  Diese Zeit Zaehlt die Variable count256. 3609,25 us * 256 = 1 s !!!!!

uint8_t volatile count256;
uint8_t volatile std,min,sek;

ISR (TIMER1_COMPA_vect);
void timer1_init(void);

/*   -------------------- UHR_INT.AVC ----------------------
     Interruptgesteuerte Softwareuhr
     
     benoetigt zwingend das DEFINE F_CPU zur Berechnung
     des Reloadfaktors des Timers

     IDE-Platform:  AVR-Context mit WINAVR
     11.01.2013

     ------------------------------------------------------ */


ISR (TIMER1_COMPA_vect)
{
  if( --count256 == 0 )
  {
    sek++;
    if (sek==60)
    {
      sek= 0;
      min++;
      if (min==60)
      {
        min= 0;
        std++;
        if (std==24) {std= 0;}
      }
    }
  }
}

void timer1_init(void)
{
  TCCR1B = 1<<WGM12 | 1<<CS10;
  OCR1A = QUARZ / t1reload-1;
  TCNT1 = 0;
  count256= 0;

  TIMSK1 = 1<<OCIE1A;
  sei();
}
