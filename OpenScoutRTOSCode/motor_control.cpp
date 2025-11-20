#include "motor_control.h"
#include <Arduino.h>

// === MOTOR PIN DEFINITIONS ===
const int enA = 3, in1 = 4, in2 = 5;  // Motor A (Left)
const int enB = 9, in3 = 7, in4 = 8;  // Motor B (Right)

// Access to global speed variable from main file
extern volatile int currentSpeed;

// === MOTOR A (LEFT) ===
void motorAForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, currentSpeed);
}

void motorABackward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, currentSpeed);
}

void stopMotorA() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}

// === MOTOR B (RIGHT) ===
void motorBForward() {
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, currentSpeed);
}

void motorBBackward() {
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, currentSpeed);
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
