/* ----------------- i2c_master_mega.h -------------------

     Header der Routinen fuer einen I2C-Master
     Unterstuetzt folgende Controller:


     MCU:              ATmega8   / ATmega48  / ATmega88 /
                       ATmega168 / ATmega328

     Compiler:         AVR-GCC
     IDE:              RSIDE

     10.03.2015        R. Seelig

   ------------------------------------------------------- */

#ifndef in_i2cmaster
  #define in_i2cmaster

  #include <util/twi.h>

  void i2c_master_init(void);
  uint8_t i2c_start(uint8_t address);
  uint8_t i2c_write(uint8_t data);
  uint8_t i2c_read_ack(void);
  uint8_t i2c_read_nack(void);
  void i2c_stop(void);

//  #define F_SCL 50000UL                                      // Taktfrequenz (kleinerer Wert = langsamer)
  #define F_SCL 150000UL                                      // Taktfrequenz (kleinerer Wert = langsamer)
  #define Prescaler 1
  #define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

#endif

