#define MQ_ANALOG_PIN 19  // A0 -> D19 (ADC)
#define MQ_DIGITAL_PIN 18 // D0 -> D18 (Digital)

// Optional: Threshold to print alerts
#define DANGEROUS_LEVEL 600  // You can adjust this based on your sensor's response

void setup() {
  Serial.begin(115200);

  pinMode(MQ_DIGITAL_PIN, INPUT);
  analogReadResolution(10); // Optional: 10-bit resolution = 0–1023
  delay(1000);

  Serial.println("🔥 MQ Sensor Full Test Starting...");
}

void loop() {
  int analogValue = analogRead(MQ_ANALOG_PIN);
  int digitalState = digitalRead(MQ_DIGITAL_PIN);

  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print(" | Digital Pin: ");
  Serial.print(digitalState == LOW ? "⚠️ GAS DETECTED" : "✅ Safe");

  if (analogValue > DANGEROUS_LEVEL) {
    Serial.print(" | Alert: HIGH GAS LEVEL!");
  }

  Serial.println();
  delay(1000);
}
