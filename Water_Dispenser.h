// reflects the open/closed state of the solenoid
enum SolenoidState {
  OPEN,
  CLOSED
};


/* Water 1 config:
    A "Water" consists of a solenoid that controls the flow of water from the resevour to the port. The port contains a beam-break sensor (SENSOR) that when broken dispenses a drop of water.
*/
#if DEPLOY_ARDUINO_IS_UNO
  // Arduino is Arduino UNO:
  #define SENSOR3PIN 3 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
  #define SOLENOID1PIN 11
#else
  // Arduino is "Adafruit pro trinket SV p2000"
  #define SENSOR3PIN 4 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
  #define SOLENOID1PIN 10
  // Will use 3 pins, (10, 11, 12)
#endif
int sensor3State = HIGH;         // variable for reading the beam-break sensor3 status
int moveOperationCounter3 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid1State = CLOSED;         // reflects the open/closed state of the solenoid

/* Water 2 config:

*/
#if DEPLOY_ARDUINO_IS_UNO
  // Arduino is Arduino UNO:
  #define SENSOR4PIN 6 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
  #define SOLENOID2PIN 12
#else
  // Arduino is "Adafruit pro trinket SV p2000"
  #define SENSOR4PIN 4 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
  #define SOLENOID2PIN 6
  // Will use 3 pins, (6, 7, 8)
#endif
int sensor4State = HIGH;         // variable for reading the beam-break sensor4 status
int moveOperationCounter4 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid2State = CLOSED;         // reflects the open/closed state of the solenoid

/*
   After a beam-break, the solenoid opens for SolenoidOpenDuration to allow water to be dispensed.
   Following SolenoidOpenDuration, the solenoid is closed (stopping the flow of water) and event
*/

#define SolenoidDoseOpenDuration 100
// The
unsigned long lastSolenoidOpenTimer = 0; // This variable keeps track of the last time the Solenoid "open" operation was performed
unsigned long lastSolenoidCloseTimer = 0; // This variable keeps track of the last time the Solenoid "close" operation was performed

// Function Prototypes:
void setupWaterDispensers();
void loopWaterDispensers(unsigned long currentLoopMillis);
void closeSolenoid(int waterPortNumber);
void openSolenoid(int waterPortNumber);


// Called from setup()
void setupWaterDispensers() {
  // initialize the sensor pins as an input:
  pinMode(SENSOR3PIN, INPUT);
  digitalWrite(SENSOR3PIN, HIGH); // turn on the pullup
  pinMode(SENSOR4PIN, INPUT);
  digitalWrite(SENSOR4PIN, HIGH); // turn on the pullup

}

//TODO: allows independent solenoid operation (simultaneous) but they're coupled by using a single timer variable (lastSolenoidOpenTimer)
void loopWaterDispensers(unsigned long currentLoopMillis) {
  // For any open solenoid, check to see if it's time to close it.
    // If it's not, do nothing.
  // For any closed solenoid, check to see if the beam-break sensor is blocked
    // If it is, open it.
    
  // Check Water Port 1:
  if (solenoid1State == OPEN) {
    if (currentLoopMillis - lastSolenoidOpenTimer >= SolenoidDoseOpenDuration) {
      // Close the solenoid
      closeSolenoid(1);
    }
  }
  else { // else the solenoid is CLOSED
    /* Check sensor beam state:
        LOW: Sensor Beam is broken
        HIGH: Sensor Beam has continuity
      */
    if ((sensor3State == LOW)) {
        openSolenoid(1);
    }
  }

  // Check Water Port 2:
  if (solenoid2State == OPEN) {
    if (currentLoopMillis - lastSolenoidOpenTimer >= SolenoidDoseOpenDuration) {
      // Close the solenoid
      closeSolenoid(2);
    }
  }
  else { // else the solenoid is CLOSED
    /* Check sensor beam state:
        LOW: Sensor Beam is broken
        HIGH: Sensor Beam has continuity
      */
    if ((sensor4State == LOW)) {
        openSolenoid(2);
    }
  }

}

void closeSolenoid(int waterPortNumber) {
  int activeSolenoidPin = 0; // This temporary port variable is set to the correct SOLENOIDPIN in the if/else if/statements
  if (waterPortNumber == 1) {
    activeSolenoidPin = SOLENOID1PIN;
  }
  else if (waterPortNumber == 2) {
    activeSolenoidPin = SOLENOID2PIN;
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error A! -----");
  }
  // Actually close the solenoid and save the time it was closed
  digitalWrite(activeSolenoidPin, LOW);
  lastSolenoidCloseTimer = millis();
  
  if (waterPortNumber == 1) {
    if (solenoid1State == OPEN) {
      solenoid1State = CLOSED; // update the state to closed
      moveOperationCounter3++; // when the solenoid is transitioned from opened to closed, count that as a move operation
    }
  }
  else if (waterPortNumber == 2) {
    if (solenoid2State == OPEN) {
      solenoid2State = CLOSED; // update the state to closed
      moveOperationCounter4++; // when the solenoid is transitioned from opened to closed, count that as a move operation
    }
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error B! -----");
  }
}

void openSolenoid(int waterPortNumber) {
  int activeSolenoidPin = 0; // This temporary port variable is set to the correct SOLENOIDPIN in the if/else if/statements
  if (waterPortNumber == 1) {
    activeSolenoidPin = SOLENOID1PIN;
    solenoid1State = OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
  }
  else if (waterPortNumber == 2) {
    activeSolenoidPin = SOLENOID2PIN;
    solenoid2State = OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error A! -----");
  }
  digitalWrite(activeSolenoidPin, HIGH);
  lastSolenoidOpenTimer = millis();
}
