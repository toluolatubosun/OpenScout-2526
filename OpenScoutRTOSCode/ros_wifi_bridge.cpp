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
  
  // Disconnect any existing connection and set mode
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(500);
  
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
    Serial.println("WiFi disconnected, attempting reconnect...");
    wifiConnected = false;
    rosClient.stop();
    delay(1000);
    connectWiFi();
  }
  
  // If WiFi is good but ROS disconnected, try to reconnect
  if (wifiConnected && !rosClient.connected()) {
    Serial.println("ROS disconnected, attempting reconnect...");
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