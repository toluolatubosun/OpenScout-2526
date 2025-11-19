// --- Dual Motor Control with Step Movement ---
// Motor A (Left Motor)
const int enA = 3;   // PWM pin
const int in1 = 4;   // Direction pin 1
const int in2 = 5;   // Direction pin 2

// Motor B (Right Motor)
const int enB = 9;   // PWM pin
const int in3 = 7;   // Direction pin 1
const int in4 = 8;   // Direction pin 2

// Motor control variables
int currentSpeed = 150;  // Default speed
int stepDuration = 300;  // Movement duration in milliseconds

void setup() {
  // Initialize Motor A pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  // Initialize Motor B pins
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Start with motors stopped
  stopAllMotors();
  
  Serial.begin(9600);
  Serial.println("=== Dual Motor Step Control ===");
  Serial.println("W = Forward step");
  Serial.println("S = Backward step");
  Serial.println("A = Turn left step");
  Serial.println("D = Turn right step");
  Serial.println("X = Stop both motors");
  Serial.println("0-9 = Set speed (0=slow, 9=fast)");
  Serial.println("+ = Increase step duration");
  Serial.println("- = Decrease step duration");
  Serial.println("================================");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    
    // Forward - both motors forward
    if (input == 'W' || input == 'w') {
      Serial.println("Step: Forward");
      stepForward();
    }
    // Backward - both motors backward
    else if (input == 'S' || input == 's') {
      Serial.println("Step: Backward");
      stepBackward();
    }
    // Turn left - left motor backward, right motor forward
    else if (input == 'A' || input == 'a') {
      Serial.println("Step: Turn Left");
      stepTurnLeft();
    }
    // Turn right - left motor forward, right motor backward
    else if (input == 'D' || input == 'd') {
      Serial.println("Step: Turn Right");
      stepTurnRight();
    }
    // Stop
    else if (input == 'X' || input == 'x') {
      stopAllMotors();
      Serial.println("Motors stopped");
    }
    // Speed control (0-9)
    else if (input >= '0' && input <= '9') {
      int speedLevel = input - '0';
      currentSpeed = map(speedLevel, 0, 9, 50, 255);  // Map to 50-255 for better range
      Serial.print("Speed set to level ");
      Serial.print(speedLevel);
      Serial.print(" (PWM: ");
      Serial.print(currentSpeed);
      Serial.println(")");
    }
    // Increase step duration
    else if (input == '+') {
      stepDuration += 50;
      if (stepDuration > 2000) stepDuration = 2000;
      Serial.print("Step duration: ");
      Serial.print(stepDuration);
      Serial.println("ms");
    }
    // Decrease step duration
    else if (input == '-') {
      stepDuration -= 50;
      if (stepDuration < 100) stepDuration = 100;
      Serial.print("Step duration: ");
      Serial.print(stepDuration);
      Serial.println("ms");
    }
  }
}

// === STEP MOVEMENT FUNCTIONS ===
void stepForward() {
  motorAForward();
  motorBForward();
  delay(stepDuration);
  stopAllMotors();
}

void stepBackward() {
  motorABackward();
  motorBBackward();
  delay(stepDuration);
  stopAllMotors();
}

void stepTurnLeft() {
  motorABackward();
  motorBForward();
  delay(stepDuration);
  stopAllMotors();
}

void stepTurnRight() {
  motorAForward();
  motorBBackward();
  delay(stepDuration);
  stopAllMotors();
}

// === MOTOR A (LEFT) CONTROL ===
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

// === MOTOR B (RIGHT) CONTROL ===
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