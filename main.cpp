#include <Arduino.h>

const int micPin = A0;
const float referenceVoltage = 5.0;
const int sampleWindow = 50;
const int sampleCount = 400;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  
  while (millis() - startMillis < sampleWindow) {
    int sample = analogRead(micPin);
    if (sample < 1023) {
      if (sample > signalMax) {
        signalMax = sample;
      }
      if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  int peakToPeak = signalMax - signalMin;
  float voltage = (peakToPeak * referenceVoltage) / 1023.0;
  float rms = voltage / 2.0f / sqrt(2.0f);

  float db = 20.0 * log10(rms / 0.00631);

  if (db>30) {
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V, SPL: ");
  Serial.print(db, 1);
  Serial.println(" dB");
  }

  delay(200);
}
