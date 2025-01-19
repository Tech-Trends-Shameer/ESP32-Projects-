const int soilMoisturePin = 34; // Analog pin connected to the sensor

void setup() {
  Serial.begin(115200); // Initialize Serial Monitor
  Serial.println("Reading raw soil moisture sensor values...");
}

void loop() {
  int sensorValue = analogRead(soilMoisturePin); // Read analog value
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue); // Print the raw value
  delay(1000); // Wait for 1 second
}
