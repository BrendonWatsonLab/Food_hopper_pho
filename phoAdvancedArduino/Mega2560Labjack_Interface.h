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


// Function Prototypes:
void setupMegaOutputInterface();
int getMegaOutputSignalPin(SystemAddress addr, EventType event);


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



int getMegaOutputSignalPin(SystemAddress addr, EventType event) {
	if (addr > 4) {
		// No signals above addr 4 should be sent.
		return -1;
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
	const int outputPin = megaOutputPins[outputPinIndex];
	return outputPin;
}
