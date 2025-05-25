#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase credentials
#define FIREBASE_HOST "your-project-id.firebaseio.com"
#define FIREBASE_AUTH "your_firebase_database_secret"

// GPS module
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

// MPU6050
Adafruit_MPU6050 mpu;

// Firebase object
FirebaseData fbdo;

void setup() {
  Serial.begin(115200);
  
  // Start GPS module
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX pins
  
  // Start MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050");
    while (1);
  }

  // Connect WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected.");

  // Firebase begin
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // Simple crash detection logic
  float accMagnitude = sqrt(a.acceleration.x * a.acceleration.x +
                            a.acceleration.y * a.acceleration.y +
                            a.acceleration.z * a.acceleration.z);

  Serial.print("Accel Magnitude: ");
  Serial.println(accMagnitude);

  // Detect strong sudden acceleration
  if (accMagnitude > 30.0) {  // adjust threshold as needed
    Serial.println("🚨 Crash Detected!");

    if (gps.location.isValid()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();

      // Send to Firebase
      String path = "/alerts/" + String(millis());
      Firebase.setFloat(fbdo, path + "/latitude", lat);
      Firebase.setFloat(fbdo, path + "/longitude", lng);
      Firebase.setFloat(fbdo, path + "/acceleration", accMagnitude);
      Firebase.setString(fbdo, path + "/status", "Crash Detected");

      Serial.println("📡 Data Sent to Firebase.");
    } else {
      Serial.println("Waiting for valid GPS data...");
    }

    delay(5000);  // delay to avoid spamming
  }

  delay(1000);  // regular loop delay
}
