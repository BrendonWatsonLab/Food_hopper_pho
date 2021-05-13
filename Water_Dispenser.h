// reflects the open/closed state of the solenoid
enum SolenoidState {
  SS_OPEN,
  SS_CLOSED
};


// HeldOpenOverrideState: reflects whether the solenoid is currently in a state where it's normal behavior is being overriden and it's remaining held open.
enum HeldOpenOverrideState {
  HOOS_OVERRIDDEN_OPEN,
  HOOS_NORMAL
};

/* Water 1 config:
    A "Water" consists of a solenoid that controls the flow of water from the resevour to the port. The port contains a beam-break sensor (SENSOR) that when broken dispenses a drop of water.
*/

// Arduino is Arduino UNO:
#define SENSOR3PIN 3 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
#define SOLENOID1PIN 8

int sensor3State = HIGH;         // variable for reading the beam-break sensor3 status
int moveOperationCounter3 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid1State = SS_CLOSED;         // reflects the open/closed state of the solenoid
#if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
HeldOpenOverrideState solenoid1HeldOpenState = HOOS_OVERRIDDEN_OPEN;
#else
HeldOpenOverrideState solenoid1HeldOpenState = HOOS_NORMAL;
#endif

/* Water 2 config:

*/

#define SENSOR4PIN 6 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
#define SOLENOID2PIN 11

int sensor4State = HIGH;         // variable for reading the beam-break sensor4 status
int moveOperationCounter4 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid2State = SS_CLOSED;         // reflects the open/closed state of the solenoid
#if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
HeldOpenOverrideState solenoid2HeldOpenState = HOOS_OVERRIDDEN_OPEN;
#else
HeldOpenOverrideState solenoid2HeldOpenState = HOOS_NORMAL;
#endif

/*
   After a beam-break, the solenoid opens for SolenoidOpenDuration to allow water to be dispensed.
   Following SolenoidOpenDuration, the solenoid is closed (stopping the flow of water) for at least SolenoidPostDoseClosedDuration before re-opening.
*/

unsigned long lastSolenoidOpenTimer1 = 0; // This variable keeps track of the last time the Solenoid1 "open" operation was performed
unsigned long lastSolenoidCloseTimer1 = 0; // This variable keeps track of the last time the Solenoid1 "close" operation was performed
unsigned long lastSolenoidOpenTimer2 = 0; // This variable keeps track of the last time the Solenoid2 "open" operation was performed
unsigned long lastSolenoidCloseTimer2 = 0; // This variable keeps track of the last time the Solenoid2 "close" operation was performed


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

  // Setup Solenoids
  pinMode(SOLENOID1PIN, OUTPUT);
  //digitalWrite(SOLENOID1PIN, LOW);

  pinMode(SOLENOID2PIN, OUTPUT);
  //digitalWrite(SOLENOID2PIN, LOW);
}


//TODO: Logic is almost correct. When DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER is true, it should open the solenoid immediately and then never close it (and open it only once).
//allows independent solenoid operation (simultaneous)
void loopWaterDispensers(unsigned long currentLoopMillis) {
  // For any open solenoid, check to see if it's time to close it.
    // If it's not, do nothing.
  // For any closed solenoid, check to see if the beam-break sensor is blocked
    // If it is, open it.
    
  // Check Water Port 1:
  if (solenoid1State == SS_OPEN) {
    // We only need to check if it's time to close the solenoid if we're not continuously dispensing water.
    if (solenoid1HeldOpenState == HOOS_OVERRIDDEN_OPEN) {
      // If it's already open and OVERRIDEN_OPEN, do nothing.
    }
    else {
        // Do the normal, non-overidden behavior
        if (currentLoopMillis - lastSolenoidOpenTimer1 >= SolenoidDoseOpenDuration) {
          // Close the solenoid
          closeSolenoid(1);
        }
    }
    
  }
  else { // else the solenoid is SS_CLOSED

    if (solenoid1HeldOpenState == HOOS_OVERRIDDEN_OPEN) {
      // If it's supposed to be HOOS_OVERRIDDEN_OPEN, we better open it!
      openSolenoid(1);
    }
    else {
      // Do the normal, non-overidden behavior
      // Check if at least SolenoidPostDoseClosedDuration msec have passed since the last solenoid close event (to prevent immediate re-opening).
      if (currentLoopMillis - lastSolenoidCloseTimer1 >= SolenoidPostDoseClosedDuration) {
        /* Check sensor beam state:
            LOW: Sensor Beam is broken
            HIGH: Sensor Beam has continuity
        */
        // The sensor must have changed state after the end of the last water dispense and timeout period
        #if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
			if (lastSensorChangeEvent3  > (lastSolenoidCloseTimer1 + SolenoidPostDoseClosedDuration)) {
				if (sensor3State == LOW) {
					#if ENABLE_LOGGING_SIGNAL_ON_CHANGE
					sendLoggingSignal(Water1, ActionDispense);
					#endif
					openSolenoid(1);
				}
			}
		#else
          if (sensor3State == LOW) {
            #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
              sendLoggingSignal(Water1, ActionDispense);
            #endif
            openSolenoid(1);
          }
        #endif

      }
    }

  }

  // Check Water Port 2:
  if (solenoid2State == SS_OPEN) {
    // We only need to check if it's time to close the solenoid if we're not continuously dispensing water.
    if (solenoid2HeldOpenState == HOOS_OVERRIDDEN_OPEN) {
      // If it's already open and OVERRIDEN_OPEN, do nothing.
    }
    else {
        // Do the normal, non-overidden behavior
        if (currentLoopMillis - lastSolenoidOpenTimer2 >= SolenoidDoseOpenDuration) {
          // Close the solenoid
          closeSolenoid(2);
        }
    }

  }
  else { // else the solenoid is SS_CLOSED

    if (solenoid2HeldOpenState == HOOS_OVERRIDDEN_OPEN) {
      // If it's supposed to be HOOS_OVERRIDDEN_OPEN, we better open it!
      openSolenoid(2);
    }
    else {
      // Do the normal, non-overidden behavior
      // Check if at least SolenoidPostDoseClosedDuration msec have passed since the last solenoid close event (to prevent immediate re-opening).
      if (currentLoopMillis - lastSolenoidCloseTimer2 >= SolenoidPostDoseClosedDuration) {
        /* Check sensor beam state:
            LOW: Sensor Beam is broken
            HIGH: Sensor Beam has continuity
          */
		#if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
			// CONCERN: I suspect the problem is coming in here??
        	if (lastSensorChangeEvent4  > (lastSolenoidCloseTimer2 + SolenoidPostDoseClosedDuration)) {
			        // The sensor must have changed state *after the end* of the last water dispense and timeout period
				if (sensor4State == LOW) {
					// And the sensor must be currently low (although this may not be the changed state... *should something be updated if it is still high?*
					#if ENABLE_LOGGING_SIGNAL_ON_CHANGE
						sendLoggingSignal(Water2, ActionDispense);
					#endif
					openSolenoid(2);
				}
			}

		#else
			if (sensor4State == LOW) {
				#if ENABLE_LOGGING_SIGNAL_ON_CHANGE
					sendLoggingSignal(Water2, ActionDispense);
				#endif
				openSolenoid(2);
			}
        #endif
      }

    }
  }

}


//TODO: The openSolenoid(...) and closeSolenoid(...) functions should actually be titled something related to dispensing.
void closeSolenoid(int waterPortNumber) {
  int activeSolenoidPin = 0;
  
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
  
  if (waterPortNumber == 1) {
    lastSolenoidCloseTimer1 = millis();
    if (solenoid1State == SS_OPEN) {
      solenoid1State = SS_CLOSED; // update the state to closed
      moveOperationCounter3++; // when the solenoid is transitioned from opened to closed, count that as a move operation
    }
  }
  else if (waterPortNumber == 2) {
    lastSolenoidCloseTimer2 = millis();
    if (solenoid2State == SS_OPEN) {
      solenoid2State = SS_CLOSED; // update the state to closed
      moveOperationCounter4++; // when the solenoid is transitioned from opened to closed, count that as a move operation
    }
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error B! -----");
  }
}

void openSolenoid(int waterPortNumber) {
  int activeSolenoidPin = 0;
  if (waterPortNumber == 1) {
    activeSolenoidPin = SOLENOID1PIN;
    solenoid1State = SS_OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
  }
  else if (waterPortNumber == 2) {
    activeSolenoidPin = SOLENOID2PIN;
    solenoid2State = SS_OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error A! -----");
  }
  // Actually close the solenoid and save the time it was closed
  digitalWrite(activeSolenoidPin, HIGH);
  if (waterPortNumber == 1) {
    lastSolenoidOpenTimer1 = millis();
  }
  else if (waterPortNumber == 2) {
    lastSolenoidOpenTimer2 = millis();
  }
  else {
    // Should never happen. Would be nice to assert.
    Serial.println("----- waterPortNumber Error B! -----");
  }
}
