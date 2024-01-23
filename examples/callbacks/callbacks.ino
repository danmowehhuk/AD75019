#include <AD75019.h>

void myPinModeCallback(uint8_t pin, uint8_t mode) {
  // using a port expander...
  // myExpander.pinMode(pin, mode);

  pinMode(pin, mode);
}

void myDigitalWriteCallback(uint8_t pin, uint8_t value) {
  // using a port expander...
  // myExpander.digitalWrite(pin, value);

  digitalWrite(pin, value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  AD75019 cpSwitch(5, 6, 7, myPinModeCallback, myDigitalWriteCallback);

  cpSwitch.begin();

  cpSwitch.connect(0, 4);
  cpSwitch.connect(4, 3);
  cpSwitch.connect(3, 2);
  cpSwitch.connect(2, 1);
  cpSwitch.connect(1, 0);

  cpSwitch.flush();

  cpSwitch.print();
}

void loop() {
	
}