#include <AlfredoCRSF.h>

// --- ESP32-C3 PIN CONFIGURATION ---
// Crossfire Nano RX
#define CRSF_RX_PIN 6  // Connect to CRSF Ch1 (TX)
#define CRSF_TX_PIN 7  // Connect to CRSF Ch2 (RX)

// Hoverboard Motherboard
#define HOVER_RX_PIN 20 // Connect to Hoverboard TX, physical pin RX on esp32-c
#define HOVER_TX_PIN 21 // Connect to Hoverboard RX, physical pin TX on esp32-c

AlfredoCRSF crsf;

// --- HOVERBOARD PROTOCOL SETUP ---
#define START_FRAME 0xABCD // The secret handshake the hoverboard looks for
typedef struct __attribute__((packed)) {
   uint16_t start;
   int16_t  steer;
   int16_t  speed;
   uint16_t checksum;
} SerialCommand;

SerialCommand command;

void setup() {
  // 1. Start PC Debug Serial (Routes through Native USB CDC on ESP32-C3)
  Serial.begin(115200);
  delay(2000); // Give the native USB a second to connect to PC
  Serial.println("ESP32-C3 Translator Brain Booting...");

  // 2. Start Hoverboard Serial on UART0 (Using the built-in Serial0 object)
  Serial0.begin(115200, SERIAL_8N1, HOVER_RX_PIN, HOVER_TX_PIN);

  // 3. Start Crossfire Serial on UART1 (Using the built-in Serial1 object)
  Serial1.begin(CRSF_BAUDRATE, SERIAL_8N1, CRSF_RX_PIN, CRSF_TX_PIN);
  crsf.begin(Serial1);
}

void loop() {
  // Update Crossfire data
  crsf.update();

  // Get raw joystick values (Usually 1000 to 2000, 1500 is center)
  int steer_raw = crsf.getChannel(1); // Ch 1 usually Right Stick L/R
  int speed_raw = crsf.getChannel(2); // Ch 2 usually Right Stick U/D

  // Failsafe: If radio is off, zero out the values
  if (steer_raw < 500 || speed_raw < 500) {
    steer_raw = 1500;
    speed_raw = 1500;
  }

  // Map radio values to hoverboard speed (-500 to 500 for initial safety!)
  int16_t steer = map(steer_raw, 1000, 2000, -500, 500);
  int16_t speed = map(speed_raw, 1000, 2000, -500, 500);

  // Apply a Deadband (so it doesn't drift when you let go of the sticks)
  if (abs(steer) < 30) steer = 0;
  if (abs(speed) < 30) speed = 0;

  // Send the math to the hoverboard
  SendToHoverboard(steer, speed);

  // Print to PC for debugging
  Serial.print("Steer: "); Serial.print(steer);
  Serial.print(" | Speed: "); Serial.println(speed);

  delay(50); // Run loop at ~20Hz
}

// Function to package and send the data frame
void SendToHoverboard(int16_t uSteer, int16_t uSpeed) {
  command.start    = START_FRAME;
  command.steer    = uSteer;
  command.speed    = uSpeed;
  command.checksum = (uint16_t)(command.start ^ command.steer ^ command.speed);

  Serial0.write((uint8_t *) &command, sizeof(command));
}