#include "mbed.h"
#include "readint.h"

#define printf      rs232.printf
#define getchar     rs232.getc
#define putchar     rs232.putc

Serial rs232(PA_9, PA_10);
DigitalOut myled(PC_13);
//DigitalOut myled(PB_1);


int main()
{
  int     min;
  struct  tm zeit;

  char    zeitstrbuf[100];


  rs232.baud(115200);

  printf("\n\n\r --------- Cortex M3 ------------\n\r");
  printf(      "       STM32F103CBT6 / 72 MHz\n\n\r");
  printf(      "       Baudrate: 115200 \n\n\r");

  printf("\n\r Uhrzeit und Datum setzen\n\r"   \
             " --------------------------------\n\n\r");

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

  while(1)
  {
    myled = 1;

    time_t seconds = time(NULL);
    min=  seconds / 60;
    min=  min % 60;

    strftime(zeitstrbuf, 100, " %H.%M:%S   %d.%m.%Y  : %a ", localtime(&seconds));
    printf(" Uhr: %s",zeitstrbuf);

    printf("   Min: %.2d \r",min);

    wait_ms(500);
    myled = 0;
    wait_ms(500);

  }

}

