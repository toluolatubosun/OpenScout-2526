#include "tcp_server.h"
#include "motor_control.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h> 

#if defined(ARDUINO_GIGA)
  #include "mbed.h"
  using namespace rtos;
#endif

// === TCP CONFIGURATION ===
const int TCP_PORT = 8888;

// === TCP STATE ===
WiFiServer* tcpServer = nullptr;
WiFiClient tcpClient;
volatile bool clientConnected = false;

// Access to global state from main
struct MotorCommand; // Forward declare the struct (must match main .ino)
extern volatile bool eStopLatched;
extern MotorCommand currentCommand;

#if defined(ARDUINO_GIGA)
  extern rtos::Mutex commandMutex;
#else
  extern SemaphoreHandle_t commandMutex;
#endif

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
  
  // Read ALL available data to get only the latest command
  if (tcpClient.available() > 0) {
    String latestCommand = "";
    
    // Read everything available and keep only the last complete line
    while (tcpClient.available() > 0) {
      String line = tcpClient.readStringUntil('\n');
      if (line.length() > 0) {
        latestCommand = line;  // Keep overwriting with the latest
      }
    }
    
    // Process only the latest command if we got one
    if (latestCommand.length() > 0) {
      latestCommand.trim();
      
      // Parse JSON with ArduinoJson
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, latestCommand);
      
      if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
      }

      MotorCommand newCmd = currentCommand;
      
      if (doc.containsKey("cmd")) {
        const char* cmdStr = doc["cmd"];
        if (cmdStr && strlen(cmdStr) > 0) {
          newCmd.cmd = cmdStr[0];
          newCmd.timestamp = millis();
          Serial.print("TCP Command: ");
          Serial.println(cmdStr);
        }
      }
      
      if (doc.containsKey("speed")) {
        int speed = doc["speed"];
        if (speed >= 0 && speed <= 255) {
          newCmd.speed = speed;
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
          newCmd.duration = dur;
          Serial.print("TCP Duration: ");
          Serial.println(dur);
        } else {
          Serial.print("Invalid duration received: ");
          Serial.println(dur);
        }
      }

      // Update the global command atomically
      commandMutex.lock();
      currentCommand = newCmd;
      commandMutex.unlock();
    }
  }
}

// === SEND STATUS JSON ===
void sendStatusJSON() {
  // Safety checks
  if (WiFi.status() != WL_CONNECTED || !clientConnected || !tcpClient.connected()) {
    return;
  }
  
  commandMutex.lock();
  MotorCommand cmd = currentCommand;
  commandMutex.unlock();

  // Build JSON with ArduinoJson
  StaticJsonDocument<200> doc;
  doc["speed"] = cmd.speed;
  doc["duration"] = cmd.duration;
  doc["command"] = String(cmd.cmd);
  doc["time"] = cmd.timestamp;
  doc["rssi"] = WiFi.RSSI();
  
  // Serialize to string
  String output;
  serializeJson(doc, output);
  output += "\n";
  
  tcpClient.print(output);
}
