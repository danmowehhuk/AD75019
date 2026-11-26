#ifndef AD75019_HAL_AD75019HAL_H
#define AD75019_HAL_AD75019HAL_H

// The hardware calls AD75019 makes: setting a pin as an output, writing it,
// and printing the configuration matrix. Arduino by default; with
// NO_ARDUINO, BareMetalHAL, where a pin is a BareMetalHAL::pin(Port, bit)
// packed value instead of an Arduino pin number.

#include <stdint.h>

#ifndef NO_ARDUINO

#include <Arduino.h>

namespace AD75019Hal {

using FlashStr = __FlashStringHelper;

const uint8_t PIN_OUTPUT = OUTPUT;
const uint8_t PIN_HIGH = HIGH;
const uint8_t PIN_LOW = LOW;

inline void pinModeOutput(uint8_t pin) { pinMode(pin, OUTPUT); }
inline void writePin(uint8_t pin, uint8_t value) { digitalWrite(pin, value); }

inline void print(const FlashStr* s) { Serial.print(s); }
inline void print(int value) { Serial.print(value); }
inline void println(const FlashStr* s) { Serial.println(s); }
inline void println() { Serial.println(); }

}

#else

namespace AD75019Hal {

class FlashStr;

extern const uint8_t PIN_OUTPUT;
extern const uint8_t PIN_HIGH;
extern const uint8_t PIN_LOW;

void pinModeOutput(uint8_t pin);
void writePin(uint8_t pin, uint8_t value);

void print(const FlashStr* s);
void print(int value);
void println(const FlashStr* s);
void println();

}

#endif

#endif
