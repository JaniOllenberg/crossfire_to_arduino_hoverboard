#include <SoftwareSerial.h>

#define PIN_RX 3  // Change to an available digital pin
#define PIN_TX 4  // Change to an available digital pin

SoftwareSerial crsfSerial(PIN_RX, PIN_TX);  // Create a software serial port

void setup() {
    Serial.begin(115200);  // USB Serial for debugging
    crsfSerial.begin(115200);  // Software serial (CRSF devices might need lower baud rates)
    Serial.println("Starting raw CRSF data test...");
}

void loop() {
    while (crsfSerial.available()) {
        uint8_t data = crsfSerial.read();
        Serial.print(data, DEC);
        Serial.print(" ");
    }
}
