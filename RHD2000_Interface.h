// RHD2000_Interface

#define RHD2000_INTERFACE_OUTPUT0_PIN 0
int rhd2000InterfaceOutput0State = LOW;

#define RHD2000_INTERFACE_OUTPUT1_PIN 1
int rhd2000InterfaceOutput1State = LOW;

#define RHD2000_INTERFACE_OUTPUT2_PIN 2
int rhd2000InterfaceOutput2State = LOW;

#define RHD2000_INTERFACE_HIGH_Duration 40

// Function Prototypes:
void setupRHD2000Interface();
void loopRHD2000Interface(unsigned long currentLoopMillis);
void outputSignals();

// Called from setup()
void setupRHD2000Interface() {
  // initialize the output pins:
  pinMode(RHD2000_INTERFACE_OUTPUT0_PIN, OUTPUT);
  digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
    pinMode(RHD2000_INTERFACE_OUTPUT1_PIN, OUTPUT);
  digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
    pinMode(RHD2000_INTERFACE_OUTPUT2_PIN, OUTPUT);
  digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
}


//TODO: check if the sensor state changed, which is all we really care about.
void loopRHD2000Interface(unsigned long currentLoopMillis) {

  int rhd2000InterfaceOutput0State_preUpdate = rhd2000InterfaceOutput0State;
  int rhd2000InterfaceOutput1State_preUpdate = rhd2000InterfaceOutput1State;
  int rhd2000InterfaceOutput2State_preUpdate = rhd2000InterfaceOutput2State;

  // Update the variables
  rhd2000InterfaceOutput0State = sensor1State;
  rhd2000InterfaceOutput1State = sensor2State;
  #if ENABLE_WATER_DISPENSE
    rhd2000InterfaceOutput2State = sensor3State;
  #endif

  // Check for changes
  //if (rhd2000InterfaceOutput0State != rhd2000InterfaceOutput0State_preUpdate) {
    digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
  //}
  //if (rhd2000InterfaceOutput1State != rhd2000InterfaceOutput1State_preUpdate) {
    digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
  //}
  if (rhd2000InterfaceOutput2State != rhd2000InterfaceOutput2State_preUpdate) {
    digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
  }
}



void outputSignals() {
  digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
  digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
  digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
}
