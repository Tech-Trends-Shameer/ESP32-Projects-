#define BLYNK_TEMPLATE_ID "TMPL3NGm4ded4694"
#define BLYNK_TEMPLATE_NAME "Real Time Light Monitor Dashboard"
#define BLYNK_AUTH_TOKEN "R1giLAsdrPHdtUbRcy5fg9A1AqvlSur-r0B-"

#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// ---- CONFIG ----
char ssid[]     = "WiFi Username";
char pass[]     = "WiFi Password";

// Light thresholds
#define TOO_DARK   200
#define TOO_BRIGHT 10000

// LCD setup (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
BH1750 lightMeter;
BlynkTimer timer;

// ---- Serial Monitor Print Function ----
void printSerialData(float lux, String status) {
  Serial.println("╔══════════════════════════════╗");
  Serial.println("║     LIGHT MONITOR READING    ║");
  Serial.println("╠══════════════════════════════╣");
  Serial.print  ("║  Lux Value  : ");
  Serial.print  (lux, 2);
  Serial.println(" lux");
  Serial.print  ("║  Status     : ");
  Serial.println(status);
  Serial.println("╠══════════════════════════════╣");

  // Light level bar indicator
  Serial.print("║  Level  : [");
  int bars = map(constrain(lux, 0, 10000), 0, 10000, 0, 20);
  for (int i = 0; i < 20; i++) {
    if (i < bars) Serial.print("█");
    else Serial.print("░");
  }
  Serial.println("]");

  // Light category
  Serial.print("║  Category   : ");
  if (lux < 100)        Serial.println("Night / Very Dark");
  else if (lux < 200)   Serial.println("Too Dark");
  else if (lux < 400)   Serial.println("Dim Light");
  else if (lux < 1000)  Serial.println("Normal Indoor");
  else if (lux < 5000)  Serial.println("Bright Indoor");
  else if (lux < 10000) Serial.println("Outdoor Light");
  else                  Serial.println("Very Bright!");

  // Alert messages
  Serial.println("╠══════════════════════════════╣");
  if (lux < TOO_DARK) {
    Serial.println("║  ALERT: Light too LOW!       ║");
    Serial.println("║  Blynk notification sent!    ║");
  } else if (lux > TOO_BRIGHT) {
    Serial.println("║  ALERT: Light too HIGH!      ║");
    Serial.println("║  Blynk notification sent!    ║");
  } else {
    Serial.println("║  Light level is NORMAL       ║");
  }

  Serial.println("╚══════════════════════════════╝");
  Serial.println();
}

// ---- Main Sensor Function ----
void sendLightData() {
  float lux = lightMeter.readLightLevel();

  // Determine status for LCD and Serial and Blynk
  String status;
  String lcdLine2;

  if (lux < 100) {
    status   = "Night/Very Dark";
    lcdLine2 = "Night/Very Dark";
  } else if (lux < 200) {
    status   = "Too Dark";
    lcdLine2 = "** Too Dark **";
  } else if (lux < 400) {
    status   = "Dim Light";
    lcdLine2 = "Dim Light";
  } else if (lux < 1000) {
    status   = "Normal Indoor";
    lcdLine2 = "Normal Indoor";
  } else if (lux < 5000) {
    status   = "Bright Indoor";
    lcdLine2 = "Bright Indoor";
  } else if (lux < 10000) {
    status   = "Outdoor Light";
    lcdLine2 = "Outdoor Light";
  } else {
    status   = "Very Bright!";
    lcdLine2 = "** Very Bright!*";
  }

  // ---- Serial Monitor Output ----
  printSerialData(lux, status);

  // ---- Update LCD ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light:");
  lcd.print(lux, 1);
  lcd.print(" lux");
  lcd.setCursor(0, 1);
  lcd.print(lcdLine2);

  // ---- Send to Blynk ----
  Blynk.virtualWrite(V0, lux);
  Blynk.virtualWrite(V1, status);

  // ---- Push Notifications ----
  if (lux < TOO_DARK) {
    Blynk.logEvent("low_light",
      "Light too low! Current: " + String(lux) + " lux");
  }
  if (lux > TOO_BRIGHT) {
    Blynk.logEvent("high_light",
      "Light too bright! Current: " + String(lux) + " lux");
  }
}

void setup() {
  Serial.begin(115200);

  // ---- Startup Serial Message ----
  Serial.println();
  Serial.println("╔══════════════════════════════╗");
  Serial.println("║   BH1750 LIGHT MONITOR v1.0  ║");
  Serial.println("║   ESP32 + Blynk + LCD        ║");
  Serial.println("╚══════════════════════════════╝");
  Serial.println();

  // ---- Initialize I2C ----
  Wire.begin(21, 22); // SDA=21, SCL=22
  Serial.println(">> I2C Initialized (SDA=21, SCL=22)");

  // ---- Initialize LCD ----
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Light Monitor ");
  lcd.setCursor(0, 1);
  lcd.print(" Initializing...");
  Serial.println(">> LCD Initialized OK");

  // ---- Initialize BH1750 ----
  if (lightMeter.begin()) {
    Serial.println(">> BH1750 Initialized OK");
  } else {
    Serial.println(">> BH1750 ERROR! Check wiring!");
  }

  // ---- Connect WiFi + Blynk ----
  Serial.print(">> Connecting to WiFi: ");
  Serial.println(ssid);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi ");
  lcd.setCursor(0, 1);
  lcd.print("Please wait.... ");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println(">> WiFi Connected OK");
  Serial.print  (">> IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println(">> Blynk Connected OK");
  Serial.println();
  Serial.println(">> Sending readings every 2 seconds...");
  Serial.println();

  // ---- Show Connected on LCD ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  WiFiConnected ");
  lcd.setCursor(0, 1);
  lcd.print(" Blynk Online ! ");
  delay(2000);
  lcd.clear();

  // ---- Start Timer ----
  timer.setInterval(2000L, sendLightData);
}

void loop() {
  Blynk.run();
  timer.run();
}
