/* ------------------ RS232_IO.C -------------------

     Kleine RS-232 Library die in den Beispielen der
     Hilfedatei verwendet wird um dort mittels
     <printf> und dgl. die Funktionsweisen der
     Beispiele aufzeigen zu koennen.

     Die eingestellte Baudrate ist 19200 Bd

     Compiler: AVR-GCC 4.7.2

     hier verfuegbare Integerlese- und
     schreibfunktion kann  mittels eines Defines

                #define rs232_nointread

     abgeschaltet werden. Immer dann empfehlenswert
     wenn der Speicherplatz knapp wird !!

      07.01.2015        R. Seelig

  -------------------------------------------------- */


#include <stdio.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>

#include "rs232_io.h"
// #include "m328ports.h"


/* --------------------------------------------------
    serielle Schnittstelle initialisieren
   -------------------------------------------------- */



  /* --------------------------------------------------
      Initialisierung der seriellen Schnittstelle:

      Uebergabe: baud = Baudrate
      Protokoll: 8 Daten-, 1 Stopbit
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

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega168__) || defined (__AVR_ATtiny2313__)

    UCSR0B = (1<<RXEN0)|(1<<TXEN0);                       // Transmitter und Receiver enable
    UCSR0C = (3<<UCSZ00);                                 // 8 Datenbit, 1 Stopbit

#else

    // ATmega8
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSRC  = (1<<URSEL) | (1<<UCSZ01) | (1<<UCSZ00);

#endif
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
      testen, ob ein Zeichen auf der Schnittstelle
      ansteht
     -------------------------------------------------- */

  unsigned char uart_ischar( void )
  {
    return (UCSR0A & (1<<RXC0));
  }

  /* --------------------------------------------------
      Zeichen von serieller Schnittstelle lesen
     -------------------------------------------------- */

  unsigned char uart_getchar( void )
  {
    while(!(UCSR0A & (1<<RXC0)));                         // warten bis Zeichen eintrifft

  #ifdef rs232_echo

    char ch;
    ch= UDR0;
    uart_putchar(ch);
    return ch;

  #else

    return UDR0;

  #endif
  }

/* --------------------------------------------------
  Zeichen ueber einen Stream (bspw. ueber
  ScanF einlesen.
 -------------------------------------------------- */

int uart_filein( FILE *stream)
{
return uart_getchar();
}

/* --------------------------------------------------
  Zeichen ueber einen Stream (bspw. ueber
  PrintF versenden.
 -------------------------------------------------- */

int uart_fileout(char ch, FILE *stream)
{
uart_putchar(ch);
}

/* --------------------------------------------------
    "Dateivariable" ueber den per Umleitung des
    Standartstreams (stdio) auf der seriellen
    Schnittstelle gesendet wird.
   -------------------------------------------------- */

// static FILE uartio = FDEV_SETUP_STREAM(uart_fileout,uart_filein,_FDEV_SETUP_RW);

/* --------------------------------------------------
    RS232_Rotuinen die die UART Funktionen auf-
    rufen.
   -------------------------------------------------- */

void rs232_init(void)
{
  uart_init(samplebaudrate);
  stdin=stdout= &uart_io;
}

/* die nachfolgende Integerlesefunktionen koennen
   mittels eines Defines

                #define rs232_nointread

  abgeschaltet werden. Immer dann empfehlenswert
  wenn der Speicherplatz knapp wird !! */

#ifndef rs232_nointread

  /* --------------------------------------------------
                         CHECKINT16
        testet, ob eine String im Integerzahlenbereich
        liegt und wenn dem so ist, wird der String in
        einen Integer gewandelt.

        Uebergabe:
                *p    : Zeiger auf einen String
                *myi  : Zeiger auf den zurueck zu
                        gebenden Integer

        Rueckgabe:
                0     : wenn nicht im Zahlenbereich
                1     : wenn im Zahlenbereich
     -------------------------------------------------- */

  int checkint16(char *p, int *myi)
  {
    char     neg,l;
    char     *p2;
    int      x;
    int32_t  z,z2;

    if ( *p == 0 )
    {
      *myi= 0;
      return 1;
    }
    p2= p;
    l= 0;
    while ( *p++) { l++; }          // ermittelt die Laenge des Strings
    p= p2;
    neg= 0;
    if ( *p == '-')                  // negative Zahl ?
    {
      neg= 1;
      p2++; p++;                   // Zahlenbereich nach dem Minuszeichen
      l--;                         // und Zahl ist eine Stelle kuerzer
    }
    p += (l-1);                    // Pointer auf die Einerzahlen setzen
    z= *p - 48;                    // nach Ziffer umrechnen
    z2= 1;
    if (l> 1)
    {
      l--;                         // Laenge "kuerzen" weil 10er Stelle
      p--;                         // Zeiger auf die 10er Stelle
      do
      {
        z2= z2*10;
        z= z+ (z2 * ( *p - 48 ));  // Ziffer * Multiplikator + alten Inhalt von z
        p--;
        l--;
      } while (l);                 // fuer alle Stellen wiederholen
    }
    if (((z> 32767) && (!neg)) | (z> 32768))
    {
      *myi= 0;
      return 0;
    }
    if (neg) { z= z * -1; }
    x= z;
    *myi= z;
    return 1;
  }

  /* --------------------------------------------------
                       RS232_READINT
       liest einen Integerzahlenwert ueber die RS-232
       ein.
       Eine Ueberpruefung auf den Integerwertebereich
       findet statt !!!!
     -------------------------------------------------- */

  signed int rs232_readint()
  {

    #define maxinlen   7         // max. 6 Zeichen + NULL

    char str[maxinlen];
    char *p;
    char *pz;
    char  ch, cnt, b;
    signed int i;


    p= &str[0];                  // p zeigt auf die Adresse von str
    pz= p;                       // pz zeigt immer auf erstes Zeichen im String
    *p = 0;                      // und setzt dort NULL Byte
    cnt= 0;
    do
    {
      do
      {
        ch= uart_getchar();
        if ((ch>= '0') && (ch<= '9'))
        {
          if (cnt < maxinlen-1)
          {
            *p++= ch;              // schreibt Char in den String und erhoeht Pointer
            *p= 0;                 // und schreibt neues NULL (Endekennung)
            cnt++;
            uart_putchar(ch);      // Echo des eingebenen Zeichens
          }
        }
        switch (ch)
        {
          case '-' :
            {
              if (cnt == 0)
              {
                *p++= ch;
                *p= 0;
                cnt++;
                uart_putchar(ch);
              }
              break;
            }
          case 8   :
            {
              if (cnt> 0)
              {
                uart_putchar(ch);
                p--;
                *p= 0;
                cnt--;
              }
              break;
            }
          default  :
            {
              break;
            };
        }
      } while (ch != 0x0d);        // wiederholen bis Returnzeichen eintrifft
      b=  (checkint16( &str[0], &i ));
      if (!b)
      {
        for (i= cnt; i> 0; i--)
        {
          uart_putchar(8);         // gemachte, fehlerhafte Eingabe durch Backspace
                                   // loeschen
        }
        cnt= 0;                    // und Zaehler zuruecksetzen
        p= pz;
        *p= 0;
      }
    } while (!b);      // Zahl soll im 16 Bit Integerbereich liegen
    return i;
  }

  /* --------------------------------------------------
                         RS232_PUTINT
       gibt einen Integerzahlenwert ueber die RS-232
       aus.
     -------------------------------------------------- */

  void rs232_putint(int i)
  {
    int  z,z2;
    char b, first;

    first= 1;
    if (!i) {uart_putchar('0'); return;}
    if (i< 0) { uart_putchar('-'); }
    i= abs(i);
    z= 10000;
    for (b= 5; b; b--)
    {
      z2= i / z;
      if (! ((first) && (z2 == 0)) )
      {
        uart_putchar(z2 + 48);
        first= 0;
      }
      z2 = z2 * z;
      i -= z2;
      z = z / 10;
    }
  }

#endif


/* --------------------------------------------------
                       RS232_CRLF
     sendet auf der RS-232 ein Linefeed und ein
     Carriage Return
   -------------------------------------------------- */

void rs232_crlf()
{
  uart_putchar(0x0a);
  uart_putchar(0x0d);
}


/* --------------------------------------------------
                  RS232_PUTRAMSTRING
     gibt einen String aus dem RAM ueber die RS-232
     aus.
   -------------------------------------------------- */

void rs232_putramstring(unsigned char *p)
{
  do
  {
    uart_putchar( *p );
  } while( *p++);
}

/* --------------------------------------------------
                  RS232_PUTROMSTRING
     gibt einen String aus dem ROM ueber die RS-232
     aus.

     Bsp.:

          rs232_putromstring(PSTR("Hallo Welt")
   -------------------------------------------------- */

void rs232_putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr))
  {
    uart_putchar(c);
  }
}

