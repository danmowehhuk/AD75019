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

  // Initialize the AD75019
  if (!cpSwitch.begin()) {
    Serial.println("AD75019 initialization failed");
    while(1);
  }

  // Configure routes
  cpSwitch.addRoute(0, 4);
  cpSwitch.addRoute(4, 3);
  cpSwitch.addRoute(3, 2);
  cpSwitch.addRoute(2, 1);
  cpSwitch.addRoute(1, 0);

  // Send the route configuration to the AD75019
  cpSwitch.flush();

  cpSwitch.print();
}

void loop() {
  
}
