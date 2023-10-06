#include "usart.h"
#include "CLI.h"
#include "LED.h"
#include "stm32f10x.h"

void USART2_IRQHandler(void);

int main(void) {
    serial_open();
		enable_user_LED();
	
    prompt_user();
		
		while(1);
}
