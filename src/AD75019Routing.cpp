/*

  AD75019Routing.cpp

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2026, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#include "AD75019Routing.h"

const uint8_t AD75019Routing::PINS;
const uint8_t AD75019Routing::BYTES;

AD75019Routing::AD75019Routing() {}

bool AD75019Routing::setPinMapping(const uint8_t xPinMapping[PINS], const uint8_t yPinMapping[PINS]) {
  bool valid = isValidMapping(xPinMapping) && isValidMapping(yPinMapping);
  if (valid) {
    for (uint8_t i = 0; i < PINS; i++) {
      _xPinMapping[i] = xPinMapping[i];
      _yPinMapping[i] = yPinMapping[i];
    }
  }
  return valid;
}

void AD75019Routing::addRoute(uint8_t x, uint8_t y) {
  if (x < PINS && y < PINS) {
    _rows[_yPinMapping[y]] |= (1U << _xPinMapping[x]);
  }
}

bool AD75019Routing::isRouted(uint8_t x, uint8_t y) const {
  bool routed = false;
  if (x < PINS && y < PINS) {
    routed = (_rows[_yPinMapping[y]] >> _xPinMapping[x]) & 0x01;
  }
  return routed;
}

void AD75019Routing::clear() {
  for (uint8_t y = 0; y < PINS; y++) {
    _rows[y] = 0;
  }
}

void AD75019Routing::getBytes(uint8_t bytes[BYTES]) const {
  for (uint8_t y = 0; y < PINS; y++) {
    uint8_t i = (PINS - 1 - y) * 2;
    bytes[i] = _rows[y] >> 8;
    bytes[i + 1] = _rows[y] & 0xFF;
  }
}

void AD75019Routing::setBytes(const uint8_t bytes[BYTES]) {
  for (uint8_t y = 0; y < PINS; y++) {
    uint8_t i = (PINS - 1 - y) * 2;
    _rows[y] = (static_cast<uint16_t>(bytes[i]) << 8) | bytes[i + 1];
  }
}

bool AD75019Routing::isValidMapping(const uint8_t pinMapping[PINS]) {
  bool valid = true;
  uint16_t used = 0;
  for (uint8_t i = 0; valid && i < PINS; i++) {
    valid = pinMapping[i] < PINS && !((used >> pinMapping[i]) & 0x01);
    if (valid) {
      used |= (1U << pinMapping[i]);
    }
  }
  return valid;
}
