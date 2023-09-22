#include "usart.h"
#include "stm32f10x.h"

/** Configure and enable the device. */
void serial_open(void) {
		// Enable the USART2 and GPIOA clocks
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
		// Configure PA2 (alternate function, push-pull, max speed 50 MHz)
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2;
	
		// Configure PA3 (input with pull-up/pull-down)
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_CNF3_1;
		
		// Configure USART2 for 115200 bps, 8 data bits, no parity, 1 stop bit
		// 36 000 000 / (16 * 115200) = 19.53
		// (Mantissa << 4) | (Fraction * 16 & 0xF)
    USART2->BRR = (19 << 4) | (9 & 0xF); 
		// Activate transmit, recieve, & USART2
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void serial_close(void) {
    // Reset USART2
		USART2->BRR = 0;
    USART2->CR1 &= ~(USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
    
    // Reset PA2 & PA3
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2 | GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
}

int sendbyte(uint8_t b) {
    // Wait for the transmit data register to be empty
    while (!(USART2->SR & USART_SR_TXE));
    
    // Send the byte
    USART2->DR = b;
    
    // Wait for transmission to complete
    while (!(USART2->SR & USART_SR_TC));
    
    return 0;
}

uint8_t getbyte(void) {
    // Wait for a character to be received
    while (!(USART2->SR & USART_SR_RXNE));
    
    // Read and return the received character
    return (uint8_t)(USART2->DR);
}
