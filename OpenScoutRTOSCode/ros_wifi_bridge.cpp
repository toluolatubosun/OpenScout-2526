#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>

// === WIFI CONFIGURATION ===
const char* WIFI_SSID = "John_A16";      // Change this
const char* WIFI_PASSWORD = "kingtolu";     // Change this

// === ROS TCP SERVER CONFIGURATION ===
const char* ROS_SERVER_IP = "10.196.173.96";     // PC running rosserial
const int ROS_SERVER_PORT = 11411;               // rosserial default port

// === WIFI STATE ===
volatile bool wifiConnected = false;

// === TCP CLIENT ===
WiFiClient rosClient;

// === CONNECT TO WIFI ===
void connectWiFi() {
  Serial.println("Attempting to connect to WiFi!");

  // Check if WiFi module exists
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: WiFi module not found!");
    wifiConnected = false;
    return;
  }
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for connection (timeout after 20 seconds)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi connected");
  } else {
    wifiConnected = false;
    Serial.println("WiFi connection failed");
  }
}

// === CONNECT TO ROS TCP SERVER ===
bool connectToROSServer() {
  if (!wifiConnected) {
    return false;
  }
  
  if (rosClient.connect(ROS_SERVER_IP, ROS_SERVER_PORT)) {
    Serial.println("Connected to ROS server");
    return true;
  } else {
    Serial.println("Failed to connect to ROS server");
    return false;
  }
}

// === CHECK WIFI CONNECTION ===
bool isWiFiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

// === CHECK ROS CONNECTION ===
bool isROSConnected() {
  return rosClient.connected();
}

// === GET ROS CLIENT ===
WiFiClient* getROSClient() {
  return &rosClient;
}

// === RECONNECT IF DISCONNECTED ===
void checkConnections() {
  // Check WiFi first
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    wifiConnected = false;
    rosClient.stop();
    connectWiFi();
  }
  
  // If WiFi is good but ROS disconnected, try to reconnect
  if (wifiConnected && !rosClient.connected()) {
    rosClient.stop();
    delay(1000);
    connectToROSServer();
  }
}

// === INITIALIZATION ===
void initializeWiFi() {
  connectWiFi();
  
  if (wifiConnected) {
    // Try to connect to ROS server
    delay(1000);
    connectToROSServer();
  }
}