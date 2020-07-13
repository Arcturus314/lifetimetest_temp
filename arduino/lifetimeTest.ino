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

void setup() {
    Serial.begin(9600);
    LEDSetup();
    tempSetup();
    setTemp(TEMP_DESTEMP);
}


void loop() {
    changeLEDState(0);
    // sampling temp
    Serial.print(fetchTemp());
    Serial.print(",");
    // sampling LEDs
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
        // turning LED on
        digitalWrite(LEDPins[ledNum], LOW)
        // delaying
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
        // sampling LED voltage


}
