#include "wifi_manager.h"
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>

// === WIFI CONFIGURATION ===
const char* WIFI_SSID = "John_A16";      // Change this
const char* WIFI_PASSWORD = "kingtolu";     // Change this

// === WIFI STATE ===
volatile bool wifiConnected = false;

// === CONNECT TO WIFI ===
void connectWiFi() {
  Serial.println("================================");
  Serial.println("Attempting to connect to WiFi!");

  // Check if WiFi module exists
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: WiFi module not found!");
    wifiConnected = false;
    return;
  }
  
  Serial.println("Resetting WiFi module...");
  // Disconnect any existing connection
  WiFi.disconnect();
  delay(1000);
  // Completely shut down WiFi hardware
  WiFi.end();          
  delay(1000); 

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for connection (timeout after 10 seconds with status updates)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    attempts++;
    
    // Print status every 2 seconds
    if (attempts % 2 == 0) {
      Serial.print("WiFi status: ");
      Serial.println(WiFi.status());
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("================================");
  } else {
    wifiConnected = false;
    Serial.print("WiFi connection failed. Final status: ");
    Serial.println(WiFi.status());
    Serial.println("================================");
  }
}

// === CHECK WIFI CONNECTION ===
bool isWiFiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

// === RECONNECT IF DISCONNECTED ===
void checkConnections() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    delay(1000);
    connectWiFi();
  }
}

// === INITIALIZATION ===
void initializeWiFi() {
  connectWiFi();
}