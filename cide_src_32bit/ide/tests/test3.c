/*  #########################

    ######################### */

// #define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdarg.h>

int main(void)
{
  volatile i;
  while (1)
  {
    i++;
    _delay_ms(100);
  }
}

