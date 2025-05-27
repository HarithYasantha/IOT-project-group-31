#include <WiFi.h>
#include <FirebaseESP32.h>

// WiFi credentials
#define WIFI_SSID "harith's iPhone"
#define WIFI_PASSWORD "harith1021"

// Firebase credentials
#define FIREBASE_HOST "crashalertsystem-1c13f.firebaseio.com"
#define FIREBASE_AUTH "yaAZORCdfBLtrrEJTqZt2w3erkMCfaVFbcNkx0Jh"

// Firebase object
FirebaseData fbdo;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Create dummy crash data
  String path = "/test_data/" + String(millis());
  float dummyLatitude = 6.9271;
  float dummyLongitude = 79.8612;
  float dummyAccel = 42.0;

  // Send dummy data to Firebase
  bool success = true;
  success &= Firebase.setFloat(fbdo, path + "/latitude", dummyLatitude);
  success &= Firebase.setFloat(fbdo, path + "/longitude", dummyLongitude);
  success &= Firebase.setFloat(fbdo, path + "/acceleration", dummyAccel);
  success &= Firebase.setString(fbdo, path + "/status", "Test Crash");

  if (success) {
    Serial.println("✅ Dummy data sent to Firebase.");
  } else {
    Serial.print("❌ Firebase Error: ");
    Serial.println(fbdo.errorReason());
  }

  delay(10000); // wait 10 seconds before next send
}
