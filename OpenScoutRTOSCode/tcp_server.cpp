#include "tcp_server.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h> 

// === TCP CONFIGURATION ===
const int TCP_PORT = 8888;

// === TCP STATE ===
WiFiServer* tcpServer = nullptr;
WiFiClient tcpClient;
volatile bool clientConnected = false;

// Access to global state from main
extern volatile char currentCommand;
extern volatile int currentSpeed;
extern volatile int stepDuration;
extern volatile unsigned long commandTime;
extern volatile bool eStopLatched;

// === INITIALIZE TCP SERVER ===
void initializeTCPServer() {
  // Double-check WiFi is actually connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ERROR: WiFi not connected - cannot start TCP server");
    return;
  }
  
  // Clean up existing server if any
  if (tcpServer != nullptr) {
    delete tcpServer;
    tcpServer = nullptr;
  }
  
  tcpServer = new WiFiServer(TCP_PORT);
  tcpServer->begin();
  
  Serial.println("================================");
  Serial.print("TCP Server started on port ");
  Serial.println(TCP_PORT);
  Serial.print("Connect to: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(TCP_PORT);
  Serial.println("================================");
}

// === HANDLE TCP CLIENT ===
void handleTCPClient() {
  // Safety check - if WiFi is down, do nothing
  if (WiFi.status() != WL_CONNECTED || tcpServer == nullptr) {
    if (clientConnected) {
      clientConnected = false;
      if (tcpClient) {
        tcpClient.stop();
      }
      Serial.println("WiFi down - TCP client disconnected");
    }
    return;
  }
  
  // Check for new client
  if (!clientConnected) {
    tcpClient = tcpServer->available();
    if (tcpClient) {
      clientConnected = true;
      Serial.println("ROS client connected!");
    }
    return;
  }
  
  // Check if existing client is still connected
  if (!tcpClient.connected()) {
    clientConnected = false;
    tcpClient.stop();
    Serial.println("ROS client disconnected");
    return;
  }
  
  // Read incoming data
  if (tcpClient.available() > 0) {
    String jsonStr = tcpClient.readStringUntil('\n');
    jsonStr.trim();
    
    // Parse JSON with ArduinoJson
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      return;
    }
    
    // Extract values safely
    if (doc.containsKey("cmd")) {
      const char* cmdStr = doc["cmd"];
      if (cmdStr && strlen(cmdStr) > 0) {
        // Support multi-character commands (store first char for now)
        // To support full strings later, change currentCommand to char array
        currentCommand = cmdStr[0];
        commandTime = millis();
        Serial.print("TCP Command: ");
        Serial.println(cmdStr);  // Print full command string
      }
    }
    
    if (doc.containsKey("speed")) {
      int speed = doc["speed"];
      if (speed >= 0 && speed <= 255) {
        currentSpeed = speed;
        Serial.print("TCP Speed: ");
        Serial.println(speed);
      } else {
        Serial.print("Invalid speed received: ");
        Serial.println(speed);
      }
    }
    
    if (doc.containsKey("duration")) {
      int dur = doc["duration"];
      if (dur >= 100 && dur <= 900) {
        stepDuration = dur;
        Serial.print("TCP Duration: ");
        Serial.println(dur);
      } else {
        Serial.print("Invalid duration received: ");
        Serial.println(dur);
      }
    }
  }
}

// === SEND STATUS JSON ===
void sendStatusJSON() {
  // Safety checks
  if (WiFi.status() != WL_CONNECTED || !clientConnected || !tcpClient.connected()) {
    return;
  }
  
  // Build JSON with ArduinoJson
  StaticJsonDocument<200> doc;
  doc["speed"] = currentSpeed;
  doc["duration"] = stepDuration;
  doc["command"] = String(currentCommand);
  doc["time"] = commandTime;
  doc["e_stop"] = eStopLatched;
  doc["rssi"] = WiFi.RSSI();
  
  // Serialize to string
  String output;
  serializeJson(doc, output);
  output += "\n";
  
  tcpClient.print(output);
}
