# mktwo-badge
All code for the *mktwo* ESP8266/WS2812 wifi-enabled badge with [Arduino]-compatible pin-outs is made by [All Hands Active], a 501(c)(3) not-for-profit [hackerspace] located in Ann Arbor, MI, USA.  The latest documentation can be found on our [Badge Wiki page].  This is an *Open Hardware* and *Open Source* project.

As any good hackerspace should, we encourage people to fully tinker with our code and designs, and to find novel uses for this project.  We'd love to hear of your projects using our badge -- please get in contact with us if you'd like to share your project!

### **Requirements**
#### **Hardware**
The safest and more reliable way to load firmware onto your *mktwo* is with a USB-serial programming cable.  You will need to use an FTDI cable, like the [FTDI cable here], *OR* you can use a cheaper cable with the Prolific *PL2303HX* chipset (look on eBay! We were able to easily find them from US sellers for under $5).  A serial voltage level of 3.3v for programming is recommended, but we've safely done it with 5v, too.  **NOTE:** Windows/Apple users, you will likely have to download a driver to use the *PL2303HX*.  Sorry, we can't provide support on how to do that.

Such cables hook up to the FTDI pin block near the bottom of the *mktwo*.  Take care when hooking up the cable, because the FTDI pins on the badge are not keyed.  When you plug it into the badge, make sure the black wire on the FTDI cable faces toward the left side of the badge (assuming you are viewing the front of the badge).  In case your cable doesn't have a single plug, but a bunch of individual wires (like the Prolific ones), the pinout should be as follows, assuming the board is facing up toward you (*NC = No Connection*):

**BLACK NC NC GREEN WHITE NC**

The red wire from the programming cable should not be connected, which also means you will need to power the badge through USB or via battery when programming it.

#### **Software**
We recommend a current version of the [Arduino IDE], either binary or git clone, as the *Library Manager* and *Board Manager* UI are a more recent feature, and make setup much easier.
Under Arduino's preferences, *Additional Boards Manager URLs*, you will need to add:

`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

Under *Tools|Board:|Boards Manager...* search for and install `esp8266`.
**NOTE: you will need to add the URL above in preferences first.**
To run the demo code, you will also need some libraries.  Under *Sketch|Include Library|Manage Libraries...* search for and make sure you have `ESP8266Wifi` (this should pull in most of the dependencies), `Adafruit NeoPixel`, and `WiFiManager`.  If you have trouble compiling the code and seem to be missing a library, check out and install missing ones from the list at the [ESP8266 Arduino libraries reference].

Now restart the Arduino IDE.

### **Badge Programming**
#### **Wired Programming**
##### **SETUP**

Under the Arduino IDE settings:

Board: "NodeMCU 1.0 (ESP-12E Module)"
CPU Frequency: "80 MHz"
Flash Size: "4M (3M SPIFFS)"
Upload Speed: "115200"
Port: (whatever port the programming cable is on.  In Linux, it'll be something like `/dev/ttyUSB0`)
Programmer: "AVRISP mkII"

##### **PROCEDURE**

Grab the demo code (the .ino file), and copy/paste it into a new Arduino sketch.  You will be replacing the entire contents of this new file (i.e. the setup() and loop() blocks).

To upload your code, you need to put the badge into "write" or "programming" mode.  To do this, locate the `PROGRAM` and `RESET` buttons on the badge.  Hold the `PROGRAM` button. Press and release the `RESET` button. You may now release `PROGRAM`. **You will need to repeat this procedure before each firmware upload.**
You may now upload as you would with any Arduino, using the Arduino IDE.

**NOTE: Some FTDI cables might not cause the ESP to reset after successful programming.  If you are observing this behavior, just use the** `RESET` **button on the badge.**

#### **Wireless Programming**
Also known as OTA (or *Over The Air*) programming.  Once you have our demo code loaded using the **Wired Programming** instructions above, you will no longer need to use the programming cable to load new code onto your *mktwo*.  **NOTE: You will also need Avahi, Bonjour, or similar running on the computer you will be using to do the programming!**

##### **PROCEDURE**
The interface to program the badge is a little rudimentary (feel free to improve upon!).  It works like this:

Step 1. Associate the *mktwo* with an AP you can access.

Step 2. Send code to the *mktwo* on that AP's network, using the *mktwo*'s hosted code upload webpage.

To break it down further:

**Step 1.** When the badge is powered on, you will see it flash green.  Hold down `PROGRAM` until it turns red, and then purple.  Your badge will clear out any existing AP credentials, and will be running as an access point you can connect to.  By default, it will show up as an AP named something like *ESP8266-(some ID)-webupdater*.  **NOTE: Make note of this AP name! You will need to connect to a similarly-named URL in the next step.**

Connect to this AP, and then visit in your browser the URL: `http://192.168.4.1`.  You will get a webpage that directs you to scan for networks.  Choose the "Configure WiFi" button, and you will see a list of APs seen by your *mktwo*.  Choose your network, and enter the password (if needed).  The *mktwo* will save the credentials, and if everything worked, will start flashing blue.

**NOTE: if you take too long to complete this step, the** *mktwo* **will reset itself.  Just start from the beginning, and try to be quicker this time!**

**Step 2.** If you are starting up the *mktwo*, wait until the LEDs turn orange, then hold down the `PROGRAM` button until the LEDs turn blue.  If you just finished Step 1, you are already here! Make sure you are connected to the same AP as your *mktwo*, then visit the URL you recorded from the last step, in the format: `http://esp8266-(some ID)-webupdater.local/update`.  **NOTE: make sure you have the http:// before the URL, otherwise the browser will think you are trying to do a search.**  You can also use the IP assigned to the *mktwo* by your AP/router, if you know it (or can get it).  Try this if the *.local* address doesn't seem to be resolving.

Compile your code using the *Verify* button.  If it completes successfully, go ahead and click on *Sketch|Export compiled Binary*.  Once that's finished, you should have a .bin file in your code directory.  You can easily find this directory by using the *Sketch|Show Sketch Folder* command.

Now go back to the *mktwo*'s upload code webpage in your browser.  Click *Choose File*, navigate to the .bin file you created, and then click *Upload*.  The lights will stop flashing as the code is loaded.  Eventually the page will hopefully update to tell you it has succeeded, and then it will reboot and run your new code.

[All Hands Active]:http://www.allhandsactive.org/
[hackerspace]:http://hackerspaces.org/
[Badge Wiki Page]:http://wiki.allhandsactive.org/Mktwo_Badge
[Arduino]:https://www.arduino.cc
[Arduino IDE]:https://www.arduino.cc/en/Main/Software
[FTDI cable here]:https://www.sparkfun.com/products/9717
[ESP8266 Arduino libraries reference]:https://github.com/esp8266/Arduino/blob/master/doc/libraries.md
