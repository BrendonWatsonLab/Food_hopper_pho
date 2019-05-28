// Common.h: contains resources/definitions/functions shared by multiple components
/* Main loop runs at around 4ms
 * 25ms when having to write out debug log to serial
 * 
 */
// ensures this file description is only included once
#ifndef Common_h
#define Common_h

#define DEPLOY_ARDUINO_IS_UNO true //DEPLOY_ARDUINO_IS_UNO: true if sketch will be running on Arduino Uno. Else assumed to be "Adafruit pro trinket SV p2000"
//#define LEDPIN 13 // Pin 13: Arduino has an LED connected on pin 1

// General Feature Enables:
/*
 * Setting one of the following preprocessor variables to false disables that subsystem.
 */
#define ENABLE_FOOD_DISPENSE true
#define ENABLE_WATER_DISPENSE true
#define ENABLE_RUNNING_WHEEL true

#define ENABLE_LOGGING_SIGNAL_ON_CHANGE true
#define ENABLE_RHD2000_INTERFACE true
#define ENABLE_MULTIPLEXER_INTERFACE true
#define ENABLE_ARDUINOMEGA_LABJACK_INTERFACE false




// FOOD:
/*
   There are two types of "move" operations: move-clockwise, move-counterclockwise
   The "move" operation performed depends on the moveOperationCounter. Every "ConsecutiveSameDirectionMovements + 1" steps we move counter-clockwise. Otherwise, we move clockwise.
   Integer overflow is not an issue, as we only care to keep track of the proper rotation direction so that we may alternate between to the two directions.
*/
#define PostDispenseTimeout 3000 //The number of milliseconds (1/1000 of a second) after dispensing that the system will wait before allowing another dispense event to occur.
// Was 5000
#define IS_DUAL_MOTOR_MODE true // IS_DUAL_MOTOR_MODE: if true, we use both motors. Otherwise we only use the motor connected to M3 & M4
#define DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_FOOD false //DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_FOOD: if this value is true the system will operate continuously, ignoring the beam break sensor. This serves to allow testing. This value should be false outside of testing.
#define SHOULD_RELEASE_MOTORS_FOR_POWER_SAVING true //SHOULD_RELEASE_MOTORS_FOR_POWER_SAVING: releases the stepper motors once their movement is complete to allow power savings. Otherwise they consume power maximally when they aren't moving to hold their position. 
#define ConsecutiveSameDirectionMovements 5 //Defines the number of times it moves in a single direction before alternating the direction of moment.
#define NumberOfStepperCoilsActivated DOUBLE // The number of coils in the stepper motor to activate. DOUBLE provides higher torque.
// StepperSpeed is a value used by the setSpeed() function controls the power level delivered to the motor. The speed parameter is a value between 0 and 255.
#define StepperSpeed 127 // The speed of the stepper in rpm (default 25, previous 127).


// WATER:
 /*
   After a beam-break, the solenoid opens for SolenoidOpenDuration to allow water to be dispensed.
   Following SolenoidOpenDuration, the solenoid is closed (stopping the flow of water) for at least SolenoidPostDoseClosedDuration before re-opening
*/
#define DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER false //DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER: if this value is true the system will operate continuously, ignoring the beam break sensor. This serves to allow testing. This value should be false outside of testing.
#define SolenoidDoseOpenDuration 200
#define SolenoidPostDoseClosedDuration 5000

// DIAGNOSTICS:
/*
 * 
 */
#define IS_DIAGNOSTIC_MODE true //IS_DIAGNOSTIC_MODE: if this value is false, all diagnostic settings will be ignored. This value should be false outside of testing.
#define ENABLE_DIAGNOSTIC_SERIAL true 
#define SHOULD_USE_INTERACTIVE_DIAGNOSTIC true //SHOULD_USE_INTERACTIVE_DIAGNOSTIC: enables extended diagnostics and testing via the helper Processing software.
#define INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT 10 //INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT: the minimum time between serial reads for interactive diagnostics

// MULTIPLEXER:
/*
 * 
 */
const int SIGNAL_ON_TIME = 10; //msec


// VARIABLES
/*
 * 
 */
unsigned long currentLoopMillis; // currentLoopMillis: the millis() recorded at the start of each iteration of the main loop.
unsigned long performanceTimer0; // performanceTimer0: a timer used to track the performance of the arduino by measuring the difference from the start of the last loop.

// reflects the open/closed state of the beambreak sensor
enum BeamBreakState {
  CLEAR,
  BROKEN
};


// Values
enum SystemAddress { Water1 = 0b000, Water2 = 0b001, Food1 = 0b010, Food2 = 0b011, RunningWheel = 0b100, Sync = 0b101};
enum EventType { SensorChange = 0b0, ActionDispense = 0b1};
// create a union to hold the data
union TimestampBuffer
{
    unsigned long longNumber;
    byte longBytes[4];
};
TimestampBuffer timestampBuffer;


#endif // end of the "#ifndef" at the start of the file that ensures this file is only included once. THIS MUST BE AT THE END OF THE FILE.
