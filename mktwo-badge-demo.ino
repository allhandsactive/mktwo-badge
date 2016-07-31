#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 13

//The number of times we try to connect to the specified network before we give up and reboot.
#define WL_RETRY_COUNT 5

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, PIN, NEO_GRB + NEO_KHZ800);


const char* host = "esp8266-%06x-webupdater";
//The longest string size we can use for the hostname.
#define MAX_HOST_NAME_LEN 26

const char* ssid = "TEST";
const char* password = "TEST";

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
  pinMode(0, INPUT_PULLUP);
}

void loop() {
  //When the program first starts up, we must check if the user wishes to enter OTA upload mode.
  if(bootup == true) {
    //Give the user a few moments to hold down the program button.
    delay(2000);

    //User is holding down the program button, so go into OTA upload mode.
    if(digitalRead(0) == LOW) {
      otaUpload();
    }
    
    //If we fall through to here, the program button isn't being held down, and we will just run the normal code.
    //Never return to this code again, until the user resets the device.
    bootup = false;
    Serial.println("Skipping OTA upload...");
  }
  
  //Brightness can be 0-255
  //0 off, 1-2 produce slight odd behavor due to low voltage
  //10 is a decent setting of battery life
  //Over 15 will likely be painful for others' eyes.
  setBrightness(10, 0);
 
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(127, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 127, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 127), 50); // Blue
  colorWipe(strip.Color(255, 255, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  //Demo code
  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
  Serial.println("Demo code cycled");
}

//This function handles the process of waiting for an OTA upload to happen at bootup.
void otaUpload(void) {
  Serial.println("Entering OTA upload mode!");
  //Start up WiFi
  WiFi.mode(WIFI_AP_STA);
 
  //Check for a good connection.
  int retryCount = 0;

  solidColor(strip.Color(0, 50, 0), 0);
  setBrightness(50, 0);
  Serial.println("Connecting to network...");
  WiFi.begin(ssid, password);
 
  while(WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    //Show LED/Serial heartbeat if we are having problems connecting.
    setBrightness(100, 2000);
    Serial.println("Connection Failed! Retrying...");
    setBrightness(50, 2000);
    
    if(retryCount == WL_RETRY_COUNT) {
      //Reboot if we can't get a connection.
      //Might be something funky going on with the state of the hardware?
      //TODO: Have more grace with this in the future?
      ESP.restart();
    }

    retryCount++;
  }

  //Begin upload handling.
  //Might cut off characters if the constant string is too long.
  char myHost[MAX_HOST_NAME_LEN];
  snprintf(myHost, sizeof(myHost), host, ESP.getChipId());
 
  MDNS.begin(myHost);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
 
  Serial.print("ESP IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", myHost);
 
  Serial.println("Waiting for upload...");
  solidColor(strip.Color(0, 0, 50), 0);
  //We just wait in here very patiently until the user sends us something. The only way out is to reset.
  while(true) {
    //Set up an LED/Serial "heartbeat" status to let user know we are waiting for data in OTA upload mode.
    setBrightness(50, 1000);
    Serial.print(" ?");
    httpServer.handleClient();
    setBrightness(255, 1000);
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
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) &
255));
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

