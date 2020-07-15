// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020

#include "setup.cpp"
#include "tempControl.cpp"

void changeLEDState(int ledState) {
    // ledState = 0 to turn off
    // ledState = 1 to turn on
    if (ledState == 0) {
        for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {digitalWrite(LEDPins[ledNum], HIGH); }
    }
    else {for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {digitalWrite(LEDPins[ledNum], LOW); }}
}

float sampleIntensity() {
    int voltageSum = 0;
    for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
        voltageSum += analogRead(A0);
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
    }
    float voltage = (float) voltageSum * 5.0/1023.0;
    return voltage;
}

void setup() {
    Serial.begin(9600);
    LEDSetup();
    tempSetup();
    setTemp(TEMP_DESTEMP);
}


void loop() {
    changeLEDState(0);
    // sampling temp
    Serial.print(String(fetchTemp(), 4));
    Serial.print(",");
    // taking calibration PD intensity
    Serial.print(String(sampleIntensity(), 4));
    Serial.print(",");
    // taking calibration 5V voltage
    Serial.print(String(analogRead(A3), 4))
    // sampling LEDs
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
        // turning LED on
        digitalWrite(LEDPins[ledNum], HIGH);
        // delaying
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
        // sampling LED voltage
        Serial.print(String(sampleIntensity(), 4));
        Serial.print(",")
        // turning LED off
        digitalWrite(LEDPins[ledNum], LOW);
    // delaying
    for (int i = 0; i < numTempCycles_delay; i++) { setTempIter(TEMP_DESTEMP); }
}
