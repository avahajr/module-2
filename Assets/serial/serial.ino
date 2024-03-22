#include <TFT_eSPI.h>
#include <string>

const char delimeter = '\r';

int xyzPins[] = { 26, 27, 25 };
int redLEDPin = 32;

// for the screen (display values from Unity)
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite UI = TFT_eSprite(&tft);


// led info
int ledState = LOW;                   // ledState used to set the LED
unsigned long previousLEDMillis = 0;  // will store last time LED was updated
const long ledInterval = 100;         // ledInterval at which to blink (milliseconds)
bool doFlash = false;

unsigned long previousMessageMillis = 0;
const long messageInterval = 500;

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  UI.createSprite(tft.width(), tft.height());
  UI.fillSprite(TFT_BLACK);

  Serial.begin(115200);
  pinMode(xyzPins[2], INPUT_PULLUP);
  pinMode(redLEDPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  
  int xVal = analogRead(xyzPins[0]);
  int yVal = analogRead(xyzPins[1]);
  int zVal = digitalRead(xyzPins[2]);

  int potentiometer = analogRead(36);

  handleSerialData();
  
  if (currentMillis - previousMessageMillis >= messageInterval) {
    // Unity reads input by line -- send over all values and seperate
    Serial.printf("%d %d %d %d\n", xVal, yVal, zVal, potentiometer);
    previousMessageMillis = currentMillis;
  }

  if (currentMillis - previousLEDMillis >= ledInterval && doFlash) {
    // save the last time you blinked the LED
    previousLEDMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED to the correct state
    digitalWrite(redLEDPin, ledState);
  }
}
bool handleSerialData() {
  // data comes in this form:
  // light_status,throttle,altitude,airspeed\n
  if (Serial.available()) {

    String lightStatus = Serial.readStringUntil(delimeter);
    String throttle = Serial.readStringUntil(delimeter);
    String altitude = Serial.readStringUntil(delimeter);
    String airspeed = Serial.readStringUntil('\n');

    if (lightStatus && throttle && altitude && airspeed) {
      if (lightStatus == "1") {
        doFlash = true;
      }
      else {
        doFlash = false;
      }

      updateHUD(throttle.c_str(), altitude.c_str(), airspeed.c_str());
      return true;
    }
  }
  return false;
}
void updateHUD(const char* throttle, const char* altitude, const char* airspeed) {
  UI.fillSprite(TFT_BLACK);
  // read from the serial connection
  
  UI.setTextColor(TFT_WHITE);
  UI.setTextDatum(4);

  char altString[100];
  char throtString[100];
  char speedString[100];
  std::sprintf(altString, "Altitude: %sm", altitude);
  std::sprintf(throtString, "Throttle: %s%%", throttle);
  std::sprintf(speedString, "Airspeed: %s km/h", airspeed);

  // display on screen
  UI.setTextSize(2);
  UI.drawString(speedString, tft.width() / 2, tft.height() / 2);
  UI.drawString(throtString, tft.width() / 2, tft.height() / 2 - 30);
  UI.drawString(altString, tft.width() / 2, tft.height() / 2 + 30);
  UI.pushSprite(0, 0);
}
