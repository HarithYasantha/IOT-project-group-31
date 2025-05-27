#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// Replace with your Wi-Fi credentials
#define WIFI_SSID "harith"
#define WIFI_PASSWORD "harith1021"

// Replace with your Firebase credentials
#define API_KEY "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w"
#define DATABASE_URL "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// MPU6050
Adafruit_MPU6050 mpu;
const float CRASH_THRESHOLD = 13.00; // m/s²

// Crash location (dummy - replace with GPS later)
float latitude = 6.9271;
float longitude = 79.8612;

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
    Serial.printf("❌ Firebase signup failed: %s\n", config.signer.signupError.message.c_str());
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
  //Serial.printf("Accel: %.2f m/s²\n", totalAccel);

  if (totalAccel > CRASH_THRESHOLD) {
    Serial.println("🚨 Crash Detected!");

    String path = "/crashes/" + String(millis());

    bool success = true;
    success &= Firebase.RTDB.setFloat(&fbdo, path + "/acceleration", totalAccel);
    success &= Firebase.RTDB.setFloat(&fbdo, path + "/latitude", latitude);
    success &= Firebase.RTDB.setFloat(&fbdo, path + "/longitude", longitude);
    success &= Firebase.RTDB.setString(&fbdo, path + "/status", "Crash Detected");

    if (success) {
      Serial.println("✅ Crash data uploaded to Firebase");
    } else {
      Serial.printf("❌ Firebase upload failed: %s\n", fbdo.errorReason().c_str());
    }

    delay(10000); // prevent spamming Firebase
  }

  delay(500);
}
