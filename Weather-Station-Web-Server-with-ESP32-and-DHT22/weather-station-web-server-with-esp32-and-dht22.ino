#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ===== CONFIGURATION =====
const char* ssid     = "WiFi Username";
const char* password = "WiFi Password";

#define DHTPIN  4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// ===== HTML PAGE =====
String buildHTML(float temp, float hum, float heatIndex) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="10">
  <title>ESP32 Weather Station</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', sans-serif;
      background: linear-gradient(135deg, #1a1a2e, #16213e, #0f3460);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      color: white;
    }
    .container {
      text-align: center;
      padding: 40px;
    }
    h1 { font-size: 2em; margin-bottom: 5px; letter-spacing: 2px; }
    .subtitle { color: #aaa; margin-bottom: 40px; font-size: 0.9em; }
    .cards {
      display: flex;
      gap: 20px;
      flex-wrap: wrap;
      justify-content: center;
    }
    .card {
      background: rgba(255,255,255,0.08);
      border: 1px solid rgba(255,255,255,0.15);
      border-radius: 20px;
      padding: 30px 40px;
      backdrop-filter: blur(10px);
      min-width: 180px;
      transition: transform 0.2s;
    }
    .card:hover { transform: translateY(-5px); }
    .icon { font-size: 2.5em; margin-bottom: 10px; }
    .label { font-size: 0.8em; color: #aaa; text-transform: uppercase; letter-spacing: 1px; }
    .value { font-size: 2.5em; font-weight: bold; margin: 8px 0; }
    .unit  { font-size: 0.9em; color: #ddd; }
    .temp  .value { color: #ff6b6b; }
    .hum   .value { color: #74b9ff; }
    .heat  .value { color: #fdcb6e; }
    .footer { margin-top: 30px; color: #666; font-size: 0.75em; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🌤️ Weather Station</h1>
    <p class="subtitle">ESP32 + DHT22 · Auto-refreshes every 10 seconds</p>
    <div class="cards">
      <div class="card temp">
        <div class="icon">🌡️</div>
        <div class="label">Temperature</div>
        <div class="value">)rawliteral";

  html += String(temp, 1);
  html += R"rawliteral(</div>
        <div class="unit">°C</div>
      </div>
      <div class="card hum">
        <div class="icon">💧</div>
        <div class="label">Humidity</div>
        <div class="value">)rawliteral";

  html += String(hum, 1);
  html += R"rawliteral(</div>
        <div class="unit">%</div>
      </div>
      <div class="card heat">
        <div class="icon">🔥</div>
        <div class="label">Heat Index</div>
        <div class="value">)rawliteral";

  html += String(heatIndex, 1);
  html += R"rawliteral(</div>
        <div class="unit">°C</div>
      </div>
    </div>
    <p class="footer">Last updated: ESP32 uptime )rawliteral";

  html += String(millis() / 1000);
  html += R"rawliteral( seconds</p>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

// ===== ROUTE HANDLERS =====
void handleRoot() {
  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    server.send(500, "text/plain", "DHT22 read error! Check wiring.");
    return;
  }

  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  server.send(200, "text/html", buildHTML(temperature, humidity, heatIndex));
}

// JSON endpoint for programmatic access
void handleData() {
  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature();
  float heatIndex   = dht.computeHeatIndex(temperature, humidity, false);

  String json = "{";
  json += "\"temperature\":" + String(temperature, 2) + ",";
  json += "\"humidity\":"    + String(humidity, 2)    + ",";
  json += "\"heatIndex\":"   + String(heatIndex, 2);
  json += "}";

  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected!");
  Serial.print("📡 IP Address: http://");
  Serial.println(WiFi.localIP());

  server.on("/",      handleRoot);
  server.on("/data",  handleData);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("🌐 Web server started!");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  delay(2);
}
