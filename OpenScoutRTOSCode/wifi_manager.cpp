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
  Serial.println("Attempting to connect to WiFi!");

  // Check if WiFi module exists
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: WiFi module not found!");
    wifiConnected = false;
    return;
  }
  
  // Disconnect any existing connection
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for connection (timeout after 30 seconds with status updates)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    attempts++;
    
    // Print status every 5 seconds
    if (attempts % 5 == 0) {
      Serial.print("WiFi status: ");
      Serial.println(WiFi.status());
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.print("WiFi connection failed. Final status: ");
    Serial.println(WiFi.status());
  }
}

// === CHECK WIFI CONNECTION ===
bool isWiFiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

// === RECONNECT IF DISCONNECTED ===
void checkConnections() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Serial.println("WiFi disconnected, attempting reconnect...");
    wifiConnected = false;
    delay(1000);
    connectWiFi();
  }
}

// === INITIALIZATION ===
void initializeWiFi() {
  connectWiFi();
}