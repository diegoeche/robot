# Truly, the Beaglebone Robot

This repository contains my pet-project in robotics using
the [Beaglebone Blue](https://github.com/beagleboard/beaglebone-blue).

<img width=50% src=/Truly.jpeg ></img>

It contains the client/server code for the rover and the design files for the custom
3d printed parts.

# Construction:

## List of Parts:

- Beaglebone Blue: [Official Website](https://beagleboard.org/blue)
- Camera Platform:
  [Aliexpress](https://www.aliexpress.com/item/Hobbywing-LED-Program-Card-For-FlyFun-SkyWalker-Brushless-ESC-Airplane-Heli-Free-Shipping/32508992281.html?spm=2114.search0104.3.8.JQo0ji&ws_ab_test=searchweb0_0,searchweb201602_4_10152_10065_5000015_10151_10344_10068_10345_10342_10547_10343_51102_10322_10340_10341_10548_10193_5130015_10609_10541_10084_10083_10307_10303_10302_10610_5870020_5080015_10312_10059_10313_10314_10184_10534_100031_10319_10604_10603_10103_10605_10594_5060015_10142_10107,searchweb201603_12,ppcSwitch_5&algo_expid=ebc1bd14-39c4-4f6d-86a6-339a5d2b482a-1&algo_pvid=ebc1bd14-39c4-4f6d-86a6-339a5d2b482a&rmStoreLevelAB=5) (includes
  the 2 servos)
- Camera: [Logitech C270](https://www.amazon.de/Logitech-C270-Webcam-720p-schwarz/dp/B01BGBJ8Y0/ref=sr_1_1?ie=UTF8&qid=1513867536&sr=8-1&keywords=logitech+c270)

## Custom Parts:

TBD

# Code

## Server

The code lives in the root directory of this repository. `scp` the C files to your
beaglebone and call `build.sh`.

To start it you'd need to do: `sudo ./robot`

## Client

The code for the client lives in `/Viewer` and assumes the build system has access to
SFML.

To build it just do `make clean && make`.

# Demo:

TBD
