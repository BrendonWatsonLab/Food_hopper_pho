// Common.h: contains resources/definitions/functions shared by multiple components
/* Main loop runs at around 4ms
 * 25ms when having to write out debug log to serial
 * 
 */
// ensures this file description is only included once
#ifndef Common_h
#define Common_h

#define DEPLOY_ARDUINO_IS_UNO true //DEPLOY_ARDUINO_IS_UNO: true if sketch will be running on Arduino Uno. Else assumed to be "Adafruit pro trinket SV p2000"

// General Feature Enables:
/*
 * Setting one of the following preprocessor variables to false disables that subsystem.
 */
#define ENABLE_FOOD_DISPENSE true
#define ENABLE_WATER_DISPENSE true

#define ENABLE_LOGGING_SIGNAL_ON_CHANGE true
#define ENABLE_RHD2000_INTERFACE true
#define ENABLE_MULTIPLEXER_INTERFACE false
#define ENABLE_ARDUINOMEGA_LABJACK_INTERFACE true



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
#define REQUIRE_STATE_CHANGE_BEFORE_SECOND_FOOD_DISPENSE true //REQUIRE_STATE_CHANGE_BEFORE_SECOND_FOOD_DISPENSE: if false, permits dispensing food pellets if the beambreak remains broken after the timeout period. If true, it requires the the beambreak state to open before it closes again. 

// WATER:
 /*
   After a beam-break, the solenoid opens for SolenoidOpenDuration to allow water to be dispensed.
   Following SolenoidOpenDuration, the solenoid is closed (stopping the flow of water) for at least SolenoidPostDoseClosedDuration before re-opening
*/
// UNCOMMENT THIS BLOCK FOR REGULAR OPERATION OR DYNAMIC DIAGNOSTIC MODE:
#define DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER false //DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER: if this value is true the system will operate continuously, ignoring the beam break sensor. This serves to allow testing. This value should be false outside of testing.
#define SolenoidDoseOpenDuration 300 //SolenoidDoseOpenDuration: The time for which the solenoid is open and the water is allowed to flow freely. 
#define SolenoidPostDoseClosedDuration 1500 //SolenoidPostDoseClosedDuration: The time after a water dispense event before another water dispense event can be re-triggered
#define REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE true //REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE: if false, permits dispensing water if the beambroke remains broken after the timeout period. If true, it requires the the beambreak state to open before it closes again. 

// UNCOMMENT THIS BLOCK FOR MANUAL FIRMWARE-MEDIATED WATERSYSTEM FLUSHING:
//#define DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER true //DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER: if this value is true the system will operate continuously, ignoring the beam break sensor. This serves to allow testing. This value should be false outside of testing.
//#define SolenoidDoseOpenDuration 300000 //SolenoidDoseOpenDuration: The time for which the solenoid is open and the water is allowed to flow freely. 
//#define SolenoidPostDoseClosedDuration 0 //SolenoidPostDoseClosedDuration: The time after a water dispense event before another water dispense event can be re-triggered
//#define REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE false //REQUIRE_STATE_CHANGE_BEFORE_SECOND_WATER_DISPENSE: if false, permits dispensing water if the beambroke remains broken after the timeout period. If true, it requires the the beambreak state to open before it closes again. 

// DIAGNOSTICS:
/*
 * 
 */
#define IS_DIAGNOSTIC_MODE true //IS_DIAGNOSTIC_MODE: if this value is false, all diagnostic settings will be ignored. This value should be false outside of testing.
//#define IS_DIAGNOSTIC_MODE false //IS_DIAGNOSTIC_MODE: if this value is false, all diagnostic settings will be ignored. This value should be false outside of testing.
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

unsigned long lastSensorChangeEvent1 = 0; // The last loop time the sensor was detected to have changed value
unsigned long lastSensorChangeEvent2 = 0; 
unsigned long lastSensorChangeEvent3 = 0;
unsigned long lastSensorChangeEvent4 = 0;


//unsigned long[4] lastSensorChangeEventTime = {0, 0, 0, 0};


// reflects the open/closed state of the beambreak sensor
// 	LOW: Sensor Beam is BROKEN
// 	HIGH: Sensor Beam has continuity (CLEAR)

enum BeamBreakState { 
	BBS_BROKEN = 0b000,
	BBS_CLEAR = 0b001
};


// Values
enum SystemAddress { Water1 = 0b000, Water2 = 0b001, Food1 = 0b010, Food2 = 0b011, RunningWheel = 0b100, Sync = 0b101};
/* SystemAddress
 * This type specifies which system/port is being referred to.
 * Water 1: 0
 * Water 2: 1
 * Food 1: 2
 * Food 2: 3
 * RunningWheel: 4
 * Sync: 5
 */
enum EventType { SensorChange = 0b0, ActionDispense = 0b1};
/* EventType
 * This type species which type of event has occured
 * Sensor Change: 0 // Occurs when a beam-break sensor changes state
 * Action Dispense: 1 // Occurs when a dispense action has occured
 */
 
// create a union to hold the data
union TimestampBuffer
{
    unsigned long longNumber;
    byte longBytes[4];
};
TimestampBuffer timestampBuffer;


#endif // end of the "#ifndef" at the start of the file that ensures this file is only included once. THIS MUST BE AT THE END OF THE FILE.
