#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "WiFiManager.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//NOTE: To put the badge into program mode (for use when programming via FTDI cable), first hold down "program", then push "reset".
//Once the board resets, you may let go of the program button.  It should now be ready to receive a program over its FTDI pins.

#define LED_DATA 13

//The pin for our "program" button, which does double duty as a simple method of user input.
#define BUTTON 0

//HOWTO: Battery check
//#define BATT_CHECK_PIN 20
//Turn batt_check pin high
//Read ADC (A0)
//Divide that value by 243.6 for voltage.
//Turn batt_check pin low

//The number of seconds we wait in the AP config portal mode before giving up and rebooting.
#define PORTAL_TIMEOUT 180

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, LED_DATA, NEO_GRB + NEO_KHZ800);

//This is the format for the badge's hostname.  Change this if you want to make it easier to identify.
//The %06x part will be filled in with the ESP8266's unique ID.
const char* host = "esp8266-%06x-webupdater";
//The longest string size we can use for the hostname.
#define MAX_HOST_NAME_LEN 26

//Keep track of the initial bootup period.  We might want to enter the OTA upload mode.
bool bootup = true;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  //Set up the pin for LED use.
  strip.begin();
  //Should clear out any colors that might be stuck on at startup. 
  strip.show();
  
  //Allows us to use the "program" (top) button in our program.
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  //When the program first starts up, we must check if the user wishes to enter OTA upload mode.
  if(bootup == true) {
    solidColor(strip.Color(0, 50, 0), 0);
    //Give the user a few moments to hold down the program button.
    setBrightness(50, 1000);
    setBrightness(100, 1000);

    //If the user holds down the programming button very close to startup, reset the WiFi connection settings.
    if(digitalRead(BUTTON) == LOW) {
      solidColor(strip.Color(100, 0, 0), 0);
      //Although this appears to be the correct way to forget association settings in the WiFiManager API, it looks like a stub function currently.
      //WiFiManager wifiManager;
      //wifiManager.resetSettings();
      //This is another way to forget association settings.
      Serial.println("Forgetting saved AP association settings!");
      WiFi.disconnect();
      //ESP.restart();
      //delay(1000);
      //Presumably from here we will want to go into the AP setup mode.
      otaUpload();
    }

    solidColor(strip.Color(50, 50, 0), 0);
    //Wait a little longer before possibly going into the OTA update stuff.
    setBrightness(50, 2000);
    setBrightness(100, 2000);

    //User is holding down the program button after a few seconds of startup, so go into OTA upload mode.
    if(digitalRead(BUTTON) == LOW) {
      otaUpload();
    }
    
    //If we fall through to here, the program button isn't being held down at any point, and we will just run the normal code.
    //Never return to this code again, until the user resets the device.
    bootup = false;
    Serial.println("Skipping OTA upload...");
  }


  //NOTE: Here's where the code for the main program should start.  Go hog wild!
  setBrightness(0, 0);
  strip.show(); 
  setBrightness(20, 0);
  
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(127, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 127, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 127), 50); // Blue
  colorWipe(strip.Color(255, 255, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

//This function gets called when the AP configuration mode is started.
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entering AP association config mode");
  Serial.println(WiFi.softAPIP());
  //Debug print out the AP name used by the badge.
  Serial.println(myWiFiManager->getConfigPortalSSID());
  solidColor(strip.Color(50, 0, 50), 0);
  setBrightness(50, 2000);
  setBrightness(100, 2000);
}

//This function handles the process of waiting for an OTA upload to happen at bootup.
void otaUpload(void) {
  Serial.println("Entering OTA upload mode!");
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT);
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //Might cut off characters if the constant string is too long.
  char myHost[MAX_HOST_NAME_LEN];
  snprintf(myHost, sizeof(myHost), host, ESP.getChipId()); 

  //This will first try to connect using the last-configured SSID and password
  //If it fails to connect, it starts an access point with the specified name (myHost) and optional password (the commented-out part)
  //It then goes into a blocking loop awaiting configuration until it times out (in PORTAL_TIMEOUT seconds).
  //HINT: Change the hostname to make it easy to find in a room full of these things.
  //HINT #2: This DOES NOT work AT ALL when connecting to a network that uses an authentication gateway!
  if(!wifiManager.autoConnect(myHost /*, "<optional-ap-password-for-your-badge>"*/)) {
    Serial.println("Failed to associate with AP, and exceeded timeout!");
    //Nothing happened before our timeout period -- reset
    solidColor(strip.Color(100, 0, 0), 0);
    setBrightness(50, 2000);
    setBrightness(100, 2000);
    ESP.reset();
    delay(1000);
  }
  
  //Begin upload handling.
  //Also broadcast this board's hostname over multicast DNS so it's easier to find.
  MDNS.begin(myHost);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  
  Serial.print("ESP IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", myHost);
  
  Serial.println("Waiting for upload...");
  solidColor(strip.Color(0, 0, 50), 0);

  //We just wait in here very patiently until the user sends us something.  The only way out is to reset.
  while(true) {
    //HINT: If the board is resetting here, you might need to supply it with more power, e.g. use a 2 amp USB charger, or plug in the battery.
    //Set up an LED/Serial "heartbeat" status to let user know we are waiting for data in OTA upload mode.
    setBrightness(50, 1000);
    Serial.print(" ?");
    httpServer.handleClient();
    setBrightness(100, 1000);
  }
}

void solidColor(uint32_t c, int wait) {
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setBrightness(uint16_t b, int wait) {
  strip.setBrightness(b);
  strip.show();
  delay(wait);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
