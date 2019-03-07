/* Water 1 config:
 *  A "Water" consists of a solenoid that controls the flow of water from the resevour to the port. The port contains a beam-break sensor (SENSOR) that when broken dispenses a drop of water.
 */
#define SENSOR3PIN 3 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
int sensor3State = HIGH;         // variable for reading the beam-break sensor3 status
int moveOperationCounter3 = 0; // This variable keeps track of the total number of "move" operations performed.


/* Water 2 config:
 *  
 */
#define SENSOR4PIN 6 // SENSOR1PIN: This pin is connected by a green wire to the beam-break sensor's "SIG" pin.
int sensor4State = HIGH;         // variable for reading the beam-break sensor4 status
int moveOperationCounter4 = 0; // This variable keeps track of the total number of "move" operations performed.



void setupWaterDispensers() {
  // initialize the sensor pins as an input:
  pinMode(SENSOR3PIN, INPUT);     
  digitalWrite(SENSOR3PIN, HIGH); // turn on the pullup
  pinMode(SENSOR4PIN, INPUT);     
  digitalWrite(SENSOR4PIN, HIGH); // turn on the pullup

}
