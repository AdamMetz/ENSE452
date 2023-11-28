#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define NUM_LIGHTS 4

// Enum for light states
typedef enum
{
	GREEN,
	YELLOW,
	RED,
	TURN,
} LightState;

// Updates the lights from one state to the next
void updateLights(uint8_t lights_data[], LightState new_state);

// The TrafficLightControllerTask handles:
//   - The state of each of the lights (Green, Yellow, Red, Turn)
//   - The state of the intersection (Normal, Maintenance, Emergency)
//   - The state of pedestrians waiting/crossing in the intersection (Waiting/Crossing top/bottom, or the sides)
//   - Sending intersection state, light states, and pedestrian state, to the ConsoleTask for display
void vTrafficLightControllerTask(void);
int checkForStateChange(uint8_t *current_state);

// Clears any pedestrians waiting/crossing out of the intersection
// This is used when the intersection state is moved into maintenance or emergency
void clearPedestrianCrossing(void);

// Sends lights to the console display task (xConsoleTask) through the xTrafficLightQueue
// Returns 1 if the intersection state changed while the lights were displaying
// Params:
//    lights_data[]: Provides the value for each of the lights
//    intervals: The number of intervals for the light to remain in its state
//    last_wake_time: The last wake time in ticks, used for the task delay
//    current_state: The current state of the intersection
int sendLightsAndWait(uint8_t lights_data[], uint8_t intervals, TickType_t *last_wake_time, uint8_t *current_state);

// Handles the updates in states and display for a pedestrian crossing
void handlePedestrianCrossing(uint8_t lights_data[]);

// Checks if any pedestrians are currently waiting to cross
// NOTE: The user pressing the USER button on the board will cause an ISR to indicate that a pedestrian would like to cross
void checkForPedestrianWaiting(void);
