#define BLYNK_TEMPLATE_ID "TMPL3hcslF41i"
#define BLYNK_TEMPLATE_NAME "Gas Leakage Notification"
#define BLYNK_AUTH_TOKEN "PX8K-cCupwFDdsfPZiiFjhyUqDrklPgMKqklbMHot"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h> //LCD DISPLAY

int lcdColumns = 16;
int lcdRows = 2;
// set LCD address, number of columns and rows 

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
 
char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "WiFi Username";  // type your wifi name
char pass[] = "WiFi Password";  // type your wifi password
//int smokeA0 = A0;
const int smokeA0 = 34; 
int data = 0;
int sensorThres = 100;


BlynkTimer timer;

void setup(){
  pinMode(smokeA0, INPUT);
   Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  lcd.begin(); // Initialize LCD 
  lcd.setCursor(3,0);
  lcd.print("Tech Trends");
  delay(1000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Gas Leakage");
  lcd.setCursor(3,1);
  lcd.print("Notification");
  delay(1000);
  lcd.clear();
  timer.setInterval(1000L, sendSensor);
}

void sendSensor(){
 
 int data = analogRead(smokeA0);
 Blynk.virtualWrite(V0, data);
  Serial.print("Pin A0: ");
  Serial.println(data);


  if(data > 50){
 
    Blynk.logEvent("gas_alert","Gas Leakage Detected");
    lcd.setCursor(0, 0);
    // print message
    lcd.print("Gas Detected:");
    lcd.setCursor(0,1);
    lcd.print("Gas Value: ");
    lcd.print(data);
    //Serial.println(data);

  }
  else {
    lcd.setCursor(0, 0);
    
   lcd.clear();
  // print message
   lcd.print("No Leakage");
   }
}


void loop(){
  Blynk.run();
  timer.run();
}
