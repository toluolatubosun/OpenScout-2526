#include "motor_control.h"
#include <Arduino.h>

#if defined(ARDUINO_GIGA)
  #include "mbed.h"
  using namespace rtos;
#endif

// === MOTOR PIN DEFINITIONS ===
const int enA = 3, in1 = 4, in2 = 5;     // Motor A (Back - Left)
const int enB = 9, in3 = 7, in4 = 8;     // Motor B (Back - Right)
const int enC = 2, in5 = 22, in6 = 24;   // Motor C (Front - Left) - CHANGED
const int enD = 6, in7 = 28, in8 = 30;   // Motor D (Front - Right) - CHANGED

// === ULTRASONIC SENSOR PIN DEFINITIONS ===
const int trigFront = 50, echoFront = 52;  // Front sensor
const int trigBack = 36, echoBack = 38;    // Back sensor

// === BUZZER PIN DEFINITION ===
const int buzzerPin = 34;

// === LED PIN DEFINITIONS ===
const int led1 = 47, led2 = 49, led3 = 51, led4 = 53;

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

// === MOTOR C (FRONT - LEFT) ===
void motorCForward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  analogWrite(enC, speed);
}

void motorCBackward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();

  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  analogWrite(enC, speed);
}

void stopMotorC() {
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  analogWrite(enC, 0);
}

// === MOTOR D (FRONT - RIGHT) ===
void motorDForward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in8, HIGH);
  digitalWrite(in7, LOW);
  analogWrite(enD, speed);
}

void motorDBackward() {
  commandMutex.lock();
  int speed = currentCommand.speed;
  commandMutex.unlock();
  
  digitalWrite(in8, LOW);
  digitalWrite(in7, HIGH);
  analogWrite(enD, speed);
}

void stopMotorD() {
  digitalWrite(in8, LOW);
  digitalWrite(in7, LOW);
  analogWrite(enD, 0);
}

// === STOP ALL ===
void stopAllMotors() {
  stopMotorA();
  stopMotorB();
  stopMotorC();
  stopMotorD();
}

// === ULTRASONIC FUNCTIONS ===
long getDistanceFront() {
  digitalWrite(trigFront, LOW);
  delayMicroseconds(2);
  digitalWrite(trigFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigFront, LOW);
  long duration = pulseIn(echoFront, HIGH);
  return duration * 0.034 / 2;  // Convert to cm
}

long getDistanceBack() {
  digitalWrite(trigBack, LOW);
  delayMicroseconds(2);
  digitalWrite(trigBack, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigBack, LOW);
  long duration = pulseIn(echoBack, HIGH);
  return duration * 0.034 / 2;  // Convert to cm
}

bool canMoveForward() {
  return getDistanceFront() > 30;  // Threshold: 30cm
}

bool canMoveBackward() {
  return getDistanceBack() > 30;   // Threshold: 30cm
}

// === BUZZER FUNCTION ===
void beepBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);  // Beep for 200ms
  digitalWrite(buzzerPin, LOW);
}

// === LED FUNCTIONS ===
void turnOffAllLEDs() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}

// === INITIALIZATION ===
void initializeMotorPins() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
  pinMode(enD, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);
  
  // Initialize ultrasonic pins
  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigBack, OUTPUT);
  pinMode(echoBack, INPUT);
  
  // Initialize buzzer pin
  pinMode(buzzerPin, OUTPUT);
  
  // Initialize LED pins
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  
  // Turn LEDs off initially
  turnOffAllLEDs();
  
  stopAllMotors();
}
