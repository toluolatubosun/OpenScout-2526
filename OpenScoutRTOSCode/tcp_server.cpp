#include "tcp_server.h"
#include <Arduino.h>
#include <WiFi.h>

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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected - cannot start TCP server");
    return;
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
    
    // Simple JSON parsing (expecting: {"cmd":"W","speed":150,"duration":300})
    // Find values between quotes/colons
    int cmdIdx = jsonStr.indexOf("\"cmd\":\"") + 7;
    if (cmdIdx > 7) {
      char cmd = jsonStr.charAt(cmdIdx);
      currentCommand = cmd;
      commandTime = millis();
      Serial.print("TCP Command: ");
      Serial.println(cmd);
    }
    
    int speedIdx = jsonStr.indexOf("\"speed\":") + 8;
    if (speedIdx > 8) {
      int speed = jsonStr.substring(speedIdx).toInt();
      if (speed >= 0 && speed <= 255) {
        currentSpeed = speed;
        Serial.print("TCP Speed: ");
        Serial.println(speed);
      }
    }
    
    int durIdx = jsonStr.indexOf("\"duration\":") + 11;
    if (durIdx > 11) {
      int dur = jsonStr.substring(durIdx).toInt();
      if (dur >= 100 && dur <= 900) {
        stepDuration = dur;
        Serial.print("TCP Duration: ");
        Serial.println(dur);
      }
    }
  }
}

// === SEND STATUS JSON ===
void sendStatusJSON() {
  if (!clientConnected || !tcpClient.connected()) {
    return;
  }
  
  // Get WiFi RSSI
  long rssi = WiFi.RSSI();
  
  // Build JSON: {"speed":150,"duration":300,"command":"W","time":1234,"e_stop":false,"rssi":-45}
  String json = "{";
  json += "\"speed\":" + String(currentSpeed) + ",";
  json += "\"duration\":" + String(stepDuration) + ",";
  json += "\"command\":\"" + String(currentCommand) + "\",";
  json += "\"time\":" + String(commandTime) + ",";
  json += "\"e_stop\":" + String(eStopLatched ? "true" : "false") + ",";
  json += "\"rssi\":" + String(rssi);
  json += "}\n";
  
  tcpClient.print(json);
}
