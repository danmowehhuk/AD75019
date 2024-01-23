#include <AD75019.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  AD75019 cpSwitch(5, 6, 7);

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