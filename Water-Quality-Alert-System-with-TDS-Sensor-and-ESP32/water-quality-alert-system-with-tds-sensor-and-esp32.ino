#define BLYNK_TEMPLATE_ID "TMPL3CQGGV8PV"
#define BLYNK_TEMPLATE_NAME "Water Quality Alert"
#define BLYNK_AUTH_TOKEN "uuDfaidVERW15dTdDOCWk5hRfO0RwsDPhO-Sce"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>



char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "WiFi Username";
char pass[] = "WiFi Password";

namespace pin {
    const byte tds_sensor = 34;
}

namespace device {
    float aref = 3.3;
}

namespace sensor {
    float ec = 0;
    unsigned int tds = 0;
    float ecCalibration = 1;
    const int tdsThreshold = 20; // Define poor water quality threshold
    bool alertSent = false;       // Prevent repeated alerts
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);
}

void loop() {
    Blynk.run();
    readTdsQuick();
    delay(1000);
}

void readTdsQuick() {
    float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0;

    Serial.print(F("Raw Analog Value: "));
    Serial.println(rawEc);

    float offset = 0.14;
    sensor::ec = (rawEc * sensor::ecCalibration) - offset;
    if (sensor::ec < 0) sensor::ec = 0;

    sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5;

    Serial.print(F("TDS: "));
    Serial.println(sensor::tds);
    Serial.print(F("EC: "));
    Serial.println(sensor::ec, 2);

    Blynk.virtualWrite(V0, sensor::tds);
    Blynk.virtualWrite(V1, sensor::ec);

    // Alert logic
    if (sensor::tds > sensor::tdsThreshold && !sensor::alertSent) {
        Blynk.logEvent("poor_water_quality", "⚠️ Water quality is poor! TDS: " + String(sensor::tds) + " ppm");
        sensor::alertSent = true;
    }
    else if (sensor::tds <= sensor::tdsThreshold && sensor::alertSent) {
        // Reset alert flag when quality improves
        sensor::alertSent = false;
    }
}
