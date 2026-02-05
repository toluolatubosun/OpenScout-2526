#include "wifi_manager.h"
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <LiquidCrystal.h>

// === WIFI CONFIGURATION ===
const char* WIFI_SSID = "John_A16";      // Change this
const char* WIFI_PASSWORD = "kingtolu";  // Change this

// === WIFI STATE ===
volatile bool wifiConnected = false;

// === LCD SETUP ===
/*
VSS → GND
VDD → 5V
V0 → 10kΩ potentiometer (contrast adjustment)
RS → Pin 12
RW → GND
E → Pin 11
D4 → Pin 10
D5 → Pin 13
D6 → Pin 14
D7 → Pin 15
A (backlight +) → 5V (through 220Ω resistor)
K (backlight -) → GND
*/
// Initialize LCD: RS, E, D4, D5, D6, D7
// Using pins: 12, 11, 10, 13, 14, 15
const int rs = 12, en = 11, d4 = 10, d5 = 13, d6 = 14, d7 = 15;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// === UPDATE LCD DISPLAY ===
void updateLCDDisplay() {
  lcd.clear();
  
  if (WiFi.status() == WL_CONNECTED) {
    // Display IP address
    lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
  }
  // If not connected, LCD stays clear (shows nothing)
}

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
    
    // Update LCD with IP address
    updateLCDDisplay();
  } else {
    wifiConnected = false;
    Serial.print("WiFi connection failed. Final status: ");
    Serial.println(WiFi.status());
    Serial.println("================================");
    
    // Clear LCD display
    lcd.clear();
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
    lcd.clear();  // Clear display when disconnected
    delay(1000);
    connectWiFi();
  }
}

// === INITIALIZATION ===
void initializeWiFi() {
  // Initialize LCD (16 columns x 2 rows)
  lcd.begin(16, 2);
  lcd.clear();
  
  // Connect to WiFi
  connectWiFi();
}