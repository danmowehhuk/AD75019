/*

  AD75019.cpp

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#include "AD75019.h"

#ifndef NO_ARDUINO
#include <Arduino.h>

static const uint8_t PIN_OUTPUT = OUTPUT;
static const uint8_t PIN_HIGH = HIGH;
static const uint8_t PIN_LOW = LOW;
#else
// The values the callbacks are passed, as Arduino's OUTPUT, HIGH and LOW.
static const uint8_t PIN_OUTPUT = 1;
static const uint8_t PIN_HIGH = 1;
static const uint8_t PIN_LOW = 0;
#endif

// Arduino's bitRead() and bitWrite(), which aren't available off Arduino.
template <typename T>
static inline bool readBit(T value, uint8_t bit) {
  return (value >> bit) & 0x01;
}

template <typename T>
static inline void writeBit(T& value, uint8_t bit, bool bitValue) {
  if (bitValue) {
    value |= (1UL << bit);
  } else {
    value &= ~(1UL << bit);
  }
}

#ifndef NO_ARDUINO
void _ad75019_pinModeDefault(uint8_t pinNumber, uint8_t /* unused */) {
  pinMode(pinNumber, OUTPUT);
}

void _ad75019_digitalWriteDefault(uint8_t pinNumber, uint8_t value) {
  digitalWrite(pinNumber, value);
}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(_ad75019_pinModeDefault), _digitalWriteCallback(_ad75019_digitalWriteDefault) {
    setUseDefaultCallbacks(true);
 }
#endif

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
          voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(pinModeCallback), _digitalWriteCallback(digitalWriteCallback) {}


bool AD75019::begin() {
    _pinModeCallback(_pclkPinNumber, PIN_OUTPUT);
    _pinModeCallback(_sclkPinNumber, PIN_OUTPUT);
    _pinModeCallback(_sinPinNumber, PIN_OUTPUT);
    _digitalWriteCallback(_pclkPinNumber, PIN_HIGH);
    _digitalWriteCallback(_sclkPinNumber, PIN_LOW);
    _digitalWriteCallback(_sinPinNumber, PIN_LOW);
    setBegun(true);
    return true;
}

bool AD75019::begin(const uint8_t xPinMapping[16], const uint8_t yPinMapping[16]) {
  bool begun = AD75019Routing::setPinMapping(xPinMapping, yPinMapping);
  if (begun) {
    begun = begin();
  }
  return begun;
}

void AD75019::addRoute(uint8_t x, uint8_t y) {
  _routing.addRoute(x, y);
}

bool AD75019::isRouted(uint8_t x, uint8_t y) {
  return _routing.isRouted(x, y);
}

void AD75019::flush() {
  if (isBegun()) {
    // Y15 to Y0, each X15 to X0, as AD75019Routing's bytes are ordered
    for (int8_t y = AD75019Routing::PINS - 1; y > -1; y--) {
      uint16_t row = _routing._rows[y];
      for (uint8_t x = 0; x < AD75019Routing::PINS; x++) {
        _digitalWriteCallback(_sinPinNumber, (row & 0x8000) ? PIN_HIGH : PIN_LOW);
        _digitalWriteCallback(_sclkPinNumber, PIN_HIGH);
        _digitalWriteCallback(_sclkPinNumber, PIN_LOW);
        row <<= 1;
      }
    }
    _digitalWriteCallback(_pclkPinNumber, PIN_LOW);
    _digitalWriteCallback(_pclkPinNumber, PIN_HIGH);
  }
}

void AD75019::flush(const AD75019Routing& routing) {
  _routing = routing;
  flush();
}

void AD75019::clear() {
  _routing.clear();
}

#ifndef NO_ARDUINO
void AD75019::print() {
	if (!isBegun()) {
    Serial.println(F("AD75019 not initialized!"));
  }
  Serial.println(F("  X: 5432 1098 7654 3210"));
  for (int8_t y = 15; y > -1; y--) {
    if (y > 9) {
      Serial.print(F("Y"));
      Serial.print(y);
      Serial.print(F(": "));
    } else {
      Serial.print(F(" Y"));
      Serial.print(y);
      Serial.print(F(": "));
    }
    for (int8_t x = 15; x > -1; x--) {
      Serial.print(readBit(_routing._rows[y], x));
      if (x % 4 == 0) {
        Serial.print(F(" "));
      }
    }
    Serial.println();
  }
}
#endif

void AD75019::setBegun(bool b) {
	writeBit(_state, 1, b);
}

bool AD75019::isBegun() {
	return readBit(_state, 1);
}

void AD75019::setUseDefaultCallbacks(bool b) {
	writeBit(_state, 0, b);
}

bool AD75019::isUseDefaultCallbacks() {
	return readBit(_state, 0);
}

