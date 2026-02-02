#include "motor_control.h"
#include <Arduino.h>

#if defined(ARDUINO_GIGA)
  #include "mbed.h"
  using namespace rtos;
#endif

// === MOTOR PIN DEFINITIONS ===
const int enA = 3, in1 = 4, in2 = 5;  // Motor A (Back - Left)
const int enB = 9, in3 = 7, in4 = 8;  // Motor B (Back - Right)
const int enC = 45, in5 = 43, in6 = 41;  // Motor C (Front - Left)
const int enD = 40, in7 = 42, in8 = 44;  // Motor D (Front - Right)

// Access to global command from main file
extern MotorCommand currentCommand;

#if defined(ARDUINO_GIGA)
  extern rtos::Mutex commandMutex;
#else
  extern SemaphoreHandle_t commandMutex;
#endif

// === MOTOR A (LEFT) ===
void motorAForward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, speed);
}

void motorABackward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, speed);
}

void stopMotorA() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}

// === MOTOR B (RIGHT) ===
void motorBForward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, speed);
}

void motorBBackward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, speed);
}

void stopMotorB() {
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enB, 0);
}

// === STOP ALL ===
void stopAllMotors() {
  stopMotorA();
  stopMotorB();
}

// === INITIALIZATION ===
void initializeMotorPins() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  stopAllMotors();
}
