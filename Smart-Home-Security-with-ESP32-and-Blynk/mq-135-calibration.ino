/*
 * ====================================================================
 *                 MQ-135 SIMPLE CALIBRATION CODE
 * ====================================================================
 */

#define MQ_PIN 34

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("===========================================");
  Serial.println("💨 MQ-135 Simple Gas Calibration Started... 💨");
  Serial.println("===========================================");
}

void loop() {
  // Read the raw analog voltage value from the sensor
  int rawGasVal = analogRead(MQ_PIN);

  // Print the raw value to the Serial Monitor
  Serial.print("Raw Gas Value: ");
  Serial.println(rawGasVal);

  // Read every 1 second for fast, real-time testing feedback
  delay(1000); 
}
