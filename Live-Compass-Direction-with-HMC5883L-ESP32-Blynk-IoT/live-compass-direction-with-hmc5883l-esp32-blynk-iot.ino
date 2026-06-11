/*
 * ============================================================
 *  ESP32 + GY-273 HMC5883L — Blynk IoT Compass Dashboard
 * ============================================================
 *  YouTube: Tech Trends Shameer
 *  Sensor : GY-273 (HMC5883L) via I2C
 *  Platform: Blynk IoT 2.0
 *
 *  Blynk Virtual Pins:
 *    V0 → Gauge widget    (Heading 0–360°)
 *    V1 → Label widget    (Cardinal direction: N/NE/E/SE/S/SW/W/NW)
 *    V2 → Label widget    (Raw heading value like "247.5°")
 *
 *  Wiring:
 *    GY-273 VCC → ESP32 3.3V
 *    GY-273 GND → ESP32 GND
 *    GY-273 SDA → ESP32 GPIO21
 *    GY-273 SCL → ESP32 GPIO22
 *
 *  Libraries required (install via Library Manager):
 *    - Blynk (by Volodymyr Shymanskyy) v1.3.2+
 *    - Adafruit HMC5883 Unified (by Adafruit)
 *    - Adafruit Unified Sensor (by Adafruit)
 * ============================================================
 */


#define BLYNK_TEMPLATE_ID "TMPL3wwKoNI6q"
#define BLYNK_TEMPLATE_NAME "Live Compass Dashboard"
#define BLYNK_AUTH_TOKEN "HkpTsWzqRtO7m3o7ajuAfuxljm5nKdsrvQswm"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// ─── WiFi credentials ───────────────────────────────────────
const char* ssid     = "WiFi Username";             // <-- Replace
const char* password = "WiFi Password";         // <-- Replace

// ─── Sensor object ──────────────────────────────────────────
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// ─── Blynk timer ────────────────────────────────────────────
BlynkTimer timer;

// ─── Calibration offsets (set after hard-iron calibration) ──
// Rotate sensor in all directions, note min/max X and Y, then:
//   offsetX = (maxX + minX) / 2
//   offsetY = (maxY + minY) / 2
float offsetX = 0.0;
float offsetY = 0.0;

// ─── Cardinal direction from heading ────────────────────────
String getCardinal(float heading) {
  if (heading >= 337.5 || heading < 22.5)   return "N";
  else if (heading < 67.5)                  return "NE";
  else if (heading < 112.5)                 return "E";
  else if (heading < 157.5)                 return "SE";
  else if (heading < 202.5)                 return "S";
  else if (heading < 247.5)                 return "SW";
  else if (heading < 292.5)                 return "W";
  else                                      return "NW";
}

// ─── Read sensor and send to Blynk ──────────────────────────
void sendCompassData() {
  sensors_event_t event;
  mag.getEvent(&event);

  // Apply hard-iron calibration offsets
  float x = event.magnetic.x - offsetX;
  float y = event.magnetic.y - offsetY;

  // Calculate heading in degrees (0–360)
  float heading = atan2(y, x) * 180.0 / PI;
  if (heading < 0) heading += 360.0;

  String cardinal = getCardinal(heading);

  // Send to Blynk
  Blynk.virtualWrite(V0, heading);                           // Gauge
  Blynk.virtualWrite(V1, cardinal);                          // Cardinal label
 // Blynk.virtualWrite(V2, String(heading, 1) + (char)176);   // "247.5°"

Blynk.virtualWrite(V2, heading);   // "247.5°"

  // Debug output
  Serial.print("Heading: ");
  Serial.print(heading, 1);
  Serial.print("°  →  ");
  Serial.println(cardinal);
}

// ─── Setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n===== ESP32 Compass Dashboard =====");

  // Initialise I2C sensor
  if (!mag.begin()) {
    Serial.println("ERROR: HMC5883L not detected. Check wiring!");
    while (1);
  }
  Serial.println("HMC5883L detected OK");

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Send data every 500ms
  timer.setInterval(500L, sendCompassData);

  Serial.println("Setup complete. Streaming compass data...");
}

// ─── Loop ───────────────────────────────────────────────────
void loop() {
  Blynk.run();
  timer.run();
}
