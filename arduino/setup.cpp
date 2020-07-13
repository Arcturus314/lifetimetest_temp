// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020

#include "definitions.h"

void LEDSetup() {
    for (int i = 0; i < LED_NUMLEDS; i++) {pinMode(LEDPins[i], OUTPUT);}
}

void tempSetup() {
    pinMode(digitalHeaterPin, INPUT);
    pinMode(heaterPin, OUTPUT);
}
