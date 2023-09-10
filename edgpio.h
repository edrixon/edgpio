//
// Linux library for Ed's RTC and GPIO board
//
//   Based on ABElectronics support library for their Pi Expander board
//

#ifndef __GOT_EDGPIO
#define __GOT_EDGPIO

#include <stdint.h>
#include <time.h>

#define INPUT     1
#define OUTPUT    0

#define ENABLED   1
#define DISABLED  0

#define INTMATCH  1
#define INTCHANGE 0

#define ON        1
#define OFF       0

// Specify I2C bus and sizes of read and write buffers to use 
void i2cInit(char *busDeviceName, int rdBuffSize, int wrBuffSize);

// Set IO direction for an individual pin
void ioSetPinDirection(uint8_t pin, uint8_t direction);


// Get IO direction for an individual pin
uint8_t ioGetPinDirection(uint8_t pin);


// Set direction for an IO port
void ioSetPortDirection(uint8_t port, uint8_t direction);

// Get the direction for an IO port
uint8_t ioGetPortDirection(uint8_t port);


// Set the internal 100K pull-up resistors for an individual pin
void ioSetPinPullup(uint8_t pin, uint8_t value);

uint8_t ioGetPinPullup(uint8_t pin);

// Set the internal 100K pull-up resistors for the selected IO port
void ioSetPortPullups(uint8_t port, uint8_t value);

// Get the internal 100K pull-up resistors for the selected IO port
uint8_t ioGetPortPullups(uint8_t port);

// Write to an individual pin 1 - 16
void ioWritePin(uint8_t pin, uint8_t value);

// Write to all pins on the selected port
void ioWritePort(uint8_t port, uint8_t value);

// Read the value of an individual pin
uint8_t ioReadPin(uint8_t pin);

// read all pins on the selected port
uint8_t ioReadPort(uint8_t port);

// Sets the type of interrupt for each pin on the selected port
void ioSetInterruptType(uint8_t port, uint8_t value);

// Get the type of interrupt for each pin on the selected port
uint8_t ioGetInterruptType(uint8_t port);


//These bits set the compare value for pins configured for interrupt-on-change on the selected port.
//If the associated pin level is the opposite of the register bit, an interrupt occurs.
void ioSetInterruptDefaults(uint8_t port, uint8_t value);

// Get the compare value for pins configured for interrupt-on-change on the selected port.
uint8_t ioGetInterruptDefaults(uint8_t port);

// Enable interrupts for the selected pin
void ioSetInterruptOnPin(uint8_t pin, uint8_t value);

// Get the interrupt-enable status for the selected pin
uint8_t ioGetInterruptOnPin(uint8_t pin);

// Enable interrupts for the pins on the selected port
void ioSetInterruptOnPort(uint8_t port, uint8_t value);

// Get the interrupt-enable status for the selected port
uint8_t ioGetInterruptOnPort(uint8_t port);

// Read the interrupt status for the pins on the selected port
uint8_t ioReadInterruptStatus(uint8_t port);

// Read the value from the selected port at the time of the last interrupt trigger
uint8_t ioReadInterruptCapture(uint8_t port);

// Reset the interrupts A and B to 0
void ioResetInterrupts();

// Initialise the MCP32017 IO chip
void ioInit(uint8_t reset, uint8_t busAddress);

// Set the date on the RTC
void rtcSetDate(struct tm *date);

// Read the date from the RTC.
void rtcReadDate(struct tm *date);

// Enable the square wave output pin
void rtcEnableOutput();

// Disable the square wave output pin
void rtcDisableOutput();

// Set the square wave output frequency
void rtcSetFrequency(uint8_t frequency);

// Write to the memory on the DS1307.  
int rtcWriteMemory(uint8_t address, int length, uint8_t *valuearray);

// Read from the memory on the DS1307
int rtcReadMemory(uint8_t address, uint8_t length, uint8_t *writearray);

#endif
