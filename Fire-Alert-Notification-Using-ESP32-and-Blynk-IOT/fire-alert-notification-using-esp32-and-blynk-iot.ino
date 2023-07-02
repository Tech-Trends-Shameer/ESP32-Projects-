#define BLYNK_TEMPLATE_ID "TMPL39xKvgzc1"
#define BLYNK_TEMPLATE_NAME "Fire Alert Notification"
#define BLYNK_AUTH_TOKEN "C1yRkGQ7-8q7RNq0fxAuC3tyimLdGZokdfjkfwv"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

 
char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password
 
#define FLAME_SENSOR  4
BlynkTimer timer;
int flag=0; 
void sendSensor(){ 
   int isButtonPressed = digitalRead(FLAME_SENSOR);
  if (isButtonPressed==0 && flag==0) {
    Serial.println("Fire in the House");  
    Blynk.logEvent("fire_alert","Fire Detected");
    
    flag=1;
  }
  else if (isButtonPressed==1)
  {
    flag=0;
    
  }
 
}

void setup(){
  pinMode(FLAME_SENSOR, INPUT); 
   Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, sendSensor);
}

void loop(){
  Blynk.run();
  timer.run();
}
