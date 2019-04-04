#define RUNNINGWHEEL_SENSOR_PIN 12 // RUNNINGWHEEL_SENSOR_PIN: This pin is connected by a grey wire to the running wheel's beam-break sensor.
int runningWheelSensorState = HIGH;         // variable for reading the beam-break running wheel status

// Function Prototypes:
void setupRunningWheel();
void loopRunningWheel(unsigned long currentLoopMillis);


// Called from setup()
void setupRunningWheel() {
  // initialize the sensor pins as an input:
  pinMode(RUNNINGWHEEL_SENSOR_PIN, INPUT);
  digitalWrite(RUNNINGWHEEL_SENSOR_PIN, HIGH); // turn on the pullup
}


//TODO: check if the sensor state changed, which is all we really care about.
void loopRunningWheel(unsigned long currentLoopMillis) {

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (runningWheelSensorState == LOW) {
    //TODO: OUTPUT TO INTAN
    //digitalWrite(TTLPIN1, HIGH);
    //delay(40);
    //digitalWrite(TTLPIN1, LOW);
  }
  else {
    // turn LED off:
    //digitalWrite(LEDPIN, LOW);
  }

}
