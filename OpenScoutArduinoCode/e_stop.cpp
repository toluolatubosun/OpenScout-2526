#include <Arduino.h>

// === E-STOP PIN ===
const int ESTOP_PIN = 2;  // Using interrupt pin

// === E-STOP STATE ===
volatile bool eStopPressed = false;
volatile bool eStopLatched = false;  // Stays true until reset

// Access to motor control
extern void stopAllMotors();
extern volatile char currentCommand;

// === INTERRUPT SERVICE ROUTINE ===
void eStopISR() {
  // This runs immediately when button is pressed
  eStopPressed = true;
  eStopLatched = true;
  currentCommand = 'X';  // Force stop command
  stopAllMotors();       // Immediate hardware stop
}

// === INITIALIZATION ===
void initializeEStop() {
  pinMode(ESTOP_PIN, INPUT_PULLUP);  // Use internal pull-up
  
  // Attach interrupt - triggers on falling edge (button press)
  attachInterrupt(digitalPinToInterrupt(ESTOP_PIN), eStopISR, FALLING);
  
  Serial.println("================================");
  Serial.println("E-Stop initialized on Pin 2");
  Serial.println("================================");
}

// === CHECK E-STOP STATE ===
bool isEStopActive() {
  return eStopLatched;
}

// === RESET E-STOP (call after releasing button) ===
void resetEStop() {
  if (digitalRead(ESTOP_PIN) == HIGH) {  // Button must be released
    eStopLatched = false;
    eStopPressed = false;
    Serial.println("E-Stop RESET - Motors enabled");
  } else {
    Serial.println("Release E-Stop button first!");
  }
}

// === CHECK IF BUTTON IS CURRENTLY PRESSED ===
bool isEStopButtonPressed() {
  return (digitalRead(ESTOP_PIN) == LOW);
}