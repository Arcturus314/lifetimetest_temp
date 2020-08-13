// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// DEFINITIONS

// temperature control parameters
#define TEMP_NUMTOAVERAGE 10     // average 10 temp meas
#define TEMP_DELAYBETWEENMEAS 10 // ms
#define TEMP_PROPGAIN 40         // proportional control term with gain of 20
#define TEMP_ALLOWEDERR 3             // accurate within ALLOWEDER degrees
#define TEMP_DESTEMP 103              // desirded temperature for LEDs
// lifetime control parameters
#define LED_LEDON 1000         // num ms to keep LED on during sampling
#define LED_SAMPLEPERIOD 60000 // num ms for total sample period (sample all LEDs -> turn off all LEDs -> wait)
#define LED_NUMLEDS 8          // num LEDs to test
#define LED_NUMSAMPLES 10      // num analog samples to take for each LED

// pinoute
const int LEDPins[] = {2, 3, 4, 5, 6, 7, 8, 9}; // leds connected to these pins. {CH1, CH2, ...}
#define heaterPin        30       // heater connected to this pin for PWM
#define PDPin            A0       // photodiode connected here
#define tempPin          A1       // temperature sensor connected here
#define v10mon           A2       // connected to 10V input, multiply by 4
#define v5mon            A3       // connected to 5V input, multiply by 2
#define v33mon           A4       // connected to 3.3V input, multiply by 1.5
#define runLED           24       // status LED
#define heatLED          25       // status LED
#define waitLED          26       // status LED
#define sampleLED        27       // status LED
#define cromeOnSw        28       // control switch
#define startSw          29       // control switch

// number of times to call setTempIter() for the delay between sample sets
int numTempCycles_delay = 1000;//00; //(LED_SAMPLEPERIOD - LED_NUMLEDS * LED_LEDON) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE);
// number of times to call setTempIter() for the delay between samples
int numTempCycles_sample = 1; //(LED_SAMPLEPERIOD) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE);


RTC_DS3231 rtc;
// general debug info available over serial

const int sd_chipSelect = BUILTIN_SDCARD;

char filename[9] = "log0.csv";

void setupFilename() {
  // searching for empty file
  int filenameIndex = 0;
  while (SD.exists(filename)) {
    filenameIndex++;
    Serial.print("File exists ");
    Serial.println(filename);
    String filenamePrefix = "log";
    String filenameString = filenamePrefix.concat(filenameIndex);
    filenameString = filenameString.concat(".csv");
    filenameString.toCharArray(filename, 9);
  }
  Serial.print("Using filename ");
  Serial.println(filename);
  return;
}

void LEDSetup() {
  for (int i = 0; i < LED_NUMLEDS; i++) {
    pinMode(LEDPins[i], OUTPUT);
  }
}

float fetchTemp() {
  delay(TEMP_DELAYBETWEENMEAS);
  float analogSum = 0.0;
  for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
    analogSum += analogRead(tempPin);
    delay(TEMP_DELAYBETWEENMEAS);
  }
  float analogVal = (float) analogSum / (float) TEMP_NUMTOAVERAGE;
  float voltage = 3.3 * analogVal / 1023.0;
  float correctedTemp = (voltage - 0.5) / 0.01;
  return correctedTemp;
}

int setTempIter(int desTemp) {
  // Brings package to a specific desired temperature
  // returns 0 if not yet at temperature
  // returns 1 if at temperature within margin of error
  int currTemp = fetchTemp();
  //Serial.println(currTemp);
  //Serial.print(",");
  if ((int) currTemp < desTemp - TEMP_ALLOWEDERR) {
    int correction = (int) (desTemp - currTemp) * TEMP_PROPGAIN;
    //Serial.println(correction);
    if (correction > 255) {
      correction = 255;
    }
    analogWrite(heaterPin, correction);
    if (correction > 20) digitalWrite(heatLED, HIGH);
    else digitalWrite(heatLED, LOW);
    return 0;
  }
  else {
    analogWrite(heaterPin, 0);
  }
  //Serial.println("ok");
  return 1;
}

void setTemp(int desTemp) {
  int tempStatus = setTempIter(desTemp);
  while (tempStatus == 0) {
    //Serial.println("setting initial temp, start");
    tempStatus = setTempIter(desTemp);
    //Serial.print("setting initial temp, done: ");
    //Serial.println(tempStatus);
  }
}

void changeLEDState(int ledState) {
  // ledState = 0 to turn off
  // ledState = 1 to turn on
  if (ledState == 0) {
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
      digitalWrite(LEDPins[ledNum], LOW);
    }
  }
  else {
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
      digitalWrite(LEDPins[ledNum], HIGH);
    }
  }
}

float sampleIntensity() {
  int voltageSum = 0;
  for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
    voltageSum += analogRead(PDPin);
    for (int i = 0; i < numTempCycles_sample; i++) {
      setTempIter(TEMP_DESTEMP);
    }
  }
  float voltage = (float) voltageSum * 3.3 / (1023.0 * (float) TEMP_NUMTOAVERAGE);
  return voltage;
}


void printToFile(int unixTime, float temp, float PDCal, float v10cal, float v5cal, float v33cal, float crome[]) {
  File datafile = SD.open(filename, FILE_WRITE);
  datafile.print(unixTime);
  datafile.print(",");
  datafile.print(String(temp, 4));
  datafile.print(",");

  datafile.print(String(PDCal, 4));
  datafile.print(",");

  datafile.print(String(v10cal, 4));
  datafile.print(",");

  datafile.print(String(v5cal, 4));
  datafile.print(",");

  datafile.print(String(v33cal, 4));
  datafile.print(",");

  for (int i = 0; i < LED_NUMLEDS; i++) {
    datafile.print(String(crome[i], 4));
    datafile.print(",");

  }
  datafile.print("\n");
  datafile.close();

}

void setup() {
  // port declarations
  pinMode(runLED, OUTPUT);
  pinMode(heatLED, OUTPUT);
  pinMode(waitLED, OUTPUT);
  pinMode(sampleLED, OUTPUT);
  pinMode(cromeOnSw, INPUT);
  pinMode(startSw, INPUT);

  pinMode(heaterPin, OUTPUT);
  LEDSetup();

  // turn on RUN led while waiting for setup
  digitalWrite(runLED, HIGH);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Avicena Burn-In V0.4");
  // initializing SD card
  if (!SD.begin(sd_chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("SD Card initialized.");

  // initializing RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  Serial.println("RTC initialized.");

  DateTime now = rtc.now();
  Serial.print("Current Date and Time ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  setupFilename();

  digitalWrite(runLED, LOW);

  digitalWrite(heatLED, HIGH);
  delay(500);
  digitalWrite(waitLED, HIGH);
  delay(500);
  digitalWrite(sampleLED, HIGH);
  delay(1000);
  digitalWrite(heatLED, LOW);
  digitalWrite(waitLED, LOW);
  digitalWrite(sampleLED, LOW);


  Serial.println("Ready to sample");
  // first step: if cromeOnSw turns LOW, then turn CROMEs on. if startSw is ever pressed, then proceed to loop
  int cromeStatus = 0;
  while (digitalRead(startSw) == 1) {
    Serial.print("PD Voltage: ");
    //Serial.println(analogRead(A0));
    Serial.println((float) analogRead(PDPin) * 3.3 / (1023.0));
    delay(50);
    if (digitalRead(cromeOnSw) == 0) { 
      if (cromeStatus == 0) cromeStatus = 1;
      else cromeStatus = 0;
      changeLEDState(cromeStatus);
      digitalWrite(sampleLED, cromeStatus);
      delay(50);
      while(digitalRead(cromeOnSw) == 0);
    }
  }
  
  digitalWrite(sampleLED, LOW);
  changeLEDState(0);
  Serial.println("Starting temperature control and sampling...");
  File datafile = SD.open(filename, FILE_WRITE);
  datafile.print("Unix time (s),Temp (C),PD Offset (V),10V Rail Voltage(V),5V Rail Voltage (V),3.3V Rail Voltage (V),");
  for (int i = 0; i < LED_NUMLEDS; i++) {
    datafile.print("Crome ");
    datafile.print(i);
    datafile.print(" Voltage (V),");
  }
  datafile.print("\n");
  datafile.close();
  digitalWrite(runLED, HIGH);
  setTemp(TEMP_DESTEMP);
}

void loop() {
    changeLEDState(0);
    digitalWrite(sampleLED, HIGH);

    // fetching time
    DateTime now = rtc.now();
    int currTime = now.unixtime();

    // sampling temp
    float temp = fetchTemp();
    
    // taking calibration PD intensity
    float pdCal = sampleIntensity();

    // taking calibration voltages
    float v10cal = (float) analogRead(v10mon) * 3.3 / 1023 * 4.6;
    float v5cal = (float) analogRead(v5mon) * 3.3 / 1023 * 2.0;
    float v33cal = (float) analogRead(v33mon) * 3.3 / 1023 * 3.0;
    
    // sampling LEDs
    float ledArr[LED_NUMLEDS];
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
        // turning LED on
        digitalWrite(LEDPins[ledNum], HIGH);
        // delaying
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
        // sampling LED voltage
        ledArr[ledNum] = sampleIntensity();
        // turning LED off
        digitalWrite(LEDPins[ledNum], LOW);
    }
    changeLEDState(1);

    // writing to file
    printToFile(currTime, temp, pdCal, v10cal, v5cal, v33cal, ledArr);
    
    // delaying
    digitalWrite(sampleLED, LOW);
    digitalWrite(waitLED, HIGH);
    for (int i = 0; i < numTempCycles_delay; i++) { setTempIter(TEMP_DESTEMP); }
    digitalWrite(waitLED, LOW);
}
