/* 
Burgess Lab IR beam (photointerrupter) motor food dispense
Written by Robert Zhang July 31st 2018
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

 
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);
#define LEDPIN 13 // Pin 13: Arduino has an LED connected on pin 1
#define SENSORPIN 4 // SENSORPIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
#define TTLPIN 13

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status
int moveOperationCounter = 0; // This variable keeps track of the total number of "move" operations performed.
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
 #define StepperSpeed 127 // The speed of the stepper in rpm (default 25).

 
void setup() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);  
  pinMode(TTLPIN, OUTPUT);
  digitalWrite(TTLPIN, LOW);
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  Serial.begin(9600); // set up Serial library at 9600 bps (for debugging)
  Serial.println("Food Hopper:");
  if (IS_DIAGNOSTIC_MODE) { //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    Serial.println("----- DIAGNOSTIC MODE -----");
  }
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  // The setSpeed() function controls the power level delivered to the motor. 
  myMotor->setSpeed(StepperSpeed); //The speed parameter is a value between 0 and 255.
}
void loop(){
  // read the state of the IR break beam sensor:
  sensorState = digitalRead(SENSORPIN);
  /* Check sensor beam state:
   * LOW: Sensor Beam is broken
   * HIGH: Sensor Beam has continuity
   */
  if ((sensorState == LOW) || IS_DIAGNOSTIC_MODE) {
    //delay(40);
    digitalWrite(LEDPIN, HIGH); // Turn status LED on
    digitalWrite(TTLPIN, HIGH);    // make motor turn
    if(digitalRead(TTLPIN)==HIGH){
      // The "%" operation is "modulus". The statement checks whether the moveOperationCounter is evenly divisible by ConsecutiveSameDirectionMovements. If it is, it performs the first case, otherwise the second.
      if (((moveOperationCounter % ConsecutiveSameDirectionMovements) == 0)) {
        //Every "ConsecutiveSameDirectionMovements + 1" steps we move counter-clockwise.
        Serial.println("Moving: Counter-Clockwise " + String(moveOperationCounter));
        myMotor->step(25, BACKWARD, NumberOfStepperCoilsActivated); // Changed from SINGLE to DOUBLE for extra torque
      }
      else {
        // Otherwise we move in the traditional (clockwise) direction
        Serial.println("Moving: Clockwise " + String(moveOperationCounter));
        myMotor->step(25, FORWARD, NumberOfStepperCoilsActivated);
      }
      moveOperationCounter++; // Increment the counter
    }
    delay(PostDispenseTimeout);
  }
  else {
    // turn status LED off:
    digitalWrite(LEDPIN, LOW);  
  }
}
 
