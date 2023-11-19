#include "usart.h"
#include "CLI.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stdbool.h"

#define NUM_LIGHTS 4
#define TLC_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define CLI_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CONSOLE_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

// Enum for light states
typedef enum
{
	GREEN,
	YELLOW,
	RED,
	TURN,
} LightState;

QueueHandle_t xTrafficLightQueue;

void USART2_IRQHandler(void);

static void vTrafficLightControllerTask();
static void vCLITask();
static void vConsoleTask();

int main(void)
{

	serial_open();
	prepare_CLI();

	xTrafficLightQueue = xQueueCreate(1, sizeof(uint8_t[4]));

	xTaskCreate(vTrafficLightControllerTask, "Main", configMINIMAL_STACK_SIZE, NULL, TLC_TASK_PRIORITY, NULL);
	// xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE, NULL, CLI_TASK_PRIORITY, NULL);
	xTaskCreate(vConsoleTask, "CLI", configMINIMAL_STACK_SIZE, NULL, CONSOLE_TASK_PRIORITY, NULL);
	vTaskStartScheduler();
}

static void updateLights(uint8_t lights_data[NUM_LIGHTS], LightState new_state)
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

static void vTrafficLightControllerTask()
{
	uint8_t lights_data[4] = {'G', 'R', 'R', 'G'};
	uint8_t red_lights[4] = {'R', 'R', 'R', 'R'};
	TickType_t last_wake_time = xTaskGetTickCount();

	const TickType_t GREEN_LIGHT_DURATION = 5000 / portTICK_PERIOD_MS;
	const TickType_t YELLOW_LIGHT_DURATION = 2000 / portTICK_PERIOD_MS;
	const TickType_t TURN_LIGHT_DURATION = 4000 / portTICK_PERIOD_MS;
	const TickType_t GENERAL_WAIT_DURATION = 1500 / portTICK_PERIOD_MS;

	for (;;)
	{
		// Green Light Phase
		xQueueSend(xTrafficLightQueue, &lights_data, portMAX_DELAY);
		vTaskDelayUntil(&last_wake_time, GREEN_LIGHT_DURATION);

		// Yellow Light Phase
		updateLights(lights_data, YELLOW);
		xQueueSend(xTrafficLightQueue, &lights_data, portMAX_DELAY);
		vTaskDelayUntil(&last_wake_time, YELLOW_LIGHT_DURATION);

		// Delay between light change
		xQueueSend(xTrafficLightQueue, &red_lights, portMAX_DELAY);
		vTaskDelayUntil(&last_wake_time, GENERAL_WAIT_DURATION);

		// Turn Light Phase
		updateLights(lights_data, TURN);
		xQueueSend(xTrafficLightQueue, &lights_data, portMAX_DELAY);
		vTaskDelayUntil(&last_wake_time, TURN_LIGHT_DURATION);

		updateLights(lights_data, GREEN);
	}
}

static void vConsoleTask()
{
	uint8_t receivedData[4];
	for (;;)
	{
		if (xQueueReceive(xTrafficLightQueue, &receivedData, 25) == pdTRUE)
		{
			Update_Intersection_Lights(receivedData);
		}
	}
}

static void vCLITask()
{
	for (;;)
	{
	}
}
