/*
  Behavior Box: the sketch name (Food_hopper_pho) results from the fact that it has been generalized from a food-only dispenser.
  Currently allows two food and two water ports.
  Written by Robert Zhang July 31st 2018
  Modifed by Pho Hale 3/6/2019
*/
#include <Wire.h>

#include "Common.h";
#if ENABLE_FOOD_DISPENSE
  #include "Food_Dispenser.h"; //Depends on Common.h
#endif
#if ENABLE_WATER_DISPENSE
  #include "Water_Dispenser.h";
#endif

#include "Diagnostics.h";


void setup() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);

  Serial.begin(9600); // set up Serial library at 9600 bps (for debugging)
  Serial.println("Behavior Box:");
  if (IS_DIAGNOSTIC_MODE) { //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    Serial.println("----- DIAGNOSTIC MODE -----");
  }

  #if ENABLE_FOOD_DISPENSE
    setupFoodDispensers();
  #endif
  #if ENABLE_WATER_DISPENSE
    setupWaterDispensers();
  #endif


}

void loop() {
  // read the state of the IR break beam sensors:
  #if ENABLE_FOOD_DISPENSE
    sensor1State = digitalRead(SENSOR1PIN);
    if (IS_DUAL_MOTOR_MODE) {
      sensor2State = digitalRead(SENSOR2PIN);
    }
  #endif
  #if ENABLE_WATER_DISPENSE
    // Read the water sensors
    sensor3State = digitalRead(SENSOR3PIN);
    sensor4State = digitalRead(SENSOR4PIN);
  #endif

  // Get the current time in milliseconds
  unsigned long currentLoopMillis = millis();

  // Performs the interfacing with the processing software (running on the computer) while in interactive diagnostic mode
  if (IS_DIAGNOSTIC_MODE && SHOULD_USE_INTERACTIVE_DIAGNOSTIC) {
    loopDiagnostics(currentLoopMillis);
  }
  else {
    // We don't dispense unless not in interactive diagnostic mode
    #if ENABLE_FOOD_DISPENSE
      loopFoodDispensers(currentLoopMillis);
    #endif
    #if ENABLE_WATER_DISPENSE
      loopWaterDispensers(currentLoopMillis);
    #endif

  } // end interactive diagnostic if

} // end loop
