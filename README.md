# OpenScout Robot - Arduino Setup Guide

Simple dual-motor robot with FreeRTOS control and emergency stop.

## Step 1: Hardware Assembly

### Motor Driver Connections (L298N)

**Motor A (Left Motor)**
- Arduino Pin 3 → L298N EN A (PWM)
- Arduino Pin 4 → L298N IN1
- Arduino Pin 5 → L298N IN2

**Motor B (Right Motor)**
- Arduino Pin 9 → L298N EN B (PWM)
- Arduino Pin 7 → L298N IN3
- Arduino Pin 8 → L298N IN4

**Connect Motors**
- Left Motor → L298N OUT1 and OUT2
- Right Motor → L298N OUT3 and OUT4

### E-Stop Button
- Any top pin → Arduino Pin 2
- Any bottom pin → Arduino GND

### Power
- L298N 12V input → 12V power supply
- L298N GND → Arduino GND (common ground)
- Arduino → USB or separate 5V supply
---

## Step 2: Configure WiFi Settings

1. Open `ros_wifi_bridge` file
2. Update WiFi credentials:
   - Change `WIFI_SSID` to your network name
   - Change `WIFI_PASSWORD` to your network password
3. Update ROS connection:
   - Set `ROS_SERVER_IP` to your computer's IP address
   - Set `ROS_SERVER_PORT` (default: 11411)
4. **Important**: Connect your PC to the same WiFi network

---

## Step 3: Install Arduino IDE Library

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries**
3. Search for `FreeRTOS`
4. Install **FreeRTOS by Richard Barry** version **11.1.0-3**

---

## Step 4: Add FreeRTOS Library (ZIP)

1. Download: https://github.com/straccio/Arduino_Due_FreeRTOS/archive/refs/heads/master.zip
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**
3. Select the downloaded ZIP file
4. Restart Arduino IDE

---

## Step 5: Upload Code

1. Open `OpenScoutArduinoCode.ino`
2. Select your board: **Tools → Board → Arduino Uno**
3. Select port: **Tools → Port → (your port)**
4. Click **Upload**

---

## Controls

Open Serial Monitor (9600 baud):

- **W** = Forward
- **S** = Backward  
- **A** = Turn Left
- **D** = Turn Right
- **X** = Stop
- **0-9** = Speed (0=slow, 9=fast)
- **+/-** = Step duration (100-900ms)
- **R** = Reset E-Stop

**E-Stop**: Press physical button for emergency stop. Release and press 'R' to reset.

---