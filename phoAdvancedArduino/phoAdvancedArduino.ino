#include "Config.h"

#include "Mega2560Labjack_Interface.h"

#include <Arduino_FreeRTOS.h>
#include <FreeRTOSConfig.h>
// Include queue support
#include <queue.h>

#include "Tasks.h"

// define tasks for Blink and AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );


void TaskReadBeambreaks(void *pvParameters);
//TODO: Instead have a beambreak_changed queue that only is posted to once a change is detected.
void TaskSignalLabjackWrite(void *pvParameters);

// TaskEndSignalLabjackWrite: ends a given signal on the queue by setting it back to its default (not signalling) value.
void TaskEndSignalLabjackWrite(void *pvParameters);




void setup() {
  // put your setup code here, to run once:
  //TODO: duplicate regular setup code here:
  setupMegaOutputInterface();
    /**
   * Create a queue.
   * https://www.freertos.org/a00116.html
   */
	structArrayQueue=xQueueCreate(10, //Queue length
                              sizeof(struct BeambreakPorts)); //Queue item size

	if(structArrayQueue!=NULL) {
		// Set up the tasks that update the queue:
		// Create other task that publish data in the queue if it was created.
  		xTaskCreate(TaskReadBeambreaks,// Task function
              "ReadBeambreaks",// Task name
              128,  // Stack size
              NULL,
              2,// Priority
              NULL);
	} // end if (structArrayQueue != NULL)

	// The queue for labjack signalling:
	labjackSignalEventArrayQueue=xQueueCreate(8, //Queue length
                              sizeof(struct LabjackSignalEvent)); //Queue item size
	
	if(labjackSignalEventArrayQueue!=NULL) {
		// Set up the tasks that update the queue:
		// Create other task that publish data in the queue if it was created.
  		xTaskCreate(TaskSignalLabjackWrite,// Task function
              "SignalLabjackWrite",// Task name
              128,  // Stack size
              NULL,
              3,// Maximum Priority (3)
              NULL);
	} // end if (labjackSignalEventArrayQueue != NULL)



	// Used for turning off signals to the labjack after the appropriate amount of time:
	labjackEndSignallingEventArrayQueue=xQueueCreate(10, //Queue length
                              sizeof(struct LabjackSignalEvent)); //Queue item size


	if(labjackEndSignallingEventArrayQueue!=NULL) {
		// Set up the tasks that update the queue:
		// Create other task that publish data in the queue if it was created.
  		xTaskCreate(TaskEndSignalLabjackWrite,// Task function
              "EndSignalLabjackWrite",// Task name
              128,  // Stack size
              NULL,
              1,// Very low priority.
              NULL);
	} // end if (labjackEndSignallingEventArrayQueue != NULL)



	// // Now set up two tasks to run independently.
	xTaskCreate(
		TaskBlink
		, "Blink" // A name just for humans
		, 128      // This stack size can be checked and adjusted by reading the Stack Highwater
		, NULL
		, 0        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL );

	// xTaskCreate(
	// 	TaskAnalogRead
	// 	, "AnalogRead"
	// 	, 128      // Stack size
	// 	, NULL
	// 	, 1        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	// 	, NULL );



}

void loop() {
  // put your main code here, to run repeatedly:

}





/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
 
void TaskBlink(void *pvParameters) // This is a task.
{
    (void) pvParameters;
 
    // initialize digital pin 25 as an output.
    pinMode(25, OUTPUT);
 
    for (;;) // A Task shall never return or exit.
    {
        digitalWrite(25, HIGH); // turn the LED on (HIGH is the voltage level)
        vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
        digitalWrite(25, LOW); // turn the LED off by making the voltage LOW
        vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    }
}


 
// void TaskAnalogRead(void *pvParameters) // This is a task.
// {
//     (void) pvParameters;
 
//     // initialize serial communication at 9600 bits per second:
//     Serial.begin(9600);
 
//     for (;;)
//     {
//         // read the input on analog pin 0:
//         int sensorValue = analogRead(A0);
//         // print out the value you read:
//         Serial.println(sensorValue);
//         vTaskDelay(1); // one tick delay (15ms) in between reads for stability
//     }
// }



/**
 * Analog read task for Pin A0 and A1
 * Reads an analog input on pin 0 and pin 1 
 * Send the readed value through the queue.
 * See Blink_AnalogRead example.
 */
void TaskReadBeambreaks(void *pvParameters){
	(void) pvParameters;
	pinMode(SENSOR1PIN, INPUT);
	pinMode(SENSOR2PIN, INPUT);
	pinMode(SENSOR3PIN, INPUT);
	pinMode(SENSOR4PIN, INPUT);

	for (;;){
		// Read the input on analog pin 0:
		struct BeambreakPorts currentVariable;
		currentVariable.pin[0] = SENSOR1PIN;
		currentVariable.pin[1] = SENSOR2PIN;
		currentVariable.pin[2] = SENSOR3PIN;
		currentVariable.pin[3] = SENSOR4PIN;

		currentVariable.ReadValue[0] = digitalRead(SENSOR1PIN);
		currentVariable.ReadValue[1] = digitalRead(SENSOR2PIN);  
		currentVariable.ReadValue[2] = digitalRead(SENSOR3PIN);
		currentVariable.ReadValue[3] = digitalRead(SENSOR4PIN);  
		/**
			* Post an item on a queue.
			* https://www.freertos.org/a00117.html
			*/
		xQueueSend(structArrayQueue, &currentVariable, portMAX_DELAY);

		// One tick delay (15ms) in between reads for stability
		vTaskDelay(1);
	} // end infinite for loop
}


// struct pinRead currentPinRead;



void TaskSignalLabjackWrite( void *pvParameters )
{
  // 
  (void) pvParameters;

	// Read any pending labjack signal events from the queue:

  for (;;) 
  {
	struct LabjackSignalEvent currentLabjackEvent;

    /**
     * Read an item from a queue.
     * https://www.freertos.org/a00118.html
     */
    if (xQueueReceive(labjackSignalEventArrayQueue, &currentLabjackEvent, portMAX_DELAY) == pdPASS) {
		// Should perform output here:
		const int megaOutputPin = getMegaOutputSignalPin(currentLabjackEvent.addr, currentLabjackEvent.event);
		digitalWrite(megaOutputPin, LOW); 		// Should set the port to signalling, but not delay, which should be handled by the lower priority handler
		// Then add the item to another queue (low priority) that stops the signalling after the correct time
		// Post the completed item to the completion queue:
		xQueueSend(labjackEndSignallingEventArrayQueue, &currentLabjackEvent, portMAX_DELAY);
    }
  } // end infinite for loop  

}




void TaskEndSignalLabjackWrite( void *pvParameters )
{
  // Monitors the labjackEndSignallingEventArrayQueue and turns off any ports that are done signalling
  (void) pvParameters;

  for (;;) 
  {
	struct LabjackSignalEvent currentCompletedLabjackEvent;
    /**
     * Read the item from the completing queue.
     */
    if (xQueueReceive(labjackEndSignallingEventArrayQueue, &currentCompletedLabjackEvent, portMAX_DELAY) == pdPASS) {
		// Delay is done in here:
		const int megaOutputPin = getMegaOutputSignalPin(currentCompletedLabjackEvent.addr, currentCompletedLabjackEvent.event); // Get the pin
		vTaskDelay(SignalOnDuration/portTICK_PERIOD_MS);
		// turns off signalling here:
		digitalWrite(megaOutputPin, HIGH);
    }

  } // end infinite for loop  

}
