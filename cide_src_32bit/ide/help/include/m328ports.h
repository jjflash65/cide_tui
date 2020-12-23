/* --------------------- M328PORTS.H ---------------------

     aus einem - mir unerfindlichen Grund wurden die
     Defines fuer die Portnamen eines ATmega328 nicht wie
     bei den anderen ATmegaxx8 Controller mit ueber-
     nommen, es fehlen schlicht die Bezeichnungen wie
     PB0, PB1 .. etc.

     Diese Datei hier definiert diese.

     In der Version AVR-GCC 4.8.1 sind diese Ports de-
     finiert, so dass die Deklarationen ueber einen
     define zu- abschaltbar sind

     03.02.2014        R. Seelig

  -------------------------------------------------------- */

// #define old_avrgcc

#ifdef old_avrgcc

  #if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328P__)

    #define PB0  0
    #define PB1  1
    #define PB2  2
    #define PB3  3
    #define PB4  4
    #define PB5  5
    #define PB6  6
    #define PB7  7

    #define PC0  0
    #define PC1  1
    #define PC2  2
    #define PC3  3
    #define PC4  4
    #define PC5  5
    #define PC6  6

    #define PD0  0
    #define PD1  1
    #define PD2  2
    #define PD3  3
    #define PD4  4
    #define PD5  5
    #define PD6  6
    #define PD7  7

  #endif

#endif
