#include "trafficLights.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t xStateQueue;
extern QueueHandle_t xTrafficLightQueue;

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

// Sends lights to the console display task (xConsoleTask) through the xTrafficLightQueue
// Returns 1 if the intersection state changed while the lights were displaying
int sendLightsAndWait(uint8_t lights_data[], uint8_t intervals, TickType_t *last_wake_time, uint8_t *current_state)
{
		const TickType_t INTERVAL_DURATION = 500 / portTICK_PERIOD_MS;
		for (unsigned i = 0; i < intervals; i++) {
			xQueueSend(xTrafficLightQueue, lights_data, portMAX_DELAY);
			vTaskDelayUntil(last_wake_time, INTERVAL_DURATION);
			if (checkForStateChange(current_state)) return 1;
		}
		return 0;
}

void vTrafficLightControllerTask()
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
			if(sendLightsAndWait(lights_data, GREEN_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state)) break;

			// Yellow Light Phase
			updateLights(lights_data, YELLOW);
			if(sendLightsAndWait(lights_data, YELLOW_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state)) break;

			// Delay between light change
			if(sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS, &last_wake_time, &current_state)) break;

			// Turn Light Phase
			updateLights(lights_data, TURN);
			if(sendLightsAndWait(lights_data, TURN_LIGHT_DURATION_INTERVALS, &last_wake_time, &current_state)) break;
			break;
		// Emergency Vehicle State
		case 'E':
			if(sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS, &last_wake_time, &current_state)) break;
			break;
		// Maintenance State
		case 'M':
			if(sendLightsAndWait(red_lights, GENERAL_WAIT_DURATION_INTERVALS / 3, &last_wake_time, &current_state)) break;
			if(sendLightsAndWait(empty_lights, GENERAL_WAIT_DURATION_INTERVALS / 3, &last_wake_time, &current_state)) break;
			break;
		default:
			break;
		}
	}
}
