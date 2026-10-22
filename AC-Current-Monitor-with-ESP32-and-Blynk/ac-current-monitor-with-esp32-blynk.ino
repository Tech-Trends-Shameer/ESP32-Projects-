/*
  AC/DC Current Monitor using ACS712 + ESP32 + I2C LCD + Blynk
  Tech Trends Shameer

  Same working logic as the Arduino Uno version (RMS current, noise-floor
  subtraction, auto-recalibration), adapted for ESP32:
    - ESP32 ADC is 12-bit (0-4095) and only tolerates 0-3.3V
    - ACS712 output (powered at 5V) swings up to ~5V, so it MUST go through
      a voltage divider before reaching the ESP32 pin - see wiring notes below.

  ---------------- WIRING: voltage divider (required) ----------------
  ACS712 OUT ---[ R1 = 10k ]---+---[ R2 = 20k ]--- GND
                                |
                          ESP32 GPIO34 (ADC1, input-only, safe pin)

  This divides the ACS712's 0-5V-ish output down to a safe 0-3.3V-ish range
  (ratio = R2/(R1+R2) = 0.667). The code below reconstructs the original
  sensor voltage mathematically, so all the current-calculation logic still
  works the same as before.
  Optional: add a 0.1uF ceramic capacitor from GPIO34 to GND to reduce noise.

  ---------------- WIRING: I2C LCD ----------------
  LCD SDA -> ESP32 GPIO21 (default)
  LCD SCL -> ESP32 GPIO22 (default)
  LCD VCC -> 3.3V or 5V (check your LCD backpack's rating)
  LCD GND -> GND

  ---------------- BLYNK SETUP ----------------
  1. In the Blynk app/console, create a new Template with one Datastream:
     Virtual Pin V0, type "Double", used for "Current (A)".
  2. Fill in BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, and BLYNK_AUTH_TOKEN
     below from your Blynk console.
  3. Add a Gauge or Value Display widget in the app bound to V0.
*/
#define BLYNK_TEMPLATE_ID "TMPL3edV1-sFs"
#define BLYNK_TEMPLATE_NAME "AC Current Monitor"
#define BLYNK_AUTH_TOKEN "c0pf5teZsdfgwNV0_vCtuiBSDG5Hg37Y-PDT"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "WiFi Username";
char pass[] = "WiFi Password";

BlynkTimer timer;

// ---------- I2C LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2); // change to 0x3F if 0x27 doesn't work

// ---------- ACS712 + divider setup ----------
const int ACS_PIN = 34;               // ADC1 pin - safe to use with WiFi active
const float ACS_SENSITIVITY = 66.0;   // mV/A -> 185 (5A), 100 (20A), 66 (30A)
const float SENSOR_VCC = 5.0;         // ACS712's own supply voltage
const float ADC_VREF = 3.3;           // ESP32 ADC reference
const int ADC_RESOLUTION = 4095;      // 12-bit ADC
const float DIVIDER_RATIO = 20000.0 / (10000.0 + 20000.0); // R2 / (R1 + R2)

float zeroCurrentVoltage = SENSOR_VCC / 2.0; // auto-calibrated in setup()
float noiseFloorCurrent = 0.0;               // auto-calibrated in setup()

// ---------- Auto-recalibration (fixes drift automatically) ----------
const float IDLE_RAW_THRESHOLD = 0.5;
const int IDLE_STREAK_NEEDED = 5;
const unsigned long AUTO_CAL_COOLDOWN = 10000UL;
int idleStreak = 0;
unsigned long lastAutoCalMillis = 0;

float lastCurrent = 0.0;

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // allows the full 0-3.3V range on this pin

  lcd.init();
  lcd.backlight();
  lcd.print("Current Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.clear();
  lcd.print("Calibrating...");
  Serial.println("Calibrating zero-current point, keep load OFF...");
  runFullCalibration();
  delay(1000);
  lcd.clear();

  // Push a fresh reading to Blynk every 1 second
  timer.setInterval(1000L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();

  float rawCurrent = readRawRMSCurrent();
  lastCurrent = applyNoiseCorrection(rawCurrent);

  if (rawCurrent < IDLE_RAW_THRESHOLD) {
    idleStreak++;
  } else {
    idleStreak = 0;
  }
  if (idleStreak >= IDLE_STREAK_NEEDED && (millis() - lastAutoCalMillis > AUTO_CAL_COOLDOWN)) {
    Serial.println("--- Auto-recalibrating (idle detected) ---");
    runFullCalibration();
    lastAutoCalMillis = millis();
    idleStreak = 0;
  }

  lcd.setCursor(0, 0);
  lcd.print("Current:        ");
  lcd.setCursor(9, 0);
  lcd.print(lastCurrent, 2);
  lcd.print("A   ");

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(lastCurrent < 0.05 ? "No Load   " : "Load ON   ");

  Serial.print("Current: ");
  Serial.print(lastCurrent, 3);
  Serial.print(" A  |  Status: ");
  Serial.println(lastCurrent < 0.05 ? "No Load" : "Load ON");

  delay(300);
}

void sendToBlynk() {
  Blynk.virtualWrite(V0, lastCurrent);
}

void runFullCalibration() {
  calibrateZeroPoint();
  Serial.print("Zero-current voltage: ");
  Serial.print(zeroCurrentVoltage, 3);
  Serial.println(" V");

  noiseFloorCurrent = readRawRMSCurrent();
  Serial.print("Measured noise floor: ");
  Serial.print(noiseFloorCurrent, 3);
  Serial.println(" A");
}

// Averages many samples with NO load connected to find the true
// zero-current voltage, at the ACTUAL sensor output (after undoing the
// voltage divider math).
void calibrateZeroPoint() {
  long sum = 0;
  const int samples = 2000;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(ACS_PIN);
    delayMicroseconds(200);
  }
  float avgADC = (float)sum / samples;
  float adcVoltage = (avgADC * ADC_VREF) / ADC_RESOLUTION;
  zeroCurrentVoltage = adcVoltage / DIVIDER_RATIO; // undo the divider to get the real sensor voltage
}

// Samples over ~200ms and returns the RAW RMS current (no noise subtraction yet).
float readRawRMSCurrent() {
  const unsigned long sampleWindow = 200000UL;
  unsigned long startTime = micros();
  double sumSquares = 0;
  unsigned int sampleCount = 0;

  while (micros() - startTime < sampleWindow) {
    int adcValue = analogRead(ACS_PIN);
    float adcVoltage = (adcValue * ADC_VREF) / ADC_RESOLUTION;
    float sensorVoltage = adcVoltage / DIVIDER_RATIO; // reconstruct the real ACS712 output voltage
    float instCurrent = (sensorVoltage - zeroCurrentVoltage) / (ACS_SENSITIVITY / 1000.0);
    sumSquares += (double)instCurrent * instCurrent;
    sampleCount++;
  }

  float meanSquare = sumSquares / sampleCount;
  return sqrt(meanSquare);
}

// Removes the known noise floor (RMS values combine as sqrt(a^2 + b^2)).
float applyNoiseCorrection(float rawCurrent) {
  float adjusted = (rawCurrent * rawCurrent) - (noiseFloorCurrent * noiseFloorCurrent);
  float corrected = (adjusted > 0) ? sqrt(adjusted) : 0.0;
  if (corrected < 0.03) corrected = 0.0;
  return corrected;
}
