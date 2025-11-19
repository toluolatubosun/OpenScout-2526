#include <Arduino_FreeRTOS.h>

void Task1(void *pvParameters) {
  while (1) {
    Serial.println("Task 1 running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2(void *pvParameters) {
  while (1) {
    Serial.println("Task 2 running");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);  // Use 9600
  
  xTaskCreate(Task1, "Task1", 128, NULL, 1, NULL);
  xTaskCreate(Task2, "Task2", 128, NULL, 1, NULL);
}

void loop() {
  // Empty
}