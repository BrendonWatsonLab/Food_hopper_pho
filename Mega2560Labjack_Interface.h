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
const int megaOutputPins[8] = {22, 24, 26, 28, 23, 25, 27, 29};

// Function Prototypes:
void setupMegaOutputInterface();
void sendMegaOutputSignal(SystemAddress addr, EventType event);


// Called from setup()
void setupMegaOutputInterface() {
  // Set up the select pins, as outputs
  for (int i=0; i<8; i++)
  {
    pinMode(megaOutputPins[i], OUTPUT);
    digitalWrite(megaOutputPins[i], LOW);
  }
}

//TODO: deal with running wheel
void sendMegaOutputSignal(SystemAddress addr, EventType event) {
  if (addr > 3) {
    // Ignore the running wheel or any other inputs for now
    return;
  }
  byte outputPinIndex = addr;
  // addr is either {0,1,2,3}
  if (event == ActionDispense) {
    // If the event type is a dispense event, add four. Shifting the outputPin to {4,5,6,7}
    outputPinIndex = outputPinIndex + 4;
  }
  int outputPin = megaOutputPins[outputPinIndex];
  // While the output is selected perform the main action
  digitalWrite(outputPin, LOW);
  delay(SIGNAL_ON_TIME);
  digitalWrite(outputPin, HIGH);
}
