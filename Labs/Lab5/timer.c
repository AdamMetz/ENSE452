#include "timer.h"
#include "stm32f10x.h"

volatile uint32_t ms_counter = 0;

void enable_TIM2(void) {
	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
	// Prescaler for 1 ms resolution (72 MHz / 7200), ARR (0-9 -> 1 ms)
	TIM2->PSC = 7199; 
	TIM2->ARR = 9; 
	TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
	NVIC_EnableIRQ(TIM2_IRQn); // Enable TIM2 IRQ
	TIM2->CR1 |= TIM_CR1_CEN; // Start TIM2
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		ms_counter += 1;
		// Clear update interrupt bit
		TIM2->SR &= ~TIM_SR_UIF;
  }
}
