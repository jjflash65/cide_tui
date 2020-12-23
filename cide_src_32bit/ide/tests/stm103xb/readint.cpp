#include "readint.h"

Serial rs232_2(PA_9, PA_10);

  #define printf      rs232_2.printf
  #define getchar     rs232_2.getc
  #define putchar     rs232_2.putc



int32_t rs232_readint(char anz)
{

  #define maxinlen   10         // max. 9 Zeichen + NULL

  char     str[maxinlen];
  char     *p;
  char     *pz;
  char     ch, cnt, b;
  int32_t  i;


  p= &str[0];                  // p zeigt auf die Adresse von str
  pz= p;                       // pz zeigt immer auf erstes Zeichen im String
  *p = 0;                      // und setzt dort NULL Byte
  cnt= 0;
  anz++;

  do
  {
    ch= getchar();
    if ((ch>= '0') && (ch<= '9'))
    {
      if (cnt < anz-1)
      {
        *p++= ch;              // schreibt Char in den String und erhoeht Pointer
        *p= 0;                 // und schreibt neues NULL (Endekennung)
        cnt++;
        putchar(ch);           // Echo des eingebenen Zeichens
      }
    }

    if ((ch== 8) || (ch==127))
    {
      if (cnt> 0)
      {
        putchar(ch);
        p--;
        *p= 0;
        cnt--;
      }
    }

    if (ch== '-')
    {
      if (cnt == 0)
      {
        *p++= ch;
        *p= 0;
        cnt++;
        putchar(ch);
      }
    }
  } while (ch != 0x0d);        // wiederholen bis Returnzeichen eintrifft

  i= atoi(str);
  return i;
}
