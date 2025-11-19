#include <Arduino_FreeRTOS.h>

// === SHARED STATE (Global) ===
volatile char currentCommand = 'X';
volatile int currentSpeed = 150;
volatile int stepDuration = 300;
volatile unsigned long commandTime = 0;

// === FORWARD DECLARATIONS  ===
// Motor Controls Declarations
void initializeMotorPins();
void motorAForward();
void motorABackward();
void motorBForward();
void motorBBackward();
void stopAllMotors();

// E-stop Declarations
void initializeEStop();
bool isEStopActive();
void resetEStop();
bool isEStopButtonPressed();

// === TASK DECLARATIONS ===
void TaskSerialRead(void *pvParameters);
void TaskMotorControl(void *pvParameters);

void setup() { 
  Serial.begin(9600);
  while (!Serial) { ; }

  initializeMotorPins();
  initializeEStop();
 
  printMenu();
  
  xTaskCreate(TaskSerialRead, "Serial", 128, NULL, 1, NULL);
  xTaskCreate(TaskMotorControl, "Motor", 128, NULL, 1, NULL);
}

void loop() {
  // Empty
}

// === SERIAL READING TASK ===
void TaskSerialRead(void *pvParameters) {
  (void) pvParameters;
  
  while (1) {
    if (Serial.available() > 0) {
      char input = Serial.read();

      // Check if e-stop is active
      if (isEStopActive() && input != 'R' && input != 'r') {
        Serial.println("E-STOP ACTIVE! Press 'R' to reset.");
        vTaskDelay(10 / portTICK_PERIOD_MS);
        continue;
      }
      
      switch (input) {
        case 'W': case 'w':
          currentCommand = 'W';
          commandTime = millis();
          Serial.println("Forward");
          break;
          
        case 'S': case 's':
          currentCommand = 'S';
          commandTime = millis();
          Serial.println("Backward");
          break;
          
        case 'A': case 'a':
          currentCommand = 'A';
          commandTime = millis();
          Serial.println("Turn Left");
          break;
          
        case 'D': case 'd':
          currentCommand = 'D';
          commandTime = millis();
          Serial.println("Turn Right");
          break;
          
        case 'X': case 'x':
          currentCommand = 'X';
          Serial.println("Stop");
          break;

        case 'R': case 'r':
          resetEStop();
          break;
          
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          updateSpeed(input - '0');
          break;
          
        case '-':
          updateDuration(-100);
          break;
          
        case '+':
          updateDuration(100);
          break;
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// === MOTOR CONTROL TASK ===
void TaskMotorControl(void *pvParameters) {
  (void) pvParameters;
  
  while (1) {
    // E-stop check - highest priority
    if (isEStopActive()) {
      stopAllMotors();
      vTaskDelay(50 / portTICK_PERIOD_MS);  // Check less frequently when stopped
      continue;
    }

    unsigned long now = millis();
    char cmd = currentCommand;
    
    if (cmd != 'X' && (now - commandTime) >= stepDuration) {
      currentCommand = 'X';
      cmd = 'X';
    }
    
    switch (cmd) {
      case 'W':
        motorAForward();
        motorBForward();
        break;
      case 'S':
        motorABackward();
        motorBBackward();
        break;
      case 'A':
        motorABackward();
        motorBForward();
        break;
      case 'D':
        motorAForward();
        motorBBackward();
        break;
      case 'X':
        stopAllMotors();
        break;
    }
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// === HELPER FUNCTIONS ===
void updateSpeed(int speedLevel) {
  currentSpeed = map(speedLevel, 0, 9, 50, 255);
  Serial.print("Speed: ");
  Serial.print(speedLevel);
  Serial.print(" (PWM ");
  Serial.print(currentSpeed);
  Serial.println(")");
}

void updateDuration(int change) {
  stepDuration += change;
  if (stepDuration < 100) stepDuration = 100;
  if (stepDuration > 900) stepDuration = 900;
  stepDuration = (stepDuration / 100) * 100;
  
  Serial.print("Duration: ");
  Serial.print(stepDuration);
  Serial.println("ms");
}

void printMenu() {
  Serial.println("=== Real-Time Motor Control ===");
  Serial.println("W/S = Forward/Backward");
  Serial.println("A/D = Turn Left/Right");
  Serial.println("X = Stop");
  Serial.println("0-9 = Speed");
  Serial.println("+/- = Duration 100-900ms");
  Serial.println("================================");
  Serial.println("E-Stop: Press button for emergency stop");
  Serial.println("================================");
}