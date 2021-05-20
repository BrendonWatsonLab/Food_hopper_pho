/*
  Behavior Box: the sketch name (Food_hopper_pho) results from the fact that it has been generalized from a food-only dispenser.
  Currently allows two food and two water ports.
  Written by Robert Zhang July 31st 2018
  Modifed by Pho Hale 3/6/2019
*/

//10msec main loop runtime goal

#include <Wire.h>
#include "Common.h"

#if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
  #include "Mega2560Labjack_Interface.h"
#endif

// Function Prototypes:
void sendLoggingSignal(SystemAddress addr, EventType event);
void loopDispense(unsigned long currentLoopMillis);
//void loop_disabled()


#if ENABLE_FOOD_DISPENSE
  #include "Food_Dispenser.h" //Depends on Common.h
#endif
#if ENABLE_WATER_DISPENSE
  #include "Water_Dispenser.h"
#endif



void setup() {
  // Serial.begin(9600); // set up Serial library at 9600 bps (for debugging)
  // Serial.println("Behavior Box:");
  // if (IS_DIAGNOSTIC_MODE) { //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
  //   Serial.println("----- DIAGNOSTIC MODE -----");
  // }

  #if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
    setupMegaOutputInterface();
  #endif
  #if ENABLE_FOOD_DISPENSE
    setupFoodDispensers();
  #endif
  #if ENABLE_WATER_DISPENSE
    setupWaterDispensers();
  #endif

  #if IS_DIAGNOSTIC_MODE 
    performanceTimer0 = millis();
  #endif
}


//void loop() {
//  // Pho: Just test the output signals
//  currentLoopMillis = millis();
//
//  debugTestOutputPorts();
//
//}

void loop() {
  // Get the current time in milliseconds
  currentLoopMillis = millis();

  #if IS_DIAGNOSTIC_MODE 
    performanceLoopCurrentLoopIterationCount++; // Increment the loop iteration counter: 
    //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    unsigned long lastLoopDifference = currentLoopMillis - performanceTimer0;
//    Serial.print("iter[");
//    Serial.print(performanceLoopCurrentLoopIterationCount);
//    Serial.print("]: lastLoopDifference(");
//    Serial.print(lastLoopDifference);  
//    Serial.print("), currentLoopMillis(");
//    Serial.print(currentLoopMillis);
//    Serial.println(");");
    
    performanceTimer0 = currentLoopMillis;
  #endif

  #if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
		// Added to turn off signals that are currently high.
		loopEndMegaOutputSignals();
	#endif

  debugTestOutputPorts();
  // CONCERN: Should these digital reads be done in batch/bulk at the start of the loop cycle (preventing delays that occur due to logging-induced latency)?
  
  // // read the state of the IR break beam sensors:
  // #if ENABLE_FOOD_DISPENSE
  //   BeamBreakState prevSensor1State = sensor1State;
  //   sensor1State = BeamBreakState(digitalRead(SENSOR1PIN));
  //   if (prevSensor1State != sensor1State) {
  //     lastSensorChangeEvent1 = currentLoopMillis;
  //   }
  //   #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
  //     if (prevSensor1State != sensor1State) {
  //       sendLoggingSignal(Food1, SensorChange);
  //     }
  //   #endif
  //   #if IS_DUAL_MOTOR_MODE
  //     BeamBreakState prevSensor2State = sensor2State;
  //     sensor2State = BeamBreakState(digitalRead(SENSOR2PIN));
  //     if (prevSensor2State != sensor2State) {
  //       lastSensorChangeEvent2 = currentLoopMillis;
  //     }
  //     #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
  //       if (prevSensor2State != sensor2State) {
  //         sendLoggingSignal(Food2, SensorChange);
  //       }
  //     #endif
  //   #endif
  // #endif
  // #if ENABLE_WATER_DISPENSE
  //   BeamBreakState prevSensor3State = sensor3State;
  //   BeamBreakState prevSensor4State = sensor4State;
  //   // Read the water sensors
  //   sensor3State = BeamBreakState(digitalRead(SENSOR3PIN));
  //   sensor4State = BeamBreakState(digitalRead(SENSOR4PIN));

	// bool didSensor3StateChange = (prevSensor3State != sensor3State);
	// bool didSensor4StateChange = (prevSensor4State != sensor4State);

  //   // Check for changes:
  //   if (didSensor3StateChange) {
  //     lastSensorChangeEvent3 = currentLoopMillis;
  //   }
  //   if (didSensor4StateChange) {
  //     lastSensorChangeEvent4 = currentLoopMillis;
  //   }
      
  //   #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
  //     // Check for changes:
  //     if (didSensor3StateChange) {
  //       sendLoggingSignal(Water1, SensorChange);
  //     }
  //     if (didSensor4StateChange) {
  //       sendLoggingSignal(Water2, SensorChange);
  //     }
  //   #endif
  // #endif

	// loopDispense(currentLoopMillis);


} // end loop


// A General logging function that calls the appropriate output functions.
void sendLoggingSignal(SystemAddress addr, EventType event) {
  #if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
    sendMegaOutputSignal(addr, event);
  #endif
}

// Called from the main loop to perform the dispense loops for enabled output modalities.
void loopDispense(unsigned long currentLoopMillis) {
  #if ENABLE_FOOD_DISPENSE
    loopFoodDispensers(currentLoopMillis);
  #endif
  #if ENABLE_WATER_DISPENSE
    loopWaterDispensers(currentLoopMillis);
  #endif
}
