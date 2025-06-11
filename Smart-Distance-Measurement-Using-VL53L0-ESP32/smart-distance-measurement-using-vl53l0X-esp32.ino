#define BLYNK_TEMPLATE_ID "TMPL3zxwFvLQW"
#define BLYNK_TEMPLATE_NAME "Distance Measurement"
#define BLYNK_AUTH_TOKEN "kO6jE0z6VBPCsMWzSHOAa-x5Z1NA3gYd"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Galaxy F23 5G 3FF5";    
char pass[] = "edrp6836ehan"; 

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
BlynkTimer timer;

// Send distance to Blynk
void sendDistance() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
    Blynk.virtualWrite(V0, measure.RangeMilliMeter);
  } else {
    Serial.println("Out of range");
    Blynk.virtualWrite(V0, 0);
  }
}

void setup() {
  Serial.begin(115200);

  // ESP32 default I2C pins: SDA = 21, SCL = 22
  Wire.begin(21, 22);  

  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, sendDistance);  // every 1 second
}

void loop() {
  Blynk.run();
  timer.run();
}
