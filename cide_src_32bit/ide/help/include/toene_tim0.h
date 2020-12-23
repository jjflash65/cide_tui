/* -------------------- TOENE_TIM0.H ---------------------

     Header-Datei fuer eine "Abspielfunktion"
     zum Abspielen eines Notenstrings

     Verwendet 8-Bit Timer0 zum Abspielen sodass der
     16-Bit Timer einer evtl. Software Uhr vorbehalten
     bleibt.

     Hardware:
        MCU     : ATtiny2313, ATmega8, ATmegaxx8

     Compiler: AVR-GCC 4.3.2

     18.06.2014        R. Seelig

  -------------------------------------------------------- */

#ifndef in_toene_tim0
  #define in_toene_tim0

  #include <util/delay.h>                                    // beinhaltet _delay_ms(char) und _delay_us(char)
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>


  // ------------------ Speaker ----------------------------

  #ifdef __AVR_ATtiny2313__

    // Beispielanschluss an einem ATtiny2313

    #define speakerdir       DDRD
    #define speakerport      PORTD
    #define speakerpin       PD5

  #else

    // Beispielanschluss an einem ATmega

    #define speakerdir       DDRC
    #define speakerport      PORTC
    #define speakerpin       PC3

  #endif

  #define setspk()         (speakerport |= (1 << speakerpin))
  #define clrspk()         (speakerport &= ~(1 << speakerpin))


  #ifndef F_CPU
    #error Keine F_CPUtaktangabe, kann Timerreload nicht bestimmen
  #endif

  #ifndef OCR1A
    #define OCR1A OCR1	                // ATtiny2313
  #endif

  #ifndef WGM12
    #define WGM12 CTC1	                // ATtiny2313
  #endif

  #ifndef TIMSK0
    #define TIMSK0 TIMSK
  #endif

  #define startreload 833
  #define playtempo 5

  extern uint8_t togglespk;
  extern volatile uint8_t sound;
  extern volatile uint8_t cnt;
  extern volatile uint8_t srout;

  extern int freqreload [24];

  ISR (TIMER0_COMPA_vect);
  void toene_init(void);
  void spk_delay(int wert);
  void spk_puls(int wert);
  void settonfreq(uint16_t wert);
  void tonlen(int w);
  void playnote(char note);
  void playstring(const unsigned char* const s);


#endif
