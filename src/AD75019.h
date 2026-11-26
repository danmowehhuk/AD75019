/*

  AD75019.h

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef AD75019_h
#define AD75019_h

#include <stddef.h>
#include <stdint.h>
#include "AD75019Routing.h"

class AD75019 {

  public:

#ifndef NO_ARDUINO
    /*
     * AD75019 constructor that uses Arduino-provided functions for
     * pinMode(pin, OUTPUT) and digitalWrite(pin, <value>). Not available
     * with NO_ARDUINO, which needs the callback constructor.
     *
     * pclkPinNumber - connect to PCLK on the AD75019
     * sclkPinNumber - connect to SCLK
     * sinPinNumber  - connect to SIN
     */
    AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber);
#endif


    /*
     * AD75019 constructor using custom callback functions to initialize
     * the pins and perform digital writes. These are necessary
     * when interacting with the device through a port expander such 
     * as the MCP23017, or when building with NO_ARDUINO.
     *
     * The pinMode callback is passed 1 (Arduino's OUTPUT), and the
     * digitalWrite callback 1 (HIGH) or 0 (LOW).
     */
    typedef void (*voidFuncCallback_t)(uint8_t i, uint8_t v);
    AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
        voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback);

    /*
     * Sets the pinModes and initializes using the default X and Y pin mappings.
     * Returns true if successful.
     */
    bool begin();

    /*
     * Sets the pinModes and initializes using a custom X and Y pin mappings.
     * Returns true if successful.
     *
     * xPinMapping, yPinMapping - 16-element arrays with element values 
     *    between 0-15 containing no repeated values. The array elements
     *    correspond to the actual X and Y pins on the AD75019.
     */
    bool begin(uint8_t xPinMapping[16], uint8_t yPinMapping[16]);

    /*
     * Update the configuration buffer adding a route between the specified X 
     * and Y pins (after applying any pin re-mapping)
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    void addRoute(uint8_t x, uint8_t y);

    /*
     * Returns true if the X pin is routed to the Y pin in the configuration
     * buffer (after applying any pin re-mapping)
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    bool isRouted(uint8_t x, uint8_t y);

    /*
     * Sends the contents of the configuration buffer to the AD75019's shift registers
     */
    void flush();

    /*
     * Replaces the configuration buffer with bytes from
     * AD75019Routing::getBytes() or getBytes(), then sends it
     *
     * bytes - AD75019Routing::BYTES bytes
     */
    void flush(const uint8_t bytes[AD75019Routing::BYTES]);

    /*
     * Copies the configuration buffer out as bytes, as
     * AD75019Routing::getBytes() does
     *
     * bytes - gets AD75019Routing::BYTES bytes
     */
    void getBytes(uint8_t bytes[AD75019Routing::BYTES]);

    /*
     * Returns a routing with this switch's pin mapping and no routes, for
     * building a configuration without changing the buffer
     */
    AD75019Routing newRouting();

    /*
     * Resets the configuration buffer to all zeros (no connections)
     */
    void clear();

#ifndef NO_ARDUINO
    /*
     * Serial.prints the configuration matrix. Not available with
     * NO_ARDUINO.
     */
    void print();
#endif

  private:
    // Only parameterized constructors may be used
    AD75019();

    // Disable copy constructor since copies would reference the same physical pins
    AD75019(AD75019 &t);

    // Functions using the _state bit vector
    void setBegun(bool b);
    bool isBegun();
    void setUseDefaultCallbacks(bool b);
    bool isUseDefaultCallbacks();

    uint8_t _pclkPinNumber;
    uint8_t _sclkPinNumber;
    uint8_t _sinPinNumber;

    // 000000 | isBegun | isUseDefaultCallbacks
    uint8_t _state = 0;

    voidFuncCallback_t _pinModeCallback = NULL;
    voidFuncCallback_t _digitalWriteCallback = NULL;
    
    // The configuration buffer, with the pin mapping
    AD75019Routing _routing;

};

#endif
