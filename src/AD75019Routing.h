/*

  AD75019Routing.h

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef AD75019Routing_h
#define AD75019Routing_h

#include <stdint.h>

/*
 * A configuration for the AD75019's 256 crosspoints, built without the
 * chip: routes are added by X and Y pin (after applying any pin
 * re-mapping), and the result can be read as bytes, kept, and later
 * loaded back or sent with AD75019::flush(bytes).
 *
 * The bytes are in the order the chip is sent them: Y15 to Y0, each as
 * X15 to X8 then X7 to X0, most significant bit first.
 *
 * Copies are independent; AD75019::newRouting() makes one with a switch's
 * pin mapping and no routes.
 */
class AD75019Routing {

  public:

    // A routing's size in bytes: one bit per crosspoint
    static const uint8_t BYTES = 32;

    /*
     * A routing with no connections and the default X and Y pin mappings
     */
    AD75019Routing();

    /*
     * Re-maps the X and Y pins for routes added afterwards. Returns false,
     * changing nothing, if either mapping is invalid.
     *
     * xPinMapping, yPinMapping - 16-element arrays with element values
     *    between 0-15 containing no repeated values. The array elements
     *    correspond to the actual X and Y pins on the AD75019.
     */
    bool setPinMapping(const uint8_t xPinMapping[16], const uint8_t yPinMapping[16]);

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

    /*
     * Copies the routing out as bytes, in the order the chip is sent them
     *
     * bytes - gets BYTES bytes
     */
    void getBytes(uint8_t bytes[BYTES]) const;

    /*
     * Replaces the routes with bytes from getBytes(), keeping the pin
     * mapping
     *
     * bytes - BYTES bytes
     */
    void setBytes(const uint8_t bytes[BYTES]);

  private:

    // Tells whether a mapping holds each pin 0-15 once
    static bool isValidMapping(const uint8_t pinMapping[16]);

    uint8_t _xPinMapping[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t _yPinMapping[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    // Y is the array index, X is the bit position (0 on the right)
    uint16_t _rows[16] = {0};

};

#endif
