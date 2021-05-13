
# Each Water Port
# 1 Input Pin: Beambreak
# 3 Output Pins: {Solenoid, SignalChangedLabjack, SignalDispenseLabjack}
int sensor3State = HIGH;         // variable for reading the beam-break sensor3 status
int moveOperationCounter3 = 0; // This variable keeps track of the total number of "move" operations performed.
SolenoidState solenoid1State = CLOSED;         // reflects the open/closed state of the solenoid

