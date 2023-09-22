#include "usart.h"
#include "CLI.h"

#define RECIEVE_BUFFER_SIZE 25

int main(void) {
    serial_open();
    
    while (1) {
        uint8_t transmitBuffer[] = "Hello World";
        uint8_t recieveBuffer[RECIEVE_BUFFER_SIZE];
        
        CLI_Transmit(transmitBuffer, sizeof(transmitBuffer));
        
        CLI_Receive(recieveBuffer, sizeof(recieveBuffer));
    }

    // serial_close();
}
