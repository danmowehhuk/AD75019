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

constexpr uint8_t PIN_OUTPUT = OUTPUT;
constexpr uint8_t PIN_HIGH = HIGH;
constexpr uint8_t PIN_LOW = LOW;

inline void pinModeOutput(uint8_t pin) { pinMode(pin, OUTPUT); }
inline void writePin(uint8_t pin, uint8_t value) { digitalWrite(pin, value); }

inline void print(const FlashStr* s) { Serial.print(s); }
inline void print(int value) { Serial.print(value); }
inline void println(const FlashStr* s) { Serial.println(s); }
inline void println() { Serial.println(); }

}

#else

#include <BareMetalHAL.h>

namespace AD75019Hal {

using FlashStr = BareMetalHAL::FlashStr;

constexpr uint8_t PIN_OUTPUT = BareMetalHAL::OUTPUT;
constexpr uint8_t PIN_HIGH = BareMetalHAL::HIGH;
constexpr uint8_t PIN_LOW = BareMetalHAL::LOW;

inline void pinModeOutput(uint8_t pin) { BareMetalHAL::pinMode(pin, BareMetalHAL::OUTPUT); }
inline void writePin(uint8_t pin, uint8_t value) { BareMetalHAL::digitalWrite(pin, value); }

// UART0, which the caller begins (BareMetalHAL::Uart0::begin()).
inline void print(const FlashStr* s) { BareMetalHAL::Uart0::print(s); }
inline void print(int value) { BareMetalHAL::Uart0::print(value); }
inline void println(const FlashStr* s) { BareMetalHAL::Uart0::println(s); }
inline void println() { BareMetalHAL::Uart0::println(""); }

}

#endif

#endif
