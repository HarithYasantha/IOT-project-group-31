#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

const float CRASH_THRESHOLD = 13.00; // 2g = 19.6 m/s²

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

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

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Compute total acceleration vector magnitude
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  //Serial.printf("Accel: X=%.2f Y=%.2f Z=%.2f | Total=%.2f\n", ax, ay, az, totalAccel);

  if (totalAccel > CRASH_THRESHOLD) {
    Serial.println("🚨 Crash Detected!");
  }

  delay(500);
}
