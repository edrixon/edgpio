/* Required package: libi2c-dev apt-get install libi2c-dev
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define __IN_I2C
#include "i2c.h"

static char *i2cFileName;
static uint8_t buf[10];

void i2cFatal()
{
    printf("** I2C Fatal error on %s!!\n", i2cFileName);
    perror("i2c");
    exit(1);
}

void i2cInit(char *busDeviceName, int rdBuffSize, int wrBuffSize)
{
    i2cFileName = malloc(strlen(busDeviceName) + 1);
    strcpy(i2cFileName, busDeviceName);

    i2cReadBuffer = malloc(rdBuffSize);
    i2cWriteBuffer = malloc(wrBuffSize);
}

uint8_t i2cReadByteData(uint8_t address, uint8_t reg)
{
    int i2cbus;

    i2cbus = open(i2cFileName, O_RDWR);
    if(i2cbus < 0)
    {
        i2cFatal();
    }

    if(ioctl(i2cbus, I2C_SLAVE, address) < 0)
    {
        i2cFatal();
    }

    buf[0] = reg;
    if((write(i2cbus, buf, 1)) != 1)
    {
        i2cFatal();
    }

    if(read(i2cbus, buf, 1) != 1)
    {
        i2cFatal();
    }

    close(i2cbus);

    return(buf[0]);
}

void i2cReadByteArray(uint8_t address, uint8_t reg, uint8_t *rdBuffer, uint8_t length)
{
    int i2cbus;

    i2cbus = open(i2cFileName, O_RDWR);
    if(i2cbus < 0)
    {
        i2cFatal();
    }

    if(ioctl(i2cbus, I2C_SLAVE, address) < 0)
    {
        i2cFatal();
    }

    buf[0] = reg;
    if((write(i2cbus, buf, 1)) != 1)
    {
        i2cFatal();
    }

    read(i2cbus, rdBuffer, length);

    close(i2cbus);
}

void i2cWriteByteData(uint8_t address, uint8_t reg, uint8_t value)
{
    int i2cbus;

    i2cbus = open(i2cFileName, O_RDWR);
    if(i2cbus < 0)
    {
        i2cFatal();
    }

    if(ioctl(i2cbus, I2C_SLAVE, address) < 0)
    {
        i2cFatal();
    }

    buf[0] = reg;
    buf[1] = value;
    if((write(i2cbus, buf, 2)) != 2)
    {
        i2cFatal();
    }

    close(i2cbus);
}

void i2cWriteByteArray(uint8_t address, uint8_t *wrBuffer, uint8_t length)
{
    int i2cbus;

    i2cbus = open(i2cFileName, O_RDWR);
    if(i2cbus < 0)
    {
        i2cFatal();
    }

    if(ioctl(i2cbus, I2C_SLAVE, address) < 0)
    {
        i2cFatal();
    }

    if((write(i2cbus, wrBuffer, length)) != length)
    {
        i2cFatal();
    }

    close(i2cbus);
}

char i2cUpdateByte(char byte, char bit, char value)
{
    if(value == 0)
    {
        return (byte &= ~(1 << bit));
    }
    else
    {
        return (byte |= 1 << bit);
    }
}

char i2cCheckBit(char byte, char bit)
{
    if(byte & (1 << bit))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

