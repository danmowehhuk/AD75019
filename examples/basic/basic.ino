#include <AD75019.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  AD75019 cpSwitch(5, 6, 7);

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