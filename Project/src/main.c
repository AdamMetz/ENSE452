#include "usart.h"
#include "CLI.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void USART2_IRQHandler(void);

int main(void) {
	
    serial_open();
		prepare_CLI();
	
		while(1);
}
