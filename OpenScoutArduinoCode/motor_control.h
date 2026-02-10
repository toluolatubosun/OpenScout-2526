#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

struct MotorCommand {
  char cmd;
  int speed;
  int duration;
  unsigned long timestamp;
};

// === MOTOR PIN DEFINITIONS ===
extern const int enA, in1, in2;  // Motor A (Back - Left)
extern const int enB, in3, in4;  // Motor B (Back - Right)
extern const int enC, in5, in6;  // Motor C (Front - Left)
extern const int enD, in7, in8;  // Motor D (Front - Right)

// === ULTRASONIC SENSOR PIN DEFINITIONS ===
extern const int trigFront, echoFront;  // Front sensor
extern const int trigBack, echoBack;    // Back sensor

// === BUZZER PIN DEFINITION ===
extern const int buzzerPin;

// === LED PIN DEFINITIONS ===
extern const int led1, led2, led3, led4;

// === FUNCTION DECLARATIONS ===
void initializeMotorPins();
void motorAForward();
void motorABackward();
void motorBForward();
void motorBBackward();
void motorCForward();
void motorCBackward();
void motorDForward();
void motorDBackward();
void stopMotorA();
void stopMotorB();
void stopMotorC();
void stopMotorD();
void stopAllMotors();

// === ULTRASONIC FUNCTIONS ===
long getDistanceFront();
long getDistanceBack();
bool canMoveForward();
bool canMoveBackward();

// === BUZZER FUNCTION ===
void beepBuzzer();

// === LED FUNCTIONS ===
void turnOffAllLEDs();

#endif // MOTOR_CONTROL_H
