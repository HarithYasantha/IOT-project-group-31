#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include <TinyGPS++.h>

// --- Wi-Fi Credentials ---
#define WIFI_SSID "harith"
#define WIFI_PASSWORD "harith1021"

// --- Firebase Credentials ---
#define API_KEY "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w"
#define DATABASE_URL "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app"

// --- Firebase Objects ---
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- MPU6050 Setup ---
Adafruit_MPU6050 mpu;
const float CRASH_THRESHOLD = 13.0; // m/s²

// --- GPS Setup ---
TinyGPSPlus gps;
#define GPS_RX 16
#define GPS_TX 17

// --- IR Sensor Setup ---
#define IR_PIN 19  // D19
bool helmetOn = false;

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

void setupGPS() {
  Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("✅ GPS initialized");
}

void setupIR() {
  pinMode(IR_PIN, INPUT);
  Serial.println("✅ IR sensor initialized (helmet detection)");
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  setupFirebase();
  setupMPU();
  setupGPS();
  setupIR();
}

void loop() {
  // --- Read IR Sensor ---
  helmetOn = digitalRead(IR_PIN) == LOW;  // Assuming LOW = object close
  if (!helmetOn) {
    Serial.println("⛔ Helmet not detected - skipping crash detection");
    delay(1000);
    return;
  }

  // --- Read MPU6050 ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float totalAccel = sqrt(a.acceleration.x * a.acceleration.x +
                          a.acceleration.y * a.acceleration.y +
                          a.acceleration.z * a.acceleration.z);

  // --- Read GPS ---
  while (Serial2.available()) {
    gps.encode(Serial2.read());
  }

  float latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
  float longitude = gps.location.isValid() ? gps.location.lng() : 0.0;

  // --- Crash Detected ---
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

    delay(10000); // Prevent rapid uploads
  }

  delay(500);
}
