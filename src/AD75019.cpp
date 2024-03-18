/*

  AD75019.cpp

  Library for the Analog Devices AD75019 Crosspoint Switch

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#include <Arduino.h>
#include "AD75019.h"

void _ad75019_pinModeDefault(uint8_t pinNumber) {
  pinMode(pinNumber, OUTPUT);
}

void _ad75019_digitalWriteDefault(uint8_t pinNumber, uint8_t value) {
  digitalWrite(pinNumber, value);
}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(_ad75019_pinModeDefault), _digitalWriteCallback(_ad75019_digitalWriteDefault) {}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
          voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(pinModeCallback), _digitalWriteCallback(digitalWriteCallback) {}


bool AD75019::begin() {
    _pinModeCallback(_pclkPinNumber, OUTPUT);
    _pinModeCallback(_sclkPinNumber, OUTPUT);
    _pinModeCallback(_sinPinNumber, OUTPUT);
    _digitalWriteCallback(_pclkPinNumber, HIGH);
    _digitalWriteCallback(_sclkPinNumber, LOW);
    _digitalWriteCallback(_sinPinNumber, LOW);
    _begun = true;
    return true;
}

bool AD75019::begin(uint8_t xPinMapping[16], uint8_t yPinMapping[16]) {
  // validate pin mapping arrays
  uint16_t used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (xPinMapping[i] > 15 || bitRead(used, xPinMapping[i])) {
      // invalid mapping
      return false;
    } else {
      _xPinMapping[i] = xPinMapping[i];
      bitWrite(used, xPinMapping[i], 1);
    }
  }

  used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (yPinMapping[i] > 15 || bitRead(used, yPinMapping[i])) {
      // invalid mapping
      return false;
    } else {
      _yPinMapping[i] = yPinMapping[i];
      bitWrite(used, yPinMapping[i], 1);
    }
  }

  return begin();
}

void AD75019::addRoute(uint8_t x, uint8_t y) {
  bitWrite(
    _configBuffer[_yPinMapping[y]],
    _xPinMapping[x], 1);
}

bool AD75019::isRouted(uint8_t x, uint8_t y) {
  return bitRead(
    _configBuffer[_yPinMapping[y]],
    _xPinMapping[x]);
}

void AD75019::flush() {
	if (!_begun) return;
  for (int8_t y = 15; y > -1; y--) {
    for (int8_t x = 15; x > -1; x--) {
      _digitalWriteCallback(_sinPinNumber,
        bitRead(_configBuffer[y], x));
      _digitalWriteCallback(_sclkPinNumber, HIGH);
      _digitalWriteCallback(_sclkPinNumber, LOW);
    }
  }
  _digitalWriteCallback(_pclkPinNumber, LOW);
  _digitalWriteCallback(_pclkPinNumber, HIGH);
}

void AD75019::clear() {
  for (uint8_t i = 0; i < 16; i++) {
    _configBuffer[i] = 0;
  }
}

void AD75019::print() {
	if (!_begun) {
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
      Serial.print(bitRead(_configBuffer[y], x));
      if (x % 4 == 0) {
        Serial.print(F(" "));
      }
    }
    Serial.println();
  }
}
