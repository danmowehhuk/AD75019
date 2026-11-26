# AD75019 Crosspoint Switch Library for Arduino

This library is intended to simplfy use of the Analog Devices AD75019 
analog crosspoint switch. For more information about this chip, see the 
[AD75019 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD75019.pdf).

Written by Dan Mowehhuk (danmowehhuk@gmail.com)\
BSD license, check license.txt for more information\
All text above must be included in any redistribution

## Installation

To install, use the Arduino IDE Library Manager. Just search for AD75019!

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


## Building Routings Ahead of Time

`AD75019Routing` builds a configuration without the chip, so it can be worked
out ahead of time and kept as bytes. `newRouting()` returns one with the
switch's pin mapping and no routes; `getBytes()` copies a configuration out
(32 bytes, in the order the chip is sent them) and `flush(bytes)` sends one.
The bytes hold physical crosspoints, with the pin mapping already applied, which
is why a routing for a remapped switch comes from `newRouting()`.
`flush(bytes)` replaces the switch's own configuration, so `addRoute()` and
`isRouted()` then work on the loaded routes, and `setBytes()` loads bytes back
into a routing.

```c
AD75019Routing routing = xptSwitch.newRouting();
routing.addRoute(0, 3);
routing.addRoute(3, 4);

uint8_t bytes[AD75019Routing::BYTES];
routing.getBytes(bytes);  // keep these...

xptSwitch.flush(bytes);   // ...and send them when needed
```


## Without Arduino

Built with `-DNO_ARDUINO`, the library needs nothing from the Arduino core or
any other library. Use the callback constructor to say how pins are set up and
written, and note that `print()` isn't available. For example, with
[BareMetalHAL](https://github.com/danmowehhuk/BareMetalHAL):

```c
#include <BareMetalHAL.h>
#include <AD75019.h>

using namespace BareMetalHAL;

void setOutput(uint8_t p, uint8_t mode) { pinMode(p, mode); }
void writePin(uint8_t p, uint8_t value) { digitalWrite(p, value); }

AD75019 xptSwitch(pin(Port::B, 4), pin(Port::B, 5), pin(Port::B, 6),
                  setOutput, writePin);
```

The pinMode callback is passed 1 (Arduino's `OUTPUT`), and the digitalWrite
callback 1 (`HIGH`) or 0 (`LOW`).

The bare-metal tests are in [test/test-suite-avr](test/test-suite-avr), built
for an ATmega2560 with `./build.sh` (`-s` for SimulIDE). They use TestTool and
BareMetalHAL, found under `~/Arduino/libraries` unless `TESTTOOL_SRC` or
`BAREMETALHAL_SRC` points elsewhere. Load the `.sim.hex` into the Mega in
[test/AD75019Test.sim2](test/AD75019Test.sim2) to run them.
