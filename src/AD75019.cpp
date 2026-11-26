/*

  AD75019.cpp

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#include "AD75019.h"
#include "hal/AD75019Hal.h"

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

void _ad75019_pinModeDefault(uint8_t pinNumber, uint8_t /* unused */) {
  AD75019Hal::pinModeOutput(pinNumber);
}

void _ad75019_digitalWriteDefault(uint8_t pinNumber, uint8_t value) {
  AD75019Hal::writePin(pinNumber, value);
}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(_ad75019_pinModeDefault), _digitalWriteCallback(_ad75019_digitalWriteDefault) {
    setUseDefaultCallbacks(true);
 }

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
          voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(pinModeCallback), _digitalWriteCallback(digitalWriteCallback) {}


bool AD75019::begin() {
    _pinModeCallback(_pclkPinNumber, AD75019Hal::PIN_OUTPUT);
    _pinModeCallback(_sclkPinNumber, AD75019Hal::PIN_OUTPUT);
    _pinModeCallback(_sinPinNumber, AD75019Hal::PIN_OUTPUT);
    _digitalWriteCallback(_pclkPinNumber, AD75019Hal::PIN_HIGH);
    _digitalWriteCallback(_sclkPinNumber, AD75019Hal::PIN_LOW);
    _digitalWriteCallback(_sinPinNumber, AD75019Hal::PIN_LOW);
    setBegun(true);
    return true;
}

bool AD75019::begin(uint8_t xPinMapping[16], uint8_t yPinMapping[16]) {
  // validate pin mapping arrays
  uint16_t used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (xPinMapping[i] > 15 || readBit(used, xPinMapping[i])) {
      // invalid mapping
      return false;
    } else {
      _xPinMapping[i] = xPinMapping[i];
      writeBit(used, xPinMapping[i], 1);
    }
  }

  used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (yPinMapping[i] > 15 || readBit(used, yPinMapping[i])) {
      // invalid mapping
      return false;
    } else {
      _yPinMapping[i] = yPinMapping[i];
      writeBit(used, yPinMapping[i], 1);
    }
  }

  return begin();
}

void AD75019::addRoute(uint8_t x, uint8_t y) {
  writeBit(
    _configBuffer[_yPinMapping[y]],
    _xPinMapping[x], 1);
}

bool AD75019::isRouted(uint8_t x, uint8_t y) {
  return readBit(
    _configBuffer[_yPinMapping[y]],
    _xPinMapping[x]);
}

void AD75019::flush() {
	if (!isBegun()) return;
  for (int8_t y = 15; y > -1; y--) {
    for (int8_t x = 15; x > -1; x--) {
      _digitalWriteCallback(_sinPinNumber,
        readBit(_configBuffer[y], x));
      _digitalWriteCallback(_sclkPinNumber, AD75019Hal::PIN_HIGH);
      _digitalWriteCallback(_sclkPinNumber, AD75019Hal::PIN_LOW);
    }
  }
  _digitalWriteCallback(_pclkPinNumber, AD75019Hal::PIN_LOW);
  _digitalWriteCallback(_pclkPinNumber, AD75019Hal::PIN_HIGH);
}

void AD75019::clear() {
  for (uint8_t i = 0; i < 16; i++) {
    _configBuffer[i] = 0;
  }
}

void AD75019::print() {
	if (!isBegun()) {
    AD75019Hal::println(F("AD75019 not initialized!"));
  }
  AD75019Hal::println(F("  X: 5432 1098 7654 3210"));
  for (int8_t y = 15; y > -1; y--) {
    if (y > 9) {
      AD75019Hal::print(F("Y"));
      AD75019Hal::print(y);
      AD75019Hal::print(F(": "));
    } else {
      AD75019Hal::print(F(" Y"));
      AD75019Hal::print(y);
      AD75019Hal::print(F(": "));
    }
    for (int8_t x = 15; x > -1; x--) {
      AD75019Hal::print(readBit(_configBuffer[y], x));
      if (x % 4 == 0) {
        AD75019Hal::print(F(" "));
      }
    }
    AD75019Hal::println();
  }
}

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

