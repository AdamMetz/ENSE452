#include "usart.h"
#include "CLI.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"

#define RECEIVE_BUFFER_SIZE 50

#define ASCII_BACKSPACE 8
#define ASCII_DELETE 127

extern QueueHandle_t xCLIQueue;
extern unsigned current_column;

static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static uint8_t receive_buffer_index = 0;

/** Configure and enable the device. */
void serial_open(void)
{
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
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;

	// Enable USART2 receive interrupts
	USART2->CR1 |= USART_CR1_RXNEIE;

	// Enable the USART2 interrupt
	NVIC_EnableIRQ(USART2_IRQn);
}

/**
	Send an 8-bit byte to the serial port, using the configured bit-rate, # of bits, etc.
	Returns 0 on success and non-zero on failure.
	@param b the 8-bit quantity to be sent.
	@pre must have already called serial_open()
*/
int sendbyte(uint8_t b)
{
	// Wait for the transmit data register to be empty
	while (!(USART2->SR & USART_SR_TXE))
		;

	// Send the byte
	USART2->DR = b;

	// Wait for transmission to complete
	while (!(USART2->SR & USART_SR_TC))
		;

	return 0;
}

void USART2_IRQHandler(void)
{
	if (USART2->SR & USART_SR_RXNE)
	{
		uint8_t received_byte = USART2->DR;

		// Check if Enter key is pressed, or the max buffer size was reached
		if (received_byte == '\r' || received_byte == '\n' || receive_buffer_index == RECEIVE_BUFFER_SIZE)
		{
			sendbyte('\n');
			sendbyte('\r');
			xQueueSendFromISR(xCLIQueue, &receive_buffer, NULL);
			// Clear buffer
			memset(receive_buffer, 0, sizeof(receive_buffer));
			receive_buffer_index = 0;
		}
		// Check for backspace/delete, ASCII values 8 and 127
		else if (received_byte == ASCII_BACKSPACE || received_byte == ASCII_DELETE)
		{
			if (receive_buffer_index > 0)
			{
				current_column -= 1;
				// Replace the deleted value with a null terminator,
				receive_buffer[receive_buffer_index] = '\0';
				receive_buffer_index--;

				// Clear the character from the CLI
				sendbyte('\b');
				sendbyte(' ');
				sendbyte('\b');
			}
		}
		else
		{
			current_column += 1;
			receive_buffer[receive_buffer_index] = received_byte;
			sendbyte(receive_buffer[receive_buffer_index]);
			receive_buffer_index++;
		}
	}
}