# Baldr

## Teensy Lighting Firmware for Thor Mutant Vehicle Art Car

### Hardware
Teensy 3.6 (or up to 4.1) with OctoWS2811:  https://www.pjrc.com/store/octo28_adaptor.html

https://www.pjrc.com/teensy/td_libs_OctoWS2811.html

==========

Lighting Panel is 1920 pixels arranged with 16 rows (8 outputs from one side that snake back to same side for 16 rows) and 120 columns.

2812B Addressable LEDs. 3x 60Ax5V (300W) power supplies in parallel.

**Limitations**
(per Teesny)

What limits the number of LEDs can you use with each Teensy?

1872: Teensy 3.0's RAM limit (90% used)

4320 tested in this project

4416: 60 HZ refresh

6000: 120V USA power*

8800: 30 Hz refresh (uses 86% of Teensy 3.2's available RAM)

10920: Limit due to DMA hardware, 1365 LEDs per pin, using 8 pins
(Teensy 4.x can use more pins)

To connect more LEDs, use multiple Teensy and OctoWS2811 Adaptors with their SYNC signals tied together.

* - Approximate, based on typical power supply efficiency and standard 15 amp circuit breakers

### Schematic

![Schematic](docs/_assets/schematic_octo28.gif)

### OctoWS2811 Pinouts

![OctoWS2811 Pinouts](docs/_assets/Octo2811_pinouts_LEDStrip.png)


## Software

Burning Bus - Fire program
```
//--------------------------------------------------------------------------------------
//  Memory Usage on Teensy 4.0:
//  FLASH: code:12324, data:3016, headers:8208   free for files:2008068
//   RAM1: variables:11808, code:10600, padding:22168   free for local variables:479712
//   RAM2: variables:28448  free for malloc/new:495840
//--------------------------------------------------------------------------------------
```

Libraries: 
http://www.pjrc.com/teensy/td_libs_OctoWS2811.html

https://www.youtube.com/watch?v=M5XQLvFPcBM


## Installation

Install ArduinoIDE 2.x.x from either your package manager or download installer from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software) then go to the settings page, add the Teensyduino source https://www.pjrc.com/teensy/package_teensy_index.json and then click on the Boards Manager and search for Teensy and install the board support.


## Files

### Fire Animation
> Location: Fire/ThorPanelFire.ino

## Windows Teensy Loader GUI precompiled and portable
I've included the teensy.exe gui loader in the vendor folder here. YOu can use that and hopefully your computer will see the teensy over usb and you can just load one of the precompiled firmware .hex files I've provided in the /vendor folder (Fire_Teensy_4_0_XXX.ino.hex).

## Doing things the hard way

ArduinoIDE doesn't really have a portable install. That's ok we can make it work ->

### Halfkay Communication Protocol
>This section, together with the command line source code, serves to document the communication protocol used by HalfKay.

HalfKay uses only 2 very simple commands. Both are USB control transfers which also correspond to a HID output report.
Write Block Command
**To write a 128 byte block, simply send this 130 byte control transfer. The first 2 bytes are the address, which should be aligned to a 128 byte block boundry.**

```c
buf[0] = addr & 255;
buf[1] = (addr >> 8) & 255;
ihex_get_data(addr, 128, buf + 2);
usb_control_msg(handle, 0x21, 9, 0x0200, 0, (char *)buf, 130, first_block ? 3000 : 200);
```

**HalfKay** always preforms a complete erase of all blocks when it receives the first write command. A timeout of 3 seconds should be allowed during the first write, through the erase operation typically takes less than 1 second. All other writes can use a timeout of 0.2 seconds.
Reboot Command
**To reboot the AVR processor, simply send HalfKay a write command to address 0xFFFF. The 128 data bytes are ignored.**

```c
buf[0] = 0xFF;
buf[1] = 0xFF;
memset(buf + 2, 0, sizeof(buf) - 2);
usb_control_msg(handle, 0x21, 9, 0x0200, 0, (char *)buf, 130, 200);
```

On Linux and Macintosh, this command will typically return a failure result, because the AVR processor executes the command very rapidly and the operating system quickly detects the device is no longer attached. On Windows, the return result is less consistent. 

## Linux Teensy udev rules
> I put this in the vendor folder

```
# UDEV Rules for Teensy boards, http://www.pjrc.com/teensy/
#
# The latest version of this file may be found at:
#   http://www.pjrc.com/teensy/00-teensy.rules
#
# This file must be placed at:
#
# /etc/udev/rules.d/00-teensy.rules    (preferred location)
#   or
# /lib/udev/rules.d/00-teensy.rules    (req'd on some broken systems)
#
# To install, type this command in a terminal:
#   sudo cp 00-teensy.rules /etc/udev/rules.d/00-teensy.rules
#
# After this file is installed, physically unplug and reconnect Teensy.
#
ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", ENV{ID_MM_DEVICE_IGNORE}="1", ENV{ID_MM_PORT_IGNORE}="1"
ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04[789a]*", ENV{MTP_NO_PROBE}="1"
KERNEL=="ttyACM*", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666", RUN:="/bin/stty -F /dev/%k raw -echo"
KERNEL=="hidraw*", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666"
KERNEL=="hidraw*", ATTRS{idVendor}=="1fc9", ATTRS{idProduct}=="013*", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="1fc9", ATTRS{idProduct}=="013*", MODE:="0666"

#
# If you share your linux system with other users, or just don't like the
# idea of write permission for everybody, you can replace MODE:="0666" with
# OWNER:="yourusername" to create the device owned by you, or with
# GROUP:="somegroupname" and mange access using standard unix groups.
#
# ModemManager tends to interfere with USB Serial devices like Teensy.
# Problems manifest as the Arduino Serial Monitor missing some incoming
# data, and "Unable to open /dev/ttyACM0 for reboot request" when
# uploading.  If you experience these problems, disable or remove
# ModemManager from your system.  If you must use a modem, perhaps
# try disabling the "MM_FILTER_RULE_TTY_ACM_INTERFACE" ModemManager
# rule.  Changing ModemManager's filter policy from "strict" to "default"
# may also help.  But if you don't use a modem, completely removing
# the troublesome ModemManager is the most effective solution.
```

## Teensy Loader CLI
make sure to grab the source code for the cli from my fork either in the submodule in this directory by cloning it and recurse or from the cloning `git@github.com:CompewterTutor/teensy_loader_cli.git`

Install build tools on linux
```bash
sudo apt install -y libusb-dev make build-essental gcc
```
edit the makefile to select your operating system, for linux it's already set, for windows, comment out line 1 and uncomment line 2

then just 
```bash
make
```

once built, you can just open up a bash terminal and:
```bash
teensy_loader_cli --mcu=TEENSY40 -w ./Fire/Fire.hex
```
## UBUNTU GUI
```
cd vendor
tar -xvzf teensy_linux64.tar.gz
sudo cp ./00-teensy.rules /etc/udev/rules.d/00-teensy.rules
```

