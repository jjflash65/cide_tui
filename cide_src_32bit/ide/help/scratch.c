 /* ----------------- scratch.c ----------------
    ----------------------------------------------- */
 #include <stdio.h>
 #include "/home/avr_programs/programs/include/rs232_io.h"



 int main(void)
 {
    int number = 12345;
    char string[6];

    rs232_init();

    itoa(number, string, 10);
    prints("\n\rIntegerzahl ist: ");
    rs232_putramstring(string);
    rs232_crlf();
    while(1);
 }
