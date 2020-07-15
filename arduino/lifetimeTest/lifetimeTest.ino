// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020

// DEFINITIONS

// temperature control parameters
#define TEMP_NUMTOAVERAGE 10     // average 10 temp meas
#define TEMP_DELAYBETWEENMEAS 10 // ms
#define TEMP_PROPGAIN 20         // proportional control term with gain of 20
#define TEMP_ALLOWEDERR 3             // accurate within ALLOWEDER degrees
#define TEMP_DESTEMP 80              // desirded temperature for LEDs
// lifetime control parameters
#define LED_LEDON 1000         // num ms to keep LED on during sampling
#define LED_SAMPLEPERIOD 60000 // num ms for total sample period (sample all LEDs -> turn off all LEDs -> wait)
#define LED_NUMLEDS 8          // num LEDs to test
#define LED_NUMSAMPLES 10      // num analog samples to take for each LED

// pinout
int LEDPins[]          = {2,3,4,5,6,7,8,9}; // leds connected to these pins. {CH1, CH2, ...}
#define heaterPin        10        // heater connected to this pin for PWM
#define digitalHeaterPin 21       // heater also connected to this, but this pin doesn't support PWM. So make this pin an analog input
#define PDPin            A0       // photodiode connected here
#define tempPin          A1       // temperature sensor connected here
#define voltPin          A3       // connected to 5V input
#define pwrLED           11       // status LED
#define waitLED          12       // status LED
#define sampleLED        13       // status LED



// calculated constants


// number of times to call setTempIter() for the delay between sample sets
int numTempCycles_delay = 50; //(LED_SAMPLEPERIOD - LED_NUMLEDS * LED_LEDON) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE);
// number of times to call setTempIter() for the delay between samples
int numTempCycles_sample = 1; //(LED_SAMPLEPERIOD) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE);


// SETUP

void LEDSetup() {
    for (int i = 0; i < LED_NUMLEDS; i++) {pinMode(LEDPins[i], OUTPUT);}
}

void tempSetup() {
    pinMode(digitalHeaterPin, INPUT);
    pinMode(heaterPin, OUTPUT);
}

// TEMPCONTROL

float fetchTemp() {
    delay(TEMP_DELAYBETWEENMEAS);
    float analogSum = 0.0;
    for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
        analogSum += analogRead(tempPin);
        delay(TEMP_DELAYBETWEENMEAS);
    }
    float analogVal = (float) analogSum / (float) TEMP_NUMTOAVERAGE;
    float voltage = 5.0*analogVal/1023.0;
    float correctedTemp = (voltage - 0.5) / 0.01;
    return correctedTemp;
}


int setTempIter(int desTemp) {
    // Brings package to a specific desired temperature
    // returns 0 if not yet at temperature
    // returns 1 if at temperature within margin of error
    int currTemp = fetchTemp();
    //Serial.print(currTemp);
    //Serial.print(",");
    if ((int) currTemp < desTemp - TEMP_ALLOWEDERR) {
        int correction = (int) (desTemp - currTemp) * TEMP_PROPGAIN;
        //Serial.println(correction);
        if (correction > 255) {correction = 255;}
        analogWrite(heaterPin, correction);
        if (correction > 20) digitalWrite(pwrLED, HIGH);
        else digitalWrite(pwrLED, LOW);
        return 0;
    }
    else { analogWrite(heaterPin, 0); }
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

// ACTUAL CODE

void changeLEDState(int ledState) {
    // ledState = 0 to turn off
    // ledState = 1 to turn on
    if (ledState == 0) {
        for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {digitalWrite(LEDPins[ledNum], LOW); }
    }
    else {for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {digitalWrite(LEDPins[ledNum], HIGH); }}
}

float sampleIntensity() {
    int voltageSum = 0;
    for (int i = 0; i < TEMP_NUMTOAVERAGE; i++) {
        voltageSum += analogRead(PDPin);
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
    }
    float voltage = (float) voltageSum * 5.0/1023.0;
    return voltage;
}

void setup() {
    Serial.begin(9600);
    LEDSetup();
    tempSetup();
    changeLEDState(1);
    pinMode(pwrLED, OUTPUT);
    pinMode(waitLED, OUTPUT);
    pinMode(sampleLED, OUTPUT);
    digitalWrite(pwrLED, HIGH);
    delay(500);
    digitalWrite(waitLED, HIGH);
    delay(500);
    digitalWrite(sampleLED, HIGH);
    delay(1000);
    digitalWrite(pwrLED, LOW);
    digitalWrite(waitLED, LOW);
    digitalWrite(sampleLED, LOW);
    changeLEDState(0);
    setTemp(TEMP_DESTEMP);
}


void loop() {
    changeLEDState(0);
    // sampling temp
    digitalWrite(sampleLED, HIGH);
    Serial.print(String(fetchTemp(), 4));
    Serial.print(",");
    // taking calibration PD intensity
    Serial.print(String(sampleIntensity(), 4));
    Serial.print(",");
    // taking calibration 5V voltage
    Serial.print(String((float) analogRead(voltPin) * 5.0 / 1023, 4));
    Serial.print(",");
    // sampling LEDs
    for (int ledNum = 0; ledNum < LED_NUMLEDS; ledNum++) {
        // turning LED on
        digitalWrite(LEDPins[ledNum], HIGH);
        // delaying
        for (int i = 0; i < numTempCycles_sample; i++) { setTempIter(TEMP_DESTEMP); }
        // sampling LED voltage
        Serial.print(String(sampleIntensity(), 4));
        Serial.print(",");
        // turning LED off
        digitalWrite(LEDPins[ledNum], LOW);
    }
    Serial.println();
    changeLEDState(1);
    // delaying
    digitalWrite(sampleLED, LOW);
    digitalWrite(waitLED, HIGH);
    for (int i = 0; i < numTempCycles_delay; i++) { setTempIter(TEMP_DESTEMP); }
    digitalWrite(waitLED, LOW);
}
