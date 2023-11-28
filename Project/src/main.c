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
QueueHandle_t xConsolePedestrianQueue;
QueueHandle_t xControllerPedestrianQueue;

void USART2_IRQHandler(void);

extern void vTrafficLightControllerTask(void);
extern void vCLITask(void);
extern void vConsoleTask(void);

int main(void)
{
	serial_open();
	prepare_CLI();
	setup_user_button();

	xTrafficLightQueue = xQueueCreate(1, sizeof(uint8_t[4]));
	xCLIQueue = xQueueCreate(1, sizeof(uint8_t[100]));
	xStateQueue = xQueueCreate(1, sizeof(uint8_t));
	xConsolePedestrianQueue = xQueueCreate(1, sizeof(uint8_t[2]));
	xControllerPedestrianQueue = xQueueCreate(1, sizeof(uint8_t));

	xTaskCreate(vTrafficLightControllerTask, "Main", configMINIMAL_STACK_SIZE, NULL, TLC_TASK_PRIORITY, NULL);
	xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE, NULL, CLI_TASK_PRIORITY, NULL);
	xTaskCreate(vConsoleTask, "Console", configMINIMAL_STACK_SIZE, NULL, CONSOLE_TASK_PRIORITY, NULL);
	vTaskStartScheduler();
}
