# ESP32 IoT Control System 🌡️💡📲

An interactive IoT system built using **ESP32**, capable of reading sensor values (temperature, humidity), controlling LED brightness via a potentiometer, and triggering a buzzer when exceeding thresholds. Firebase is used as the cloud backend, and a custom Android app (Flutter App) acts as the front-end interface.

---

## 🔧 Features

- 🌡️ DHT11 Temperature & Humidity monitoring
- 🎚️ Potentiometer to control LED brightness (PWM via GPIO25)
- 🔔 Buzzer alert when temperature exceeds threshold
- 🌐 Firebase Realtime Database sync (data & control)
- 📱 Android App UI built using Flutter 
- 🖥️ Firebase dashboard to control buzzer, LED and threshold remotely

---

## 🧰 Hardware Components

- ESP32 Dev Board
- DHT11 Sensor
- Potentiometer
- LED + Resistor
- Buzzer
- Breadboard + Jumper Wires

---

## 🔌 Circuit Diagram
![Wiring Diagram](https://github.com/user-attachments/assets/ddadbef5-ae7a-43ac-9815-dbc353ee4b98)

---

## 🖼️ Project in Action

| Prototype | Mobile App |
|----------|-------------|
| ![With_firebase_Dashboard](https://github.com/user-attachments/assets/ece7fdc4-fee3-43f2-bf6e-bc9e9aeaf0a2)| ![App](https://github.com/user-attachments/assets/e485c1f3-bcfd-41d9-86a3-4402aa72cc8c)|

| Circuit | Circuit With App |
|----------|-------------|
|![Circuit_in_detail](https://github.com/user-attachments/assets/d5732b49-3574-4913-9d9a-00d78b0f401e) | ![Circuit_with_app](https://github.com/user-attachments/assets/cf48c155-e86a-4b09-b860-58f539baa261) |

---

## 📲 App Interface

The mobile app allows:
- Real-time temperature and humidity display
- LED brightness as a percentage
- Toggling light/buzzer states
- Setting temperature threshold

---

## 🧠 Cloud Structure – Firebase Realtime DB

```json
sensor_readings: {
  "2025-07-18T12:30:00": {
    "temperature": 28.3,
    "humidity": 61,
    "led_state": "on",
    "led_brightness": 122
  }
},
latest: {
  "temperature": 28.3,
  "humidity": 61,
  "led_state": "on",
  "led_brightness": 122,
  "timestamp": "2025-07-18T12:30:00"
},
control: {
  "led_state": "on",
  "buzzer": "on",
  "temp_threshold": 30.5
}
```

---

## 🚀 How to Run

1. Flash the Arduino code to your ESP32
2. Update your Wi-Fi credentials and Firebase URL
3. Install the Flutter Apk.
4. Make sure your hardware setup is good.
5. Control and monitor remotely

---

## 👨‍💻 Author

- [Muhamed Magdi (Mo-Alsehli)](https://github.com/Mo-Alsehli)

