#include "usart.h"
#include "CLI.h"
#include "LED.h"
#include "timer.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

# define mainBLINKY_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
# define mainCLI_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

QueueHandle_t xQueue;
QueueHandle_t xCLIQueue;

void USART2_IRQHandler(void);
void TIM2_IRQHandler(void);

static void vBlinkTask(void * parameters);
static void vCLITask(void * parameters);

unsigned frequency = 5;

int main(void) {
		xQueue = xQueueCreate(1, sizeof(char));
	  xCLIQueue = xQueueCreate(1, sizeof(char));
	
    serial_open();
		enable_TIM2();
		enable_user_LED();
	
		prepare_CLI();
	
		xTaskCreate(vBlinkTask, "Blinky", configMINIMAL_STACK_SIZE, NULL, mainBLINKY_TASK_PRIORITY, NULL);
	  xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE, NULL, mainCLI_TASK_PRIORITY, NULL);
		vTaskStartScheduler();
}

static void vBlinkTask(void * parameters) {
	char command = 0;
	for(;;){
		if (xQueueReceive(xQueue, &command, 0) == pdTRUE) {
			if (command == '1') frequency += 1;
			if (command == '2' && frequency >= 2) frequency -= 1;
			xQueueSend(xCLIQueue, &command, 0);
		}
		GPIOA->ODR ^= GPIO_ODR_ODR5;
		vTaskDelay(1000 / frequency);
	}
}


static void vCLITask(void * parameters) {
	char command;
	for(;;){
		if (xQueueReceive(xCLIQueue, &command, 0) == pdTRUE) {
			CLI_Update_Frequency_Status(frequency);
		}
	}
}
