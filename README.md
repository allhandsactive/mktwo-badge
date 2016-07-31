# mktwo-badge
All code for the ESP8266/WS2812 wifi-enabled badge with [Arduino]-compatible pin-outs is made by [All Hands Active], a 501(c)(3) not-for-profit [hackerspace] located in Ann Arbor, MI, USA.  The latest documentation can be found on our [Badge Wiki page].  This is an *Open Hardware* and *Open Source* project.

As any good hackerspace should, we encourage people to fully tinker with our code and designs, and to find novel uses for this project.  We'd love to hear of your projects using our badge -- please get in contact with us if you'd like to share your project!

###**Requirements**
####**Hardware**
You will need to use an FTDI cable, like the [FTDI cable here].

Careful when using such a cable, because the FTDI pins on the badge are not keyed.  When you plug it into the badge, make sure the black wire on the FTDI cable faces toward the left side of the badge (assuming you are viewing the front of the badge).

####**Software**
We recommend a current version of the [Arduino IDE], either binary or git clone, as the *Library Manager* and *Board Manager* UI are a more recent feature, and make setup much easier.
Under Arduino's preferences, *Additional Boards Manager URLs*, you will need to add:

`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

Under *Library Manager* search for and install `Adafruit NeoPixel`.
Under *Board Manager* search for and install `esp8266`.
**NOTE: you will need to add the URL above in preferences first.**

Now restart the Arduino IDE.

###**Badge Programming**
####**Wired Programming**
#####**SETUP**

Under the Arduino IDE settings:

Board: "NodeMCU 1.0 (ESP-12E Module)"
CPU Frequency: "80 MHz"
Flash Size: "4M (3M SPIFFS)"
Upload Speed: "115200"
Port: (whatever the FTDI is on -- in Linux, something like `/dev/ttyUSB0`)
Programmer: "AVRISP mkII"

#####**PROCEDURE**

To upload your code, you need to put the badge into "write" or "programming" mode.  To do this, locate the `PROGRAM` and `RESET` buttons on the badge.  Hold the `PROGRAM` button. Press and release the `RESET` button. You may now release `PROGRAM`. **You will need to repeat this procedure before each firmware upload.**
You may now upload as you would with any Arduino, using the Arduino IDE.

**NOTE: Some FTDI cables might not cause the ESP to reset after successful programming.  If you are observing this behavior, just use the** `RESET` **button on the badge.**

####**Wireless**
TODO

[All Hands Active]:http://www.allhandsactive.org/
[hackerspace]:http://hackerspaces.org/
[Badge Wiki Page]:http://wiki.allhandsactive.org/Mktwo_Badge
[Arduino]:https://www.arduino.cc
[Arduino IDE]:https://www.arduino.cc/en/Main/Software
[FTDI cable here]:https://www.sparkfun.com/products/9717
