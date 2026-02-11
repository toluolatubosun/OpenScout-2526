
# Circuit Assembly  
*The components required for the circuits assembly are as follows:*

(Note: Setup the libraries on the Arduino IDE) [Arduino Libraries](library_setup.md)


## Motor Driver PinOuts (LN298N)

![Motor Driver Image](https://components101.com/sites/default/files/component_pin/L298N-Module-Pinout.jpg)

Source: [Motor Driver Pinout Link](https://components101.com/modules/l293n-motor-driver-module)

![alt text](Images/motor_pinouts.png)

Source: [Motor PinOut Connections (Power & Encoder)](http://www.cqrobot.wiki/index.php/DC_Gearmotor_SKU:_CQR37D)


## OpenScout — Wiring connection guide

The schematic for the openscout can be used as an reference for building/connecting the components:

![alt text](Images/schematics.png)

---

## Hardware Overview

This OpenScout robot uses **two Arduino boards**:

1. **Arduino GIGA R1 WiFi** — Main robot controller (receiver)  
2. **Arduino Uno** — Wireless remote control (transmitter)

---

## Step 1: Hardware Assembly

### Robot / Robot Controller (Arduino GIGA R1 WiFi)

#### Motor Driver Connections (L298N)

**Motor A (Left Motor):**

- Arduino  **D3** → L298N **EN A (PWM)**
- Arduino  **D4** → L298N **IN1**
- Arduino  **D5** → L298N **IN2**

**Motor B (Right Motor):**

- Arduino  **D9** → L298N **EN B (PWM)**
- Arduino  **D7** → L298N **IN3**
- Arduino  **D8** → L298N **IN4** 

---

#### LCD Display Connections (16×2 LCD)

Connect the LCD to the Arduino GIGA:

- **VSS** → GND  
- **VDD** → **5V**  
- **V0** → center pin of a 10 kΩ potentiometer (for contrast)  
  - Potentiometer other pins: **5V** and **GND**  
- **RS** →  **D12**  
- **RW** → GND  
- **E** →  **D11**  
- **D4** →  **D10**  
- **D5** →  **D13**  
- **D6** →  **D14**  
- **D7** →  **D15**  
- **A (backlight +)** → 5V via **220 Ω resistor**  
- **K (backlight −)** → GND

> The LCD displays the Arduino’s IP address when connected to WiFi and remains blank when not connected.

---

#### HC-12 Wireless Module (Receiver)

Connect as follows:

- **HC-12 VCC** → 5V  
- **HC-12 GND** → GND  
- **HC-12 TXD** →  **D18 (RX2)**  
- **HC-12 RXD** →  **D19 (TX2)**  
- **HC-12 SET** → *Leave unconnected (normal mode)*  

> **The GIGA uses hardware Serial2 on D18/D19.**

---

#### Power Connections

- **L298N 12V input** → 12V power supply  
- **L298N GND** ↔ Arduino GIGA **GND** (common ground)  
- **Arduino GIGA** → USB or external power supply

---


### Membrane Switch Module (Emergency Stop)

Connect:

- **Signal** →  **D2**  
- **VCC** → 5V  
- **GND** → GND  

> **Internal pull-up resistor (goes LOW when pressed).**

---




## Step 2: Configure WiFi Settings

1. Open `wifi_manager.cpp` in the Arduino IDE  
2. Set your WiFi network credentials:  
   - `WIFI_SSID` → your network name  
   - `WIFI_PASSWORD` → your network password  
3. Ensure your computer is on the same WiFi network during setup.

---

## Step 3: Upload Robot Controller Code

1. Open `OpenScoutArduinoCode/OpenScoutArduinoCode.ino`  
2. In the Arduino IDE:  
   - **Tools → Board:** select **Arduino GIGA R1 WiFi**  
   - **Tools → Port:** choose the correct port  
3. Click **Upload**  
4. Open **Serial Monitor** at **9600 baud** to see:  
   - WiFi connection status  
   - Arduino’s IP address (shown on LCD)  
   - HC-12 receiver status

---

## Step 4: (Optional) Start ROS Bridge or setup the wireless 
[Wireless transmitter reciever code (Optional)](circuit_assembly_wireless.md)

1. Use Docker Compose to start the ROS bridge:  
   ```bash
   cd OpenScoutROS
   docker-compose build
   docker-compose run ros python3 /app/ros_bridge.py YOUR_ARDUINO_IP

