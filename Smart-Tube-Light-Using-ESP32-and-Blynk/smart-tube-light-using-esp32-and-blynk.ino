#define BLYNK_TEMPLATE_ID "TMPL3YWAs8cXL"
#define BLYNK_TEMPLATE_NAME "Control LED Tube Light Using ESP 32"
#define BLYNK_AUTH_TOKEN "9qKwShAGgJnAfdgkllOQx-m1uCstzvIn0nTp4D"

#define BLYNK_PRINT Serial
#include <WiFi.h> 
 
#include <BlynkSimpleEsp32.h>
 

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password

int relaypin = 2;
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
