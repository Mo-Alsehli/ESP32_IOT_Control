#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <time.h>

// 🛜 Wi-Fi Credentials
const char* ssid = "MM";
const char* password = "00000000";

// Send time interval variables.

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10-seconds.

static unsigned long lastControlCheck = 0;
const unsigned long controlCheckInterval = 1000; // every 1 seconds

// time configuration (setup ntp server).
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 3600;
const int dayLightOffset_sec = 0;


// Firebase Configuration.
const char* firebaseHost = "https://esp32-iot-64134-default-rtdb.europe-west1.firebasedatabase.app/";
const char* firbaseAuth = ""; // empty for test mode.

// Default temperature threshold.
float tempThreshold = 30.0;
bool buzzerState = true;

// 📡 HTTP Server runs on port 80
WebServer server(80);

// 🌡 DHT11 sensor config (GPIO 26)
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 💡 LED pin
#define LED_PIN 2
#define BRIGHTNESS_LED_PIN 25
#define LED_CHANNEL 0
#define LED_FREQ 5000
#define LED_RESOLUTION 8 // map it from 0 to 255

// Potentiometere pin.
#define POT_PIN 34

//Buzzer Pin.
#define BUZZER_PIN 14

// Get Timestamp function.
String getTimeStamp(){
  struct tm timeInfo;
  if(!getLocalTime(&timeInfo)){
    return "1970-01-01T00:00:00";
  }

  char buff[25];
  strftime(buff, sizeof(buff), "%Y-%m-%dT-%H:%M:%S", &timeInfo);
  return String(buff);
}

// Send To Firebase.
/* Json format Ex:
 *  {
  "2025-07-15T14:28:35": {
    "temperature": 28.5,
    "humidity": 60,
    "led_state": "on"
  }
}
 */
void sendToFirebase(float temperature, float humidity, String ledState, int brightness){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi not connected. Skiping Firebase send.");
    return;
  }

  String timeStamp = getTimeStamp();

  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"led_state\":\"" + ledState + "\",";
  json += "\"led_brightness\":" + String(brightness);
  json += "}";

  // Sending Current Reading the sensor_readings node.
  String path1 = String(firebaseHost) + "sensor_readings/" + timeStamp + ".json";
  HTTPClient http1;
  http1.begin(path1);
  http1.addHeader("Content-Type", "application/json");

   int httpResponseCode1 = http1.PUT(json); // PUT is used for sending with timestamp key.

   Serial.println("Firebase Log Response: " + String(httpResponseCode1));
   http1.end();

  // Updating the Last response.
  String path2 = String(firebaseHost) + "latest.json";
  HTTPClient http2;
  http2.begin(path2);
  http2.addHeader("Content-Type", "application/json");

  // Add Time stamp to latest.
  String jsonLatest = json;
  jsonLatest.remove(jsonLatest.length() - 1); // Remove closing }
  jsonLatest += ",\"timestamp\":\"" + timeStamp + "\"}";

   int httpResponseCode2 = http2.PUT(jsonLatest); // PUT is used for sending with timestamp key.

   Serial.println("Firebase Letest Response: " + String(httpResponseCode2));
   http2.end();
}

// Controlling the Led Through the app.
void checkLedControl(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Faild to control Led - WiFi Disconnected...");
    return;
  }

  HTTPClient http;
  String path = String(firebaseHost) + "control/led_state.json";
  http.begin(path);

  int httpCode = http.GET();
  if(httpCode == 200){
    String payload = http.getString();
    payload.trim();
    Serial.println("LED Control payload: " + payload);

    if(payload.indexOf("on") > 0){
      digitalWrite(LED_PIN, HIGH);
    }else if(payload.indexOf("off") > 0){
      digitalWrite(LED_PIN, LOW);
    }
  }else {
    Serial.println("Faild to fetch LED control: " + String(httpCode));
  }

  http.end();
}

void checkBuzzerControl(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Faild To Fetch Buzzer Control, WiFi Disconnected....");
    return;
  }

  String path = String(firebaseHost) + "control/buzzer.json";
  HTTPClient http;
  http.begin(path);

  int httpCode = http.GET();
  if(httpCode == 200){
    String payload = http.getString();
    payload.trim();
    Serial.println("Buzzer control payload: " + payload);

    if(payload.indexOf("on") > 0){
      buzzerState = true;
    }else if(payload.indexOf("off") > 0){
      buzzerState = false;
    }
  }else {
    Serial.println("Faild to Fetch Buzzer conrol, Firbase Code: " + String(httpCode));
  }
  http.end();
}

void checkTempThresholdControl(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Faild to Get Temperature Threshold, WiFi Disconnected...");
    return;
  }

  HTTPClient http;
  String path = String(firebaseHost) + "control/temp_threshold.json";
  http.begin(path);

  int httpResponseCode = http.GET();
  if(httpResponseCode == 200){
    String payload = http.getString();
    payload.trim();
    Serial.println("Temp threshold payload: " + payload);

    tempThreshold = payload.toFloat();
  }else{
    Serial.println("Faild to get Temperature Threshold: " + String(httpResponseCode));
  }

  http.end();
}

// 📊 Handle /data (JSON response)
void handleData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    server.send(500, "application/json", "{\"error\":\"Failed to read from DHT sensor\"}");
    return;
  }

  String json = "{";
  json += "\"temperature\": " + String(temp, 1) + ",";
  json += "\"humidity\": " + String(hum, 1);
  json += "}";

  server.send(200, "application/json", json);
}

// 💡 Handle /led/on
void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "LED turned ON");
}

// 💡 Handle /led/off
void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "LED turned OFF");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 🧠 Init DHT and LED
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(BRIGHTNESS_LED_PIN, LED_FREQ, LED_RESOLUTION);                   // Attach pin to PWM
  pinMode(POT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // 🛜 Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  configTime(gmtOffset_sec, dayLightOffset_sec, ntpServer);
  Serial.println("Waiting for time sync...");
  delay(2000);

  // 🔗 Define routes
  server.on("/data", handleData);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // ▶️ Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  int potValue = analogRead(POT_PIN);
  int brightness = map(potValue, 0, 4095, 0, 255);
  ledcWrite(BRIGHTNESS_LED_PIN, brightness);
  
  if((millis() - lastSendTime) >= sendInterval){
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    String ledState = digitalRead(LED_PIN) ? "on": "off";
    if(!isnan(temp) && !isnan(humidity)){
      sendToFirebase(temp, humidity, ledState, brightness);
      if(temp > tempThreshold && buzzerState){
        digitalWrite(BUZZER_PIN, HIGH);
      }else{
        digitalWrite(BUZZER_PIN, LOW);
      }
      lastSendTime = millis();
    }
  }

  if (millis() - lastControlCheck >= controlCheckInterval) {
    checkLedControl();
    checkBuzzerControl();
    checkTempThresholdControl();
    
    lastControlCheck = millis();
  }


}
