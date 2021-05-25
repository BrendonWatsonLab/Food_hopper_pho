/*
Hardware Hookup:
Mux Breakout ----------- Arduino
     S0 ------------------- 9
     S1 ------------------- 10
     S2 ------------------- 13
     Z -------------------- 2
    VCC ------------------- 5V
    GND ------------------- GND
    (VEE should be connected to GND)
*/
/////////////////////
// Pin Definitions //
/////////////////////
const int multiplexerSelectPins[3] = {9, 10, 13}; // S0~9, S1~10, S2~13
const int multiplexerZOutputPin = 2; // Connect common (Z) to 2


// Function Prototypes:
void setupMultiplexerInterface();
void sendMultiplexerSignal(SystemAddress addr, EventType event);
void selectMuxPin(byte pin);
void zeroAll();

// Called from setup()
void setupMultiplexerInterface() {
  // Set up the select pins, as outputs
  for (int i=0; i<3; i++)
  {
    pinMode(multiplexerSelectPins[i], OUTPUT);
    digitalWrite(multiplexerSelectPins[i], LOW);
  }
  pinMode(multiplexerZOutputPin, OUTPUT); // Set up Z as an output
  zeroAll();
}


void sendMultiplexerSignal(SystemAddress addr, EventType event) {
  if (addr > 3) {
    // Ignore the running wheel or any other inputs for now
    return;
  }

  byte outputPin = addr;
  // addr is either {0,1,2,3}
  if (event == ActionDispense) {
    // If the event type is a dispense event, add four. Shifting the outputPin to {4,5,6,7}
    outputPin = outputPin + 4;
  }

  // Set the S0, S1, and S2 pins to select our active output (Y0-Y7):
  selectMuxPin(outputPin);
  // While the output is selected perform the main action
  digitalWrite(multiplexerZOutputPin, LOW);
  delay(SIGNAL_ON_TIME);
  digitalWrite(multiplexerZOutputPin, HIGH);
}



// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin) {
  if (pin > 7) return; // Exit if pin is out of scope
  for (int i=0; i<3; i++)
  {
    if (pin & (1<<i))
      digitalWrite(multiplexerSelectPins[i], HIGH);
    else
      digitalWrite(multiplexerSelectPins[i], LOW);
  }
}


// Loops through all outputs and sets them to low.
void zeroAll() {
  // Cycle from pins Y0 to Y7 first
  for (int pin=0; pin<=7; pin++)
  {
    // Set the S0, S1, and S2 pins to select our active output (Y0-Y7):
    selectMuxPin(pin);
    // While the output is selected perform the main action
    digitalWrite(multiplexerZOutputPin, LOW);
    delay(10);
  }
}
