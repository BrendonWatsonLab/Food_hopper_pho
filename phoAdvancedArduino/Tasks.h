// Tasks.h: contains resources/definitions/functions to define FreeRTOS tasks:

// ensures this file description is only included once
#ifndef Tasks_h
#define Tasks_h

// Define a struct
// struct pinRead 
// {
//   int pin;  // analog channel indentifier
//   int value; // sensor output value
// };
// QueueHandle_t structQueue;

// Define a Structure Array
struct BeambreakPorts{
  int pin[4];
  int ReadValue[4];
};

/* 
 * Declaring a global variable of type QueueHandle_t 
 * 
 */
QueueHandle_t structArrayQueue;




struct LabjackSignalEvent{
	SystemAddress addr;
	EventType event;
};




/* 
 * Declaring a global variable of type QueueHandle_t 
 * 
 */
QueueHandle_t labjackSignalEventArrayQueue;

QueueHandle_t labjackEndSignallingEventArrayQueue;




#endif // end of the "#ifndef" at the start of the file that ensures this file is only included once. THIS MUST BE AT THE END OF THE FILE.
