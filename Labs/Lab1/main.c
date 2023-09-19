#include "stm32f10x.h"

// Delay length of 7 200 000 for ~1s delay, (72 MHz clock)
#define DELAY_LENGTH 7200000

void delay(int ticks) {
    while (ticks--);
}

int main(void) {
	
    // Enable Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA5 as a push-pull Output
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= GPIO_CRL_MODE5_0;

    while (1) {
			GPIOA->BSRR |= GPIO_BSRR_BS5;
			delay(DELAY_LENGTH);
			GPIOA->BSRR |= GPIO_BSRR_BR5;
			delay(DELAY_LENGTH);
    }
}