#include "mbed.h"
#include "usb.h"

#include "readint.h"
#include "pcx.h"
#include "ferrari.h"

#define printf      rs232.printf
#define getchar     rs232.getc
#define putchar     rs232.putc

Serial rs232(PA_9, PA_10);
DigitalOut myled(PB_1);



int main()
{
  char    w1;
  int     winkel= 48;
  int     i= 0;
  int     dummy, anz;
  float   f;
  char    myzahlen[30] = "13.89 #65.02 #15.97";
  char    *ptr;

  struct  tm zeit;

  char    zeitstrbuf[100];

  int    min;


  rs232.baud(19200);
  showinc_pcx(&wallpaper[0],wallpaperbytes,0,0);

/*
  f= sin(45 * M_PI / 180);
  printf("\n\r Sinus 45 Grad = %.4f \n\r",f);
*/

  printf("\n\r Das ist die STM32F103XB Show \n\r");
  printf("\n\r Uhrzeit und Datum setzen\n\r  "   \
             " ------------------------\n\n\r");

  printf("\n\rStunden : ");
  zeit.tm_hour= rs232_readint(2);

  printf("\n\rMinuten : ");
  zeit.tm_min= rs232_readint(2);

  zeit.tm_sec = 0;

  printf("\n\rTag     : ");
  zeit.tm_mday= rs232_readint(2);

  printf("\n\rMonat   : ");
  zeit.tm_mon= rs232_readint(2);

  printf("\n\rJahr    : ");
  zeit.tm_year= rs232_readint(4);

  // Zeitstempel korrigierein
  zeit.tm_year = zeit.tm_year - 1900;
  zeit.tm_mon = zeit.tm_mon - 1;

  // Zeit setzen
  set_time(mktime(&zeit));

  printf("\n\n\r");

  i= 0;

  // display the time

  while(!rs232.readable())
  {
    myled = !myled;

    time_t seconds = time(NULL);
    min=  seconds / 60;
    min=  min % 60;

    strftime(zeitstrbuf, 100, " %H.%M:%S   %d.%m.%Y  : %a ", localtime(&seconds));
    printf(" Uhr: %s",zeitstrbuf);

    printf("   Min: %.2d \n\r",min);

    wait_ms(500);
    myled = !myled;
    wait_ms(500);

  }

  w1= getchar();

  printf("Extraktion einer Zahlenreihe\n\r\n\r");

  /* strtok platziert ein 0-Byte an der Stelle
    an der das Token gefunden wurde. ptr ist nun ein
    Zeiger auf einen String der an der Stelle
    terminiert ist, an der das Token WAR                */

  ptr = strtok(myzahlen, "#");

  while (ptr)                // solange ptr nicht auf ein
  {                          // Textende zeigt

    if (ptr)  printf("%s\n\r", ptr);

    /* Ein weiterer Aufruf von strtok mit einem NULL
       Zeiger sucht das naechste Token ersetzt dieses,
       uebergibt wieder den Zeiger darauf */
    ptr = strtok(NULL, "#");
  }

  f= sin(winkel * M_PI / 180);
  printf("Hallo Welt mit MBED!\n\r");
  printf("... und Hallo Tim\n\r");
  printf("PRINTF ist automatisch auf Serial umgeleitet ? \n\r");

  printf("> ");
  w1= getchar();
  printf("%d \n\r",w1);

  printf("Integer eingeben: ");

  dummy= rs232_readint(2);

  printf("\n\n\rEingabe war: %d\n\n\r",dummy);
  printf("Sinus %d = %.3f\n\n\r",winkel, f);
  winkel= 0;
  while(1) {
      wait(1);
/*      printf(" Programmlaufzeit: %d Sekunden.\r", i++); */
      f= sin(winkel * M_PI / 180);
      printf(" Sinus %d = %.3f  \n\r",winkel,f);
      winkel++;
      winkel= winkel % 360;
      myled = !myled;
  }
}

