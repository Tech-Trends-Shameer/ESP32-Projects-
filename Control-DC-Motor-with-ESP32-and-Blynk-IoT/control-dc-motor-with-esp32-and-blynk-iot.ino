#define BLYNK_PRINT Serial


#define BLYNK_TEMPLATE_ID "TMPL3XW9noTXk"
#define BLYNK_TEMPLATE_NAME "Control DC Motor with ESP32"
#define BLYNK_AUTH_TOKEN "ibA3EUu7-XQ3oCc07WcBTme8sddfwDYdnU1hdxU"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

 
char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password
 
int M1PWM = 22;
int M2PWM = 19;
int M1F = 2; //GPIO5
int M1R = 4; //GPIO4
int M2F = 12; //GPIO0
int M2R = 13; //GPIO2

int pinValue1;
int pinValue2;

BLYNK_WRITE(V1)
{
  int pinValue1 = param.asInt();
  analogWrite(M1PWM,pinValue1);
  Blynk.virtualWrite(V1,pinValue1);
  Serial.print("V1 Slider Value is ");
  Serial.println(pinValue1);
}

BLYNK_WRITE(V2)
{
  int pinValue2 = param.asInt();
  analogWrite(M2PWM,pinValue2);
  Blynk.virtualWrite(V1,pinValue2);
  Serial.print("V2 Slider Value is ");
  Serial.println(pinValue2);
}
 

void setup(){
  pinMode(M1PWM, OUTPUT);
  pinMode(M2PWM, OUTPUT);
  pinMode(M1F, OUTPUT);
  pinMode(M1R, OUTPUT);
  pinMode(M2F, OUTPUT);
  pinMode(M2R, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  
}

void loop(){
  Blynk.run();
 
}
