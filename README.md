# OpenScout Robot - Arduino Setup Guide

Simple dual-motor robot with FreeRTOS control, LCD display, wireless HC-12 control, and emergency stop.

## Hardware Overview

This project uses **TWO Arduino boards**:
1. **Arduino GIGA R1 WiFi** - Main robot controller (receiver)
2. **Arduino Uno** - Wireless remote control (transmitter)

---

## Step 1: Hardware Assembly

### Robot / Robot Controller (Arduino GIGA R1 WiFi)

#### Motor Driver Connections (L298N)

**Motor A (Left Motor)**
- Arduino Pin 3 → L298N EN A (PWM)
- Arduino Pin 4 → L298N IN1
- Arduino Pin 5 → L298N IN2

**Motor B (Right Motor)**
- Arduino Pin 9 → L298N EN B (PWM)
- Arduino Pin 7 → L298N IN3
- Arduino Pin 8 → L298N IN4

#### LCD Display Connections (16x2 LCD)

Connect the 16x2 LCD to Arduino GIGA:

- VSS → GND
- VDD → 5V
- V0 → 10kΩ potentiometer center pin (for contrast adjustment)
  - Potentiometer: One end to 5V, other end to GND
- RS → Pin 12
- RW → GND
- E → Pin 11
- D4 → Pin 10
- D5 → Pin 13
- D6 → Pin 14
- D7 → Pin 15
- A (backlight +) → 5V through 220Ω resistor
- K (backlight -) → GND

**Function:** The LCD displays the Arduino's IP address when WiFi is connected. The screen remains blank when WiFi is disconnected.

#### HC-12 Wireless Module (Receiver)

Connect HC-12 to Arduino GIGA:
- HC-12 VCC → 5V
- HC-12 GND → GND
- HC-12 TXD → Pin D18 (RX2)
- HC-12 RXD → Pin D19 (TX2)
- HC-12 SET → Leave unconnected (normal mode)

**Note:** The GIGA uses hardware Serial2 on pins D18/D19.

#### Power
- L298N 12V input → 12V power supply
- L298N GND → Arduino GIGA GND (common ground)
- Arduino GIGA → USB or separate power supply

---

### Remote Control (Arduino Uno - Transmitter)

#### HC-12 Wireless Module (Transmitter)

Connect HC-12 to Arduino Uno:
- HC-12 VCC → 5V
- HC-12 GND → GND
- HC-12 TXD → Pin 3 (SoftwareSerial RX)
- HC-12 RXD → Pin 4 (SoftwareSerial TX)
- HC-12 SET → Leave unconnected (normal mode)

**Note:** The Uno uses SoftwareSerial on pins 3/4.

#### 4x4 Matrix Keypad

Connect to Arduino Uno:
- **Row pins:** 5, 6, 7, 8
- **Column pins:** 9, 10, 11, 12

#### Membrane Switch Module (Emergency Stop)

Connect to Arduino Uno:
- Signal → Pin 2
- VCC → 5V
- GND → GND

**Note:** Uses internal pull-up resistor (goes LOW when pressed).

#### Power
- Arduino Uno → USB or 5V supply

---

## Step 2: Upload Transmitter Code (Remote Control)

**Upload to Arduino Uno:**

1. Open `OpenScoutTransmitterCode/OpenScoutTransmitterCode.ino`
2. Select **Tools → Board → Arduino Uno**
3. Select your COM port
4. Click **Upload**
5. Open Serial Monitor (9600 baud) to verify transmission

**Remote Keypad Controls:**
- **2** = Forward
- **8** = Backward
- **4** = Turn Left
- **6** = Turn Right
- **5** = Stop
- **A** = Increase Speed (+25 PWM)
- **B** = Decrease Speed (-25 PWM)
- **C** = Increase Duration (+100ms)
- **D** = Decrease Duration (-100ms)
- **Membrane Switch** = Emergency Stop

---

## Step 3: Configure WiFi Settings

1. Open `wifi_manager.cpp` file
2. Update WiFi credentials:
   - Change `WIFI_SSID` to your network name
   - Change `WIFI_PASSWORD` to your network password
3. **Important**: Connect your PC to the same WiFi network

---

## Step 4: Install Arduino IDE Libraries

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries**
3. Search for and install:
   - **ArduinoJson** version **7.4.2**
   - **LiquidCrystal by Arduino** version **1.0.7**
   - **Keypad by Mark Stanley, Alexander Brevig** version **3.0.0**

---

## Step 5: Upload Robot Code

1. Open `OpenScoutArduinoCode/OpenScoutArduinoCode.ino`
2. Select your board: **Tools → Board → Arduino GIGA R1 WiFi**
3. Select port: **Tools → Port → (your port)**
4. Click **Upload**
5. Open Serial Monitor (9600 baud) to see:
   - WiFi connection status
   - Arduino's IP address (also shown on LCD)
   - HC-12 receiver status

---

## Step 6: Start ROS Bridge

Use the Arduino IP from Serial Monitor in these commands:

**Terminal 1 (Start Bridge):**
```bash
cd OpenScoutROS
docker-compose build
docker-compose run ros python3 /app/ros_bridge.py YOUR_ARDUINO_IP
```

**Terminal 2 (Start Teleop):**
```bash
docker exec -it CONTAINER_ID bash -c "source /opt/ros/humble/setup.bash && ros2 run teleop_twist_keyboard teleop_twist_keyboard"
```

Replace `YOUR_ARDUINO_IP` with the IP shown in Arduino Serial Monitor.
Replace `CONTAINER_ID` with the container ID from Terminal 1.

**Alternatively**: Use the provided `.sh` scripts: `./quick_start.sh` and `./start_teleop.sh`

---

## Control Methods

### 1. HC-12 Wireless Remote (Primary)

Use the 4x4 keypad on the Arduino Uno transmitter:

- **2** = Forward
- **8** = Backward
- **4** = Turn Left
- **6** = Turn Right
- **5** = Stop
- **A** = Increase Speed
- **B** = Decrease Speed
- **C** = Increase Duration
- **D** = Decrease Duration
- **Membrane Switch** = Emergency Stop

**Range:** Approximately 100 meters in open space.

### 2. Serial Monitor (USB Direct)

Open Serial Monitor (9600 baud):

- **W** = Forward
- **S** = Backward  
- **A** = Turn Left
- **D** = Turn Right
- **X** = Stop
- **0-9** = Speed (0=slow, 9=fast)
- **+/-** = Step duration (100-900ms)

### 3. ROS 2 Control (WiFi Network)

Use teleop_twist_keyboard:

- **i** = Forward
- **,** = Backward
- **j** = Turn Left
- **l** = Turn Right
- **k** = Stop
- **q/z** = Increase/Decrease speed

---

## System Status

**Arduino GIGA (Robot):**
- Serial Monitor shows WiFi status, IP address, and all commands
- LCD Display shows IP address when connected (blank when disconnected)

**Arduino Uno (Transmitter):**
- Serial Monitor shows "Sent: X" for each keypad press
- Shows "Emergency button pressed!" when membrane switch activated

---