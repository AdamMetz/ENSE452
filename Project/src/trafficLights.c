#include "trafficLights.h"

extern QueueHandle_t xStateQueue;
extern QueueHandle_t xTrafficLightQueue;
extern QueueHandle_t xControllerPedestrianQueue;
extern QueueHandle_t xConsolePedestrianQueue;

// Values used to hold the state of pedestrians waiting/crossing
static int pedestrian_crossing_top_and_bottom = 0;
static int pedestrian_crossing_sides = 0;
static int pedestrian_waiting_top_and_bottom = 0;
static int pedestrian_waiting_sides = 0;

// Constants used for pedestrian states
#define PEDESTRIANS_TOP_BOTTOM 0
#define PEDESTRIANS_SIDES 1
#define PEDESTRIANS_BOTH 2

#define PEDESTRIAN_WAITING 0
#define PEDESTRIAN_CROSSING 1
#define PEDESTRIAN_DONE_CROSSING 2

// Updates the lights from one state to the next
void updateLights(uint8_t lights_data[], LightState new_state)
{
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		switch (new_state)
		{
		case GREEN:
			lights_data[i] = (lights_data[i] == 'T') ? 'G' : lights_data[i];
			break;
		case YELLOW:
			lights_data[i] = (lights_data[i] == 'G') ? 'Y' : lights_data[i];
			break;
		case TURN:
			lights_data[i] = (lights_data[i] == 'Y') ? 'R' : (lights_data[i] == 'R') ? 'T'
																					 : lights_data[i];
			break;
		case RED:
			break;
		}
	}
}

// Returns 1 if the intersection state changed
int checkForStateChange(uint8_t *current_state)
{
	uint8_t received_data;
	if (xQueueReceive(xStateQueue, &received_data, 1) == pdTRUE && *current_state != received_data)
	{
		*current_state = received_data;
		return 1;
	}
	return 0;
}

// Checks if any pedestrians are currently waiting to cross
// NOTE: The user pressing the USER button on the board will cause an ISR to indicate that a pedestrian would like to cross
void checkForPedestrianWaiting(void)
{
	uint8_t pedestrian_waiting;
	uint8_t pedestrian_state[2] = {0, 0};
	if (xQueueReceive(xControllerPedestrianQueue, &pedestrian_waiting, 1) == pdTRUE)
	{
		if (pedestrian_waiting_top_and_bottom == 0 && pedestrian_crossing_top_and_bottom == 0)
		{
			pedestrian_waiting_top_and_bottom = 1;
			pedestrian_state[0] = PEDESTRIANS_TOP_BOTTOM;
			pedestrian_state[1] = PEDESTRIAN_WAITING;
			xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		}
		else if (pedestrian_waiting_sides == 0 && pedestrian_crossing_sides == 0)
		{
			pedestrian_waiting_sides = 1;
			pedestrian_state[0] = PEDESTRIANS_SIDES;
			pedestrian_state[1] = PEDESTRIAN_WAITING;
			xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		}
	}
}

// Sends lights to the console display task (xConsoleTask) through the xTrafficLightQueue
// Returns 1 if the intersection state changed while the lights were displaying
// Params:
//    lights_data[]: Provides the value for each of the lights
//    intervals: The number of intervals for the light to remain in its state
//    last_wake_time: The last wake time in ticks, used for the task delay
//    current_state: The current state of the intersection
int sendLightsAndWait(uint8_t lights_data[], uint8_t intervals, TickType_t *last_wake_time, uint8_t *current_state)
{
	const TickType_t INTERVAL_DURATION = 500 / portTICK_PERIOD_MS;
	xQueueSend(xTrafficLightQueue, lights_data, portMAX_DELAY);
	for (unsigned i = 0; i < intervals; i++)
	{
		vTaskDelayUntil(last_wake_time, INTERVAL_DURATION);
		if (*current_state == 'N')
			checkForPedestrianWaiting(); // Only check for pedestrians in the normal state
		// Check for change in state
		if (checkForStateChange(current_state))
		{
			clearPedestrianCrossing();
			return 1;
		}
	}
	return 0;
}

// Handles the updates in states and display for a pedestrian crossing
void handlePedestrianCrossing(uint8_t lights_data[])
{
	uint8_t pedestrian_state[2] = {0, 0};

	// Pedestrian waiting at the top/bottom lights -> crossing
	if (pedestrian_waiting_top_and_bottom && lights_data[1] == 'T' && lights_data[2] == 'T')
	{
		pedestrian_state[0] = PEDESTRIANS_TOP_BOTTOM;
		pedestrian_state[1] = PEDESTRIAN_CROSSING;
		xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		pedestrian_waiting_top_and_bottom = 0;
		pedestrian_crossing_top_and_bottom = 1;
	}
	// Pedestrian crossing at the top/bottom lights -> cleared crossing
	else if (pedestrian_crossing_top_and_bottom)
	{
		pedestrian_state[0] = PEDESTRIANS_TOP_BOTTOM;
		pedestrian_state[1] = PEDESTRIAN_DONE_CROSSING;
		xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		pedestrian_crossing_top_and_bottom = 0;
	}

	// Pedestrian waiting at the side lights -> crossing
	if (pedestrian_waiting_sides && lights_data[0] == 'T' && lights_data[3] == 'T')
	{
		pedestrian_state[0] = PEDESTRIANS_SIDES;
		pedestrian_state[1] = PEDESTRIAN_CROSSING;
		xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		pedestrian_waiting_sides = 0;
		pedestrian_crossing_sides = 1;
	}
	// Pedestrian crossing at the side lights -> cleared crossing
	else if (pedestrian_crossing_sides)
	{
		pedestrian_state[0] = PEDESTRIANS_SIDES;
		pedestrian_state[1] = PEDESTRIAN_DONE_CROSSING;
		xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
		pedestrian_crossing_sides = 0;
	}
}

// Clears any pedestrians waiting/crossing out of the intersection
// This is used when the intersection state is moved into maintenance or emergency
void clearPedestrianCrossing(void)
{
	uint8_t pedestrian_state[2] = {PEDESTRIANS_BOTH, PEDESTRIAN_DONE_CROSSING};
	pedestrian_waiting_top_and_bottom = 0;
	pedestrian_waiting_sides = 0;
	pedestrian_crossing_top_and_bottom = 0;
	pedestrian_crossing_sides = 0;
	xQueueSend(xConsolePedestrianQueue, &pedestrian_state, NULL);
}

// The TrafficLightControllerTask handles:
//   - The state of each of the lights (Green, Yellow, Red, Turn)
//   - The state of the intersection (Normal, Maintenance, Emergency)
//   - The state of pedestrians waiting/crossing in the intersection (Waiting/Crossing top/bottom, or the sides)
//   - Sending intersection state, light states, and pedestrian state, to the ConsoleTask for display
void vTrafficLightControllerTask(void)
{
	uint8_t current_state = 'N';
	uint8_t lights_data[4] = {'T', 'R', 'R', 'T'};
	uint8_t red_lights[4] = {'R', 'R', 'R', 'R'};
	uint8_t empty_lights[4] = {' ', ' ', ' ', ' '};
	TickType_t last_wake_time = xTaskGetTickCount();

	const uint8_t GREEN_LIGHT_DURATION_INTERVALS = 10;
	const uint8_t YELLOW_LIGHT_DURATION_INTERVALS = 4;
	const uint8_t TURN_LIGHT_DURATION_INTERVALS = 8;
	const uint8_t GENERAL_WAIT_DURATION_INTERVALS = 3;

	for (;;)
	{
		uint8_t received_data;
		if (xQueueReceive(xStateQueue, &received_data, 25) == pdTRUE)
		{
			current_state = received_data;
		}
		switch (current_state)
		{
		// Normal State
		case 'N':
			// Green Light Phase
			updateLights(lights_data, GREEN);
			if (sendLightsAndWait(lights_data, GREEN_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state))
				break;

			// Pedestrian Crossing
			handlePedestrianCrossing(lights_data);

			// Yellow Light Phase
			updateLights(lights_data, YELLOW);
			if (sendLightsAndWait(lights_data, YELLOW_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state))
				break;

			// Delay between light change
			if (sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS, &last_wake_time, &current_state))
				break;

			// Turn Light Phase
			updateLights(lights_data, TURN);
			if (sendLightsAndWait(lights_data, TURN_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state))
				break;

			// Pedestrian Crossing
			handlePedestrianCrossing(lights_data);
			break;

		// Emergency Vehicle State
		case 'E':
			if (sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS, &last_wake_time, &current_state))
				break;
			break;

		// Maintenance State
		case 'M':
			if (sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS / 3, &last_wake_time, &current_state))
				break;
			if (sendLightsAndWait(empty_lights, GENERAL_WAIT_DURATION_INTERVALS / 3, &last_wake_time, &current_state))
				break;
			break;
		default:
			break;
		}
	}
}
