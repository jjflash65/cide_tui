#include "mbed.h"

#define  M_PI    3.14159265359

#define printf      rs232.printf
#define getchar     rs232.getc

Serial rs232(PA_9, PA_10);
DigitalOut myled(PB_1);

int main()
{
  char    w1;
  int     winkel= 48;
  int     i= 0;
  float   f;
  char    myzahlen[30] = "13.89 #65.02 #15.97";
  char    *ptr;

  struct  tm zeit;

  char    zeitstrbuf[100];

  int    min;


  rs232.baud(19200);


  zeit.tm_year= 2016;
  zeit.tm_mon = 3;
  zeit.tm_mday= 18;
  zeit.tm_hour= 15;
  zeit.tm_min = 30;
  zeit.tm_sec = 0;

  // Zeitstempel korrigierein
  zeit.tm_year = zeit.tm_year - 1900;
  zeit.tm_mon = zeit.tm_mon - 1;

  // Zeit setzen
  set_time(mktime(&zeit));

  printf("\n\n\r");
  // display the time
  for (i= 0; i< 5; i++)
  {
    time_t seconds = time(NULL);
    min=  seconds / 60;
    min=  min % 60;

    strftime(zeitstrbuf, 100, " %H.%M:%S   %d.%m.%Y  : %a ", localtime(&seconds));
    printf(" Uhr: %s",zeitstrbuf);

    printf("   Min: %.2d \r",min);

    wait(1);
  }

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
  printf("umdefiniertes PRINTF aktiv \n\r");
  printf("Zeichen eingeben [kein enter]: ");
  w1= getchar();
  printf("\n\rEingabe war: %d \n\r",w1);
  printf("Sinus %d = %.3f\n\n\r",winkel, f);
  winkel= 0;
  while(1) {
      wait(1);
/*      printf(" Programmlaufzeit: %d Sekunden.\r", i++); */
      f= sin(winkel * M_PI / 180);
      printf(" Sinus %d = %.3f  \r",winkel,f);
      winkel++;
      winkel= winkel % 360;
      myled = !myled;
  }
}


