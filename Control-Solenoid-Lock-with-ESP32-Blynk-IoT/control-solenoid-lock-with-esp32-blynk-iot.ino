#define BLYNK_TEMPLATE_ID "TMPL35o_skIt8"
#define BLYNK_TEMPLATE_NAME "Control Solenoid Door Lock"
#define BLYNK_AUTH_TOKEN "zjjgCEoi3XoOok7tKNWv8RhsFfaRydT5IBSw"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "WiFi Username";    
char pass[] = "WiFi Password"; 

#define SOLENOID_PIN 26  // GPIO pin connected to Relay or MOSFET

BLYNK_WRITE(V1) {  
  int value = param.asInt();
  digitalWrite(SOLENOID_PIN, value); // Unlock (HIGH) / Lock (LOW)
}

void setup() {
  Serial.begin(115200);
  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, LOW);  // Start locked

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();  
}
