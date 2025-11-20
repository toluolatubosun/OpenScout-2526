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

1. Open `wifi_manager.cpp` file
2. Update WiFi credentials:
   - Change `WIFI_SSID` to your network name
   - Change `WIFI_PASSWORD` to your network password
3. **Important**: Connect your PC to the same WiFi network

---

## Step 3: Install Arduino IDE Libraries

1. Open Arduino IDE
2. Go to **Tools → Manage Libraries**
3. Search for and install:
   - **FreeRTOS by Richard Barry** version **11.1.0-3**
   - **ArduinoJson** version **7.4.2**

---

## Step 4: Add FreeRTOS Library (ZIP)

1. Download: https://github.com/straccio/Arduino_Due_FreeRTOS/archive/refs/heads/master.zip
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**
3. Select the downloaded ZIP file
4. Restart Arduino IDE

---

## Step 5: Upload Code

1. Open `OpenScoutRTOSCode.ino`
2. Select your board: **Tools → Board → Arduino Uno**
3. Select port: **Tools → Port → (your port)**
4. Click **Upload**
5. Open Serial Monitor to see the Arduino's IP address

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