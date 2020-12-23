/* -------------------------     TASTEN.C    ---------------------------

     Include - Datei zum Auswerten von Tastendruecken

   ---------------------------------------------------------------------

   Belegung Tastenpins:
                   PD3 = Taster L   ( Key 1 )
                   PD4 = Taster R	( Key 2 )
                   PD5 = Taster S	( Key 3 )
                   PD6 = Taster D	( Key 4 ) */


// ----------------- "Tastenbit-Macros" ------------------

#define tast_port   PORTD
#define tast_in     PIND
#define tast_ioreg  DDRD
#define tastl 3                  // PortD.3
#define tastr 4                  // PortD.4
#define tasts 5                  // PortD.5
#define tastd 6                  // PortD.6

#define tast_inmask ((1 << tastl)|(1 << tastr)|(1 << tasts)|(1 << tastd))

#define isbitD(nr)  ((PIND) & (1<<nr))

#define pr_time     30           // 30ms als Entprellzeit annehmen
#define pr_time2    30          // Zeit nach dem Loslassen einer Taste

#define key1        !(((PIND) & (1<<tastl)) >> tastl)
#define key2        !(((PIND) & (1<<tastr)) >> tastr)
#define key3        !(((PIND) & (1<<tasts)) >> tasts)
#define key4        !(((PIND) & (1<<tastd)) >> tastd)

// --------------------------------------------------------
//                       Prototypen
// --------------------------------------------------------

uint8_t readkey(void);
uint8_t keypressed(void);


/* ---------------------------------------------------
                        READKEY
    wartet auf einen Tastendruck und gibt den Wert
    der gedrueckten Taste zurueck
   --------------------------------------------------- */                       

uint8_t readkey(void)
{
  uint8_t invalue;
  
  invalue= tast_in & tast_inmask;

  while(!invalue);				   // solange warten bis Taste gedrueckt
  _delay_ms(pr_time);
  if (key1) { return 1; }
  if (key2) { return 2; }
  if (key3) { return 3; }
  if (key4) { return 4; }  
  return 0;
}

/* ---------------------------------------------------
                       KEYPRESSED
    stellt fest, ob eine Taste gedrueckt ist oder 
    nicht, wartet aber nicht darauf!
   --------------------------------------------------- */                       

uint8_t keypressed(void)
{
  if (~(tast_in) & tast_inmask) { return 1; } else { return 0; }
}


void tasten_init()
{
  tast_ioreg &= ~(tast_inmask);    // Pins als Eingang schalten
  tast_port |= tast_inmask;        // interne Pull-Up Widerstände einschalten
}
