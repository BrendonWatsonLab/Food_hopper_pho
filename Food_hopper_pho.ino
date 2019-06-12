/*
  Behavior Box: the sketch name (Food_hopper_pho) results from the fact that it has been generalized from a food-only dispenser.
  Currently allows two food and two water ports.
  Written by Robert Zhang July 31st 2018
  Modifed by Pho Hale 3/6/2019
*/

//10msec main loop runtime goal

#include <Wire.h>

#include "Common.h";
#if ENABLE_RHD2000_INTERFACE
  #include "RHD2000_Interface.h";
#endif
#if ENABLE_MULTIPLEXER_INTERFACE
  #include "Multiplexer_Interface.h";
#endif
#if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
  #include "Mega2560Labjack_Interface.h";
#endif

// Function Prototypes:
void sendLoggingSignal(SystemAddress addr, EventType event, int newValue);
void loopDispense(unsigned long currentLoopMillis);

#if ENABLE_FOOD_DISPENSE
  #include "Food_Dispenser.h"; //Depends on Common.h
#endif
#if ENABLE_WATER_DISPENSE
  #include "Water_Dispenser.h";
#endif
#if ENABLE_RUNNING_WHEEL
  #include "RunningWheel.h";
#endif

#include "Diagnostics.h";



void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps (for debugging)
  Serial.println("Behavior Box:");
  if (IS_DIAGNOSTIC_MODE) { //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    Serial.println("----- DIAGNOSTIC MODE -----");
  }

  #if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
    setupMegaOutputInterface();
  #endif
  #if ENABLE_MULTIPLEXER_INTERFACE
    setupMultiplexerInterface();
  #endif
  #if ENABLE_RHD2000_INTERFACE
    setupRHD2000Interface();
  #endif
  #if ENABLE_FOOD_DISPENSE
    setupFoodDispensers();
  #endif
  #if ENABLE_WATER_DISPENSE
    setupWaterDispensers();
  #endif
  #if ENABLE_RUNNING_WHEEL
    setupRunningWheel();
  #endif

  #if IS_DIAGNOSTIC_MODE 
    performanceTimer0 = millis();
  #endif
}

void loop() {
  // Get the current time in milliseconds
  currentLoopMillis = millis();
  #if IS_DIAGNOSTIC_MODE 
    //If the system is in diagnostic mode, output a line to the serial terminal indicating this to prevent diagnostic builds being deployed to production hardware.
    unsigned long lastLoopDifference = currentLoopMillis - performanceTimer0;
    Serial.println(lastLoopDifference);
    performanceTimer0 = currentLoopMillis;
  #endif

  
  // read the state of the IR break beam sensors:
  #if ENABLE_FOOD_DISPENSE
    int prevSensor1State = sensor1State;
    sensor1State = digitalRead(SENSOR1PIN);
    #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
      if (prevSensor1State != sensor1State) {
        sendLoggingSignal(Food1, SensorChange, sensor1State);
      }
    #endif
    #if IS_DUAL_MOTOR_MODE
      int prevSensor2State = sensor2State;
      sensor2State = digitalRead(SENSOR2PIN);
      #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
        if (prevSensor2State != sensor2State) {
          sendLoggingSignal(Food2, SensorChange, sensor2State);
        }
      #endif
    #endif
  #endif
  #if ENABLE_WATER_DISPENSE
    int prevSensor3State = sensor3State;
    int prevSensor4State = sensor4State;
    // Read the water sensors
    sensor3State = digitalRead(SENSOR3PIN);
    sensor4State = digitalRead(SENSOR4PIN);
    #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
      // Check for changes:
      if (prevSensor3State != sensor3State) {
        sendLoggingSignal(Water1, SensorChange, sensor3State);
      }
      if (prevSensor4State != sensor4State) {
        sendLoggingSignal(Water2, SensorChange, sensor4State);
      }
    #endif
  #endif
  #if ENABLE_RUNNING_WHEEL
    int prevRunningWheelSensorState = runningWheelSensorState;
    // read the state of the sensor pin:
    runningWheelSensorState = digitalRead(RUNNINGWHEEL_SENSOR_PIN);
    #if ENABLE_LOGGING_SIGNAL_ON_CHANGE
      if (prevRunningWheelSensorState != runningWheelSensorState) {
        sendLoggingSignal(RunningWheel, SensorChange, runningWheelSensorState);
      }
    #endif
  #endif



  // Do full loopRHD2000Interface even in INTERACTIVE DIAGNOSTIC MODE
  #if ENABLE_RHD2000_INTERFACE
    loopRHD2000Interface(currentLoopMillis);
  #endif

  // Performs the interfacing with the processing software (running on the computer) while in interactive diagnostic mode
  if (IS_DIAGNOSTIC_MODE && SHOULD_USE_INTERACTIVE_DIAGNOSTIC) {
    loopDiagnostics(currentLoopMillis);
    // Perform normal dispense events unless diagnostic_disable_dispensing is true
    if (diagnostic_disable_dispensing == false) {
      loopDispense(currentLoopMillis);
    }
  }
  else {
    loopDispense(currentLoopMillis);
  } // end interactive diagnostic if


} // end loop


// A General logging function that calls the appropriate output functions.
void sendLoggingSignal(SystemAddress addr, EventType event, int newValue) {
  #if ENABLE_RHD2000_INTERFACE
    sendRHD2000Signal(addr, event);
  #endif
  #if ENABLE_MULTIPLEXER_INTERFACE
    sendMultiplexerSignal(addr, event);
  #endif
  #if ENABLE_ARDUINOMEGA_LABJACK_INTERFACE
    sendMegaOutputSignal(addr, event, newValue);
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
    #if ENABLE_RUNNING_WHEEL
      loopRunningWheel(currentLoopMillis);
    #endif
}
