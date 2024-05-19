//Tech Trends Shameer
//Temperature Monitor Using DS18B20 Sensor

#define BLYNK_TEMPLATE_ID "TMPL38XsT08Nc"
#define BLYNK_TEMPLATE_NAME "Temperature Monitor Using DS18B20 Sensor"
#define BLYNK_AUTH_TOKEN "J5qiBXJjEWsdfecGR8sdoyOpGC6-mhlmxdfgjlbVi"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20 4    //Connect to GPIO2 pin
OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);

char auth[] = BLYNK_AUTH_TOKEN;


char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password

BlynkTimer timer;

void sendSensor()
{
  // Request temperature to all devices on the data line
  sensor.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  //We can connect more than one IC on the same data wire. 0 refers to the first IC on the wire
  Serial.print(sensor.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensor.getTempFByIndex(0));
  int tempC=sensor.getTempCByIndex(0);
  int tempF=sensor.getTempFByIndex(0);
  delay(1000);
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, tempC);
    Blynk.virtualWrite(V1, tempF);
    delay(500);
}
void setup()
{   
  
   Serial.begin(9600);
  sensor.begin();

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);
 
  }

void loop()
{
  Blynk.run();
  timer.run();
 }
