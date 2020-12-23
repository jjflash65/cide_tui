/* --------------------     TXLCD.H    --------------------

     grundlegende Funktionen zum Betrieb eines
     LC-Textdisplays an einem AVR-Microcontroller.

     MCU: alle

     10.02.2015        R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss am Bsp. Pollin-Display C0802-04 an
      einen ATtiny2313
      Pinkonfiguration fuer - lcdpindef1 -
      ---------------------------------------------------
         o +5V
         |                            Display                 Controller
         _                        Funktion   PIN            PIN    Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------   19    PB7
        \ /                          GND      5 ------------
        -'-                    (Takt) E       6 ------------   18    PB6
         |                           D4       7 ------------   14    PB2
        --- GND                      D5       8 ------------   15    PB3
                                     D6       9 ------------   16    PB4
                                     D7      10 ------------   17    PB5
*/

#ifndef in_txlcd
  #define in_txlcd

//  #define lcdpindef1
//  #define lcdpindef2
//  #define i2cdisplay
  #define lcd8515pindef

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <stdlib.h>



  /* -------------------------------------------------------
       Macros der Pindefinitionen, an welche Pins das
       Textdisplay angeschlossen wird
     ------------------------------------------------------- */

  #ifdef lcdpindef1

    #define txlcd_dbio         DDRB                           // Richtungsregister der Datenbits
    #define txlcd_db           PORTB

    #define txlcd_d7           PB5
    #define txlcd_d6           PB4
    #define txlcd_d5           PB3
    #define txlcd_d4           PB2


    #define txlcd_ctrlio       DDRB                           // Richtungsregister der Steuerbits
    #define txlcd_ctrl         PORTB                          // Steuerleitungen (RS und E) sind an PortB angeschlossen

    #define txlcd_rs           PB7                            // RS Leitung Display auf PB7 anschliessen
    #define txlcd_e            PB6                            // E Leitung Display auf PB6 anschliessen

  #endif

  #ifdef lcdpindef2

    #define txlcd_dbio         DDRB                           // Richtungsregister der Datenbits
    #define txlcd_db           PORTB

    #define txlcd_d7           PB5
    #define txlcd_d6           PB4
    #define txlcd_d5           PB3
    #define txlcd_d4           PB2


    #define txlcd_ctrlio       DDRD                           // Richtungsregister der Steuerbits
    #define txlcd_ctrl         PORTD                          // Steuerleitungen (RS und E) sind an PortD angeschlossen

    #define txlcd_rs           PD7                            // RS Leitung Display auf PB4 anschliessen
    #define txlcd_e            PD6                            // E Leitung Display auf PB5 anschliessen

  #endif


  #ifdef lcd8515pindef

    #define txlcd_dbio         DDRA                           // Richtungsregister der Datenbits
    #define txlcd_db           PORTA

    #define txlcd_d7           PA3
    #define txlcd_d6           PA0
    #define txlcd_d5           PA2
    #define txlcd_d4           PA1


    #define txlcd_ctrlio       DDRA                           // Richtungsregister der Steuerbits
    #define txlcd_ctrl         PORTA                          // Steuerleitungen (RS und E) sind an PortB angeschlossen

    #define txlcd_rs           PA5                            // RS Leitung Display auf PA5 anschliessen
    #define txlcd_e            PA4                            // E Leitung Display auf PA4 anschliessen

  #endif

  #ifdef i2cdisplay

    // alternative Belegung wenn lcdpindef1 auskommentiert ist

    #define txlcd_dbio         DDRB                           // Richtungsregister der Datenbits
    #define txlcd_db           PORTB

    #define txlcd_d7           PB0
    #define txlcd_d6           PB1
    #define txlcd_d5           PB2
    #define txlcd_d4           PB3


    #define txlcd_ctrlio       DDRB                           // Richtungsregister der Steuerbits
    #define txlcd_ctrl         PORTB                          // Steuerleitungen (RS und E) sind an PortB angeschlossen

    #define txlcd_rs           PB6                            // RS Leitung Display auf PB6 anschliessen
    #define txlcd_e            PB4                            // E Leitung Display auf PB4 anschliessen

  #endif


  #define txlcd_prints(tx)      (txlcd_putromstring(PSTR(tx)))       // Anzeige String aus Flashrom: prints("Text");

  /* -------------------------------------------------------
       diverse Macros
     ------------------------------------------------------- */

  #define setbit(reg,pos)  (reg|= 1<<pos)                   // Setzt Bit in reg (Port oder Variable) an Position pos
  #define clrbit(reg,pos)  (reg&= ~(1<<pos))                // dto. fuer loeschen
  #define testbit(reg,pos) ((reg) & (1<<pos))               // testet an der Bitposition pos das Bit auf 1 oder 0


  /* -------------------------------------------------------
       Prototypen
     ------------------------------------------------------- */

    void delay(char wert);
    void nibbleout(unsigned char wert, unsigned char hilo);
    void txlcd_takt(void);
    void txlcd_io(char wert);
    void txlcd_init(void);
    void txlcd_setuserchar(char nr, const char *userchar);
    void gotoxy(char x, char y);
    void txlcd_putchar(char ch);
    void txlcd_putramstring(char *c);
    void txlcd_putromstring(const unsigned char *dataPtr);
    void txlcd_putint(int i);


    extern char wherex,wherey;

#endif
