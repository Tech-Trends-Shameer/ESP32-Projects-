#define BLYNK_TEMPLATE_ID "TMPL3xXaLaFM_"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System with ESP32"
#define BLYNK_AUTH_TOKEN "MAgifsJ32VZovXsdfygeFY6IWfdhFLsLjfgjk7GbpUb"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h> 

#define SOIL_MOISTURE_PIN 34   // Analog pin for soil moisture sensor
#define THRESHOLD_MOISTURE 100 // Adjust this value based on your soil moisture sensor readings
#define PUMP_PIN 2  // Digital pin for controlling the pump
#define PUMP_SWITCH V6  // Virtual pin for controlling the pump manually

char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk auth token
char ssid[] = "WiFi Username";   // Replace with your WiFi credentials
char pass[] = "WiFi Password";

BlynkTimer timer;

bool isPumpOn = false;  // Variable to track pump status

void sendSensorData()
{
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  Serial.print("Soil Moisture ");
  Serial.println(soilMoisture);
  // Map the analog sensor values to a percentage (0-100) 
  int soilMoisturePercentage = map(soilMoisture, 3500, 4095, 100, 0);
  Serial.print("Soil Moisture Percentage ");
  Serial.println(soilMoisturePercentage);
  // Send soil moisture data to Blynk
  Blynk.virtualWrite(V5, soilMoisturePercentage);

  // Check if the pump should be on based on manual switch or soil moisture level
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE)
  {
    // Turn on the pump
    digitalWrite(PUMP_PIN, HIGH);
    // Check if the pump is turned on automatically (not manually)
    if (!isPumpOn) {
      // Send alert notification to Blynk app if the pump is turned on automatically
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!");
      Serial.println("Soil moisture is below the threshold!");
    }
  }
  else
  {
    // Turn off the pump only if it was not turned on manually
    if (!isPumpOn) {
      digitalWrite(PUMP_PIN, LOW);
    }
  }
}

BLYNK_WRITE(PUMP_SWITCH)
{
  isPumpOn = param.asInt();
  if (isPumpOn) {
    Serial.println("Pump manually turned ON");
  } else {
    Serial.println("Pump manually turned OFF");
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT); // Set pump pin as an output

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(3000L, sendSensorData); // Set the interval for checking soil moisture (every 3 seconds)

  // Setup switch widget
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn);
  Blynk.syncVirtual(PUMP_SWITCH);
}

void loop()
{
  Blynk.run();
  timer.run();
}
