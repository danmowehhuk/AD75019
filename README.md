# AD75019 Crosspoint Switch Library for Arduino

This library is intended to simplfy use of the Analog Devices AD75019 
analog crosspoint switch. For more information about this chip, see the 
[AD75019 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD75019.pdf).

Written by Dan Mowehhuk (danmowehhuk@gmail.com)\
BSD license, check license.txt for more information\
All text above must be included in any redistribution


## Quick Start

Simply instantiate the `AD75019` class with the Arduino pin numbers for PCLK,
SCLK and SIN. The chip works just like a 256-bit shift register, where bits
are clocked in serially, then applied by setting the PCLK pin HIGH then back 
to LOW.

```c
#include <AD75019.h>

// Connect PCLK, SCLK and SIN to pins 5, 6 and 7
AD75019 xptSwitch(5, 6, 7);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize the AD75019
  if (!xptSwitch.begin()) {
    Serial.println("AD75019 initialization failed")
    while(1);
  }

  // Configure routes
  xptSwitch.addRoute(0, 3); // X0 to Y3
  xptSwitch.addRoute(3, 4); // X3 to Y4
  // all other crosspoints left unconnected

  // Send the route configuration to the AD75019
  xptSwitch.flush();
}
```


## Using Callbacks

If you are using a port expander such as the MCP23017, you will need to provide
callback functions to the `AD75019` class telling it how to perform a `pinMode`
and `digitalWrite`.

See the [callback example](examples/callbacks/callbacks.ino) example for more 
details.


## Pin Remapping

PCB layout considerations may require X and Y pins to be used non-sequentially,
while your code would prefer sequential numbers. You can provide `xMapping` and
`yMapping` arrays to let the library take care of this remapping for you.

**NOTE:** The remapping arrays must be 16 elements in size, with values between
0-15, and without repeats. If a remapping array is invalid, the `begin(xMapping, yMapping)`
method will return `false` and calling `flush()` will be disabled.

See the [remapping example](examples/remapped/remapped.ino) example for more 
details.
