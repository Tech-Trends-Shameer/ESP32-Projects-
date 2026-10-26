#define BLYNK_TEMPLATE_ID "TMPL3Rtg_1Ew1"
#define BLYNK_TEMPLATE_NAME "Wireless Anti Theft Alarm"
#define BLYNK_AUTH_TOKEN "aTTxajgs0MEW6mrNtuM2dRFbUUH2Zti3TrA"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

Adafruit_MPU6050 mpu;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "WiFi Username";
char pass[] = "WiFi Password";

// Set thresholds for sensitivity (lower values = more sensitive)
const float MOVEMENT_THRESHOLD = 1.5; 

void setup() {
  Serial.begin(115200);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  // Configure sensor ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate the total force magnitude minus gravity (~9.8 m/s^2)
  float totalAcceleration = sqrt(a.acceleration.x * a.acceleration.x + 
                                 a.acceleration.y * a.acceleration.y + 
                                 a.acceleration.z * a.acceleration.z);
  
  float motionForce = abs(totalAcceleration - 9.8);

  // If measured force crosses your threshold, trigger the IoT alert
  if (motionForce > MOVEMENT_THRESHOLD) {
    Serial.println("ALERT! Motion Detected!");
    
    // Sends a physical push notification to your phone via the Blynk app
    Blynk.logEvent("alarm_triggered", "Warning: Device has been moved!");
    
    delay(5000); // Cool-down to prevent spamming your phone
  }
  
  delay(100);
}
