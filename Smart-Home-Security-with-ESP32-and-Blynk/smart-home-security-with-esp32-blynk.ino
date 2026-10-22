/*
 * DIY Smart Home Safety Hub (Gas + Smoke + Fire + Climate)
 * Fully Fixed Character Variable Declarations for ESP32 Compiler Stability
 */
 
#define BLYNK_TEMPLATE_ID "TMPL35D_fmz3V"
#define BLYNK_TEMPLATE_NAME "Smart Home Security"
#define BLYNK_AUTH_TOKEN "Kx7W6sd7VeM8RYfgWWjhgjluoDtNGh_gDLmwPS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PIN ASSIGNMENTS
#define DHTPIN        23
#define DHTTYPE       DHT22 
#define MQ_PIN        34
#define FLAME_PIN     18    
#define BUZZER_PIN    25
#define LED_PIN       26

// CALIBRATION THRESHOLD (Updated to your optimal room calculation)
const int GAS_THRESHOLD = 400; 

// NETWORK PROFILE (FIXED WITH STRING POINTERS)
const char* auth = BLYNK_AUTH_TOKEN;
const char* ssid = "WiFi Username";     
const char* pass = "WiFi Password"; 

// INSTANCE DECLARATIONS
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
BlynkTimer timer;

void readAndDisplaySensors() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int gasVal = analogRead(MQ_PIN);
  
  bool fireDetected = (digitalRead(FLAME_PIN) == LOW); 

  Serial.println("\n-------------------------------------------");
  Serial.println("[SYSTEM DATA STATUS LOG]");

  if (isnan(hum) || isnan(temp)) {
    Serial.println("❌ ERROR: Could not fetch valid DHT data from sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    return;
  }

  Serial.print("🌡️ Temperature: "); Serial.print(temp, 1); Serial.println(" °C");
  Serial.print("💧 Humidity:    "); Serial.print(hum, 0);  Serial.println(" %");
  Serial.print("💨 Gas Level:   "); Serial.print(gasVal);    Serial.print(" / 4095 (Threshold: "); Serial.print(GAS_THRESHOLD); Serial.println(")");
  Serial.print("🔥 Flame Sensor: "); Serial.println(fireDetected ? "DETECTED! [LOW]" : "SAFE [HIGH]");

  // PUSH CHANNELS TO BLYNK SERVER
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);
  Blynk.virtualWrite(V3, gasVal);
  Blynk.virtualWrite(V5, fireDetected ? 1 : 0);

  // EVALUATE THREAT LEVEL
  if (fireDetected || gasVal > GAS_THRESHOLD) {
    Serial.println("⚠️ EMERGENCY CONDITION CRITERIA MET!");
    
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Blynk.virtualWrite(V4, 1); 
    
    if (fireDetected && gasVal > GAS_THRESHOLD) {
      Serial.println("🚨 CRITICAL: DOUBLE THREAT DETECTED!");
      Blynk.logEvent("gas_leak_detected", "🚨 CRITICAL: BOTH FIRE & GAS LEAK DETECTED!");
    }
    else if (fireDetected) {
      Serial.println("🚨 ALERT: Fire emergency routing. Pushing notification...");
      Blynk.logEvent("gas_leak_detected", "🚨 FIRE EMERGENCY! Flame detected!");
    } else {
      Serial.println("🚨 ALERT: Gas Leak emergency routing. Pushing notification...");
      Blynk.logEvent("gas_leak_detected", "⚠️ GAS LEAK! Dangerous smoke/gas levels.");
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    if (fireDetected) {
      lcd.print("!! FIRE ALERT !!");
      lcd.setCursor(0, 1);
      lcd.print("EVACUATE NOW!");
    } else {
      lcd.print("!!! DANGER !!!");
      lcd.setCursor(0, 1);
      lcd.print("GAS LEAK: ");
      lcd.print(gasVal);
    }
  } 
  else {
    Serial.println("🟢 STATUS: Everything is secure.");
    
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Blynk.virtualWrite(V4, 0);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C  H:");
    lcd.print(hum, 0);
    lcd.print("%");
    
    lcd.setCursor(0, 1);
    lcd.print("Gas: ");
    lcd.print(gasVal);
    lcd.print(" | SAFE");
  }
  Serial.println("-------------------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  Serial.println("\n===========================================");
  Serial.println("⚡ Starting Ultimate Smart Safety Hub v3.0 Setup... ⚡");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FLAME_PIN, INPUT); 
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  lcd.setCursor(0, 1);
  lcd.print("Ultimate Hub v3");

  dht.begin();
  
  Serial.println("🌐 Connecting to Wi-Fi and syncing with Blynk Cloud...");
  Blynk.begin(auth, ssid, pass);
  
  lcd.clear();
  Serial.println("🚀 System running successfully!");
  Serial.println("===========================================");

  timer.setInterval(2500L, readAndDisplaySensors);
}

void loop() {
  Blynk.run();
  timer.run();
}
