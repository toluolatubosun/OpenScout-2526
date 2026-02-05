#include <SoftwareSerial.h>
#include <Keypad.h>

SoftwareSerial HC12(3, 4); // TX, RX

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {9, 10, 11, 12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buttonPin = 2;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("HC-12 Transmitter Ready");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    HC12.print(key);
    Serial.print("Sent: ");
    Serial.println(key);
  }
  
  if (digitalRead(buttonPin) == LOW) {
    HC12.print("EMERGENCY");
    Serial.println("Emergency button pressed!");
    delay(500);
  }
}