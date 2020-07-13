// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020

#include "definitions.h"


float fetchTemp() {
    delay(TEMP_DELAYBETWEENMEAS);
    float analogSum = 0;
    for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
        analogSum += analogRead(tempPin);
        delay(TEMP_DELAYBETWEENMEAS);
    }
    float analogVal = analogSum / (float) TEMP_NUMTOAVERAGE;
    float voltage = 5.0*analogVal/1023.0;
    float correctedTemp = (voltage - 0.5) / 0.01;
    return correctedTemp;
}


int setTempIter(int desTemp) {
    // Brings package to a specific desired temperature
    // returns 0 if not yet at temperature
    // returns 1 if at temperature within margin of error
    currTemp = fetchTemp();
    if ((int) currTemp < desTemp - ALLOWEDERR) {
        int correction = (int) (desTemp - currTemp) * TEMP_PROPGAIN;
        if (correction > 255) {correction = 255;}
        analogWrite(heaterPin, correction);
        return 0;
    }
    return 1;
}

void setTemp(int desTemp) {
    int tempStatus = setTempIter(desTemp);
    while (tempStatus == 0) {
        tempStatus = setTempIter(desTemp);
    }
}
