// === RTOS DETECTION ===
#if defined(ARDUINO_GIGA)
  #define USE_MBED_OS
  #include "mbed.h"
  using namespace rtos;
  using namespace std::chrono;
#else
  #define USE_FREERTOS
  #include <Arduino_FreeRTOS.h>
#endif

// === TASK DECLARATIONS ===
#ifdef USE_FREERTOS
  void Task1(void* pvParameters);
  void Task2(void* pvParameters);
#else
  void Task1();
  void Task2();
#endif

// === RTOS THREAD/TASK HANDLES ===
#ifdef USE_MBED_OS
  Thread thread1;
  Thread thread2;
#else
  TaskHandle_t task1Handle;
  TaskHandle_t task2Handle;
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
    thread1.start(Task1);
    thread2.start(Task2);
  #else
    xTaskCreate(Task1, "Task1", 128, NULL, 1, &task1Handle);
    xTaskCreate(Task2, "Task2", 128, NULL, 1, &task2Handle);
    vTaskStartScheduler();
  #endif
}

// === TASKS ===
#ifdef USE_FREERTOS
void Task1(void* pvParameters) {
#else
void Task1() {
#endif
  while (1) {
    Serial.println("Task 1 running");
    rtos_delay_ms(1000);  // 1 second
  }
}

#ifdef USE_FREERTOS
void Task2(void* pvParameters) {
#else
void Task2() {
#endif
  while (1) {
    Serial.println("Task 2 running");
    rtos_delay_ms(500);  // 0.5 seconds
  }
}

// === SETUP ===
void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  
  Serial.println("=== RTOS Test ===");
  #ifdef USE_MBED_OS
    Serial.println("Platform: Arduino Giga (Mbed OS)");
  #else
    Serial.println("Platform: FreeRTOS");
  #endif
  Serial.println("Starting tasks...\n");
  
  rtos_create_tasks();
}

void loop() {
  // Empty - RTOS handles everything
}