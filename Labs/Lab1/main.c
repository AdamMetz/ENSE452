#include "stm32f10x.h"

int main(void) {
	
    // Enable Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA5 as a push-pull Output
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= GPIO_CRL_MODE5_0;

    // Turn on PA5
    GPIOA->BSRR |= GPIO_BSRR_BS5;

    while (1) {

    }
}