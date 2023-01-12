#define BLYNK_TEMPLATE_ID "TMPL4dGjjY5l1G"
#define BLYNK_DEVICE_NAME "Control FAN ESP32"
#define BLYNK_AUTH_TOKEN "sWJdiSLM8H_8oNDoWmrkfgjhkkFSFyMysJOy85"


#define BLYNK_PRINT Serial
#include <WiFi.h> 
 
#include <BlynkSimpleEsp32.h>
 

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password

int relaypin = 4;
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
