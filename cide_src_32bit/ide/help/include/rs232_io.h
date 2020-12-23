/* ------------------ RS232_IO.H -------------------

     Kleine RS-232 Library die in den Beispielen der
     Hilfedatei verwendet wird um dort mittels
     <printf> und dgl. die Funktionsweisen der
     Beispiele aufzeigen zu koennen.

     Die eingestellte Baudrate ist 19200 Bd

     Compiler: AVR-GCC 4.3.2

     hier verfuegbare Integerlese- und
     schreibfunktion kann  mittels eines Defines

                #define rs232_nointread

     abgeschaltet werden. Immer dann empfehlenswert
     wenn der Speicherplatz knapp wird !!

      07.01.2015        R. Seelig

  -------------------------------------------------- */


#ifndef in_rs232_io
  #define in_rs232_io

//   #define rs232_nointread     // hiermit kann die Integereinlesefunktion
                                 // ueber die serielle Schnittstelle ab-
                                 // geschaltet werden

  #include <stdio.h>
  #include <avr/pgmspace.h>
  #include <stdint.h>
  #include <stdlib.h>

  #include "m328ports.h"

  #define  samplebaudrate   19200
  // #define  rs232_echo                      // wenn nicht auskommentiert wird ein
                                              // empfangenes Zeichen zurueck gesendet

  void rs232_init(void);
  void rs232_crlf();
  void rs232_putramstring(unsigned char *p);
  void rs232_putromstring(const unsigned char *dataPtr);

  #ifndef rs232_nointread
    int checkint16(char *p, int *myi);
    signed int rs232_readint();
    void rs232_putint(int i);
  #endif

  void uart_init(uint32_t baud);
  void uart_putchar(unsigned char ch);
  unsigned char uart_getchar(void);
  unsigned char uart_ischar(void);
  int uart_filein(FILE *stream);
  int uart_fileout(char ch, FILE *stream);
  static FILE uart_io = FDEV_SETUP_STREAM(uart_fileout,uart_filein,_FDEV_SETUP_RW);

  #define prints(tx)            rs232_putromstring(PSTR(tx))         // Benutzung: prints("Hallo Welt\n\r");
  #define rs232_putchar(ch)     uart_putchar(ch)
  #define rs232_getchar()       uart_getchar()

  /* --------------------------------------------------
      Register der seriellen Schnittstelle den
      Registernamen von ATmegaxx8 anpassen
     -------------------------------------------------- */

  #if defined (__AVR_ATmega8__) || defined (__AVR_ATtiny2313__)  || defined (__AVR_ATmega8515__)

    #define UBRR0       UBRR
    #define UBRR0L      UBRRL
    #define UBRR0H      UBRRH
    #define UCSR0A      UCSRA
    #define UCSR0B      UCSRB
    #define UCSR0C      UCSRC
    #define U2X0        U2X
    #define RXEN0       RXEN
    #define TXEN0       TXEN
    #define UCSZ00      UCSZ0
    #define UCSZ01      UCSZ1
    #define UDRE0       UDRE
    #define UDR0        UDR
    #define RXC0        RXC

  #endif

#endif
