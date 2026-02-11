# Wireless transmitter connections setup

## Step 1: Setup Remote Control (Arduino Uno — Transmitter)

### HC-12 Wireless Module (Transmitter)

Connect:

- **HC-12 VCC** → 5V  
- **HC-12 GND** → GND  
- **HC-12 TXD** →  **Pin 3** *(SoftwareSerial RX)*  
- **HC-12 RXD** →  **Pin 4** *(SoftwareSerial TX)*  
- **HC-12 SET** → *Leave unconnected (normal mode)*

---


### 4×4 Matrix Keypad Connections

Connect to Arduino Uno:

- **Row pins:** Pin 5, Pin 6, Pin 7, Pin 8  
- **Column pins:** Pin 9, Pin 10, Pin 11, Pin 12

---

### Power for Transmitter

- **Arduino Uno** → USB or 5V supply

---

## Step 2: Upload Transmitter Code (Remote Control)

1. Open `OpenScoutTransmitterCode/OpenScoutTransmitterCode.ino`  
2. In the Arduino IDE:  
   - **Tools → Board:** select **Arduino Uno**  
   - **Tools → Port:** select the COM port  
3. Click **Upload**  
4. Open **Serial Monitor** at **9600 baud** to verify transmission

**Remote Keypad Controls:**

- **2** = Forward  
- **8** = Backward  
- **4** = Turn Left  
- **6** = Turn Right  
- **5** = Stop  
- **A** = Increase Speed (+25 PWM)  
- **B** = Decrease Speed (−25 PWM)  
- **C** = Increase Duration (+100 ms)  
- **D** = Decrease Duration (−100 ms)  
- **Emergency switch** = Emergency Stop

---