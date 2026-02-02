#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

struct MotorCommand {
  char cmd;
  int speed;
  int duration;
  unsigned long timestamp;
};

// === MOTOR PIN DEFINITIONS ===
extern const int enA, in1, in2;  // Motor A (Left)
extern const int enB, in3, in4;  // Motor B (Right)

// === FUNCTION DECLARATIONS ===
void initializeMotorPins();
void motorAForward();
void motorABackward();
void motorBForward();
void motorBBackward();
void stopMotorA();
void stopMotorB();
void stopAllMotors();

#endif // MOTOR_CONTROL_H
