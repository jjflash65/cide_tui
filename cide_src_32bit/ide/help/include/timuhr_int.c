/*   --------------------- UHR_INT.C ----------------------
     Library Datei fuer eine interruptgesteuerte
     Softwareuhr.

     benoetigt zwingend das DEFINE F_CPU zur Berechnung
     des Reloadfaktors des Timers

     Controller: ATmega8 / ATmegaxx8 / ATtiny2313

     IDE-Platform:  AVR-Context mit WINAVR
     17.06.2014

     ------------------------------------------------------ */

#include "timuhr_int.h"

uint8_t volatile count256;
uint8_t volatile std,min,sek,vsek;


/*   -------------------- UHR_INT.AVC ----------------------
     Interruptgesteuerte Softwareuhr

     benoetigt zwingend das DEFINE F_CPU zur Berechnung
     des Reloadfaktors des Timers

     IDE-Platform:  AVR-Context mit WINAVR
     11.01.2013

     ------------------------------------------------------ */


ISR (TIMER1_COMPA_vect)
{
  if ( (count256 % 64)== 0)
  {
    vsek++;
    vsek = vsek % 4;
  }
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
  OCR1A = F_CPU / t1reload-1;
  TCNT1 = 0;
  count256= 0;

  TIMSK1 = 1<<OCIE1A;
  sei();
}
