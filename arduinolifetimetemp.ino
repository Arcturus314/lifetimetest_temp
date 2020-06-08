#define LEDON 1       // each led is on for this many seconds during sampling
#define SAMPLETIME 60 // total time for one sample - delay iteration
#define NUMLEDS 8     // will break if >11
#define NUMSAMPLES 10 // number of voltage samples averaged together for each LED measurement

#define TARGETTEMP 100   // desired temperature for testing
#define ALLOWEDTEMPVAR 1 // can deviate from target temp by this amount

// 10 LEDs on pins 2 -> NUMLEDS+2
int photodiodePin = A0;
int tempPin = A1;
int heaterPin = 13;
long waittimehundredths    = ((long)SAMPLETIME - (long)NUMLEDS * (long)LEDON)*(long)100;

// we run temperature management constantly, 100 times a second, and every waittimehundredths cycles we record LED status

void setup() {
  // put your setup code here, to run once:
  for (int i = 2; i < NUMLEDS+2;i++) pinMode(i, OUTPUT); // LED control pins
  pinMode(heaterPin, OUTPUT); // heater control pin
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  // printing temperature
  Serial.print(String(fetchTemp(), 4));

  // turning LEDs off
  for (int lednum = 0; lednum < NUMLEDS; lednum++) {
    digitalWrite(lednum+2, HIGH) ;
  }
  // sampling LEDs
  for (int lednum = 0; lednum < NUMLEDS; lednum++) {
    digitalWrite(lednum+2, LOW);
    delayWithTemp(LEDON*100);

    // average NUMSAMPLES vals
    float adcSum = 0;
    for (int samplenum = 0; samplenum < NUMSAMPLES; samplenum++) {
      adcSum += float(analogRead(photodiodePin));
    }
    adcSum /= NUMSAMPLES;

    Serial.print(String(5*adcSum/1023));
    if (lednum < NUMLEDS-1) Serial.print(",");
    digitalWrite(lednum+2, HIGH);
  }
  Serial.print("\n");
  // turning LEDs on
  for (int lednum = 0; lednum < NUMLEDS; lednum++) {
    digitalWrite(lednum+2, LOW);
  }

  delayWithTemp(waittimehundredths);

}

void delayWithTemp(int waittimehundredths) {
  //delays 10ms and does an iteration of temp control loop
  for (int iteration = 0; iteration < waittimehundredths; iteration++) {
    float temp = fetchTemp();
    if (temp > TARGETTEMP + ALLOWEDTEMPVAR) digitalWrite(heaterPin, LOW);
    else if (temp < TARGETTEMP - ALLOWEDTEMPVAR) digitalWrite(heaterPin, HIGH);
    delay(10);
  }
}

float fetchTemp() {
  float rawTemp = 5*analogRead(tempPin) / 1023;
  float correctedTemp = (rawTemp - 0.5) / 0.01;
  return correctedTemp;
}
