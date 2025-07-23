# 🌐 ESP32 IoT Monitoring System (Cloud-Connected)

This project demonstrates a cloud-connected ESP32-based IoT system that reads sensor data, controls actuators, and syncs in real-time with Firebase. It includes a mobile app interface for monitoring and remote control.

---

## 📦 Features

- 🌡️ Reads temperature and humidity via DHT11
- 🔆 Controls external LED brightness using a potentiometer
- 🚨 Buzzer triggered by temperature threshold (cloud or automatic)
- 📲 Remote control of LED and buzzer via Firebase
- ☁️ Realtime data sync with Firebase Realtime Database
- 📱 Flutter mobile app (or MIT App Inventor alternative)

---

## ⚙️ Hardware Used

| Component     | Description              |
|---------------|--------------------------|
| ESP32         | Dev Module               |
| DHT11         | Temperature & Humidity   |
| Potentiometer | Analog LED Brightness    |
| LED + Resistor| Visual output            |
| Buzzer        | Audio Alert              |
| Breadboard    | Wiring & prototyping     |

---

## 🔌 Circuit Diagram

![Wiring Diagram](https://github.com/user-attachments/assets/ddadbef5-ae7a-43ac-9815-dbc353ee4b98)

---

## 🧠 Firebase Structure

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
