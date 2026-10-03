# 🤖 Otto DIY Robot - ESP32 Wi-Fi Upgrade

An upgraded, open-source version of the famous **Otto DIY Robot**. This project replaces the original Arduino Nano with an **ESP32** microcontroller, adding built-in Wi-Fi Access Point functionality and a web-based control interface.

---

## 🌟 Key Upgrades & Features
- **ESP32 Core:** Enhanced processing power and built-in Wi-Fi capability.
- **Web Interface Control:** Control movements directly from your smartphone browser (No dedicated app required).
- **SoftAP Mode:** Creates its own Wi-Fi network automatically.
- **3D Printable:** Fully compatible with standard 3D printed Otto parts.

---

## 🛠️ Hardware Requirements
- **ESP32 Development Board** (30-pin or 38-pin)
- **SG90 Servo Motors** x4
- **HC-SR04 Ultrasonic Sensor** x1
- **LiPo / 18650 Battery or AA Battery Shield**
- **3D Printed Parts** (Body, Legs, Feet, Head)

---

## 🚀 Quick Start Guide

### 1. Circuit & Wiring
Refer to the schematics in the `docs/` folder for exact pin configurations between the ESP32, servos, and ultrasonic sensor.

### 2. Uploading Firmware
1. Open the code in `firmware/` using **Arduino IDE**.
2. Install the ESP32 board manager and required libraries.
3. Select your ESP32 board and upload the sketch.

### 3. Connecting & Controlling
1. Power on the robot.
2. On your phone or PC, connect to the Wi-Fi network:
   - **SSID:** `Otto-ESP32` (or your set name)
   - **Password:** `123456789`
3. Open any web browser and go to: `http://192.168.4.1`
4. Use the web interface to control Otto's movements!

---

## 📽️ Tutorials & Credits
- **Assembly Guide:** Watch [Video Tutorial Title]([LINK_TO_ASSEMBLY_VIDEO](https://youtu.be/mAdSBA00tjY?si=oOSM3z597e1U_ZP3)) for mechanical assembly steps.
- **ESP32 Demo & Explanation:** Watch [Our Project Overview](LINK_TO_YOUR_YOUTUBE_VIDEO) to see how the Wi-Fi upgrade works.
- **Original Project:** Based on the open-source [Otto DIY](https://www.ottodiy.com/) project.

---

## 📄 License
Distributed under the **MIT License**. See `LICENSE` for more information.
