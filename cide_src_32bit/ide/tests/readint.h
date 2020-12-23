
#ifndef in_readint
#define in_readint

  #include "mbed.h"

  #define rxd_pin    dp15
  #define txd_pin    dp16

  int32_t rs232_readint(char anz);

#endif

