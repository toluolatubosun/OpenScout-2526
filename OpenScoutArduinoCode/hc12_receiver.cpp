#include "hc12_receiver.h"
#include "motor_control.h"
#include <Arduino.h>

#if defined(ARDUINO_GIGA)
  #include "mbed.h"
  using namespace rtos;
#endif

// === HC-12 CONFIGURATION ===
const int HC12_BAUD_RATE = 9600;

// Arduino GIGA R1 WiFi Serial3 on pins:
// TX3 = Pin 18 (PD8)
// RX3 = Pin 19 (PD9)
// Connect HC-12 TX to GIGA RX3 (pin 19)
// Connect HC-12 RX to GIGA TX3 (pin 18)

// Access to global state from main
extern MotorCommand currentCommand;

#if defined(ARDUINO_GIGA)
  extern rtos::Mutex commandMutex;
#else
  extern SemaphoreHandle_t commandMutex;
#endif

// === INITIALIZE HC-12 ===
void initializeHC12() {
  Serial3.begin(HC12_BAUD_RATE);
  
  Serial.println("================================");
  Serial.println("HC-12 Receiver Initialized");
  Serial.println("Using Serial3 (TX3=Pin18, RX3=Pin19)");
  Serial.print("Baud Rate: ");
  Serial.println(HC12_BAUD_RATE);
  Serial.println("Wiring:");
  Serial.println("  HC-12 TX -> GIGA RX3 (Pin 19)");
  Serial.println("  HC-12 RX -> GIGA TX3 (Pin 18)");
  Serial.println("  HC-12 VCC -> 5V");
  Serial.println("  HC-12 GND -> GND");
  Serial.println("  HC-12 SET -> Leave disconnected");
  Serial.println("================================");
}

// === HANDLE HC-12 COMMANDS ===
void handleHC12Commands() {
  if (Serial3.available() > 0) {
    char latestChar = 0;
    
    // Read all buffered characters and keep only the last valid one
    while (Serial3.available() > 0) {
      char c = Serial3.read();
      // Skip whitespace but keep the last valid character
      if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
        latestChar = c;
      }
    }
    
    // Only process if we got a valid character
    if (latestChar == 0) {
      return;
    }
    
    Serial.print("HC-12 Command: ");
    Serial.println(latestChar);
    
    // Process the command
    switch (latestChar) {
      // Directional movement
      case '2': // Forward
        commandMutex.lock();
        currentCommand.cmd = 'W';
        currentCommand.timestamp = millis();
        commandMutex.unlock();
        break;
        
      case '8': // Backward
        commandMutex.lock();
        currentCommand.cmd = 'S';
        currentCommand.timestamp = millis();
        commandMutex.unlock();
        break;
        
      case '4': // Left
        commandMutex.lock();
        currentCommand.cmd = 'A';
        currentCommand.timestamp = millis();
        commandMutex.unlock();
        break;
        
      case '6': // Right
        commandMutex.lock();
        currentCommand.cmd = 'D';
        currentCommand.timestamp = millis();
        commandMutex.unlock();
        break;
        
      case '5': // Stop
        commandMutex.lock();
        currentCommand.cmd = 'X';
        commandMutex.unlock();
        Serial.println("HC-12: Stop");
        break;
      
      // Speed adjustment
      case 'A': case 'a': { // Increase speed
        commandMutex.lock();
        currentCommand.speed += 25;
        if (currentCommand.speed > 255) currentCommand.speed = 255;
        int speedA = currentCommand.speed;
        commandMutex.unlock();
        Serial.print("HC-12: Speed increased to ");
        Serial.println(speedA);
        break;
      }
        
      case 'B': case 'b': { // Decrease speed
        commandMutex.lock();
        currentCommand.speed -= 25;
        if (currentCommand.speed < 50) currentCommand.speed = 50;
        int speedB = currentCommand.speed;
        commandMutex.unlock();
        Serial.print("HC-12: Speed decreased to ");
        Serial.println(speedB);
        break;
      }
      
      // Duration adjustment
      case 'C': case 'c': { // Increase duration
        commandMutex.lock();
        currentCommand.duration += 100;
        if (currentCommand.duration > 900) currentCommand.duration = 900;
        int durC = currentCommand.duration;
        commandMutex.unlock();
        Serial.print("HC-12: Duration increased to ");
        Serial.print(durC);
        Serial.println("ms");
        break;
      }
        
      case 'D': case 'd': { // Decrease duration
        commandMutex.lock();
        currentCommand.duration -= 100;
        if (currentCommand.duration < 100) currentCommand.duration = 100;
        int durD = currentCommand.duration;
        commandMutex.unlock();
        Serial.print("HC-12: Duration decreased to ");
        Serial.print(durD);
        Serial.println("ms");
        break;
      }
      
      default:
        Serial.print("HC-12: Unknown command '");
        Serial.print(latestChar);
        Serial.println("'");
        break;
    }
  }
}
