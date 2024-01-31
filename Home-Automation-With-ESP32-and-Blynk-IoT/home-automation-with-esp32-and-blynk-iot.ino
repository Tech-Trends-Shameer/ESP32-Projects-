#define BLYNK_TEMPLATE_ID "TMPL3VSSG11Y5"
#define BLYNK_TEMPLATE_NAME "Home Automation Using ESP32"
#define BLYNK_AUTH_TOKEN "Z7EJiKisERugdtddJK3wTsgde5wSWghfggscsIRqEN"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password


BlynkTimer timer;

#define relay1_pin 18
#define relay2_pin 19
#define relay3_pin 22
#define relay4_pin 23

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;
int relay4_state = 0;

#define button1_vpin V1
#define button2_vpin V2
#define button3_vpin V3
#define button4_vpin V4

BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
  Blynk.syncVirtual(button3_vpin);
  Blynk.syncVirtual(button4_vpin);
}

BLYNK_WRITE(button1_vpin) {
  relay1_state = param.asInt();
  digitalWrite(relay1_pin, relay1_state);
}

BLYNK_WRITE(button2_vpin) {
  relay2_state = param.asInt();
  digitalWrite(relay2_pin, relay2_state);
}

BLYNK_WRITE(button3_vpin) {
  relay3_state = param.asInt();
  digitalWrite(relay3_pin, relay3_state);
}

BLYNK_WRITE(button4_vpin) {
  relay4_state = param.asInt();
  digitalWrite(relay4_pin, relay4_state);
}

void setup() {
  Serial.begin(115200);

  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  pinMode(relay4_pin, OUTPUT);

  digitalWrite(relay1_pin, HIGH);
  digitalWrite(relay2_pin, HIGH);
  digitalWrite(relay3_pin, HIGH);
  digitalWrite(relay4_pin, HIGH);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  timer.run();
}
