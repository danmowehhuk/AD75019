// test/test-suite-avr/test-suite-avr.cpp
//
// Bare-metal AVR tests (NO_ARDUINO): callbacks around BareMetalHAL drive
// real pins, and recording callbacks check the bit stream flush() shifts
// out.
#include <BareMetalHAL.h>
#include <TestTool.h>
#include <AD75019.h>
#include <avr/io.h>

using namespace BareMetalHAL;

// Pin ids for the recording callbacks, which never touch a real pin.
const uint8_t PCLK = 1;
const uint8_t SCLK = 2;
const uint8_t SIN = 3;

// What the recording callbacks saw: SIN at each SCLK rising edge, and each
// PCLK rising edge (a latch).
uint8_t sinLevel = 0;
uint8_t sclkLevel = 0;
uint8_t pclkLevel = 0;
uint16_t clocks = 0;
uint8_t latches = 0;
bool stream[256];

void halPinMode(uint8_t pinId, uint8_t mode) {
  pinMode(pinId, mode);
}

void halWrite(uint8_t pinId, uint8_t value) {
  digitalWrite(pinId, value);
}

void recordPinMode(uint8_t, uint8_t) {}

void recordWrite(uint8_t pinId, uint8_t value) {
  if (pinId == SIN) {
    sinLevel = value;
  } else if (pinId == SCLK) {
    if (value && !sclkLevel) {
      if (clocks < 256) {
        stream[clocks] = sinLevel;
      }
      clocks++;
    }
    sclkLevel = value;
  } else if (pinId == PCLK) {
    if (value && !pclkLevel) {
      latches++;
    }
    pclkLevel = value;
  }
}

void resetRecording() {
  clocks = 0;
  latches = 0;
  for (uint16_t i = 0; i < 256; i++) {
    stream[i] = false;
  }
}

// Where the bit for physical pins x and y falls in flush()'s stream, which
// runs Y15 to Y0 and, within each, X15 to X0.
uint16_t streamIndex(uint8_t x, uint8_t y) {
  return (15 - y) * 16 + (15 - x);
}

uint16_t countSet() {
  uint16_t count = 0;
  for (uint16_t i = 0; i < 256; i++) {
    if (stream[i]) {
      count++;
    }
  }
  return count;
}

void testBeginSetsPins(TestInvocation* t) {
  t->setName(F("begin() makes the pins outputs, PCLK high"));
  AD75019 cpSwitch(pin(Port::B, 4), pin(Port::B, 5), pin(Port::B, 6), halPinMode, halWrite);
  t->verify(cpSwitch.begin(), F("begin() should succeed"));
  t->verify((DDRB & _BV(PB4)) && (DDRB & _BV(PB5)) && (DDRB & _BV(PB6)),
            F("PB4-PB6 should be outputs"));
  t->verify(PORTB & _BV(PB4), F("PCLK (PB4) should be high"));
  t->verify(!(PORTB & _BV(PB5)), F("SCLK (PB5) should be low"));
  t->verify(!(PORTB & _BV(PB6)), F("SIN (PB6) should be low"));
}

void testFlushOnRealPins(TestInvocation* t) {
  t->setName(F("flush() on real pins ends latched, clock low"));
  AD75019 cpSwitch(pin(Port::B, 4), pin(Port::B, 5), pin(Port::B, 6), halPinMode, halWrite);
  cpSwitch.begin();
  cpSwitch.addRoute(15, 0);
  // The opposite of where flush() leaves them, so it must drive both.
  digitalWrite(pin(Port::B, 4), LOW);
  digitalWrite(pin(Port::B, 5), HIGH);
  cpSwitch.flush();
  t->verify(PORTB & _BV(PB4), F("PCLK (PB4) should end high"));
  t->verify(!(PORTB & _BV(PB5)), F("SCLK (PB5) should end low"));
}

void testFlushShiftsEveryBitThenLatches(TestInvocation* t) {
  t->setName(F("flush() shifts 256 bits, then latches once"));
  AD75019 cpSwitch(PCLK, SCLK, SIN, recordPinMode, recordWrite);
  cpSwitch.begin();
  cpSwitch.addRoute(0, 4);
  cpSwitch.addRoute(4, 3);
  resetRecording();
  cpSwitch.flush();
  t->verify(clocks == 256, F("expected 256 clocks"));
  t->verify(latches == 1, F("expected one latch"));
  t->verify(stream[streamIndex(0, 4)], F("X0-Y4 should be set"));
  t->verify(stream[streamIndex(4, 3)], F("X4-Y3 should be set"));
  t->verify(countSet() == 2, F("only the two routes should be set"));
}

void testFlushBeforeBeginDoesNothing(TestInvocation* t) {
  t->setName(F("flush() before begin() sends nothing"));
  AD75019 cpSwitch(PCLK, SCLK, SIN, recordPinMode, recordWrite);
  cpSwitch.addRoute(0, 0);
  resetRecording();
  cpSwitch.flush();
  t->verify(clocks == 0 && latches == 0, F("nothing should be sent"));
}

void testClearRemovesRoutes(TestInvocation* t) {
  t->setName(F("clear() removes every route"));
  AD75019 cpSwitch(PCLK, SCLK, SIN, recordPinMode, recordWrite);
  cpSwitch.begin();
  cpSwitch.addRoute(1, 2);
  t->verify(cpSwitch.isRouted(1, 2), F("X1-Y2 should be routed"));
  cpSwitch.clear();
  t->verify(!cpSwitch.isRouted(1, 2), F("X1-Y2 should be cleared"));
  resetRecording();
  cpSwitch.flush();
  t->verify(countSet() == 0, F("no bits should be set"));
}

void testRemappedPins(TestInvocation* t) {
  t->setName(F("A pin mapping moves routes to the mapped pins"));
  uint8_t xMapping[16] = {13, 15, 14, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 12};
  uint8_t yMapping[16] = {4, 6, 5, 0, 1, 2, 3, 12, 13, 14, 15, 10, 9, 8, 11, 7};
  AD75019 cpSwitch(PCLK, SCLK, SIN, recordPinMode, recordWrite);
  t->verify(cpSwitch.begin(xMapping, yMapping), F("begin() should accept the mapping"));
  cpSwitch.addRoute(0, 0);
  t->verify(cpSwitch.isRouted(0, 0), F("X0-Y0 should be routed"));
  t->verify(!cpSwitch.isRouted(1, 0), F("X1-Y0 should not be routed"));
  resetRecording();
  cpSwitch.flush();
  t->verify(stream[streamIndex(13, 4)], F("physical X13-Y4 should be set"));
  t->verify(countSet() == 1, F("only one bit should be set"));
}

void testBadMappingsRefused(TestInvocation* t) {
  t->setName(F("begin() refuses repeated or out-of-range pins"));
  uint8_t ok[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  uint8_t repeated[16] = {0, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  uint8_t outOfRange[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16};
  AD75019 cpSwitch(PCLK, SCLK, SIN, recordPinMode, recordWrite);
  t->verify(!cpSwitch.begin(repeated, ok), F("a repeated X pin should be refused"));
  t->verify(!cpSwitch.begin(ok, outOfRange), F("an out-of-range Y pin should be refused"));
}

int main() {
  BareMetalHAL::Uart0::begin(9600);
  BareMetalHAL::timingInit();

  TestFunction tests[] = {
    testBeginSetsPins,
    testFlushOnRealPins,
    testFlushShiftsEveryBitThenLatches,
    testFlushBeforeBeginDoesNothing,
    testClearRemovesRoutes,
    testRemappedPins,
    testBadMappingsRefused
  };

  runTestSuiteShowMem(tests);

  while (true) {}
  return 0;
}
