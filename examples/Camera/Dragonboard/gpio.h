/*
  This is a general purpose file for using the GPIO...generally
 */
#ifndef __HARDWARE_GPIO_H__
#define __HARDWARE_GPIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HIGH 1
#define LOW 0
#define OUT "out"
#define IN "in"

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"

// The Dragonboard 410c has its own mapping of
// GPIO pins on the J8 40-pin header to the pin
// recognized in Linux. This function is used
// to get the Linux pin with the J8 pin
// Returns 0 of invalid pin
uint16_t GpioDB410cMapping(uint16_t pin);

// Enables access to GPIO
// Return 0 if successful, otherwise failed
int GpioEnable(uint16_t gpio);

// Disables access to GPIO
// Return 0 if successful, otherwise failed
int GpioDisable(uint16_t gpio);

// Sets the direction of gpio passed in
// Input direction  == IN
// Output direction == OUT
// Return 0 if successful, otherwise failed
int GpioSetDirection(uint16_t gpio, char* direction);

// Gets the direction of gpio passed in
// Invalid gpio      == -1
// Input direction  ==  0
// Output direction ==  1
// Return 0 if successful, otherwise failed
int GpioGetDirection(uint16_t gpio);

// Sets the value of gpio passed in
// Return 0 if successful, otherwise failed
int GpioSetValue(uint16_t gpio, uint16_t value);

// Gets the value of gpio passed in
// Returns -1 for invalid gpio
int GpioGetValue(uint16_t gpio);

/////////////////////////////////////////////////////////
// These next functions are wrapping of the calls      //
// above to help make main code less verbose if wanted //

// enable and set as input
// returns gpio if successful, 0 if failed
uint16_t GpioInput(uint16_t gpio);
uint16_t GpioInputPin(uint16_t pin);

// enable and set as output with value
// returns gpio if successful, 0 if failed
uint16_t GpioOutput(uint16_t gpio, uint16_t value);
uint16_t GpioOutputPin(uint16_t pin, uint16_t value);

#endif