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
// const int megaOutputRegisters[9] = {PORTA0, PORTA2, PORTA4, PORTA6, PORTA1, PORTA3, PORTA5, PORTA7};


// Variables

// reflects the open/closed state of the solenoid
enum LabjackSignalPinState {
  LabjackSignalPinState_Signalling = 0b0,
  LabjackSignalPinState_Rest = 0b1
};

LabjackSignalPinState labjackSignalPinState[8] = {LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest, LabjackSignalPinState_Rest};         // reflects the HIGH/LOW state of the labjack signal pins
unsigned long lastSignalSignallingTimer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long lastSignalSignalCompleteTimer[8] = {0, 0, 0, 0, 0, 0, 0, 0};



// Function Prototypes:
void setupMegaOutputInterface();
void sendMegaOutputSignal(SystemAddress addr, EventType event);
void loopEndMegaOutputSignals();
void debugTestOutputPorts();


// bool turnOffSignal(void *argument /* optional argument given to in/at/every */);


// Called from setup()
void setupMegaOutputInterface() {
  // Set up the select pins, as outputs
  for (int i=0; i<9; i++)
  {
    pinMode(megaOutputPins[i], OUTPUT);
	// Do I need to turn on the pull-ups?

    digitalWrite(megaOutputPins[i], LabjackSignalPinState_Rest);
  }
}

void sendMegaOutputSignal(SystemAddress addr, EventType event) {
  if (addr > 4) {
    // No signals above addr 4 should be sent.
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
	  /*	Water1: 0+4 = 4; -> Pin 23
	  	Water2: 1+4 = 5; -> Pin 25
		
			* Water 1: 0+4 = 4; -> Pin 23
			* Water 2: 1+4 = 5; -> Pin 25
			// * Food 1: 2+4 = 6; -> Pin 27
			// * Food 2: 3+4 = 7; -> Pin 29
			* RunningWheel: 4
			* Sync: 5
		*/
    }
  }
  int outputPin = megaOutputPins[outputPinIndex];
  // While the output is selected perform the main action

  // check if it's still set low, and if not, set it low and update the lastSignalLowTimer:


  if (labjackSignalPinState[outputPinIndex] == LabjackSignalPinState_Rest) {
  	labjackSignalPinState[outputPinIndex] = LabjackSignalPinState_Signalling;
  	digitalWrite(outputPin, LabjackSignalPinState_Signalling);
  	lastSignalSignallingTimer[outputPinIndex] = millis(); // Capture the time when the pin was set low
  }
  else {
	  // Error, the pin was attempted to be set low while it was already still low. 
	  // TODO: figure how much time is left and maybe perform and update??
	  Serial.println("ERROR: Attempted to set Labjack pin to Signalling (LOW) when already signalling!!!");
  }
}


// Set all simultaneously:
/*
PORTA = 0b10000000

*/


// Called to check whether to turn off the pin
void loopEndMegaOutputSignals() {
	// Get the actual millis again
	unsigned long currentMillis = millis();

	for (int i=0; i<8; i++)
	{
		// Loop through the labjackSignalPins and see if any are up for termination
		if (labjackSignalPinState[i] == LabjackSignalPinState_Signalling) {
			// Check the lastSignalLowTimer to see how long it has been LabjackSignalPinState_Signalling
			if ((currentMillis - lastSignalSignallingTimer[i]) >= SignalOnDuration) {
				// Turn off the signal
				// digitalWrite(megaOutputPins[i], LabjackSignalPinState_Rest); // Set its output pin to Rest/high
				digitalWrite(megaOutputPins[i], LabjackSignalPinState_Rest); // Set its output pin to Rest/high
				// WRITE(megaOutputRegisters[i], LabjackSignalPinState_Rest);
				labjackSignalPinState[i] = LabjackSignalPinState_Rest;  // Set the pin state
        lastSignalSignalCompleteTimer[i] = currentMillis;
			}
      else {
        // TEsting only:
        Serial.print("DEBUG: loopEndMegaOutputSignals() encountered but signal[");
        Serial.print(i);
        Serial.print("] is not finished yet and has ");
        Serial.print((currentMillis - lastSignalSignallingTimer[i]));
        Serial.println(" [ms] remaining.");
      }
		} // end if 
	} // end for
}


void debugTestOutputPorts() {

 // loopEndMegaOutputSignals();
  sendMegaOutputSignal(Water1, ActionDispense);

  performanceTestingGeneralPurposeCounter0++;

  
 // delay(10);
//  sendMegaOutputSignal(Water1, ActionDispense);
//  delay(10);
//  loopEndMegaOutputSignals();

//  sendMegaOutputSignal(Water2, ActionDispense);
//  delay(10);
//  loopEndMegaOutputSignals();

 // delay(2000); // 2 sec delay btween iterations of the loop

}
