#define BLYNK_TEMPLATE_ID "TMPL3KL1XytSn"
#define BLYNK_TEMPLATE_NAME "Temperature and Humidity Monitor Using ESP32"
#define BLYNK_AUTH_TOKEN "huPIsfRmPv-drw-iwvRGU7lv-RxLyhJIBfaH7e"

#define BLYNK_PRINT Serial
#include <WiFi.h> 
#include <BlynkSimpleEsp32.h>

#include <DHT.h>


char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password

BlynkTimer timer;


#define DHTPIN 4 //Connect Out pin to D4 in ESP32
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);


void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print("    Humidity : ");
    Serial.println(h);
}
void setup()
{   
  
  Serial.begin(115200);
  

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(100L, sendSensor);
 
  }

void loop()
{
  Blynk.run();
  timer.run();
 }
