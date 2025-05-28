#define IR_SENSOR_PIN 15  // Change to 4 or 19 if needed

void setup() {
  Serial.begin(9600);
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
  int helmetWorn = digitalRead(IR_SENSOR_PIN);

  if (helmetWorn == LOW) {
    Serial.println("✅ Helmet detected (IR proximity < 1cm)");
  } else {
    Serial.println("❌ Helmet not detected");
  }

  delay(1000);
}
