#define BLYNK_TEMPLATE_ID "TMPL3Kl4MBcEl"
#define BLYNK_TEMPLATE_NAME "Environment Monitoring System"
#define BLYNK_AUTH_TOKEN "yw6KwyHTdr4uQZcCF36SOYq5Cb345nOaoHjvGIi"

#define BLYNK_PRINT Serial
#include <WiFi.h> 
#include <BlynkSimpleEsp32.h>

#include <DHT.h>


char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password

BlynkTimer timer;


#define DHTPIN 4 //Connect Out pin to D4 in ESP32
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
#define SensorPin 34 //Connect Out pin to D4 in ESP32
const int dryValue  = 2910;   //you need to replace this value with Value_1
const int wetValue = 1465;  //you need to replace this value with Value_2
//const int SensorPin = 34;

void sendSensor()
{
  /*int soilmoisturevalue = analogRead(A0);
   soilmoisturevalue = map(soilmoisturevalue, 0, 1023, 0, 100);*/
   int value = analogRead(SensorPin);
  //value = map(value,400,1023,100,0);
   int soilMoisturePercent  = map(value, dryValue, wetValue, 0, 100);
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, soilMoisturePercent);
    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, h);
    Serial.print("Soil Moisture : ");
    Serial.print(soilMoisturePercent);
    Serial.print("   Temperature : ");
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
