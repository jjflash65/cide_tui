/* ----------------     ANALOG_TEST.C    ------------------

     Demonstriert ein S6D02A1 Display als einfachen
     "Schreiber": Analoge Spannungen werden ueber den
     ADC eingelesen und als Kurvengrafik fortlaufend auf
     dem Display angezeigt

     MCU:              ATmega168
     Quarz:            16MHz

     Compiler:         AVR-GCC
     IDE:              Geany

     17.03.2014        R. Seelig

   --------------------------------------------------------*/

#define F_CPU 16000000

// --------------------------------------------------------

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/m328ports.h"
#include "../include/coldisplay.h"
#include "../include/adc_mega.c"

// ----------------------- ADC ---------------------------

#define uref           11.223                         // Referenzspannung des ADC's


void sinus_demo(void)
{
  int cx, y1,yo;
  float r;

  line(0,0,0,127,rgbfromvalue(0x80,0x80,0x80));
  line(0,64,159,64,rgbfromvalue(0x80,0x80,0x80));
  yo= 64;
  for (cx= 0; cx< 159; cx++)
  {
    r= (-(sin((cx*3)*3.1415/180)*62))+64;
    y1= r;
    line(cx+1,y1,cx,yo,rgbfromvalue(0x80,0xff,0x00));
    yo= y1;
  }
}

// ---------------------------------------------------------------------------------------------------------
//                                                    M A I N
// ---------------------------------------------------------------------------------------------------------


int main()
{
  char b;
  uint8_t i;
  int cx,y1,yo,adcw;
  int xr,yr;
  float r;

#define rart  0x80
#define ragn  0x80
#define rabl  0x80

  lcd160_init(s6d02a_seq);
  adc_init(0,1);

  stdout= &lcd_out;
  cx= 160;

  adcw= getadc_10bit();
  r= adcw; r= (r*(uref / 1023));
  r= 127- (r*12.7);
  if (r< 0) {r= 0;}
  yo= r;

  while(1)
  {
    if (cx> 159)
    {
      outmode= 1;
      bkcolor= rgbfromvalue(0,100,0);
      clrscr();
      textcolor= rgbfromega(lightgreen);

      for (xr= 1; xr< 10; xr++)
      {
        for (yr= 0; yr<9; yr++)
        {
          rectangle(xr*16,yr*16,(xr+1)*16,(yr+1)*16,rgbfromvalue(rart,ragn,rabl));
        }
      }
      lcd_putcharxy(7,119,'0');
      lcd_putcharxy(7,92,' ');
      lcd_putcharxy(7,60,'5');
      lcd_putcharxy(0,0,'1'); lcd_putcharxy(7,0,'0');
      outmode= 0;
      gotoxy(3,18); printf("U/V");
      outmode= 1;
      textcolor= rgbfromega(yellow);
//      gotoxy(3,1); printf("Ohm= %c",0x82);
      textcolor= rgbfromega(white);
      cx= 17;
    }

    adcw= getadc_10bit();
    r= adcw; r= (r*(uref / 1023));
    gotoxy(15,1); printf("%1.2fV",r);

    r= 127- (r*12.7);
    if (r< 0) {r= 0;}
    y1= r;
    line(cx+1,y1,cx,yo,rgbfromvalue(0x80,0xff,0x00));
    cx++;
    yo= y1;

    _delay_ms(50);
  }
}
