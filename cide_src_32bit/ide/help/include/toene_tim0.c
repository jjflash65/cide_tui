/* --------------------- TOENE_TIM0.C --------------------

     Library-Datei fuer eine "Abspielfunktion"
     zum Abspielen eines Notenstrings

     Verwendet 8-Bit Timer0 zum Abspielen sodass der
     16-Bit Timer einer evtl. Software Uhr vorbehalten
     bleibt.

     Hardware:
        MCU     : ATtiny2313, ATmega8, ATmegaxx8

     Compiler: AVR-GCC 4.3.2

     18.06.2014        R. Seelig

  -------------------------------------------------------- */


#include "toene_tim0.h"


uint8_t togglespk;
volatile uint8_t sound;
volatile uint8_t cnt;
volatile uint8_t srout;

int freqreload [24] = { 523,  555,  587,  623,  659,  699,  740,  784,  831,  881,  933, 988,
                        1047, 1109, 1175, 1245, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1975 };

ISR (TIMER0_COMPA_vect)
{
  if (sound)
  {
    if (togglespk & 1)
    {
      setspk();
      togglespk= 0;
    }
    else
    {
      clrspk();
      togglespk= 1;
    }
  }
  TCNT0= 0;
}


void toene_init(void)
{
  togglespk= 1;
  speakerdir |= (1 << speakerpin);                                       // Anschlusspin des Lautsprechers als Ausgang schalten

  TCCR0B = 1<<CS12;
  OCR0B = (F_CPU/256) / startreload-1;
  TCNT0 = 0;

  TIMSK0 = 1<<OCIE0A;
  sei();
}


// --------- Ende TIMERINTERRUPT --------------


void spk_delay(int wert)
{
  int i;

  for (i= 0; i< wert; i++)
  {
    _delay_us(1);
  }
}

void spk_puls(int wert)
{
  clrspk();
  spk_delay(wert);
  setspk();
  spk_delay(wert);
}

void settonfreq(uint16_t wert)
{
  OCR0A = (F_CPU/256) / wert-1;
}

void tonlen(int w)
{
  int cx;

  w = w*100;
  for (cx= 0; cx< w; cx++) { _delay_us(100); }
  sound= 0;
  _delay_ms(30);
}

void playnote(char note)
{
  settonfreq(freqreload[note]);
  sound= 1;
}

void playstring(const unsigned char* const s)
{
  char ch;
  char aokt;
  int dind;

  aokt= 0; dind= 0;
  ch= pgm_read_byte(&(s[dind]));
  while (ch)
  {
    ch= pgm_read_byte(&(s[dind]));
    switch(ch)
    {
      case '-': { aokt= aokt-12; break; }
      case '+': { aokt= aokt+12; break; }
      case 'c': { playnote(aokt); break; }
      case 'C': { playnote(aokt+1); break; }
      case 'd': { playnote(aokt+2); break; }
      case 'D': { playnote(aokt+3); break; }
      case 'e': { playnote(aokt+4); break; }
      case 'f': { playnote(aokt+5); break; }
      case 'F': { playnote(aokt+6); break; }
      case 'g': { playnote(aokt+7); break; }
      case 'G': { playnote(aokt+8); break; }
      case 'a': { playnote(aokt+9); break; }
      case 'A': { playnote(aokt+10); break; }
      case 'h': { playnote(aokt+11); break; }
      case '1': { tonlen(16*playtempo); break; }
      case '2': { tonlen(8*playtempo); break; }
      case '3': { tonlen(6*playtempo); break; }
      case '4': { tonlen(4*playtempo); break; }
      case '5': { tonlen(3*playtempo); break; }
      case '8': { tonlen(2*playtempo); break; }
    }
    dind++;
  }
}
