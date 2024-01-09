#define BLYNK_TEMPLATE_ID "TMPL3VQJFi72R"
#define BLYNK_TEMPLATE_NAME "Control Stepper Motor"
#define BLYNK_AUTH_TOKEN "FjNwp0kfG1dfj-yRhfkR8agkX5SssdfkJjBZuyNON"

#include <AccelStepper.h>
#include <BlynkSimpleEsp32.h>

const char auth[] = BLYNK_AUTH_TOKEN;  // Blynk authentication token
const char ssid[] = "WiFi Username";   // Your WiFi SSID
const char pass[] = "WiFi Password"; // Your WiFi password

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 18
#define IN2 19
#define IN3 22
#define IN4 23

// Blynk virtual pins for the buttons
#define buttonClockwisePin V1
#define buttonCounterclockwisePin V2

// initialize the stepper library
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // set the speed and acceleration
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(100);
}

void loop() {
  Blynk.run();

  // move the stepper motor (one step at a time)
  stepper.runSpeed();
}

BLYNK_WRITE(buttonClockwisePin) {
  int buttonClockwiseState = param.asInt();
  
  if (buttonClockwiseState == HIGH) {
    stepper.setSpeed(500);  // Set speed for clockwise rotation
    stepper.enableOutputs();  // Enable motor outputs
    Serial.println("Clockwise");
  } else {
    //if (digitalRead(buttonClockwisePin) == LOW && digitalRead(buttonCounterclockwisePin) == LOW) {
      stepper.setSpeed(0);  // Set speed to 0 to stop the motor
      stepper.disableOutputs();  // Disable motor outputs
      Serial.println("Motor Stopped");
    //}
  }
}

BLYNK_WRITE(buttonCounterclockwisePin) {
  int buttonCounterclockwiseState = param.asInt();
  
  if (buttonCounterclockwiseState == HIGH) {
    stepper.setSpeed(-500);  // Set speed for counterclockwise rotation
    stepper.enableOutputs();  // Enable motor outputs
    Serial.println("Counterclockwise");
  } else {
    //if (digitalRead(buttonClockwisePin) == LOW && digitalRead(buttonCounterclockwisePin) == LOW) {
      stepper.setSpeed(0);  // Set speed to 0 to stop the motor
      stepper.disableOutputs();  // Disable motor outputs
      Serial.println("Motor Stopped");
    //}
  }
}
