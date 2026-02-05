#include "hc12_receiver.h"
#include "motor_control.h"
#include <Arduino.h>

#if defined(ARDUINO_GIGA)
  #include "mbed.h"
  using namespace rtos;
#endif

// === HC-12 CONFIGURATION ===
const int HC12_BAUD_RATE = 9600;

// Arduino GIGA R1 WiFi Serial2 on pins:
// Connect HC-12 RXD to GIGA D19 (TX2)
// Connect HC-12 TXD to GIGA D18 (RX2)

// Access to global state from main
extern MotorCommand currentCommand;

#if defined(ARDUINO_GIGA)
  extern rtos::Mutex commandMutex;
#else
  extern SemaphoreHandle_t commandMutex;
#endif

// === INITIALIZE HC-12 ===
void initializeHC12() {
  Serial2.begin(HC12_BAUD_RATE);
  
  Serial.println("================================");
  Serial.println("HC-12 Receiver Initialized");
  Serial.println("Using Serial2 on pins D18 (RX2) and D19 (TX2)");
  Serial.print("Baud Rate: ");
  Serial.println(HC12_BAUD_RATE);
  Serial.println("Wiring:");
  Serial.println("  HC-12 TXD -> GIGA D18 (RX2)");
  Serial.println("  HC-12 RXD -> GIGA D19 (TX2)");
  Serial.println("Waiting for commands...");
  Serial.println("================================");
}

// === HANDLE HC-12 COMMANDS ===
void handleHC12Commands() {  
  if (Serial2.available()) {
    String received = "";
    
    // Read all available characters
    while (Serial2.available()) {
      char c = Serial2.read();
      received += c;
      delay(2);  // Small delay to allow buffer to fill
    }
    
    Serial.print("Received: ");
    Serial.println(received);
    
    // Process single character commands
    if (received.length() == 1) {
      char latestChar = received[0];
      
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
    } else if (received == "EMERGENCY") {
      Serial.println(">>> EMERGENCY ALERT <<<");
      commandMutex.lock();
      currentCommand.cmd = 'X';
      commandMutex.unlock();
    }
  }
}
