// Common.h: contains resources/definitions/functions shared by multiple components

// ensures this file description is only included once
#ifndef Common_h
#define Common_h

#define DEPLOY_ARDUINO_IS_UNO true //DEPLOY_ARDUINO_IS_UNO: true if sketch will be running on Arduino Uno. Else assumed to be "Adafruit pro trinket SV p2000"
//#define LEDPIN 13 // Pin 13: Arduino has an LED connected on pin 1


#define ENABLE_FOOD_DISPENSE true
#define ENABLE_WATER_DISPENSE false
#define ENABLE_RUNNING_WHEEL true
#define ENABLE_RHD2000_INTERFACE true

#define IS_DIAGNOSTIC_MODE true //IS_DIAGNOSTIC_MODE: if this value is false, all diagnostic settings will be ignored. This value should be false outside of testing.


#define PostDispenseTimeout 3000 //The number of milliseconds (1/1000 of a second) after dispensing that the system will wait before allowing input.
// Was 5000


#endif // end of the "#ifndef" at the start of the file that ensures this file is only included once. THIS MUST BE AT THE END OF THE FILE.
