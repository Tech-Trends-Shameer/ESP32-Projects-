//Tech Trends Shameer
//Control Water Pump Using Blynk 2.0

#define BLYNK_TEMPLATE_ID "TMPL3QwCB4K0S"
#define BLYNK_TEMPLATE_NAME "Control Pump with ESP32"
#define BLYNK_AUTH_TOKEN "dm55SBZ9PF_cf7sgCiPGK55edjhghjSXBgfhkt2zhv"


#define BLYNK_PRINT Serial
#include <WiFi.h> 
 
#include <BlynkSimpleEsp32.h>
 

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // Enter your wifi name
char pass[] = "WiFi Password";  // Enter your wifi password

int relaypin = 4;  //Connect to Pin D4 in ESP32
void setup()
{     
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);    
  pinMode(relaypin,OUTPUT);
 
}

void loop()
{
  Blynk.run(); 
 }
