// RHD2000_Interface

//#define RHD2000_INTERFACE_OUTPUT0_PIN 0
//int rhd2000InterfaceOutput0State = LOW;

//#define RHD2000_INTERFACE_OUTPUT1_PIN 1
//int rhd2000InterfaceOutput1State = LOW;

#define RHD2000_INTERFACE_OUTPUT2_PIN 2
int rhd2000InterfaceOutput2State = LOW;

#define RHD2000_INTERFACE_HIGH_Duration 40

// Timestamp buffer
char timestampCharBuffer [11];
char valueBuffer [50];

// Function Prototypes:
void setupRHD2000Interface();
void loopRHD2000Interface(unsigned long currentLoopMillis);
void sendRHD2000BinarySignal(SystemAddress addr, EventType event);
void sendRHD2000ASCIISignal(SystemAddress addr, EventType event);
void sendRHD2000Signal(SystemAddress addr, EventType event);
void outputSignals();

// Called from setup()
void setupRHD2000Interface() {
  // initialize the output pins:
//  digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, LOW);
//  pinMode(RHD2000_INTERFACE_OUTPUT0_PIN, OUTPUT);
//  digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, LOW);
//  digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
  //pinMode(RHD2000_INTERFACE_OUTPUT1_PIN, OUTPUT);
  //digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
  pinMode(RHD2000_INTERFACE_OUTPUT2_PIN, OUTPUT);
  digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
}


//TODO: check if the sensor state changed, which is all we really care about.
void loopRHD2000Interface(unsigned long currentLoopMillis) {

  //int rhd2000InterfaceOutput0State_preUpdate = rhd2000InterfaceOutput0State;
  //int rhd2000InterfaceOutput1State_preUpdate = rhd2000InterfaceOutput1State;
  int rhd2000InterfaceOutput2State_preUpdate = rhd2000InterfaceOutput2State;

  // Update the variables
  //rhd2000InterfaceOutput0State = sensor1State;
  //rhd2000InterfaceOutput1State = sensor2State;
  #if ENABLE_WATER_DISPENSE
    //rhd2000InterfaceOutput2State = sensor3State;
  #endif

  //rhd2000InterfaceOutput0State = !rhd2000InterfaceOutput0State;
  rhd2000InterfaceOutput2State = !rhd2000InterfaceOutput2State;
  //delay(50);
  
  // Check for changes
//  if (rhd2000InterfaceOutput0State != rhd2000InterfaceOutput0State_preUpdate) {
//    digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
//  }
  //if (rhd2000InterfaceOutput1State != rhd2000InterfaceOutput1State_preUpdate) {
    //digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
  //}
  if (rhd2000InterfaceOutput2State != rhd2000InterfaceOutput2State_preUpdate) {
    digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
  }
}


void sendRHD2000Signal(SystemAddress addr, EventType event) {
  //sendRHD2000BinarySignal(addr, event);
  sendRHD2000ASCIISignal(addr, event); // Ah, is this done for the Diagnostic software?
}

void sendRHD2000ASCIISignal(SystemAddress addr, EventType event) {
  // Get the current time in milliseconds
  unsigned long currentLoopMillis = millis();
  // Max value of millis() is 4,294,967,295 (10 chars long)
  int successFlag = snprintf(timestampCharBuffer, 11, "%010ld", currentLoopMillis); // Always print as fixed width 10 character string
  if (successFlag>=0 && successFlag<100) {
    Serial.print("FFF");
    Serial.print(timestampCharBuffer); // send four timestamp bytes
    Serial.print(addr);
    Serial.print(event);
    Serial.println("EEE");
  }
}

void sendRHD2000BinarySignal(SystemAddress addr, EventType event) {
  // Get the current time in milliseconds
  unsigned long currentLoopMillis = millis();
  // unsigned long: 32 bits (4 bytes)
  timestampBuffer.longNumber = currentLoopMillis;
  
  //byte binary1 = addr;
  //byte binary1 = event;
  byte output = byte((addr<<3) + event);

  Serial.write(255);
  Serial.write(timestampBuffer.longBytes, 4); // send four timestamp bytes
  Serial.write(output); // send byte
  Serial.write(0);  
}

void outputSignals() {
  //digitalWrite(RHD2000_INTERFACE_OUTPUT0_PIN, rhd2000InterfaceOutput0State);
  //digitalWrite(RHD2000_INTERFACE_OUTPUT1_PIN, rhd2000InterfaceOutput1State);
  digitalWrite(RHD2000_INTERFACE_OUTPUT2_PIN, rhd2000InterfaceOutput2State);
}
