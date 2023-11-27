#include "usart.h"
#include "CLI.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "trafficLights.h"
#include "userButton.h"

#define TLC_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define CLI_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define CONSOLE_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

QueueHandle_t xTrafficLightQueue;
QueueHandle_t xCLIQueue;
QueueHandle_t xStateQueue;

void USART2_IRQHandler(void);

extern void vTrafficLightControllerTask();
static void vCLITask();
static void vConsoleTask();

int main(void)
{
	serial_open();
	prepare_CLI();
	setup_user_button();
	
	xTrafficLightQueue = xQueueCreate(1, sizeof(uint8_t[4]));
	xCLIQueue = xQueueCreate(1, sizeof(uint8_t[100]));
	xStateQueue = xQueueCreate(1, sizeof(uint8_t));

	xTaskCreate(vTrafficLightControllerTask, "Main", configMINIMAL_STACK_SIZE, NULL, TLC_TASK_PRIORITY, NULL);
	xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE, NULL, CLI_TASK_PRIORITY, NULL);
	xTaskCreate(vConsoleTask, "Console", configMINIMAL_STACK_SIZE, NULL, CONSOLE_TASK_PRIORITY, NULL);
	vTaskStartScheduler();
}

static void vConsoleTask()
{
	uint8_t received_data[4];
	for (;;)
	{
		if (xQueueReceive(xTrafficLightQueue, &received_data, 25) == pdTRUE)
		{
			Update_Intersection_Lights(received_data);
		}
	}
}

static void vCLITask()
{
	uint8_t received_data[100];
	for (;;)
	{
		if (xQueueReceive(xCLIQueue, &received_data, 25) == pdTRUE)
		{
			process_command(received_data);
		}
	}
}
