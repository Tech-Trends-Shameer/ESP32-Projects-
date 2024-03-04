//Tech Trends Shameer
//Read Data from Google SpreadSheet and display in LCD Display with ESP32 Board

//Install the below libraries
//---------------------------------------------------------------------

#include <WiFi.h>
#include <HTTPClient.h>
//#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  

// Set the LCD address
LiquidCrystal_I2C lcd(0x27, 16, 2); 


//Enter WIFI Credentials
//---------------------------------------------------------------------

const char * ssid = "WiFi Username";  // Enter your Wifi Username
const char * password = "WiFi Password";  // Enter your Wifi Password

String GOOGLE_SCRIPT_ID = "Script Id";
//---------------------------------------------------------------------



const int sendInterval = 50;
String message = "Welcome";
int x;
int len=0;
int k=0;
WiFiClientSecure client;
 
void setup() 
{ 
  Serial.begin(9600);
  lcd.begin();
  delay(10);
  //--------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    lcd.home();
    lcd.print("Waiting for Wifi");
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  lcd.home();
  lcd.print("Wifi Connected");
  Serial.println("OK");
  //--------------------------------------------
}

void loop() 
{
  int inc =0;
  if(k==0)
  read_google_sheet();
  len =message.length();
  for (int a=15; a>=-len;a--)
  { 
    lcd.clear();
    lcd.home();
    lcd.print("Smart Display");
    if(a>=0)
    {
        lcd.setCursor(a,1);
        lcd.print(message.substring(0,15));
    }
    else
    {
       inc++;
       lcd.setCursor(0,1);
       lcd.print(message.substring(inc,len));           
    }         
    delay(400);
    //lcd.clear();
  }
  k++;
  if(k>0)
  k=0;
}

void read_google_sheet(void) 
{
   //-----------------------------------------------------------------------------------
//   std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
   //client->setInsecure();
   HTTPClient https;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read";
   //String url="https://script.google.com/macros/library/d/1wm8LCo6DDIn8N5Yy20ON-18yuOJ9wb7a10PwcWjpj4JdR9mA5OrY0MWQ/1?read";
   //String url ="https://script.google.com/macros/s/AKfycbxNFPD66JZr9_4qGlW4yWQttpQq_MgZoO0PC2Y8gzbwpQWTuBy2DcW_mmLjHde9LNl-/exec?read";
   //String url ="https://script.google.com/macros/s/AKfycbyGhgw3EdrS5nKVDtWZmS0qFQzuZ1Qe04hlUuoHxjNbVhZEcKzgEWq5EWYqI5SVtAfn/exec?read";
   Serial.println("Reading Data From Google Sheet.....");
//   https.begin(*client, url.c_str());
   https.begin(url.c_str()); //Specify the URL and certificate
    //http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
   //-----------------------------------------------------------------------------------
   //Removes the error "302 Moved Temporarily Error"
   https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
   //-----------------------------------------------------------------------------------
   //Get the returning HTTP status code
   int httpCode = https.GET();
   Serial.print("HTTP Status Code: ");
   Serial.println(httpCode);
   //-----------------------------------------------------------------------------------
   if(httpCode <= 0){Serial.println("Error on HTTP request"); https.end(); return;}
   //-----------------------------------------------------------------------------------
   //reading data comming from Google Sheet
   String payload = https.getString();
   Serial.println("Payload: "+payload);
   //-----------------------------------------------------------------------------------
   if(httpCode == 200)
   message= payload;
   Serial.println("Message");
   Serial.println(message);
   //-------------------------------------------------------------------------------------
   https.end();
}
