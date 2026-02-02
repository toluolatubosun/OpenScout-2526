#if defined(ARDUINO_GIGA)
  #define USE_MBED_OS
  #include "mbed.h"
  using namespace rtos;
  using namespace std::chrono;
#else
  #define USE_FREERTOS
  #include <Arduino_FreeRTOS.h>
#endif

#include "tcp_server.h"
#include "wifi_manager.h"
#include "motor_control.h"

// === SHARED STATE (Global) ===
MotorCommand currentCommand = {'X', 150, 300, 0};

// Mutex to protect currentCommand from race conditions
#ifdef USE_MBED_OS
  rtos::Mutex commandMutex;
#else
  SemaphoreHandle_t commandMutex;
#endif

// === Helper Functions ===
void updateSpeed(int speedLevel);
void updateDuration(int change);
void printMenu();

// === TASK DECLARATIONS ===
#ifdef USE_FREERTOS
  void TaskSerialRead(void* pvParameters);
  void TaskMotorControl(void* pvParameters);
  void TaskWiFiMonitor(void* pvParameters);
  void TaskTCPServer(void* pvParameters);
#else
  void TaskSerialRead();
  void TaskMotorControl();
  void TaskWiFiMonitor();
  void TaskTCPServer();
#endif

// === RTOS THREAD/TASK HANDLES ===
#ifdef USE_MBED_OS
  Thread serialThread;
  Thread motorThread;
  Thread wifiThread;
  Thread tcpThread;
#else
  TaskHandle_t serialTaskHandle;
  TaskHandle_t motorTaskHandle;
  TaskHandle_t wifiTaskHandle;
  TaskHandle_t tcpTaskHandle;
#endif

// === RTOS ABSTRACTION LAYER ===
inline void rtos_delay_ms(unsigned long ms) {
  #ifdef USE_MBED_OS
    ThisThread::sleep_for(std::chrono::milliseconds(ms));
  #else
    vTaskDelay(ms / portTICK_PERIOD_MS);
  #endif
}

void rtos_create_tasks() {
  #ifdef USE_MBED_OS
    serialThread.start(TaskSerialRead);
    motorThread.start(TaskMotorControl);
    wifiThread.start(TaskWiFiMonitor);
    tcpThread.start(TaskTCPServer);
  #else
    xTaskCreate(TaskSerialRead, "Serial", 256, NULL, 1, &serialTaskHandle);
    xTaskCreate(TaskMotorControl, "Motor", 256, NULL, 1, &motorTaskHandle);
    xTaskCreate(TaskWiFiMonitor, "WiFi", 128, NULL, 1, &wifiTaskHandle);
    xTaskCreate(TaskTCPServer, "TCP", 256, NULL, 1, &tcpTaskHandle);
    vTaskStartScheduler();
  #endif
}

void setup() { 
  Serial.begin(9600);
  while (!Serial) { ; }

  initializeMotorPins();
  initializeWiFi();

  // Initialize mutex (only needed for FreeRTOS)
  #ifndef USE_MBED_OS
    commandMutex = xSemaphoreCreateMutex();
  #endif

  printMenu();

  rtos_create_tasks();
}

void loop() {
  // Empty
}

// === SERIAL READING TASK (unchanged) ===
#ifdef USE_FREERTOS
void TaskSerialRead(void* pvParameters) {
#else
void TaskSerialRead() {  
#endif
  while (1) {
    char input = 0;
    String commandSource = "Serial";
    bool commandReceived = false;
    
    // Check serial for commands
    if (!commandReceived && Serial.available() > 0) {
      input = Serial.read();
      commandReceived = true;
      commandSource = "Serial";
    }

    // Skip processing whitespace/empty input
    if (input == ' ' || input == '\t' || input == '\n' || input == '\r') {
      continue; 
    }

    if (commandReceived) {
      Serial.print(commandSource);
      Serial.print(" Command: ");
      Serial.println(input);
    }
    
    // Process the command if we received one
    if (commandReceived) {
      switch (input) {
        case 'W': case 'w':
          commandMutex.lock();
          currentCommand.cmd = 'W';
          currentCommand.timestamp = millis();
          commandMutex.unlock();
          break;
          
        case 'S': case 's':
          commandMutex.lock();
          currentCommand.cmd = 'S';
          currentCommand.timestamp = millis();
          commandMutex.unlock();
          break;
          
        case 'A': case 'a':
          commandMutex.lock();
          currentCommand.cmd = 'A';
          currentCommand.timestamp = millis();
          commandMutex.unlock();
          break;
          
        case 'D': case 'd':
          commandMutex.lock();
          currentCommand.cmd = 'D';
          currentCommand.timestamp = millis();
          commandMutex.unlock();
          break;
          
        case 'X': case 'x':
          commandMutex.lock();
          currentCommand.cmd = 'X';
          commandMutex.unlock();
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
    rtos_delay_ms(10);
  }
}

// === MOTOR CONTROL TASK (unchanged) ===
#ifdef USE_FREERTOS
void TaskMotorControl(void* pvParameters) {
#else
void TaskMotorControl() {
#endif
  char lastCmd = '\0';  // Track previous command to avoid repeated printing
  
  while (1) {
    // Read command atomically
    commandMutex.lock();
    MotorCommand cmd = currentCommand;
    commandMutex.unlock();

    unsigned long now = millis();
    if (cmd.cmd != 'X' && (now - cmd.timestamp) >= cmd.duration) {
      commandMutex.lock();
      currentCommand.cmd = 'X';
      commandMutex.unlock();
      cmd.cmd = 'X';
    }
    
    // Only print when command changes
    if (cmd.cmd != lastCmd) {
      switch (cmd.cmd) {
        case 'W':
          Serial.println("Moving Forward");
          break;
        case 'S':
          Serial.println("Moving Backward");
          break;
        case 'A':
          Serial.println("Turning Left");
          break;
        case 'D':
          Serial.println("Turning Right");
          break;
        case 'X':
          Serial.println("Stopping Motors");
          break;
      }
      lastCmd = cmd.cmd;
    }
    
    // Always execute the motor commands
    switch (cmd.cmd) {
      case 'W':
        motorAForward();
        motorBForward();
        motorCForward();
        motorDForward();
        break;
      case 'S':
        motorABackward();
        motorBBackward();
        motorCBackward();
        motorDBackward();
        break;
      case 'A':
        motorABackward();
        motorBForward();
        motorCBackward();
        motorDForward();
        break;
      case 'D':
        motorAForward();
        motorBBackward();
        motorCForward();
        motorDBackward();
        break;
      case 'X':
        stopAllMotors();
        break;
    }
    
    rtos_delay_ms(10);
  }
}

// === WIFI MONITORING TASK ===
#ifdef USE_FREERTOS
void TaskWiFiMonitor(void* pvParameters) {
#else
void TaskWiFiMonitor() {
#endif
  bool tcpInitialized = false;
  
  while (1) {
    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    
    // If WiFi connected and TCP not initialized
    if (wifiConnected && !tcpInitialized) {
      initializeTCPServer();
      tcpInitialized = true;
    }
    
    // If WiFi disconnected, mark TCP as needing re-init
    if (!wifiConnected && tcpInitialized) {
      Serial.println("WiFi lost - TCP will reinitialize on reconnect");
      tcpInitialized = false;
    }
    
    // Check/reconnect WiFi
    checkConnections();
    
    rtos_delay_ms(15000);  // Check every 15 seconds
  }
}

// === NEW: TCP SERVER TASK ===
#ifdef USE_FREERTOS
void TaskTCPServer(void* pvParameters) {
#else
void TaskTCPServer() {
#endif
  while (1) {
    // Handle incoming TCP connections and commands
    handleTCPClient();
    
    // Send status every 100ms if client connected
    static unsigned long lastStatusTime = 0;
    unsigned long now = millis();
    if (clientConnected && (now - lastStatusTime) >= 100) {
      sendStatusJSON();
      lastStatusTime = now;
    }
    
    rtos_delay_ms(10);
  }
}

// === HELPER FUNCTIONS (unchanged) ===
void updateSpeed(int speedLevel) {
  int pwm = map(speedLevel, 0, 9, 50, 255);
  
  commandMutex.lock();
  currentCommand.speed = pwm;
  commandMutex.unlock();
  
  Serial.print("Speed: ");
  Serial.print(speedLevel);
  Serial.print(" (PWM ");
  Serial.print(pwm);
  Serial.println(")");
}

void updateDuration(int change) {
  commandMutex.lock();
  currentCommand.duration += change;
  if (currentCommand.duration < 100) currentCommand.duration = 100;
  if (currentCommand.duration > 900) currentCommand.duration = 900;
  currentCommand.duration = (currentCommand.duration / 100) * 100;
  int dur = currentCommand.duration;
  commandMutex.unlock();
  
  Serial.print("Duration: ");
  Serial.print(dur);
  Serial.println("ms");
}
void printMenu() {
  Serial.println("================================");
  Serial.println("=== Real-Time Motor Control ===");
  Serial.println("W/S = Forward/Backward");
  Serial.println("A/D = Turn Left/Right");
  Serial.println("X = Stop");
  Serial.println("0-9 = Speed");
  Serial.println("+/- = Duration 100-900ms");
  Serial.println("================================");
}