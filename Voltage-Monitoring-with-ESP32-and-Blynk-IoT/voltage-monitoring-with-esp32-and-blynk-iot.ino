#define BLYNK_TEMPLATE_ID "TMPL3k-B39Oiw"
#define BLYNK_TEMPLATE_NAME "AC Voltmeter"
#define BLYNK_AUTH_TOKEN "4vGeHNxEpeF4hSwvmrMZBwKwVo02T2XM"


#define BLYNK_PRINT Serial
 
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
 
// Constants for calibration
const float vCalibration = 175;

// Blynk and WiFi credentials

char auth[] = BLYNK_AUTH_TOKEN; // Blynk authentication token
const char* ssid = "Galaxy F23 5G 3FF5"; // Your WiFi network SSID
const char* pass = "edrp6836ehan"; // Your WiFi network password
 
// EnergyMonitor instance
EnergyMonitor emon;
 
// Timer for regular updates
BlynkTimer timer; 

unsigned long lastMillis = millis();
  
void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
 
  // Initialize the LCD
   lcd.begin();    
 // Display "Tech Trends Shameer"
  lcd.setCursor(3, 0);
  lcd.print("Tech Trends ");
  lcd.setCursor(3, 1);
  lcd.print("  Shameer    ");
  delay(3000);  // Wait for 3 seconds

    // Clear the LCD and display "AC Voltmeter"
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("AC Voltmeter"); 
  delay(2000);
 
  // Setup voltage and current inputs
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift  
 
  // A small delay for system to stabilize
  delay(1000);
}
 
 
void loop()
{
  emon.calcVI(25, 1000);       // Calculate all. No. of half wavelengths (crossings), time-out
  
  float supplyVoltage = emon.Vrms;  // Extract Vrms into a variable
  
  lcd.clear();  // Clear the LCD display
  
  if (supplyVoltage >= 100)
  {
    lcd.setCursor(0, 0);
    lcd.print("Voltage: ");
    lcd.print(supplyVoltage);
    lcd.print(" V");
    Serial.print("Voltage : ");
    Serial.println(supplyVoltage);
    Blynk.virtualWrite(V1, supplyVoltage); // Send voltage value to Blynk virtual pin V1
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Power OFF");
    Serial.println("Power OFF");
    Blynk.virtualWrite(V1, supplyVoltage);
  }

  delay(1000);  // Delay before taking the next reading
  Blynk.run(); // Run Blynk
}

 

 
 
 
