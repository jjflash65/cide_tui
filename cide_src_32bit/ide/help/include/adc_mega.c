/* -------------------- ADC_MEGA.C ------------------

     Include -Datei fuer einfache Grundfunktionen
     zum Einlesen analoger Spannungswerte

     Hardware:
        MCU     : ATmegaxx8

     Versuch    : diverses

     Compiler: AVR-GCC 4.7.2

     22.07.2014        R. Seelig

  -------------------------------------------------- */
  
// ----------------------- ADC ---------------------------

#define adc_low        ADCL
#define adc_high       ADCH

unsigned int getadc_10bit (void)
{
  unsigned int adcwert,adch;

  adcwert= adc_low;
  adch= adc_high;
  adch = adch<<8;
  adcwert|= adch;
  return(adcwert);
}

void adc_init(uint8_t refmode, uint8_t channel)
{
/* <refmode> bezeichnet die zu verwendende Referenz
     0 : externe Referenz an Aref
     1 : +AVcc mit ext. Kondensator
     2 : reserviert
     3 : interne Referenz: 1,1V ATmega168 / 2,56V ATmega8

   <channel> bezeichnet den Kanal auf dem gemessen werden soll

   Spannungsreferenz ATMEGA168:

Bit: 7      6
   REFS1  REFS0      Referenz
     0      0        ext. Referenz an  Aref
     0      1        +AVcc mit ext. Kondensator
     1      0        reserviert
     1      1        Interne Spannungsreferenz: 1,1V (ATmega168) / 2,56V (ATmega8)

  Bit5: ADLAR = 0 (rechtsbuendig); MUX= 0 => ADC0 gewaehlt

Bit3..0: Selektor Analogmultiplexer


   // ADMUX= 1<<REFS0;                                          // AVcc (Betriebsspannung) als Referenz
   // ADMUX= 1<<REFS0 | 1<<REFS1;                               // int. Referenz
*/



   ADMUX = refmode<<6;
   ADMUX |= channel;

   ADCSRA= 0xe7;						// ADC enable, single gestartet, free running mode
           							// Taktteiler / 128
   _delay_ms(1);
   ADCSRA= 0xa7;
}
