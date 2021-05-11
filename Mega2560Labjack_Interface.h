// Mega2560Labjack_Interface.h
// The Arduino Mega 2560 doesn't require a multiplexer to output to the LabJack T7.
// This file is only used by the Arduino Mega.


/*
Hardware Hookup:
Labjack Input ----------- Arduino
     FIO1 ------------------- 22
     FIO2 ------------------- 24
     FIO3 ------------------- 26
     FIO4 ------------------- 28
     FIO5 ------------------- 23
     FIO6 ------------------- 25
     FIO7 ------------------- 27
     MIO0 ------------------- 29
     MIO1 ------------------- 31
     MIO2 ------------------- 30
     GND ------------------- GND
*/
/////////////////////
// Pin Definitions //
/////////////////////
const int megaOutputPins[9] = {22, 24, 26, 28, 23, 25, 27, 29, 30};

// Function Prototypes:
void setupMegaOutputInterface();
void sendMegaOutputSignal(SystemAddress addr, EventType event);
bool turnOffSignal(void *argument /* optional argument given to in/at/every */);


// Called from setup()
void setupMegaOutputInterface() {
  // Set up the select pins, as outputs
  for (int i=0; i<9; i++)
  {
    pinMode(megaOutputPins[i], OUTPUT);
    digitalWrite(megaOutputPins[i], HIGH);
  }
}

void sendMegaOutputSignal(SystemAddress addr, EventType event) {
  if (addr > 4) {
    // Ignore the running wheel or any other inputs for now
    return;
  }
  byte outputPinIndex = addr;
  if (addr == 4) {
    // Running Wheel addr is {8}
    outputPinIndex = 8;
  }
  else {
    // addr is either {0,1,2,3}
    if (event == ActionDispense) {
      // If the event type is a dispense event, add four. Shifting the outputPin to {4,5,6,7}
      outputPinIndex = outputPinIndex + 4;
    }
  }
  int outputPin = megaOutputPins[outputPinIndex];
  // While the output is selected perform the main action
  digitalWrite(outputPin, LOW);

// CONCERN: Can this timer routine be queued up multiple times? What happens if a new signal comes in during that time?

  //timer.in(SIGNAL_ON_TIME, [](void *argument) -> bool { return argument; }, argument);
  timer.in(SIGNAL_ON_TIME, turnOffSignal, outputPin); // or with an optional argument for function_to_call
  //delay(SIGNAL_ON_TIME);
  //digitalWrite(outputPin, HIGH);
}


// Called when ready to turn off the pin
bool turnOffSignal(void *argument /* optional argument given to in/at/every */) {
   digitalWrite(argument, HIGH);
   return false; // to repeat the action - false to stop
}
