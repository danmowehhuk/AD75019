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
  pclkPinNumber(pclkPinNumber), sclkPinNumber(sclkPinNumber), sinPinNumber(sinPinNumber),
  pinModeCallback(_ad75019_pinModeDefault), digitalWriteCallback(_ad75019_digitalWriteDefault) {}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
          voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback):
  pclkPinNumber(pclkPinNumber), sclkPinNumber(sclkPinNumber), sinPinNumber(sinPinNumber),
  pinModeCallback(pinModeCallback), digitalWriteCallback(digitalWriteCallback) {}


void AD75019::begin() {
    pinModeCallback(pclkPinNumber, OUTPUT);
    pinModeCallback(sclkPinNumber, OUTPUT);
    pinModeCallback(sinPinNumber, OUTPUT);
    digitalWriteCallback(pclkPinNumber, HIGH);
    digitalWriteCallback(sclkPinNumber, LOW);
    digitalWriteCallback(sinPinNumber, LOW);
}

void AD75019::begin(uint8_t newXPinMapping[16], uint8_t newYPinMapping[16]) {
  begin();

  // validate pin mapping arrays
  uint16_t used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (newXPinMapping[i] > 15 || bitRead(used, newXPinMapping[i])) {
      // invalid mapping
      while(1) {}
    } else {
      xPinMapping[i] = newXPinMapping[i];
      bitWrite(used, newXPinMapping[i], 1);
    }
  }

  used = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (newYPinMapping[i] > 15 || bitRead(used, newYPinMapping[i])) {
      // invalid mapping
      while(1) {}
    } else {
      yPinMapping[i] = newYPinMapping[i];
      bitWrite(used, newYPinMapping[i], 1);
    }
  }
}

void AD75019::connect(uint8_t x, uint8_t y) {
  bitWrite(
    configBuffer[yPinMapping[y]],
    xPinMapping[x], 1);
}

bool AD75019::isConnected(uint8_t x, uint8_t y) {
  return bitRead(
    configBuffer[yPinMapping[y]],
    xPinMapping[x]);
}

void AD75019::flush() {
  for (int8_t y = 15; y > -1; y--) {
    for (int8_t x = 15; x > -1; x--) {
      digitalWriteCallback(sinPinNumber,
        bitRead(configBuffer[y], x));
      digitalWriteCallback(sclkPinNumber, HIGH);
      digitalWriteCallback(sclkPinNumber, LOW);
    }
  }
  digitalWriteCallback(pclkPinNumber, LOW);
  digitalWriteCallback(pclkPinNumber, HIGH);
}

void AD75019::clear() {
  memset(configBuffer, 0, 16);
}

void AD75019::print() {
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
      Serial.print(bitRead(configBuffer[y], x));
      if (x % 4 == 0) {
        Serial.print(F(" "));
      }
    }
    Serial.println();
  }
}
