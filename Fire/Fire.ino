// Animated Fire Example - OctoWS2811 Library
//  http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
// 
// Based on the simple algorithm explained here:
//  http://caraesnaur.github.io/fire/
//
// This example code is in the public domain.

#include <OctoWS2811.h>

// The display size and color to use
const unsigned int width = 120;
const unsigned int height = 16;

// Extra Pin Setup
// The Octo board breaks out these pins:
// 0 RX1/CRX2/PWM 
// 1 TX1/CTX2/PWM
// 22 CTX1/PWM/Analog/A9
// 23 CRX1/PWM/Analog/A8
// 19 SCL0(I2C)/AnalogA5/PWM
// 18 SDA0(I2C)/AnalogA4/PWM
// 17 SDA1(I2C)/AnalogA3/PWM/RX4



// These parameters control the fire appearance
// (try controlling these with knobs / analogRead....)
unsigned int heat = width / 5;
unsigned int focus = 9;
unsigned int cool = 26;

// Arrays for fire animation
unsigned char canvas[width*height];
extern const unsigned int fireColor[100];

// OctoWS2811 objects
const int ledsPerPin = width * height / 8;
DMAMEM int displayMemory[ledsPerPin*6];
int drawingMemory[ledsPerPin*6];
// =====================================================================
// THIS IS WHERE YOU WANT TO CONFIGURE THE LED LAYOUT AND REFRESH RATE
// 
// WS2811_RGB	LITERAL1 <---  This is what the docs say the layout is
// WS2811_RBG	LITERAL1
// WS2811_GRB	LITERAL1 <---  Most US are labeled this way
// WS2811_GBR	LITERAL1
// WS2811_BRG	LITERAL1
// WS2811_BGR	LITERAL1 <---- IIRC though Thor's are wired this way
//
// RIGHT UNDER HERE JUST CHANGE THE FIRST VALUE, the WS2811_XXX
// DO NOT CHANGE THE REFRESH RATE (WS2811_800kHz)
//
// AND YES I REALIZE WE HAVE WS2812B BUT THE VALUES ARE THE SAME FOR WS_2811 IN THIS CODE

// =====================================================================
const int config = WS2811_BRG | WS2811_800kHz;


// ---------------------------------------------------------------
// DO NOT EDIT BELOW
// ===============================================================
OctoWS2811 leds(ledsPerPin, displayMemory, drawingMemory, config);


// Run setup once
void setup() {
  // turn on the display
  leds.begin();
  leds.show();
}


// A simple xy() function to turn display matrix coordinates
// into the index numbers OctoWS2811 requires.  If your LEDs
// are arranged differently, edit this code...
unsigned int xy(unsigned int x, unsigned int y) {
  if ((y & 1) == 0) {
    // even numbered rows (0, 2, 4...) are left to right
    return y * width + x;
  } else {
    // odd numbered rows (1, 3, 5...) are right to left
    return y * width + width - 1 - x;
  }
}

elapsedMillis msec;

// Run repetitively
void loop() {
  if (msec >= 45) {
    msec = 0;
    animateFire();
  }
}


void animateFire() {
  unsigned int i, c, n, x, y;

  // Step 1: move all data up one line
  memmove(canvas + width, canvas, width * (height - 1));
  memset(canvas, 0, width);

  // Step 2: draw random heatspots on bottom line
  i = heat;
  if (i > width-8) i = width-8;
  while (i > 0) {
    x = random(width - 2) + 1;
    if (canvas[x] == 0) {
      canvas[x] = 99;
      i--;
    }
  }

  // Step 3: interpolate
  for (y=0; y < height; y++) {
    for (x=0; x < width; x++) {
      c = canvas[y * width + x] * focus;
      n = focus;
      if (x > 0) {
        c = c + canvas[y * width + (x - 1)];
        n = n + 1;
      }
      if (x < width-1) {
        c = c + canvas[y * width + (x + 1)];
        n = n + 1;
      }
      if (y > 0) {
        c = c + canvas[(y -1) * width + x];
        n = n + 1;
      }
      if (y < height-1) {
        c = c + canvas[(y + 1) * width + x];
        n = n + 1;
      }
      c = (c + (n / 2)) / n;
      i = (random(1000) * cool) / 10000;
      if (c > i) {
        c = c - i;
      } else {
        c = 0;
      }
      canvas[y * width + x] = c;
    }
  }

  // Step 4: render canvas to LEDs
  for (y=0; y < height; y++) {
    for (x=0; x < width; x++) {
      c = canvas[((height - 1) - y) * width + x];
      leds.setPixel(xy(x, y), fireColor[c]);
    }
  }
  leds.show();
}


