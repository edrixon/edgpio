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

#include "edgpio.h"
#include "i2c.h"

// Default I2C address for MCP23017
// Can be overwridden by ioInit()
// Call ioInit() with 0x24 as address if R19 is fitted
#define IOADDRESS 0x20

// MCP23017 register definititions
// See data sheet
#define IODIRA   0x00
#define IODIRB   0x01
#define IPOLA    0x02
#define IPOLB    0x03
#define GPINTENA 0x04
#define GPINTENB 0x05
#define DEFVALA  0x06
#define DEFVALB  0x07
#define INTCONA  0x08
#define INTCONB  0x09
#define IOCON    0x0A
#define GPPUA    0x0C
#define GPPUB    0x0D
#define INTFA    0x0E
#define INTFB    0x0F
#define INTCAPA  0x10
#define INTCAPB  0x11
#define GPIOA    0x12
#define GPIOB    0x13
#define OLATA    0x14
#define OLATB    0x15

// See datasheet
#define IOCON_RESET 0x02

static uint8_t ioAddress = IOADDRESS;

static uint8_t set_pin(uint8_t pin, uint8_t value, uint8_t reg)
{
    uint8_t newVal;

    if(pin >= 1 && pin <= 8)
    {
        pin--;
    }
    else
    {
        if(pin >= 9 && pin <= 16)
	{
            reg++;
            pin = pin - 9;
        }
        else
        {
            return -1;
        }
    }

    if (value > 1)
    {
        value = 1;
    }

    newVal = i2cUpdateByte(i2cReadByteData(ioAddress, reg), pin, value);
    i2cWriteByteData(ioAddress, reg, newVal);

    return 0;
}

static uint8_t get_pin(uint8_t pin, uint8_t reg)
{
    if(pin >= 1 && pin <= 8)
    {
        pin--;
    }
    else
    {
        if(pin >= 9 && pin <= 16)
        {
            pin = pin - 9;
            reg++;
        }
        else
        {
            return -1;
        }
    }

    return i2cCheckBit(i2cReadByteData(ioAddress, reg), pin);
}

static uint8_t set_port(uint8_t port, uint8_t value, uint8_t reg)
{
    if(port == IO_PORTA)
    {
        i2cWriteByteData(ioAddress, reg, value);
    }
    else
    {
        if(port == IO_PORTB)
        {
            i2cWriteByteData(ioAddress, reg + 1, value);
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

static uint8_t get_port(uint8_t port, uint8_t reg)
{
    if(port == IO_PORTA)
    {
        return (i2cReadByteData(ioAddress, reg));
    }
    else
    {
        if(port == IO_PORTB)
        {
            return (i2cReadByteData(ioAddress, reg + 1));
	}
        else
        {
            return -1;
        }
    }
}

/*===============================Public Functions===============================*/


void ioSetPinDirection(uint8_t pin, uint8_t direction)
{
    /**
    * Set IO direction for an individual pin
    * @param pins - 1 to 16
    * @param direction - 1 = input, 0 = output
    */

    set_pin(pin, direction, IODIRA);
}

uint8_t ioGetPinDirection(uint8_t pin)
{
    /**
    * Get IO direction for an individual pin
    * @param pins - 1 to 16
    * @returns 1 = input, 0 = output
    */

    return get_pin(pin, IODIRA);
}

void ioSetPortDirection(uint8_t port, uint8_t direction)
{
    /**
    * Set direction for an IO port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param direction - 0 to 255 (0xFF).  For each bit 1 = input, 0 = output
    */

    set_port(port, direction, IODIRA);
}

uint8_t ioGetPortDirection(uint8_t port)
{
    /**
    * Get the direction for an IO port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns 0 to 255 (0xFF).  For each bit 1 = input, 0 = output
    */

    return get_port(port, IODIRA);
}

void ioSetPinPullup(uint8_t pin, uint8_t value)
{
    /**
    * Set the internal 100K pull-up resistors for an individual pin
    * @param pin - 1 to 16
    * @param value - 1 = enabled, 0 = disabled
    */

    set_pin(pin, value, GPPUA);
}

uint8_t ioGetPinPullup(uint8_t pin)
{
    /**
    * Get the internal 100K pull-up resistors for an individual pin
    * @param pin - 1 to 16
    * @returns 1 = enabled, 0 = disabled
    */

    return get_pin(pin, GPPUA);
}

void ioSetPortPullups(uint8_t port, uint8_t value)
{
    /**
    * Set the internal 100K pull-up resistors for the selected IO port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param value - 0 to 255 (0xFF). For each bit 1 = enabled, 0 = disabled
    */

    set_port(port, value, GPPUA);
}

uint8_t ioGetPortPullups(uint8_t port)
{
    /**
    * Get the internal 100K pull-up resistors for the selected IO port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns 0 to 255 (0xFF). For each bit 1 = enabled, 0 = disabled
    */

    return get_port(port, GPPUA);
}

void ioWritePin(uint8_t pin, uint8_t value)
{
    /**
    * Write to an individual pin 1 - 16
    * @param pin - 1 to 16
    * @param value - 0 = logic low, 1 = logic high
    */

    set_pin(pin, value, GPIOA);
}

void ioWritePort(uint8_t port, uint8_t value)
{
    /**
    * Write to all pins on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param value - 0 to 255 (0xFF)
    */

    set_port(port, value, GPIOA);
}

uint8_t ioReadPin(uint8_t pin)
{
    /**
    * Read the value of an individual pin
    * @param pin - 1 to 16
    * @returns - 0 = logic low, 1 = logic high
    */

    return get_pin(pin, GPIOA);
}

uint8_t ioReadPort(uint8_t port)
{
    /**
    * Read all pins on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns - 0 to 255 (0xFF). For each bit 1 = logic high, 0 = logic low
    */

    return get_port(port, GPIOA);
}

uint8_t ioReadOutputLatch(uint8_t port)
{
    return get_port(port, OLATA);
}

void ioSetInterruptType(uint8_t port, uint8_t value)
{
    /**
    * Sets the type of interrupt for each pin on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param value - 0 to 255 (0xFF). For each bit 1 = interrupt is fired when the pin matches the default value, 0 = the interrupt is fired on state change
    */

    set_port(port, value, INTCONA);
}

uint8_t ioGetInterruptType(uint8_t port)
{
    /**
    * Get the type of interrupt for each pin on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns 0 to 255 (0xFF). For each bit 1 = interrupt is fired when the pin matches the default value, 0 = the interrupt is fired on state change
    */

    return get_port(port, INTCONA);
}

void ioSetInterruptDefaults(uint8_t port, uint8_t value)
{
    /**
    * These bits set the compare value for pins configured for interrupt-on-change on the selected port.
    * If the associated pin level is the opposite of the register bit, an interrupt occurs.
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param value - default state for the port. 0 to 255 (0xFF).
    */

    set_port(port, value, DEFVALA);
}

uint8_t ioGetInterruptDefaults(uint8_t port)
{
    /**
    * Get the compare value for pins configured for interrupt-on-change on the selected port.
    * If the associated pin level is the opposite of the register bit, an interrupt occurs.
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns default state for the port. 0 to 255 (0xFF).
    */

    return get_port(port, DEFVALA);
}

void ioSetInterruptOnPin(uint8_t pin, uint8_t value)
{
    /**
    * Enable interrupts for the selected pin
    * @param pin - 1 to 16
    * @param value - 0 = interrupt disabled, 1 = interrupt enabled
    */

    set_pin(pin, value, GPINTENA);
}

 uint8_t ioGetInterruptOnPin(uint8_t pin)
 {
    /**
    * Get the interrupt-enable status for the selected pin
    * @param pin - 1 to 16
    * @returns 0 = interrupt disabled, 1 = interrupt enabled
    */

    return get_pin(pin, GPINTENA);
 }

void ioSetInterruptOnPort(uint8_t port, uint8_t value)
{
    /**
    * Enable interrupts for the pins on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @param value - 0 to 255 (0xFF). For each bit 0 = interrupt disabled, 1 = interrupt enabled
    */

    set_port(port, value, GPINTENA);
}

uint8_t ioGetInterruptOnPort(uint8_t port)
{
    /**
    * Get the interrupt-enable status for the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns 0 to 255 (0xFF). For each bit 0 = interrupt disabled, 1 = interrupt enabled
    */

    return get_port(port, GPINTENA);
}

uint8_t ioReadInterruptStatus(uint8_t port)
{
    /**
    * Read the interrupt status for the pins on the selected port
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns - 0 to 255 (0xFF). For each bit 1 = interrupt triggered, 0 = interrupt not triggered
    */

    return get_port(port, INTFA);
}

uint8_t ioReadInterruptCapture(uint8_t port)
{
    /**
    * Read the value from the selected port at the time of the last interrupt trigger
    * @param port - 0 = pins 1 to 8, port 1 = pins 9 to 16
    * @returns - 0 to 255 (0xFF). For each bit 1 = interrupt triggered, 0 = interrupt not triggered
    */

    return get_port(port, INTCAPA);
}

void ioAckInterrupts(uint8_t port)
{
    /**
    * Reset the interrupts on port
    */

//    usleep(200000);
    ioReadInterruptCapture(port);
}

void ioInit(uint8_t reset, uint8_t busAddress)
{
    /**
    * Initialise the MCP32017 IO chip
    * @param reset - If set to 1 reset registers to default values. Ports are inputs, pull-up resistors are disabled and ports are not inverted.
    * @param busAddress - if non-zero, use this as i2c bus address for MCP23017, otherwise use default
    */

    if(busAddress != 0)
    {
        ioAddress = busAddress;
    }
    else
    {
        ioAddress = IOADDRESS;
    }

    i2cWriteByteData(ioAddress, IOCON, IOCON_RESET);
    if(reset == 1)
    {
        i2cWriteByteData(ioAddress, GPIOA, 0x00);
        i2cWriteByteData(ioAddress, GPIOB, 0x00);
        i2cWriteByteData(ioAddress, IODIRA, 0xFF);
        i2cWriteByteData(ioAddress, IODIRB, 0xFF);
        i2cWriteByteData(ioAddress, GPPUA, 0x00);
        i2cWriteByteData(ioAddress, GPPUB, 0x00);
        i2cWriteByteData(ioAddress, IPOLA, 0x00);
        i2cWriteByteData(ioAddress, IPOLB, 0x00);
	i2cWriteByteData(ioAddress, GPINTENA, 0x00);
	i2cWriteByteData(ioAddress, GPINTENB, 0x00);
        i2cWriteByteData(ioAddress, DEFVALA, 0x00);
        i2cWriteByteData(ioAddress, DEFVALB, 0x00);

        ioAckInterrupts(IO_PORTA);
        ioAckInterrupts(IO_PORTB);
    }
}
