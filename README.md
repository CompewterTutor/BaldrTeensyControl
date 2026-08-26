# Baldr

## Teensy Lighting Firmware for Thor Mutant Vehicle Art Car

### Hardware
Teensy 3.6 (or up to 4.1) with OctoWS2811
==========

Lighting Panel is 1920 pixels arranged with 16 rows (8 outputs from one side that snake back to same side for 16 rows) and 120 columns.

2812B Addressable LEDs. 3x 60Ax5V (300W) power supplies in parallel.

## Programs

Burning Bus - Fire program based on 

Control thousands of WS2811/2812 LEDs at video refresh speeds

http://www.pjrc.com/teensy/td_libs_OctoWS2811.html

https://www.youtube.com/watch?v=M5XQLvFPcBM


## Installation

Install ArduinoIDE 2.x.x from either your package manager or download installer from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software) then go to the settings page, add the Teensyduino source https://www.pjrc.com/teensy/package_teensy_index.json and then click on the Boards Manager and search for Teensy and install the board support.
