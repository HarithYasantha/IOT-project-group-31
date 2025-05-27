#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Replace with your Wi-Fi credentials
#define WIFI_SSID "harith"
#define WIFI_PASSWORD "harith1021"

// Replace with your Firebase credentials
#define API_KEY "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w"
#define DATABASE_URL "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app"  // e.g. https://project-id-default-rtdb.asia-southeast1.firebasedatabase.app

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Optional: print token status updates
  config.token_status_callback = [](TokenInfo info) {
    Serial.printf("Token Status: %s\n", info.status == token_status_ready ? "Ready" : "Not ready");
  };

  // Start Firebase with config and auth
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Use anonymous signup
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("✔️ Firebase anonymous signup successful");
  } else {
    Serial.printf("❌ Signup failed: %s\n", config.signer.signupError.message.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  setupFirebase();
}

void loop() {
  if (Firebase.RTDB.setInt(&fbdo, "/test/value", millis())) {
    Serial.println("✅ Data uploaded");
  } else {
    Serial.printf("❌ Upload failed: %s\n", fbdo.errorReason().c_str());
  }
  delay(5000); // every 5 seconds
}
