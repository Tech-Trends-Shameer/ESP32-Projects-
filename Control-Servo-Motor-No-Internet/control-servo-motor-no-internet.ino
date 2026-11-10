/*
  ESP32 Servo Control - Captive Portal (v3: no servo movement at startup)
  Board: any ESP32 Dev Module | Library: ESP32Servo (Library Manager)
  Wiring: Servo signal (orange/yellow) -> GPIO18, red -> 5V/VIN, brown -> GND
  Connect to Wi-Fi "Servo-Control" (password: 12345678), then open http://192.168.4.1
  Serial Monitor: 115200 baud
  The servo is only attached after the first command from the phone.
*/
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* AP_SSID = "Servo-Control";
const char* AP_PASS = "12345678";   // min 8 chars, or set "" for open network
const int SERVO_PIN = 18;

DNSServer dns;
WebServer server(80);
Servo servo;

int angle = 90;
bool sweeping = false;
int sweepDir = 1;
unsigned long lastStep = 0;

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Servo Control</title>
<style>
body{margin:0;font-family:sans-serif;background:#0f1b2d;color:#fff;text-align:center;padding:20px}
h2{margin-bottom:4px}p{color:#9fb3c8;margin-top:0}
#val{font-size:72px;font-weight:700;margin:20px 0}
input[type=range]{width:90%;height:36px}
button{background:#16304f;color:#fff;border:1px solid #3b6ea5;border-radius:8px;
padding:12px 16px;margin:5px;font-size:16px}
button:active{background:#3b6ea5}
#st{margin-top:18px;font-size:14px;color:#4ade80}
</style></head><body>
<h2>SERVO CONTROL</h2><p>Move the slider to rotate the servo</p>
<div id="val">90&deg;</div>
<input type="range" id="s" min="0" max="180" value="90" oninput="setA(this.value)">
<div style="display:flex;justify-content:space-between;width:90%;margin:0 auto;font-size:13px">
<span>0&deg;</span><span>90&deg;</span><span>180&deg;</span></div>
<div style="margin-top:16px">
<button onclick="setA(0)">0&deg;</button><button onclick="setA(45)">45&deg;</button>
<button onclick="setA(90)">90&deg;</button><button onclick="setA(135)">135&deg;</button>
<button onclick="setA(180)">180&deg;</button></div>
<div><button id="sw" onclick="sweep()">Auto sweep</button>
<button onclick="cal()">Calibrate</button></div>
<div id="st">&#9679; Connected</div>
<script>
let t;
function show(a){document.getElementById('val').innerHTML=a+'&deg;';document.getElementById('s').value=a;}
function setA(a){show(a);clearTimeout(t);
 t=setTimeout(()=>req('/set?a='+a),30);}
function req(u){return fetch(u).then(r=>{st(true);return r.text()}).catch(()=>st(false));}
function st(ok){let e=document.getElementById('st');
 e.style.color=ok?'#4ade80':'#f87171';e.innerHTML='&#9679; '+(ok?'Connected':'Disconnected');}
function sweep(){req('/sweep');}
function cal(){req('/cal');}
setInterval(()=>req('/angle').then(a=>{if(a)show(a)}),1000);
</script></body></html>
)rawliteral";

// ---- Wi-Fi connect / disconnect messages in Serial Monitor ----
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
    Serial.printf("Phone connected: %02X:%02X:%02X:%02X:%02X:%02X | Clients: %d\n",
      info.wifi_ap_staconnected.mac[0], info.wifi_ap_staconnected.mac[1],
      info.wifi_ap_staconnected.mac[2], info.wifi_ap_staconnected.mac[3],
      info.wifi_ap_staconnected.mac[4], info.wifi_ap_staconnected.mac[5],
      WiFi.softAPgetStationNum());
  }
  if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
    Serial.println("Phone disconnected");
  }
}

// Attach the servo only when the first move is requested (no startup twitch)
void moveServo(int a) {
  if (!servo.attached()) {
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 500, 2400);   // tweak pulse range if servo doesn't reach full travel
  }
  servo.write(a);
}

void handleRoot() {
  Serial.println("Page requested");
  server.send_P(200, "text/html", PAGE);
}

void handleSet() {
  if (server.hasArg("a")) {
    sweeping = false;
    angle = constrain(server.arg("a").toInt(), 0, 180);
    moveServo(angle);
    Serial.printf("Servo angle: %d\n", angle);
  }
  server.send(200, "text/plain", String(angle));
}

void handleSweep() {
  sweeping = !sweeping;
  Serial.println(sweeping ? "Auto sweep ON" : "Auto sweep OFF");
  server.send(200, "text/plain", sweeping ? "on" : "off");
}

void handleCal() {            // quick range test: 0 -> 90 -> 180 -> 90
  sweeping = false;
  Serial.println("Calibrating...");
  int pts[] = {0, 90, 180, 90};
  for (int p : pts) { moveServo(p); angle = p; delay(700); }
  server.send(200, "text/plain", "done");
}

void handleAngle() { server.send(200, "text/plain", String(angle)); }

void redirectToPortal() {     // captive portal: send every unknown URL to the page
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(onWiFiEvent);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  dns.start(53, "*", WiFi.softAPIP());  // answer all DNS queries with ESP32's IP
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/sweep", handleSweep);
  server.on("/cal", handleCal);
  server.on("/angle", handleAngle);
  server.onNotFound(redirectToPortal);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  dns.processNextRequest();
  server.handleClient();

  if (sweeping && millis() - lastStep > 15) {
    lastStep = millis();
    angle += sweepDir;
    if (angle >= 180) { angle = 180; sweepDir = -1; }
    if (angle <= 0)   { angle = 0;   sweepDir = 1; }
    moveServo(angle);
  }
}
