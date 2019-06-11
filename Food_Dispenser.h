
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
/*
   The stepper motor is a SY35ST28-0504A
   Steps per Revolution: 200
   Voltage Rating: 10V
   Current Rating: 500 [ma] (per coil)
   Step Angle: 1.8 degrees
   The getStepper(#steps, portNumber) command uses portNumber=2 for M3 or M4.
*/

/* Feeder 1 config:
   A "Feeder" consists of a food hopper containing pellets, a stepper motor to rotate the dispenser, a shoot, and a port. The port contains a beam-break sensor (SENSOR) that when broken dispenses food.
   CONCERN: they're going to figure out that you don't have to wait as long if you alternate between the two types of food.
*/
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 2); // The motor connected to M3 & M4
#define SENSOR1PIN 5 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
int sensor1State = HIGH;         // variable for reading the beam-break sensor1 status
int moveOperationCounter1 = 0; // This variable keeps track of the total number of "move" operations performed.

/* Feeder 2 config:

*/
Adafruit_StepperMotor *motor2 = AFMS.getStepper(200, 1); // The motor connected to M1 & M2
#define SENSOR2PIN 7 // SENSOR2PIN: This pin is connected by a green wire to the second beam-break sensor's "SIG" pin.
int sensor2State = HIGH;         // variable for reading the beam-break sensor2 status
int moveOperationCounter2 = 0; // This variable keeps track of the total number of "move" operations performed.





unsigned long lastDispenseTimer = 0; // This variable keeps track of the last time dispense was performed



// Function Prototypes:
void setupFoodDispensers();
void loopFoodDispensers(unsigned long currentLoopMillis);
void dispenseFeeder1();
void dispenseFeeder2();
void dispenseFood(int feederNumber);
void clockwiseDispense(Adafruit_StepperMotor *activeMotor);
void unjamDispenseBySimpleReverse(Adafruit_StepperMotor *activeMotor);
void unjamDispenseByTickTock(Adafruit_StepperMotor *activeMotor);



// Called from setup()
void setupFoodDispensers() {
  // initialize the sensor pin as an input:
  pinMode(SENSOR1PIN, INPUT);
  digitalWrite(SENSOR1PIN, HIGH); // turn on the pullup

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

void loopFoodDispensers(unsigned long currentLoopMillis) {
  // Check to see if it's possibly time to dispense
  if (currentLoopMillis - lastDispenseTimer >= PostDispenseTimeout) {
    //If we're continuously dispensing food, just do that without worrying about beam-break status.
    if (IS_DIAGNOSTIC_MODE && DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_FOOD) {
      dispenseFeeder1();
      if (IS_DUAL_MOTOR_MODE) {
        dispenseFeeder2();
      }
      return; // return without executing any more of the function
    }
    else {
      /* Check sensor beam state:
        LOW: Sensor Beam is broken
        HIGH: Sensor Beam has continuity
      */
      // Currently DIAGNOSTIC_MODE Only dispenses feeder1
      if ((sensor1State == LOW)) {
        //delay(40);
        dispenseFeeder1();
      }
      else if (IS_DUAL_MOTOR_MODE && (sensor2State == LOW)) {
        dispenseFeeder2();
      }
      else {
        // turn status LED off:
        //digitalWrite(LEDPIN, LOW);
      }
    }

  }
  // The status LED is left lit until it's possible to dispense again.
}



void dispenseFeeder1() {
  #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
    sendLoggingSignal(Food1, ActionDispense);
  #endif
  dispenseFood(1);
}

void dispenseFeeder2() {
  #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
    sendLoggingSignal(Food2, ActionDispense);
  #endif
  dispenseFood(2);
}

// dispense(int feederNumber): takes a feeder number (currently either 1 or 2) as an argument.
/* Called to dispense a single pellet for a single feeder, either feeder 1 or feeder 2.
   Keeps track of each feeders' moveOperationCounter to perform unjam operations.
*/
void dispenseFood(int feederNumber) {
  // save the last time you dispensed a pellet:
  lastDispenseTimer = millis();
  //digitalWrite(LEDPIN, HIGH); // Turn status LED on
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
  if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
    Serial.println("Moving: Clockwise " + String(moveOperationCounter1));
  }
  activeMotor->step(25, FORWARD, NumberOfStepperCoilsActivated);
  #if SHOULD_RELEASE_MOTORS_FOR_POWER_SAVING
    activeMotor->release();
  #endif
}

// Attempts to unjam by just moving backwards for this iteration.
void unjamDispenseBySimpleReverse(Adafruit_StepperMotor *activeMotor) {
  if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
    Serial.println("Moving: Counter-Clockwise " + String(moveOperationCounter1));
  }
  activeMotor->step(25, BACKWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
  #if SHOULD_RELEASE_MOTORS_FOR_POWER_SAVING
    activeMotor->release();
  #endif
}

// Attempts to unjam by quickly moving backwards, and then forward again for this iteration. This runs the risk of double-dispensing
void unjamDispenseByTickTock(Adafruit_StepperMotor *activeMotor) {
  if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
      Serial.println("Moving: UNJAM " + String(moveOperationCounter1));    
  }
  activeMotor->step(25, BACKWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
  activeMotor->step(25, FORWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
  #if SHOULD_RELEASE_MOTORS_FOR_POWER_SAVING
    activeMotor->release();
  #endif
}
