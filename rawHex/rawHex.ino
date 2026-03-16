#include <HardwareSerial.h>

//#define PIN_RX 3
#define PIN_RX -1
#define PIN_TX -1

HardwareSerial crsfSerial(0);
//HardwareSerial crsfSerial;
int line = 0;

void setup() {
    pinMode(-1, INPUT_PULLUP);
    Serial.begin(115200);
    //Serial.begin(9600);
    Serial.println("Starting raw CRSF data test...");

    crsfSerial.begin(420000, SERIAL_8N1, PIN_RX, PIN_TX, false);
    //crsfSerial.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
}

void loop() {
    while (crsfSerial.available()) {
        uint8_t data = crsfSerial.read();
        //Serial.print(data, HEX);
        Serial.print(data, 10);
        Serial.print(" ");
        if(line % 16 == 0)
          Serial.println(" ");
        line++;
    }
}
