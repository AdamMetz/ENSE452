#include "CLI.h"
#include "usart.h"

void CLI_Transmit(uint8_t *pData, uint16_t Size) {
	for (int i = 0; i < Size; i++) {
		sendbyte(pData[i]);
	}
	sendbyte('\n');
	sendbyte('\r');
}
void CLI_Receive(uint8_t *pData, uint16_t Size) {
	for (int i = 0; i < Size; i++) {
		uint8_t receivedByte = getbyte();
		
		// Check if Enter key is pressed
		if (receivedByte == '\r' || receivedByte == '\n') {
				sendbyte('\n');
			  sendbyte('\r');
				break;
		} else {
				pData[i] = receivedByte;
				sendbyte(pData[i]);
		}
	}
}
