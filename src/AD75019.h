/*

  AD75019.h

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef AD75019_h
#define AD75019_h

#include <stdint.h>

class AD75019 {

  public:
    typedef void (*voidFuncCallback_t)(uint8_t i, uint8_t v);

    /*
     * AD75019 constructor that uses Arduino-provided functions for
     * pinMode(pin, OUTPUT) and digitalWrite(pin, <value>)
     *
     * pclkPinNumber - connect to PCLK on the AD75019
     * sclkPinNumber - connect to SCLK
     * sinPinNumber  - connect to SIN
     */
    AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber);

    /*
     * AD75019 constructor using custom callback functions to initialize
     * the pins and perform digital writes. These are necessary
     * when interacting with the device through a port expander such 
     * as the MCP23017.
     */
    AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
        voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback);

    /*
     * Sets the pinModes and initializes using the default X and Y pin mappings
     */
    void begin();

    /*
     * Sets the pinModes and initializes using a custom X and Y pin mappings
     *
     * xPinMapping, yPinMapping - 16-element arrays with element values 
     *    between 0-15 containing no repeated values. The array elements
     *    correspond to the actual X and Y pins on the AD75019.
     */
    void begin(uint8_t xPinMapping[16], uint8_t yPinMapping[16]);

    /*
     * Update the configuration buffer connecting the specified X and Y pins (after
     * applying any pin re-mapping)
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    void connect(uint8_t x, uint8_t y);

    /*
     * Returns true if the specified X and Y pins are connected in the configuration
     * buffer (after applying any pin re-mapping)
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    bool isConnected(uint8_t x, uint8_t y);

    /*
     * Sends the contents of the configuration buffer to the AD75019's shift registers
     */
    void flush();

    /*
     * Resets the configuration buffer to all zeros (no connections)
     */
    void clear();

    /*
     * Serial.prints the configuration matrix
     */
    void print();

  private:
    uint8_t pclkPinNumber;
    uint8_t sclkPinNumber;
    uint8_t sinPinNumber;

    voidFuncCallback_t pinModeCallback = NULL;
    voidFuncCallback_t digitalWriteCallback = NULL;
    
    uint8_t xPinMapping[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t yPinMapping[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    // Y is the array index, X is the bit position (0 on the right)
    uint16_t configBuffer[16] = {0};

};

#endif
