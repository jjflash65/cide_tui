/* ----------------- RS232_M168.C ------------------

     Include -Datei fuer einfache Grundfunktionen
     zur Nutzung des Hardware - USART

     Hardware:
        MCU     : ATmegaxx8

     Versuch    : diverses

     Compiler: AVR-GCC 4.7.2

     22.07.2014        R. Seelig

  -------------------------------------------------- */


#include <stdio.h>

void uart_init(uint32_t baud);
void uart_putchar(unsigned char ch);
unsigned char uart_getchar(void);

static int uart_fileout(char ch, FILE *stream);



/* --------------------------------------------------
    Initialisierung der seriellen Schnittstelle:

    Uebergabe: baud = Baudrate
    Protokoll: 8 Daten-, 1 Stopbit
   -------------------------------------------------- */

/* --------------------------------------------------
    serielle Schnittstelle initialisieren
   -------------------------------------------------- */

void uart_init(uint32_t baud)
{
  uint16_t ubrr;

  if (baud> 57600)
  {
    baud= baud>>1;
    ubrr= (F_CPU/16/baud);
    ubrr--;
    UCSR0A |= 1<<U2X0;                                  // Baudrate verdoppeln
  }
  else
  {
    ubrr= (F_CPU/16/baud-1);
  }
  UBRR0H = (unsigned char)(ubrr>>8);                    // Baudrate setzen
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);                       // Transmitter und Receiver enable
  UCSR0C = (3<<UCSZ00);                                 // 8 Datenbit, 1 Stopbit
}

/* --------------------------------------------------
    Zeichen ueber die serielle Schnittstelle senden
   -------------------------------------------------- */

void uart_putchar(unsigned char ch)
{
  while (!( UCSR0A & (1<<UDRE0)));                      // warten bis Transmitterpuffer leer ist
  UDR0 = ch;                                            // Zeichen senden
}

/* --------------------------------------------------
    Zeichen ueber einen Stream (bspw. ueber
    PrintF versenden.
   -------------------------------------------------- */

static int uart_fileout(char ch, FILE *stream)
{
  uart_putchar(ch);
}


/* --------------------------------------------------
    Zeichen von serieller Schnittstelle lesen
   -------------------------------------------------- */

unsigned char uart_getchar( void )
{
  while(!(UCSR0A & (1<<RXC0)));                         // warten bis Zeichen eintrifft
return UDR0;
}


/* --------------------------------------------------
    "Dateivariable" ueber den per Umleitung des
    Standartstreams (stdio) auf der seriellen
    Schnittstelle gesendet wird.
   -------------------------------------------------- */

static FILE uartout = FDEV_SETUP_STREAM(uart_fileout,NULL,_FDEV_SETUP_WRITE);
