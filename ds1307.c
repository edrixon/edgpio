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

#include "i2c.h"

// DS1807 Definitions
#define RTCADDRESS 0x68

#define SECONDS    0x00
#define MINUTES    0x01
#define HOURS      0x02
#define DAYOFWEEK  0x03
#define DAY        0x04
#define MONTH      0x05
#define YEAR       0x06
#define CONTROL    0x07

#define RS0        0
#define RS1        1
#define SQWE       4

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

// RTC Variables
uint8_t rtcConfig = 0x03;
uint16_t rtcCentury = 2000;

static unsigned char bcd_to_dec(unsigned char bcd) {
	// internal RTC function for converting a BCD formatted number to decimal
	return (unsigned char)((HI_NIBBLE(bcd) * 10) + (LO_NIBBLE(bcd)));
}

static unsigned char dec_to_bcd(char dec) {
	// internal RTC function for converting a decimal formatted number to BCD
	return (unsigned char)((dec / 10) * 16) + (dec % 10);
}

void rtcSetDate(struct tm *date)
{
    /**
    * Set the date on the RTC
    * @param date - struct tm formated date and time
    */
    i2cWriteBuffer[0] = SECONDS; // register address for seconds
    i2cWriteBuffer[1] = dec_to_bcd(date -> tm_sec); // seconds
    i2cWriteBuffer[2] = dec_to_bcd(date -> tm_min); // minutes
    i2cWriteBuffer[3] = dec_to_bcd(date -> tm_hour);// hours
    i2cWriteBuffer[4] = dec_to_bcd(date -> tm_wday);// dayofweek
    i2cWriteBuffer[5] = dec_to_bcd(date -> tm_mday);// day
    i2cWriteBuffer[6] = dec_to_bcd(date -> tm_mon) + 1;// month
    i2cWriteBuffer[7] = dec_to_bcd(date -> tm_year % 100); // strip the century from the date

    i2cWriteByteArray(RTCADDRESS, i2cWriteBuffer, 8);
}

void rtcReadDate(struct tm *date)
{
    /**
    * Read the date from the RTC.
    * @returns - date as a tm struct
    */

    i2cReadByteArray(RTCADDRESS, 0, i2cReadBuffer, 7);
    date -> tm_sec = bcd_to_dec(i2cReadBuffer[0]); // seconds
    date -> tm_min = bcd_to_dec(i2cReadBuffer[1]); // minutes
    date -> tm_hour = bcd_to_dec(i2cReadBuffer[2]);// hours
    date -> tm_wday = bcd_to_dec(i2cReadBuffer[3]);// dayofweek
    date -> tm_mday = bcd_to_dec(i2cReadBuffer[4]);// day
    date -> tm_mon = bcd_to_dec(i2cReadBuffer[5]) - 1;// month
    date -> tm_year = bcd_to_dec(i2cReadBuffer[6]) + (rtcCentury - 1900);// year
}

void rtcEnableOutput()
{
    /**
    * Enable the squarewave output pin
    */
    rtcConfig = i2cUpdateByte(rtcConfig, SQWE, 1);
    i2cWriteByteData(RTCADDRESS, CONTROL, rtcConfig);
}

void rtcDisableOutput()
{
    /**
    * Disable the squarewave output pin
    */
    rtcConfig = i2cUpdateByte(rtcConfig, SQWE, 0);
    i2cWriteByteData(RTCADDRESS, CONTROL, rtcConfig);
}

void rtcSetFrequency(uint8_t frequency)
{
    /**
    * Set the squarewave output frequency
    * @param frequency - 1 = 1Hz, 2 = 4.096KHz, 3 = 8.192KHz, 4 = 32.768KHz
    */
    switch(frequency)
    {
        case 1:
            rtcConfig = i2cUpdateByte(rtcConfig, RS0, 0);
            rtcConfig = i2cUpdateByte(rtcConfig, RS1, 0);
            break;

        case 2:
            rtcConfig = i2cUpdateByte(rtcConfig, RS0, 1);
            rtcConfig = i2cUpdateByte(rtcConfig, RS1, 0);
            break;

        case 3:
            rtcConfig = i2cUpdateByte(rtcConfig, RS0, 0);
            rtcConfig = i2cUpdateByte(rtcConfig, RS1, 1);
            break;

        default:
            rtcConfig = i2cUpdateByte(rtcConfig, RS0, 1);
            rtcConfig = i2cUpdateByte(rtcConfig, RS1, 1);
            break;
    }

    i2cWriteByteData(RTCADDRESS, CONTROL, rtcConfig);
}

int rtcWriteMemory(uint8_t address, int length, uint8_t *valuearray)
{
    /**
    * write to the memory on the DS1307.  The DS1307 contains a 56-byte, battery-backed RAM with unlimited writes
    * @param address - 0x08 to 0x3F
    * @param valuearray - byte array containing data to be written to memory
    */

    if(address + length <= 0x3F)
    {
        i2cWriteBuffer[0] = address;
        bcopy(valuearray, &i2cWriteBuffer[1], length);
        i2cWriteByteArray(RTCADDRESS, i2cWriteBuffer, (uint8_t)length + 1);

        return 0;
    }
    else
    {
        return -1;
    }
}

int rtcReadMemory(uint8_t address, uint8_t length, uint8_t *readarray)
{
    /**
    * Read from the memory on the DS1307
    * The DS1307 contains 56-Byte, battery-backed RAM with Unlimited Writes
    * @param address - 0x08 to 0x3F
    * @param length - up to 32 bytes.  length can not exceed the available address space.
    * @returns - pointer to a byte array where the data will be saved
    */

    if(address + length <= 0x3F)
    {
        i2cReadByteArray(RTCADDRESS, address, readarray, length);

        return 0;
    }
    else
    {
        return -1;
    }
}
