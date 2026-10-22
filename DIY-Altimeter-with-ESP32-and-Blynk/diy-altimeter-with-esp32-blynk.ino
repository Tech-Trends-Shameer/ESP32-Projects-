/*
  ESP32 + MS5611 — DIY Altimeter with LCD + Blynk Dashboard

  Libraries needed (Library Manager):
    - MS5611 by Rob Tillaart
    - LiquidCrystal_I2C
    - Blynk (Blynk IoT / Blynk.Cloud latest version)

  Wiring (shared I2C bus, ESP32 default pins):
    MS5611 VCC -> 3.3V        LCD VCC -> 5V
    MS5611 GND -> GND         LCD GND -> GND
    MS5611 SDA -> GPIO 21     LCD SDA -> GPIO 21
    MS5611 SCL -> GPIO 22     LCD SCL -> GPIO 22
    MS5611 CSB -> 3.3V
    MS5611 PS  -> 3.3V
    Pull-ups (4.7k-5.6k) to 3.3V on SDA/SCL if your module needs them

  Blynk setup:
    1. Create a template in Blynk.Cloud
    2. Add Datastreams:
         V0 - Temperature (Double, °C)
         V1 - Pressure    (Double, hPa)
         V2 - Altitude    (Double, m)
    3. Add matching widgets (Gauge/Value Display/Chart) for each
    4. Fill in Template ID, Template Name, Auth Token below
*/
#define BLYNK_TEMPLATE_ID "TMPL3aqsEGznZ"
#define BLYNK_TEMPLATE_NAME "DIY Altimeter"
#define BLYNK_AUTH_TOKEN "vHkuNI3sdDn1HFCS$dfshkkjM_-wgYao6LZ44rU10OH"

#include <Wire.h>
#include <MS5611.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "WiFi Username";
char pass[] = "WiFi Password";

MS5611 MS5611(0x77);
LiquidCrystal_I2C lcd(0x27, 16, 2); // change to 0x3F if needed

byte degreeSymbol[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

const float SEA_LEVEL_PRESSURE = 1013.25;
const unsigned long READ_INTERVAL_MS = 1000;
unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);
  lcd.setCursor(0, 0);
  lcd.print("MS5611 Init...");

  if (!MS5611.begin()) {
    Serial.println("MS5611 not found. Check wiring.");
    lcd.setCursor(0, 1);
    lcd.print("Sensor not found");
    while (1) delay(10);
  }
  MS5611.setOversampling(OSR_ULTRA_HIGH);
  lcd.clear();
   
  lcd.setCursor(3, 0);
  lcd.print("Tech Trends");
  lcd.setCursor(5, 1);
  lcd.print("Shameer");
  delay(1500);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  Blynk.run();

  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();
    updateReadings();
  }
}

float calculateAltitude(float pressure, float seaLevelPressure) {
  return 44330.0 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

void updateReadings() {
  int result = MS5611.read();
  if (result != MS5611_READ_OK) {
    Serial.print("Read error: ");
    Serial.println(result);
    lcd.setCursor(0, 0);
    lcd.print("Sensor read err ");
    return;
  }

  float temperature = MS5611.getTemperature();
  float pressure     = MS5611.getPressure();
  float altitude      = calculateAltitude(pressure, SEA_LEVEL_PRESSURE);

  // --- Serial debug ---
  Serial.print("T: ");
  Serial.print(temperature, 2);
  Serial.print(" C  P: ");
  Serial.print(pressure, 2);
  Serial.print(" hPa  Alt: ");
  Serial.print(altitude, 2);
  Serial.println(" m");

  // --- LCD ---
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.write((byte)0);
  lcd.print("C P:");
  lcd.print(pressure, 0);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("Alt: ");
  lcd.print(altitude, 2);
  lcd.print(" m   ");

  // --- Blynk ---
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, pressure);
  Blynk.virtualWrite(V2, altitude);
}
