#include <TFT_eSPI.h>
#include <string>

const char delimeter = '\r';

int xyzPins[] = { 26, 27, 25 };
const int redLEDPin = 12;
const int yellowLEDPin = 13;
const int greenLEDPin = 2;

// for the screen (display values from Unity)
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite UI = TFT_eSprite(&tft);


// red led info (needs to flash)
int redLEDState = LOW;                   // ledState used to set the LED
unsigned long previousRedLEDMillis = 0;  // will store last time LED was updated
const long redLEDInterval = 100;         // ledInterval at which to blink (milliseconds)
bool doFlashRed = false;

// other led states
int yellowLEDState = LOW;
int greenLEDState = LOW;

bool isTorndown = false;

unsigned long previousMessageMillis = 0;
const long messageInterval = 250;  // how often to send a message to Unity over the serial connection.

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
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  resetUI();
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

  if (currentMillis - previousRedLEDMillis >= redLEDInterval && doFlashRed) {
    // save the last time you blinked the LED
    previousRedLEDMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (redLEDState == LOW) {
      redLEDState = HIGH;
    } else {
      redLEDState = LOW;
    }
  }
  if (!isTorndown) {
    // set each LED to the correct state
    digitalWrite(redLEDPin, redLEDState);
    digitalWrite(yellowLEDPin, yellowLEDState);
    digitalWrite(greenLEDPin, greenLEDState);
  }
}
bool handleSerialData() {
  // data comes in this form:
  // light_status,throttle,altitude,airspeed\n
  if (Serial.available()) {
    String doTeardown = Serial.readStringUntil(delimeter);
    String lightStatus = Serial.readStringUntil(delimeter);
    String throttle = Serial.readStringUntil(delimeter);
    String altitude = Serial.readStringUntil(delimeter);
    String airspeed = Serial.readStringUntil('\n');
    if (doTeardown == "X") {
      resetUI();
      Serial.flush();
      return false;
    }
    isTorndown = false;
    if (lightStatus && throttle && altitude && airspeed) {
      if (lightStatus == "R") {
        doFlashRed = true;
        greenLEDState = LOW;
        yellowLEDState = LOW;
      } else if (lightStatus == "Y") {
        doFlashRed = false;
        redLEDState = LOW;
        greenLEDState = LOW;
        yellowLEDState = HIGH;
      } else {
        doFlashRed = false;
        redLEDState = LOW;
        greenLEDState = HIGH;
        yellowLEDState = LOW;
      }

      updateHUD(lightStatus.charAt(0), throttle.c_str(), altitude.c_str(), airspeed.c_str());
      return true;
    }
  }
  return false;
}
void updateHUD(char lightStatus, const char* throttle, const char* altitude, const char* airspeed) {
  UI.fillSprite(TFT_BLACK);

  switch (lightStatus) {
    case 'R':
      {
        UI.setTextColor(TFT_RED);
        break;
      }
    case 'Y':
      {
        UI.setTextColor(TFT_YELLOW);
        break;
      }
    case 'G':
      {
        UI.setTextColor(TFT_WHITE);
        break;
      }
    default:
      {
        break;
      }
  }
  UI.setTextDatum(4);

  char altString[100];
  char throtString[100];
  char speedString[100];
  std::sprintf(altString, "Altitude: %sm", altitude);
  std::sprintf(throtString, "Throttle: %s%%", throttle);
  std::sprintf(speedString, "Airspeed: %s km/h", airspeed);

  // display on screen
  UI.setTextSize(2);

  UI.drawString(altString, tft.width() / 2, tft.height() / 2 + 30);

  UI.setTextColor(TFT_WHITE);
  UI.drawString(speedString, tft.width() / 2, tft.height() / 2);
  UI.drawString(throtString, tft.width() / 2, tft.height() / 2 - 30);

  UI.pushSprite(0, 0);
}

void resetUI() {
  isTorndown = true;
  UI.fillSprite(TFT_BLACK);

  // turn off LEDs
  digitalWrite(redLEDPin, LOW);
  digitalWrite(yellowLEDPin, LOW);
  digitalWrite(greenLEDPin, LOW);
  doFlashRed = false;

  UI.setTextSize(2);
  UI.setTextDatum(4);
  UI.drawString("Unity not connected.", tft.width() / 2, tft.height() / 2);

  UI.pushSprite(0, 0);
}
