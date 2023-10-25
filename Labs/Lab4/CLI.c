#include "CLI.h"
#include "usart.h"
#include <string.h>
#include "LED.h"
#include <stdio.h>

static uint8_t clearScreenSequence[] = "\x1b[2J";
static uint8_t cursorPositionSequence[] = "\x1b[8;6H";
static uint8_t scrollSequence[] = "\x1b[8;r";

static unsigned current_line = 8;

void CLI_Transmit(uint8_t *pData) {
	unsigned size = strlen((char *)pData);
	for (unsigned i = 0; i < size; i++) {
		sendbyte(pData[i], 1000);
	}
}

// Moves the cursor to the provided line number
void move_cursor_to_line(unsigned line) {
	char dynamicCursorPositionSequence[11];
	snprintf(dynamicCursorPositionSequence, sizeof(dynamicCursorPositionSequence), "\x1b[%d;0H", line);
	CLI_Transmit((uint8_t *)dynamicCursorPositionSequence);
}

// Transmits data to the status window
void CLI_Transmit_Status_Window(uint8_t *pData) {
	// Clear the top 7 lines (the status window)
	for (unsigned i = 0; i <= 7; i++) {
		move_cursor_to_line(i);
		CLI_Transmit((uint8_t *)"\x1b[K");
	}

	// Move cursor back up to the top of the status window and print
	move_cursor_to_line(0);
	CLI_Transmit(pData);
	
	// Move the cursor back to the line it was on
	current_line += 1;
	move_cursor_to_line(current_line);
	CLI_Transmit((uint8_t *)"cmd> ");
}

// Prepares the status window and scrollable command window
void prepare_CLI(void){
	CLI_Transmit((uint8_t *)clearScreenSequence);
	CLI_Transmit((uint8_t *)scrollSequence);
	CLI_Transmit((uint8_t *)"Use command 'help' to see available commands.");
	
	// Outputting the first "cmd> " on row 8
	CLI_Transmit((uint8_t *)"\x1b[8;0H");
	CLI_Transmit((uint8_t *)"cmd> ");
	
	CLI_Transmit((uint8_t *)cursorPositionSequence);
}

// Handles the users input, and displays a response message in the status window
void process_command(uint8_t *user_input){
	uint8_t *message;
	if (strcmp((char *)user_input, "Toggle LED") == 0) {
		toggle_user_LED();
		message = (uint8_t *)"LED has been toggled";
	} else if (strcmp((char *)user_input, "Query LED") == 0) {
		message = query_user_LED();
	} else if (strcmp((char *)user_input, "Help") == 0 || strcmp((char *)user_input, "help") == 0) { 
		message = (uint8_t *)
			"Command Descriptions:"
			"\r\nToggle LED: Toggles the state of the user LED\r\n"
			"Query LED: Prints the current state of the user LED\r\n"
			"Help: Describes the function of each command";
	} else {
		message = (uint8_t *)"Invalid command (use command 'help' to see available commands)";
	}
	
	CLI_Transmit_Status_Window((uint8_t *)message);
}

// Displays the ASCII value of the character entered, useful for debugging inputs like backspace
void debug_return_characters(uint32_t received_byte){
	// Convert the ASCII code to a string
	char asciiCodeString[100]; // Large enough to hold any ASCII code (up to 3 digits)
	sprintf(asciiCodeString, "%d", received_byte);

	// Transmit the ASCII code string
	CLI_Transmit((uint8_t *)asciiCodeString);
}
