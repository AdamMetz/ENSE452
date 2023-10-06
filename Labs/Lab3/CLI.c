#include "CLI.h"
#include "usart.h"
#include <string.h>
#include "LED.h"
#include <stdio.h>

void CLI_Transmit(uint8_t *pData, uint16_t Size) {
	sendbyte('\n');
	sendbyte('\r');
	
	for (int i = 0; i < Size; i++) {
		sendbyte(pData[i]);
	}
	sendbyte('\n');
	sendbyte('\r');
}

/*
void CLI_Receive(uint8_t *pData, uint16_t Size) {
	for (int i = 0; i < Size; i++) {
		uint8_t received_byte = getbyte();
		//debug_return_characters(received_byte);

		// Check if Enter key is pressed
		if (received_byte == '\r' || received_byte == '\n') {
				sendbyte('\n');
			  sendbyte('\r');
			  break;
		// Check for backspaces, ASCII values 8 and 127
		} else if (received_byte == 8 || received_byte == 127) {
			if (i > 0) {
				i--;
				// Replace the deleted value with a null terminator, 
				pData[i] = '\0';
				
				// Clear the character from the CLI
				sendbyte('\b');   
				sendbyte(' ');    
				sendbyte('\b');   
			}
			i--;
		} else {
				pData[i] = received_byte;
				sendbyte(pData[i]);
		}
	}
}
*/

void prompt_user(void){
	uint8_t prompt_message[] = 
		"-------------------------------------------------\r\n"
		"Please enter one of the following commands:" 
		"\r\nToggle LED"
		"\r\nQuery LED"
		"\r\nHelp\r\n";
	CLI_Transmit(prompt_message, sizeof(prompt_message));
}

void process_command(uint8_t *user_input){
	uint8_t *message;
	if (strcmp((char *)user_input, "Toggle LED") == 0) {
		toggle_user_LED();
		message = (uint8_t *)"LED has been toggled";
	} else if (strcmp((char *)user_input, "Query LED") == 0) {
		message = query_user_LED();
	} else if (strcmp((char *)user_input, "Help") == 0) { 
		message = (uint8_t *)
			"Command Descriptions:"
			"\r\nToggle LED: Toggles the state of the user LED\r\n"
			"Query LED: Prints the current state of the user LED\r\n"
			"Help: Describes the function of each command";
	} else {
		message = (uint8_t *)"Invalid command, please try again";
	}
	CLI_Transmit(user_input, (uint16_t)strlen((char *)user_input));
	CLI_Transmit(message, (uint16_t)strlen((char *)message));
}

// Displays the ASCII value of the character entered, useful for debugging inputs like backspace
void debug_return_characters(uint8_t received_byte){
	// Convert the ASCII code to a string
	char asciiCodeString[4]; // Large enough to hold any ASCII code (up to 3 digits)
	sprintf(asciiCodeString, "%d", received_byte);

	// Transmit the ASCII code string
	CLI_Transmit((uint8_t *)asciiCodeString, (uint16_t)strlen(asciiCodeString));
}
