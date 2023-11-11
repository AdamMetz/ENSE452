#include <stdint.h>

extern volatile uint32_t ms_counter;
void enable_TIM2(void);
void TIM2_IRQHandler(void);
