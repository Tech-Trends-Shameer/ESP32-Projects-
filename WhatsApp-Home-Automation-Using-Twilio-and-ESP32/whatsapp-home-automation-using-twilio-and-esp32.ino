//Tech Trends Shameer

#include <WiFi.h>
#include <ThingESP.h>

ThingESP32 thing("techtrends", "ControlESP8266", "techtrends");

int LIGHT = 23;

unsigned long previousMillis = 0;
const long INTERVAL = 6000;  

void setup()
{
  Serial.begin(115200);
  pinMode(LIGHT, OUTPUT);
  thing.SetWiFi("Wifi Username", "Wifi Password");
  thing.initDevice();
}


String HandleResponse(String query)
{
  if (query == "light on") {
    digitalWrite(LIGHT, 1);
    return "Done: Light Turned ON";
  }

  else if (query == "light off") {
    digitalWrite(LIGHT, 0);
    return "Done: Light Turned OFF";
  }

  else if (query == "light status")
    return digitalRead(LIGHT) ? "LIGHT is OFF" : "LIGHT is ON";
  else return "Your query was invalid..";

}


void loop()
{
  thing.Handle();

}
