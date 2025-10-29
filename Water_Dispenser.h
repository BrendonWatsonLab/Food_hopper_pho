// Began editing on 3/26/2025
#include <Servo.h>

Servo Servo1;
Servo Servo2;


// reflects the open/closed state of the solenoid
enum SolenoidState {
    OPEN,
    CLOSED
};

/* Water 1 config:
    A "Water" consists of a solenoid that controls the flow of water from the resevour to the port. The port contains a beam-break sensor (SENSOR) that when broken dispenses a drop of water.
*/

// Arduino is Arduino UNO:
#define SENSOR3PIN 3 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
#define SOLENOID1PIN 10 //Servo 1 on motor shield 

int sensor3State = HIGH;         // variable for reading the beam-break sensor3 status
int moveOperationCounter3 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid1State = CLOSED;         // reflects the open/closed state of the solenoid

/* Water 2 config:

*/

#define SENSOR4PIN 6 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
#define SOLENOID2PIN 9 //Servo 2 on motor shield 

int sensor4State = HIGH;         // variable for reading the beam-break sensor4 status
int moveOperationCounter4 = 0; // This variable keeps track of the total number of "move" operations performed.

SolenoidState solenoid2State = CLOSED;         // reflects the open/closed state of the solenoid

/* Set Sensors for buttons */

int sensor5State = HIGH;
int sensor6State = HIGH;
int expel1state = HIGH;
int expel2state = HIGH;
#define SENSOR5PIN 12
#define SENSOR6PIN 13
#define EXPELPIN1 18
#define EXPELPIN2 19

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
bool turnOffSig(void* argument);
void ServoForceOpen(void* serve);
void ServoForceClose(void* serve);
bool fillUp1();
bool fillUp2();
bool expel1();
bool expel2();
bool hasbeenfilled1 = false;
bool hasbeenfilled2 = false;
bool expelling1 = false;
bool expelling2 = false;


// Called from setup()
void setupWaterDispensers() {
    // initialize the sensor pins as an input:
    pinMode(SENSOR3PIN, INPUT);
    digitalWrite(SENSOR3PIN, HIGH); // turn on the pullup
    pinMode(SENSOR4PIN, INPUT);
    digitalWrite(SENSOR4PIN, HIGH); // turn on the pullup
    pinMode(SENSOR5PIN, INPUT);
    digitalWrite(SENSOR5PIN, HIGH);
    pinMode(SENSOR6PIN, INPUT);
    digitalWrite(SENSOR6PIN, HIGH);
    pinMode(EXPELPIN1, INPUT);
    digitalWrite(EXPELPIN1, HIGH);
    pinMode(EXPELPIN2, INPUT);
    digitalWrite(EXPELPIN2, HIGH);


    // Setup Servos
    Servo1.attach(SOLENOID1PIN);

    Servo2.attach(SOLENOID2PIN);
}


//allows independent solenoid operation (simultaneous)
void loopWaterDispensers(unsigned long currentLoopMillis) {
    // For any open solenoid, check to see if it's time to close it.
      // If it's not, do nothing.
    // For any closed solenoid, check to see if the beam-break sensor is blocked
      // If it is, open it.

    // Check Water Port 1:
    if (solenoid1State == OPEN) {
        if (currentLoopMillis - lastSolenoidOpenTimer1 >= SolenoidDoseOpenDuration) {
            // Close the solenoid
            closeSolenoid(1);
        }
    }
    else { // else the solenoid is CLOSED
        // Check if at least SolenoidPostDoseClosedDuration msec have passed since the last solenoid close event (to prevent immediate re-opening).
        if (currentLoopMillis - lastSolenoidCloseTimer1 >= SolenoidPostDoseClosedDuration) {
            /* Check sensor beam state:
                LOW: Sensor Beam is broken
                HIGH: Sensor Beam has continuity
            */
            // The sensor must have changed state after the end of the last water dispense and timeout period
#if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
            if (lastSensorChangeEvent3 > (lastSolenoidCloseTimer1 + SolenoidPostDoseClosedDuration)) {
#endif
                if ((sensor3State == LOW) || (IS_DIAGNOSTIC_MODE && DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER)) {
#if ENABLE_LOGGING_SIGNAL_ON_CHANGE
                    sendLoggingSignal(Water1, ActionDispense);
#endif
                    openSolenoid(1);
                }
#if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
            }
#endif
        }
    }
    // Check Water Port 2:
    if (solenoid2State == OPEN) {
        if (currentLoopMillis - lastSolenoidOpenTimer2 >= SolenoidDoseOpenDuration) {
            // Close the solenoid
            closeSolenoid(2);
        }
    }
    else { // else the solenoid is CLOSED
        // Check if at least SolenoidPostDoseClosedDuration msec have passed since the last solenoid close event (to prevent immediate re-opening).
        if (currentLoopMillis - lastSolenoidCloseTimer2 >= SolenoidPostDoseClosedDuration) {
            /* Check sensor beam state:
                LOW: Sensor Beam is broken
                HIGH: Sensor Beam has continuity
              */
              // The sensor must have changed state after the end of the last water dispense and timeout period
#if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
            if (lastSensorChangeEvent4 > (lastSolenoidCloseTimer2 + SolenoidPostDoseClosedDuration)) {
#endif
                if ((sensor4State == LOW)) {
#if ENABLE_LOGGING_SIGNAL_ON_CHANGE
                    sendLoggingSignal(Water2, ActionDispense);
#endif
                    openSolenoid(2);
                }
#if REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE
            }
#endif
        }
    }


    if (sensor5State == LOW) {
        hasbeenfilled1 = fillUp1();
    }
    else if (hasbeenfilled1) {
        closeSolenoid(1);
        hasbeenfilled1 = false;
        Serial.write("wtf");
    }
    if (sensor6State == LOW) {
        hasbeenfilled2 = fillUp2();
    }
    else if (hasbeenfilled2) {
        closeSolenoid(2);
        hasbeenfilled2 = false;
        Serial.write("wtf");
    }

    // This is the code for expelling the servo pumps
    if (expel1state == LOW) {
        expelling1 = expel1();
    }
    else if (expelling1) {
        closeSolenoid(1);
        expelling1 = false;
    }
    if (expel2state == LOW) {
        expelling2 = expel2();
    }
    else if (expelling2) {
        closeSolenoid(2);
        expelling2 = false;
    }

    if (currentLoopMillis > topup + TOP_UP_TIME) {
        topup = currentLoopMillis;
        Servo1.write(180);
        Servo2.write(180);
        digitalWrite(23, LOW);
        digitalWrite(25, LOW);
        Serial.write("Topped up");

        timer.in(82, ServoForceClose, 1);
        timer.in(82, ServoForceClose, 2);
        timer.in(SIGNAL_ON_TIME, turnOffSig, 23);
        timer.in(SIGNAL_ON_TIME, turnOffSig, 25);
    }

}






void closeSolenoid(int waterPortNumber) {
    int activeSolenoidPin = 0;
    Servo* activeServo;
    if (waterPortNumber == 1) {
        activeServo = &Servo1;
    }
    else if (waterPortNumber == 2) {
        activeServo = &Servo2;
    }
    else {
        // Should never happen. Would be nice to assert.
        Serial.println("----- waterPortNumber Error A! -----");
    }
    // Actually close the solenoid and save the time it was closed
    activeServo->write(90);

    if (waterPortNumber == 1) {
        lastSolenoidCloseTimer1 = millis();
        if (solenoid1State == OPEN) {
            solenoid1State = CLOSED; // update the state to closed
            moveOperationCounter3++; // when the solenoid is transitioned from opened to closed, count that as a move operation
        }
    }
    else if (waterPortNumber == 2) {
        lastSolenoidCloseTimer2 = millis();
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
    int activeSolenoidPin = 0;
    Servo* activeServo;
    if (waterPortNumber == 1) {
        activeServo = &Servo1;
        solenoid1State = OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
    }
    else if (waterPortNumber == 2) {
        activeServo = &Servo2;
        solenoid2State = OPEN; // update the state to closed before actually closing it (just to avoid another if/elseif/else block)
    }
    else {
        // Should never happen. Would be nice to assert.
        Serial.println("----- waterPortNumber Error A! -----");
    }
    // Actually open the solenoid and save the time it was closed
    activeServo->write(180);
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





bool fillUp1() {
    Servo* activeServo = &Servo1;
    // pull back on syringe to refill water
    activeServo->write(0);
    Serial.write("fillUp1");
    return true;
}

bool fillUp2() {
    Servo* activeServo = &Servo2;
    activeServo->write(0);
    Serial.write("fillUp2");
    return true;
}

bool expel1() {
    Servo* activeServo = &Servo1;
    // pull back on syringe to refill water
    activeServo->write(180);
    Serial.write("Expel1");
    return true;
}

bool expel2() {
    Servo* activeServo = &Servo2;
    activeServo->write(180);
    Serial.write("Expel2");
    return true;
}

// This is copied from the Mega2560Labjack_interface.h file. 
/* This is needed because timer.in() function used for timing dispenses can
only accept function handles that contain a single argument... This sucks lowkey...
One of the few times that I actually wish we were in python because of the tuples...*/
bool turnOffSig(void* argument) {
    digitalWrite(argument, HIGH);
    return false; // to repeat the action - false to stop
}

void ServoForceOpen(void* serve) {
    if (serve == 1) {
        Servo1.write(105);
    }
    else if (serve == 2) {
        Servo2.write(105);
    }
}

/* I needed this for the same reason as the turnOffSign above...
 * I have ServoForceOpen as well just in case if we need this in the future
 */
void ServoForceClose(void* serve) {
    if (serve == 1) {
        Servo1.write(90);
    }
    else if (serve == 2) {
        Servo2.write(90);
    }
}