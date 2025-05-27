#include <WiFi.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Wi-Fi credentials
#define WIFI_SSID "harith"
#define WIFI_PASSWORD "harith1021"

// Firebase credentials
#define API_KEY "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w"
#define DATABASE_URL "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app"

// Telegram bot info
const String BOT_TOKEN = "8143693558:AAHrdIHE6PPhYR532nBtw60dN56qjCEnbaw";
const String CHAT_ID = "6083299552";

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Accelerometer
Adafruit_MPU6050 mpu;
const float CRASH_THRESHOLD = 13.00;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to Wi-Fi");
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  config.token_status_callback = [](TokenInfo info) {
    Serial.printf("Token Status: %s\n", info.status == token_status_ready ? "Ready" : "Not ready");
  };

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("✔️ Firebase anonymous signup successful");
  } else {
    Serial.printf("❌ Signup failed: %s\n", config.signer.signupError.message.c_str());
  }
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN +
                 "/sendMessage?chat_id=" + CHAT_ID +
                 "&text=" + message;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("✅ Telegram message sent");
    } else {
      Serial.printf("❌ Telegram error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("❌ WiFi not connected");
  }
}

void setupMPU() {
  if (!mpu.begin()) {
    Serial.println("❌ Failed to find MPU6050 chip");
    while (1) delay(10);
  }

  Serial.println("✅ MPU6050 initialized");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  setupFirebase();
  setupMPU();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  if (totalAccel > CRASH_THRESHOLD) {
    Serial.println("🚨 Crash Detected!");

    // Send to Firebase
    String path = "/crashes/" + String(millis());
    Firebase.RTDB.setFloat(&fbdo, path + "/acceleration", totalAccel);
    Firebase.RTDB.setString(&fbdo, path + "/status", "Crash Detected");

    // Send Telegram message
    sendTelegramMessage("🚨 Crash Detected! Acceleration: " + String(totalAccel, 2));

    delay(10000); // wait to avoid spamming
  }

  delay(500);
}