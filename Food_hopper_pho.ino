/* 
Burgess Lab IR beam (photointerrupter) motor food dispense
Written by Robert Zhang July 31st 2018
Modifed by Pho Hale 3/6/2019
*/
#include <Wire.h>
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
/*
 * The stepper motor is a SY35ST28-0504A
 * Steps per Revolution: 200
 * Voltage Rating: 10V
 * Current Rating: 500 [ma] (per coil)
 * Step Angle: 1.8 degrees
 * The getStepper(#steps, portNumber) command uses portNumber=2 for M3 or M4.
 */
#define IS_DIAGNOSTIC_MODE true //IS_DIAGNOSTIC_MODE: if this value is true the system will operate continuously, ignoring the beam break sensor. This serves to allow testing. This value should be false outside of testing.
#define SHOULD_USE_INTERACTIVE_DIAGNOSTIC true //SHOULD_USE_INTERACTIVE_DIAGNOSTIC: enables extended diagnostics and testing via the helper Processing software.
char diagnostic_val; // Data received from the serial port
unsigned long lastDiagnosticSerialReadTimer = 0; // This variable keeps track of the last time diagnostic serial read was performed
#define INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT 10 //INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT: the minimum time between serial reads for interactive diagnostics

#define LEDPIN 13 // Pin 13: Arduino has an LED connected on pin 1
unsigned long lastDispenseTimer = 0; // This variable keeps track of the last time dispense was performed

/* Feeder 1 config:
 *  A "Feeder" consists of a food hopper containing pellets, a stepper motor to rotate the dispenser, a shoot, and a port. The port contains a beam-break sensor (SENSOR) that when broken dispenses food.
 *  CONCERN: they're going to figure out that you don't have to wait as long if you alternate between the two types of food. 
 */
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 2); // The motor connected to M3 & M4
#define SENSOR1PIN 4 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
int sensor1State = 0;         // variable for reading the beam-break sensor1 status
int moveOperationCounter1 = 0; // This variable keeps track of the total number of "move" operations performed.

/* Feeder 2 config:
 *  
 */
#define IS_DUAL_MOTOR_MODE false // IS_DUAL_MOTOR_MODE: if true, we use both motors. Otherwise we only use the motor connected to M3 & M4
Adafruit_StepperMotor *motor2 = AFMS.getStepper(200, 1); // The motor connected to M1 & M2
#define SENSOR2PIN 6 // SENSOR2PIN: This pin is connected by a green wire to the second beam-break sensor's "SIG" pin.
int sensor2State = 0;         // variable for reading the beam-break sensor2 status
int moveOperationCounter2 = 0; // This variable keeps track of the total number of "move" operations performed.


/*
 * There are two types of "move" operations: move-clockwise, move-counterclockwise
 * The "move" operation performed depends on the moveOperationCounter. Every "ConsecutiveSameDirectionMovements + 1" steps we move counter-clockwise. Otherwise, we move clockwise.
 * Integer overflow is not an issue, as we only care to keep track of the proper rotation direction so that we may alternate between to the two directions.
 */
 #define ConsecutiveSameDirectionMovements 5 //Defines the number of times it moves in a single direction before alternating the direction of moment.
 #define PostDispenseTimeout 3000 //The number of milliseconds (1/1000 of a second) after dispensing that the system will wait before allowing input.
// Was 5000
 //#define NumberOfStepperCoilsActivated SINGLE // The number of coils in the stepper motor to activate. DOUBLE provides higher torque.
 #define NumberOfStepperCoilsActivated DOUBLE // The number of coils in the stepper motor to activate. DOUBLE provides higher torque.

 // The StepperSpeed is not in RPM (contrary to what the previous implementor thought).
 // It's a value The setSpeed() function controls the power level delivered to the motor. The speed parameter is a value between 0 and 255.
 #define StepperSpeed 127 // The speed of the stepper in rpm (default 25, previous 127).

 
void setup() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);
  // initialize the sensor pin as an input:
  pinMode(SENSOR1PIN, INPUT);     
  digitalWrite(SENSOR1PIN, HIGH); // turn on the pullup
  
  Serial.begin(19200); // set up Serial library at 9600 bps (for debugging)
  Serial.println("Food Hopper:");
  if (IS_DIAGNOSTIC_MODE) { //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    Serial.println("----- DIAGNOSTIC MODE -----");
  }
  AFMS.begin();  // create with the default frequency 1.6KHz
  // The setSpeed() function controls the power level delivered to the motor. 
  motor1->setSpeed(StepperSpeed); //The speed parameter is a value between 0 and 255.
  // Set up the second feeder if needed
  if (IS_DUAL_MOTOR_MODE) {
     // initialize the sensor pin as an input:
     pinMode(SENSOR2PIN, INPUT);     
     digitalWrite(SENSOR2PIN, HIGH); // turn on the pullup
     motor2->setSpeed(StepperSpeed); //The speed parameter is a value between 0 and 255.
  }
}

void loop(){
  // read the state of the IR break beam sensor:
  sensor1State = digitalRead(SENSOR1PIN);
  if (IS_DUAL_MOTOR_MODE) {
    sensor2State = digitalRead(SENSOR2PIN);
  }
  
  // Get the current time in milliseconds
  unsigned long currentMillis = millis();

  // Performs the interfacing with the processing software (running on the computer) while in interactive diagnostic mode
  if (IS_DIAGNOSTIC_MODE && SHOULD_USE_INTERACTIVE_DIAGNOSTIC) {
    if (currentMillis - lastDiagnosticSerialReadTimer >= INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT) {
      diagnostic_read_command();
      lastDiagnosticSerialReadTimer = currentMillis;
    }
  }
  else {
    // We don't dispense unless not in interactive diagnostic mode 
    // Check to see if it's possibly time to dispense
    if (currentMillis - lastDispenseTimer >= PostDispenseTimeout) {
        /* Check sensor beam state:
         * LOW: Sensor Beam is broken
         * HIGH: Sensor Beam has continuity
         */
        // Currently DIAGNOSTIC_MODE Only dispenses feeder1
        if ((sensor1State == LOW) || IS_DIAGNOSTIC_MODE) {
          //delay(40);
          dispenseFeeder1();
        }
        else if (IS_DUAL_MOTOR_MODE && ((sensor2State == LOW) || IS_DIAGNOSTIC_MODE)) {
          dispenseFeeder2();
        }
        else {
          // turn status LED off:
          digitalWrite(LEDPIN, LOW);  
        }
  
    }
    // The status LED is left lit until it's possible to dispense again.
    
  } // end interactive diagnostic if

  
  
  
} // end loop



void dispenseFeeder1() {
  dispense(1);
}

void dispenseFeeder2() {
  dispense(2);
}

// dispense(int feederNumber): takes a feeder number (currently either 1 or 2) as an argument.
/* Called to dispense a single pellet for a single feeder, either feeder 1 or feeder 2.
 * Keeps track of each feeders' moveOperationCounter to perform unjam operations.
 */
void dispense(int feederNumber) {
    // save the last time you dispensed a pellet:
    lastDispenseTimer = millis();
    digitalWrite(LEDPIN, HIGH); // Turn status LED on
    int moveOperationCounter = 0; // This temporary counter is set to the correct counter in the if/else if/statements
    Adafruit_StepperMotor *activeMotor;
    // Gets the correct moveOperationCounter value and motor pointer for the specified feederNumber
    if (feederNumber == 1) {
      moveOperationCounter = moveOperationCounter1;
      activeMotor = motor1;
    }
    else if (feederNumber == 2) {
      moveOperationCounter = moveOperationCounter2;
      activeMotor = motor2;
    }
    else {
      // Should never happen. Would be nice to assert.
      Serial.println("----- FeederNumber Error A! -----");
    }
    
    // The "%" operation is "modulus". The statement checks whether the moveOperationCounter is evenly divisible by ConsecutiveSameDirectionMovements. If it is, it performs the first case, otherwise the second.
    if (((moveOperationCounter % ConsecutiveSameDirectionMovements) == 0)) {
      //Every "ConsecutiveSameDirectionMovements + 1" steps we attempt to unjam
      //unjamDispenseByTickTock();
      unjamDispenseBySimpleReverse(activeMotor);
    }
    else {
      // Otherwise we perform a normal dispense operation
      clockwiseDispense(activeMotor);
    }

    // Increment the counter
    if (feederNumber == 1) {
      moveOperationCounter1++;
    }
    else if (feederNumber == 2) {
      moveOperationCounter2++;
    }
    else {
      // Should never happen. Would be nice to assert.
      Serial.println("----- FeederNumber Error B! -----");
    }
}




// The traditional (clockwise) movement that dispenses a pellet
void clockwiseDispense(Adafruit_StepperMotor *activeMotor) {
  // Otherwise we move in the traditional (clockwise) direction
  Serial.println("Moving: Clockwise " + String(moveOperationCounter1));
  activeMotor->step(25, FORWARD, NumberOfStepperCoilsActivated);
}

// Attempts to unjam by just moving backwards for this iteration.
void unjamDispenseBySimpleReverse(Adafruit_StepperMotor *activeMotor) {
  Serial.println("Moving: Counter-Clockwise " + String(moveOperationCounter1));
  activeMotor->step(25, BACKWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
}

// Attempts to unjam by quickly moving backwards, and then forward again for this iteration. This runs the risk of double-dispensing
void unjamDispenseByTickTock(Adafruit_StepperMotor *activeMotor) {
  Serial.println("Moving: UNJAM " + String(moveOperationCounter1));
  activeMotor->step(25, BACKWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
  activeMotor->step(25, FORWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
}


void diagnostic_read_command() {
    if (Serial.available()) { // If data is available to read,
       diagnostic_val = Serial.read(); // read it and store it in val
      // Check the character recieved
      if (diagnostic_val == '0') {
        Serial.println("diagnostic_val: 0");
      }
      else if (diagnostic_val == '1') {
        Serial.println("diagnostic_val: 1");
        clockwiseDispense(motor1);
      }
      else if (diagnostic_val == '2') {
        Serial.println("diagnostic_val: 2");
        unjamDispenseBySimpleReverse(motor1);
      }
      else if (diagnostic_val == '3') {
        Serial.println("diagnostic_val: 3");
        unjamDispenseByTickTock(motor1);
      }
      else if (diagnostic_val == '4') {
        Serial.println("diagnostic_val: 4");
        dispenseFeeder1();
      }
      else if (diagnostic_val == '5') {
        Serial.println("diagnostic_val: 5");
      }
      else if (diagnostic_val == '6') {
        Serial.println("diagnostic_val: 6");
      }
      else if (diagnostic_val == 'A') {
        Serial.println("diagnostic_val: A");
        clockwiseDispense(motor2);
      }
      else if (diagnostic_val == 'B') {
        Serial.println("diagnostic_val: B");
        unjamDispenseBySimpleReverse(motor2);
      }
      else if (diagnostic_val == 'C') {
        Serial.println("diagnostic_val: C");
        unjamDispenseByTickTock(motor2);
      }
      else if (diagnostic_val == 'D') {
        Serial.println("diagnostic_val: D");
        dispenseFeeder2();
      }
      else if (diagnostic_val == 'E') {
        Serial.println("diagnostic_val: E");
      }
      else if (diagnostic_val == 'F') {
        Serial.println("diagnostic_val: F");
      }
      else {
        Serial.println("Unknown diagnostic_val");
      } // end cases
    }
    //delay(10); // Wait 10 milliseconds for next reading
}
 
