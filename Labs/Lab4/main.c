#include "usart.h"
#include "CLI.h"
#include "LED.h"
#include "timer.h"
#include "stm32f10x.h"

void USART2_IRQHandler(void);
void TIM2_IRQHandler(void);

int main(void) {
    serial_open();
		enable_TIM2();
		enable_user_LED();
	
		prepare_CLI();
		
		while(1);
}
