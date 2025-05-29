#include <Arduino.h>

class SPLMonitor {
  const int micPin = A0;
  const float referenceVoltage = 5.0;
  const float micSensitivity = 0.00631;

  float thresholdDb;
  String serialBuffer;

public:
  //konstruktor
  SPLMonitor(float initialThreshold = 30.0) : thresholdDb(initialThreshold) {
    setup();
  }

  void setup() {
    Serial.begin(9600);
    serialBuffer.reserve(32);
    Serial.print("SPL Monitor Ready. Initial threshold: ");
    Serial.print(thresholdDb, 1);
    Serial.println(" dB. Type THRESH=XX to set threshold.");
  }

  //główna pętla
  void loop() {
    checkSerial();

    const unsigned long sampleTime = 50;
    unsigned long startTime = millis();

    int maxVal = 0;
    int minVal = 1023;
    int samples = 0;

    while (millis() - startTime < sampleTime) {
      int val = analogRead(micPin);
      if (val < 1023) {
        if (val > maxVal) maxVal = val;
        if (val < minVal) minVal = val;
        samples++;
      }
    }

    if (samples > 0) {
      float vpp = (maxVal - minVal) * referenceVoltage / 1023.0;
      float vrms = vpp / 2.0 / sqrt(2.0);
      float db = 20.0 * log10(vrms / micSensitivity);

      if (db > thresholdDb) {
        Serial.print("Voltage: ");
        Serial.print(vpp, 3);
        Serial.print(" V, SPL: ");
        Serial.print(db, 1);
        Serial.println(" dB");
      }
    }

    delay(200);
  }

//komendy
private:
  void checkSerial() {
    while (Serial.available()) {
      char ch = Serial.read();

      if (ch == '\n' || ch == '\r') {
        serialBuffer.trim();
        if (serialBuffer.startsWith("THRESH=")) {
          //zmiana punktu odcięcia
          float val = serialBuffer.substring(7).toFloat();
          if (val >= 0.0 && val <= 120.0) {
            thresholdDb = val;
            Serial.print("New threshold set to ");
            Serial.print(thresholdDb, 1);
            Serial.println(" dB");
          } else {
            Serial.println("Invalid threshold. Range: 0–120 dB.");
          }
        } else if (serialBuffer == "?") {
          //zapytanie o aktualny punkt odcięcia
          Serial.print("Current threshold: ");
          Serial.print(thresholdDb, 1);
          Serial.println(" dB");
        } else if (serialBuffer.length() > 0) {
          //nieznana komenda
          Serial.println("Unknown command. Use THRESH=XX or ?");
        }
        serialBuffer = "";
      } else {
        serialBuffer += ch;
      }
    }
  }
};


int main() {
  init();

  SPLMonitor splMonitor(20);
  //splMonitor.setup();

  while (true) {
    splMonitor.loop();
  }

  return 0;
}
