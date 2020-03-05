// Diagnostics.h
/*
   This file contains diagnostic settings and functions that can be used to test and manipulate the operation of the behavior box.
   "Interactive" refers to the user's ability to control the arduino via the Processing proram while the Arduino is connected over USB.
   The Processing sketch must be quit before the Arduino Serial Monitor can be used or a new sketch uploaded to the Arduino. This is because only one program can be using the Serial port at the same time.
*/

char diagnostic_val; // Data received from the serial port
unsigned long lastDiagnosticSerialReadTimer = 0; // This variable keeps track of the last time diagnostic serial read was performed
bool diagnostic_disable_dispensing = false; //diagnostic_disable_dispensing: if true, dispense events are not performed as a result of beambreak events.

// Function Prototypes:
void loopDiagnostics(unsigned long currentLoopMillis);
void diagnostic_read_command();
void diagnostic_send_info();


void loopDiagnostics(unsigned long currentLoopMillis) {
  if (currentLoopMillis - lastDiagnosticSerialReadTimer >= INTERACTIVE_DIAGNOSTIC_SERIAL_READ_TIMEOUT) {
    diagnostic_read_command();
    lastDiagnosticSerialReadTimer = currentLoopMillis;
  }
  #if ENABLE_DIAGNOSTIC_SERIAL
    diagnostic_send_info();
  #endif
}

// Called to read in a "command" that has been sent by the Processing sketch over serial.
/*
enum EventType { SensorChange = 0b0, ActionDispense = 0b1};

// +------------------------------+------------+------------+
// |           Command            | Dispenser1 | Dispenser2 |
// +------------------------------+------------+------------+
// | NOOP                         | '0'        | '0'        |
// | ClockwiseDispense            | '1'        | 'A'        |
// | UnjamDispenseBySimpleReverse | '2'        | 'B'        |
// | UnjamDispenseByTickTock      | '3'        | 'C'        |
// | NormalDispense               | '4'        | 'D'        |
// | NormalWaterDispense          | '5'        | 'E'        |
// | SolenoidOpen                 | '6'        | 'F'        |
// | SolenoidClose                | '7'        | 'G'        |
// +------------------------------+------------+------------+


*/
void diagnostic_read_command() {
  if (Serial.available()) { // If data is available to read,
    diagnostic_val = Serial.read(); // read it and store it in val
    // Check the character recieved
    if (diagnostic_val == '0') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 0");
        //NO-OP
      }
    }
    else if (diagnostic_val == '1') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 1");
      }
#if ENABLE_FOOD_DISPENSE
      clockwiseDispense(motor1);
#endif
    }
    else if (diagnostic_val == '2') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 2");
      }
#if ENABLE_FOOD_DISPENSE
      unjamDispenseBySimpleReverse(motor1);
#endif
    }
    else if (diagnostic_val == '3') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 3");
      }
#if ENABLE_FOOD_DISPENSE
      unjamDispenseByTickTock(motor1);
#endif
    }
    else if (diagnostic_val == '4') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 4");
      }
#if ENABLE_FOOD_DISPENSE
      dispenseFeeder1();
#endif
    }
    else if (diagnostic_val == '5') { //NormalWaterDispense
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 5");
      }
      #if ENABLE_WATER_DISPENSE
        // Disable any existing HeldOpenState overiride unless it was set by the firmware preprocessor variable DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER:
        #if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
          // If DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER is true, don't allow anything to set the HeldOpenState to false
        #else
          solenoid1HeldOpenState = false;
          // DECISION: If the solenoid is currently being held open and the user pushes the regular dispense event, it will remain open for the duration expected and then close. This is opposed to closing first, and then opening like normal. A normal dispense issued by diagnostic command will effectively end the open period then.
          openSolenoid(1);
        #endif

      #endif
    }
    else if (diagnostic_val == '6') { // SolenoidOpen
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 6");
      }
      #if ENABLE_WATER_DISPENSE
        solenoid1HeldOpenState = true;
      #endif
    }
    else if (diagnostic_val == '7') { // SolenoidClose
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: 7");
      }
      #if ENABLE_WATER_DISPENSE
        #if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
          // If DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER is true, don't allow anything to set the HeldOpenState to false
        #else
          solenoid1HeldOpenState = false;
        #endif
      #endif
    }
    else if (diagnostic_val == 'A') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: A");
      }
#if ENABLE_FOOD_DISPENSE
      clockwiseDispense(motor2);
#endif
    }
    else if (diagnostic_val == 'B') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: B");
      }
#if ENABLE_FOOD_DISPENSE
      unjamDispenseBySimpleReverse(motor2);
#endif
    }
    else if (diagnostic_val == 'C') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: C");
      }
#if ENABLE_FOOD_DISPENSE
      unjamDispenseByTickTock(motor2);
#endif
    }
    else if (diagnostic_val == 'D') {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: D");
      }
#if ENABLE_FOOD_DISPENSE
      dispenseFeeder2();
#endif
    }
    else if (diagnostic_val == 'E') { //NormalWaterDispense
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: E");
      }
      #if ENABLE_WATER_DISPENSE
        // Disable any existing HeldOpenState overiride unless it was set by the firmware preprocessor variable DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER:
        #if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
          // If DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER is true, don't allow anything to set the HeldOpenState to false
        #else
          solenoid2HeldOpenState = false;
          // DECISION: If the solenoid is currently being held open and the user pushes the regular dispense event, it will remain open for the duration expected and then close. This is opposed to closing first, and then opening like normal. A normal dispense issued by diagnostic command will effectively end the open period then.
          openSolenoid(2);
        #endif

      #endif
    }
    else if (diagnostic_val == 'F') { // SolenoidOpen
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: F");
      }
      #if ENABLE_WATER_DISPENSE
        solenoid2HeldOpenState = true;
      #endif
    }
    else if (diagnostic_val == 'G') { // SolenoidClose
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("diagnostic_val: G");
      }
      #if ENABLE_WATER_DISPENSE
        #if DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER
          // If DIAGNOSTIC_SHOULD_CONTINUOUSLY_DISPENSE_WATER is true, don't allow anything to set the HeldOpenState to false
        #else
          solenoid2HeldOpenState = false;
        #endif
      #endif
    }
    else {
      if (IS_DIAGNOSTIC_MODE && ENABLE_DIAGNOSTIC_SERIAL) {
        Serial.println("Unknown diagnostic_val");
      }
    } // end cases
  }
  //delay(10); // Wait 10 milliseconds for next reading
}


// Called to send stats/info over serial to the Processing sketch running on a computer connected to the Arduino via USB.
void diagnostic_send_info() {
  // Food Sensors
#if ENABLE_FOOD_DISPENSE
  Serial.print("INFO://");
  Serial.print("sensor1State//");
  Serial.println(sensor1State);

  Serial.print("INFO://");
  Serial.print("moveOperationCounter1//");
  Serial.println(moveOperationCounter1);

  Serial.print("INFO://");
  Serial.print("sensor2State//");
  Serial.println(sensor2State);

  Serial.print("INFO://");
  Serial.print("moveOperationCounter2//");
  Serial.println(moveOperationCounter2);
#endif

  // Water Sensors:
#if ENABLE_WATER_DISPENSE
  Serial.print("INFO://");
  Serial.print("sensor3State//");
  Serial.println(sensor3State);

  Serial.print("INFO://");
  Serial.print("moveOperationCounter3//");
  Serial.println(moveOperationCounter3);

  Serial.print("INFO://");
  Serial.print("sensor4State//");
  Serial.println(sensor4State);

  Serial.print("INFO://");
  Serial.print("moveOperationCounter4//");
  Serial.println(moveOperationCounter4);
#endif


}
