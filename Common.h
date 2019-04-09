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

#define ENABLE_FOOD_DISPENSE true
#define ENABLE_WATER_DISPENSE true
#define ENABLE_RUNNING_WHEEL true
#define ENABLE_RHD2000_INTERFACE true
#define ENABLE_RHD2000_SERIAL_EMULATION true

#define IS_DIAGNOSTIC_MODE true //IS_DIAGNOSTIC_MODE: if this value is false, all diagnostic settings will be ignored. This value should be false outside of testing.
#define ENABLE_DIAGNOSTIC_SERIAL false 

#define PostDispenseTimeout 3000 //The number of milliseconds (1/1000 of a second) after dispensing that the system will wait before allowing input.
// Was 5000

unsigned long timer1;
unsigned long timer2;


// reflects the open/closed state of the beambreak sensor
enum BeamBreakState {
  CLEAR,
  BROKEN
};


// Values
enum SystemAddress { Water1 = 0b000, Water2 = 0b001, Food1 = 0b010, Food2 = 0b011, RunningWheel = 0b100};
enum EventType { SensorChange = 0b0, ActionDispense = 0b1};
// create a union to hold the data
union TimestampBuffer
{
    unsigned long longNumber;
    byte longBytes[4];
};
TimestampBuffer timestampBuffer;



#endif // end of the "#ifndef" at the start of the file that ensures this file is only included once. THIS MUST BE AT THE END OF THE FILE.
