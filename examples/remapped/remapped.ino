#include <AD75019.h>

/*
 * Imagine that these indices make the most sense for the
 * business logic
 */
static const uint8_t MAIN_OUT = 0, MAIN_IN = 0;
static const uint8_t SEND_1 = 1,   RETURN_1 = 1;
static const uint8_t SEND_2 = 2,   RETURN_2 = 2;
static const uint8_t SEND_3 = 3,   RETURN_3 = 3;
static const uint8_t SEND_4 = 4,   RETURN_4 = 4;
static const uint8_t SEND_5 = 5,   RETURN_5 = 5;
static const uint8_t SEND_6 = 6,   RETURN_6 = 6;
static const uint8_t SEND_7 = 7,   RETURN_7 = 7;
static const uint8_t SEND_8 = 8,   RETURN_8 = 8;
static const uint8_t SEND_9 = 9,   RETURN_9 = 9;
static const uint8_t SEND_10 = 10, RETURN_10 = 10;
static const uint8_t SEND_11 = 11, RETURN_11 = 11;
static const uint8_t SEND_12 = 12, RETURN_12 = 12;
static const uint8_t SEND_13 = 13, RETURN_13 = 13;
static const uint8_t SEND_14 = 14, RETURN_14 = 14;
static const uint8_t SEND_15 = 15, RETURN_15 = 15;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  /* 
   * For PCB layout reasons, these are the pins
   * on the AD75019 that had to be used.
   */ 
  uint8_t xMapping[16];
  uint8_t yMapping[16];
  xMapping[MAIN_IN] = 12;	yMapping[MAIN_OUT] = 3;
  xMapping[RETURN_1] = 0;	yMapping[SEND_1] = 12;
  xMapping[RETURN_2] = 1;	yMapping[SEND_2] = 13;
  xMapping[RETURN_3] = 2;	yMapping[SEND_3] = 14;
  xMapping[RETURN_4] = 3;	yMapping[SEND_4] = 15;
  xMapping[RETURN_5] = 15;	yMapping[SEND_5] = 0;
  xMapping[RETURN_6] = 14;	yMapping[SEND_6] = 1;
  xMapping[RETURN_7] = 13;	yMapping[SEND_7] = 2;
  xMapping[RETURN_8] = 11;	yMapping[SEND_8] = 7;
  xMapping[RETURN_9] = 4;	yMapping[SEND_9] = 6;
  xMapping[RETURN_10] = 5;	yMapping[SEND_10] = 5;
  xMapping[RETURN_11] = 6;	yMapping[SEND_11] = 4;
  xMapping[RETURN_12] = 7;	yMapping[SEND_12] = 11;
  xMapping[RETURN_13] = 10;	yMapping[SEND_13] = 10;
  xMapping[RETURN_14] = 9;	yMapping[SEND_14] = 9;
  xMapping[RETURN_15] = 8;	yMapping[SEND_15] = 8;


  AD75019 cpSwitch(5, 6, 7);

  // Initialize the AD75019
  if (!cpSwitch.begin(xMapping, yMapping)) {
    Serial.println("AD75019 initialization failed");
    while(1);
  }

  /*
   * Route configurations are a lot more readable
   */
  cpSwitch.addRoute(MAIN_IN, SEND_1);
  cpSwitch.addRoute(RETURN_1, SEND_6);
  cpSwitch.addRoute(RETURN_6, SEND_11);
  cpSwitch.addRoute(RETURN_11, SEND_2);
  cpSwitch.addRoute(RETURN_2, MAIN_OUT);

  // Send the route configuration to the AD75019
  cpSwitch.flush();

  cpSwitch.print();
}

void loop() {
	
}