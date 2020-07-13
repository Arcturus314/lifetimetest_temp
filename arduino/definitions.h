// Kaveh Pezeshki
// kaveh@avicena.tech
// 6-11-2020


// temperature control parameters
#define TEMP_NUMTOAVERAGE 10     // average 10 temp meas
#define TEMP_DELAYBETWEENMEAS 10 // ms
#define TEMP_PROPGAIN 20         // proportional control term with gain of 20
#define TEMP_ALLOWEDERR 3             // accurate within ALLOWEDER degrees
#define TEMP_DESTEMP 100              // desirded temperature for LEDs
// lifetime control parameters
#define LED_LEDON 1000         // num ms to keep LED on during sampling
#define LED_SAMPLEPERIOD 60000 // num ms for total sample period (sample all LEDs -> turn off all LEDs -> wait)
#define LED_NUMLEDS 3          // num LEDs to test
#define LED_NUMSAMPLES 10      // num analog samples to take for each LED

// pinout
int LEDPins[]        = {9,7,5}; // leds connected to these pins. {CH1, CH2, ...}
int heaterPin        = 6;       // heater connected to this pin for PWM
int digitalHeaterPin = 21;      // heater also connected to this, but this pin doesn't support PWM. So make this pin an analog input
int PDPin            = A0;      // photodiode connected here
int tempPin          = A1;      // temperature sensor connected here


// calculated constants


// number of times to call setTempIter() for the delay between sample sets
int numTempCycles_delay = (LED_SAMPLEPERIOD - LED_NUMLEDS * LED_LEDON) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE)
// number of times to call setTempIter() for the delay between samples
int numTempCycles_sample = (LED_SAMPLEPERIOD) / (TEMP_DELAYBETWEENMEAS * TEMP_NUMTOAVERAGE)


