/*

  AD75019Routing.h

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2026, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef AD75019Routing_h
#define AD75019Routing_h

#include <stdint.h>

/*
 * A configuration for the AD75019's 256 crosspoints, built without the
 * chip and sent later with AD75019::flush(routing). It holds only the 256
 * crosspoint bits, so routings can be worked out ahead of time and kept.
 * Copies are independent.
 *
 * The X and Y pin mapping is shared by every routing: it's set once, by
 * setPinMapping() or AD75019::begin(xPinMapping, yPinMapping), for the
 * board the chip is on.
 */
class AD75019Routing {

  public:

    // X pins, and Y pins
    static const uint8_t PINS = 16;

    /*
     * A routing with no connections
     */
    AD75019Routing();

    /*
     * Re-maps the X and Y pins for every routing's routes added afterwards.
     * Returns false, changing nothing, if either mapping is invalid.
     *
     * xPinMapping, yPinMapping - 16-element arrays with element values
     *    between 0-15 containing no repeated values. The array elements
     *    correspond to the actual X and Y pins on the AD75019.
     */
    static bool setPinMapping(const uint8_t xPinMapping[PINS], const uint8_t yPinMapping[PINS]);

    /*
     * Adds a route between the specified X and Y pins (after applying any
     * pin re-mapping). Pins above 15 are ignored.
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    void addRoute(uint8_t x, uint8_t y);

    /*
     * Returns true if the X pin is routed to the Y pin (after applying any
     * pin re-mapping), false if not or for a pin above 15
     *
     * x - a number from 0-15
     * y - a number from 0-15
     */
    bool isRouted(uint8_t x, uint8_t y) const;

    /*
     * Removes every route (no connections)
     */
    void clear();

  private:

    // AD75019 sends and prints the rows directly
    friend class AD75019;

    // Tells whether a mapping holds each pin 0-15 once
    static bool isValidMapping(const uint8_t pinMapping[PINS]);

    static uint8_t _xPinMapping[PINS];
    static uint8_t _yPinMapping[PINS];

    // Y is the array index, X is the bit position (0 on the right), after
    // the pin mapping
    uint16_t _rows[PINS] = {0};

};

#endif
