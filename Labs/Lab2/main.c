#include "usart.h"
#include "CLI.h"
#include "LED.h"

#define RECIEVE_BUFFER_SIZE 25

int main(void) {
    serial_open();
		enable_user_LED();
    
    while (1) {
        uint8_t recieve_buffer[RECIEVE_BUFFER_SIZE];
			  // Clear the buffer
				memset(recieve_buffer, 0, sizeof(recieve_buffer));
			
        prompt_user();
			
        CLI_Receive(recieve_buffer, sizeof(recieve_buffer));
				process_command(recieve_buffer);
    }

    // serial_close();
}
