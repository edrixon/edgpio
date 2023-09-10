#ifndef __GOT_I2C

#define __GOT_I2C

// I2C Bus Variables
#ifdef  __IN_I2C

uint8_t *i2cWriteBuffer;
uint8_t *i2cReadBuffer;

#else

extern uint8_t i2cWriteBuffer[];
extern uint8_t i2cReadBuffer[];

#endif

extern uint8_t i2cReadByteData(uint8_t address, uint8_t reg);
extern void i2cWriteByteData(uint8_t address, uint8_t reg, uint8_t value);

extern void i2cReadByteArray(uint8_t address, uint8_t reg, uint8_t *rdBuffer, uint8_t length);
extern void i2cWriteByteArray(uint8_t address, uint8_t *wrBuffer, uint8_t length);

extern char i2cUpdateByte(char byte, char bit, char value);
extern char i2cCheckBit(char byte, char bit);

#endif
