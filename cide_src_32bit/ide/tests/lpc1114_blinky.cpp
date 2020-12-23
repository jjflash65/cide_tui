#include "mbed.h"
#include "readint.h"

#define printf     rs232.printf

#define led1       dp28
#define but1       dp27
#define rxd_pin    dp15
#define txd_pin    dp16



Serial rs232(txd_pin, rxd_pin);

DigitalOut myled(led1);
DigitalIn  button1(but1);


int main()
{

  int cx;
  float f;

  rs232.baud(115200);


  printf("\n\n\r --------- Cortex M0 ------------\n\r");
  printf(      "            LPC1114      \n\r");
  printf(      "        Baudrate: 115200     \n\r");
  printf(      "   compiled with ThinClient HP5000\n\r");
  printf(      " --------------------------------\n\n\r");

  printf(      "      Hallo LPC1114 - Welt \n\n\r");
  printf(      "        Blinky - Demo \n\n\r");

  printf(      " Beliebige Integerzahl eingeben fuer weiter: ");
  cx= rs232_readint(4);

  cx= 0;
  while(1)
  {
    if (button1)
    {
      myled = !myled;

      wait_ms(500);
      myled = !myled;
      wait_ms(500);
      printf("  Delay - Counter: %d          \r",cx);
      cx++;
    }

  }

}

