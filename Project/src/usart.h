#include <stdint.h>

/** Configure and enable the device. */
void serial_open(void);

/**
Send an 8-bit byte to the serial port, using the configured
bit-rate, # of bits, etc. Returns 0 on success and non-zero on
failure.
@param b the 8-bit quantity to be sent.
@pre must have already called serial_open()
*/
int sendbyte(uint8_t b);

void USART2_IRQHandler(void);
