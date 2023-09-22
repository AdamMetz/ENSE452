#include "stm32f10x.h"
#include "LED.h"

void enable_user_LED(void) {
	// Configure PA5 as a push-pull Output
	GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
	GPIOA->CRL |= GPIO_CRL_MODE5_0;
}
void toggle_user_LED(void) {
	GPIOA->ODR ^= GPIO_ODR_ODR5;
}

uint8_t* query_user_LED(void) {
	if (GPIOA->IDR & GPIO_IDR_IDR5) {
			return (uint8_t *)"LED is on";
	} else {
			return (uint8_t *)"LED is off";
	}
}
