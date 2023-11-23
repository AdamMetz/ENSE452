#include "CLI.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define INTERSECTION_DISPLAY_LENGTH 12

#define INTERSECTION_STATE_DISPLAY_LINE 1
#define INTERSECTION_TIMER_DISPLAY_LINE 2

#define INTERSECTION_NORTH_LIGHT_DISPLAY_LINE 3
#define INTERSECTION_EAST_AND_WEST_LIGHT_DISPLAY_LINE 7
#define INTERSECTION_SOUTH_LIGHT_DISPLAY_LINE 12

#define SCROLLABLE_REGION_STARTING_LINE 17
#define SCROLLABLE_REGION_STARTING_COLUMN 5

static uint8_t clearScreenSequence[] = "\x1b[2J";
static uint8_t cursorPositionSequence[] = "\x1b[17;6H";
static uint8_t scrollSequence[] = "\x1b[17;r";
static uint8_t clearLineSequence[] = "\x1b[K";

static unsigned current_line = SCROLLABLE_REGION_STARTING_LINE;
unsigned current_column = SCROLLABLE_REGION_STARTING_COLUMN;

static uint8_t intersectionDisplay[INTERSECTION_DISPLAY_LENGTH][100] = {
	"   State: Emergency Vehicle        ",
	//"   Timer: 5s                       ",
	"                                   ",
	"                G                  ",
	"          |     |     |            ",
	"     _____|     |     |_____       ",
	"                |                  ",
	"   R _ _ _ _ _ _|_ _ _ _ _ _ R     ",
	"                |                  ",
	"     _____      |      _____       ",
	"          |     |     |            ",
	"          |     |     |            ",
	"                G                  "};

void CLI_Transmit(uint8_t *pData)
{
	unsigned size = strlen((char *)pData);
	for (unsigned i = 0; i < size; i++)
	{
		sendbyte(pData[i]);
	}
}

// Moves the cursor to the provided line number
void move_cursor_to_line(unsigned line, unsigned column)
{
	char dynamicCursorPositionSequence[11];
	snprintf(dynamicCursorPositionSequence, sizeof(dynamicCursorPositionSequence), "\x1b[%d;%dH", line, column);
	CLI_Transmit((uint8_t *)dynamicCursorPositionSequence);
}

void clear_line(unsigned line)
{
	move_cursor_to_line(line, 0);
	CLI_Transmit((uint8_t *)clearLineSequence);
}

void Update_Intersection_State(uint8_t *state)
{
	char stateDisplayLine[50];
	clear_line(INTERSECTION_STATE_DISPLAY_LINE);
	snprintf(stateDisplayLine, sizeof(stateDisplayLine), "   State: %.20s        ", state);
	CLI_Transmit((uint8_t *)stateDisplayLine);
	move_cursor_to_line(current_line, current_column);
}
/*
void Update_Intersection_Timer(uint8_t time){
	char timerDisplayLine[50];
	clear_line(INTERSECTION_TIMER_DISPLAY_LINE);
	snprintf(timerDisplayLine, sizeof(timerDisplayLine), "   Timer: %ds          ", time);
	CLI_Transmit((uint8_t *)timerDisplayLine);
	move_cursor_to_line(current_line, 5);
}
*/
void Update_Intersection_Lights(uint8_t lights[4])
{
	char lightDisplayLine[50];

	clear_line(INTERSECTION_NORTH_LIGHT_DISPLAY_LINE);
	snprintf(lightDisplayLine, sizeof(lightDisplayLine), "                %c                  ", lights[0]);
	CLI_Transmit((uint8_t *)lightDisplayLine);

	clear_line(INTERSECTION_EAST_AND_WEST_LIGHT_DISPLAY_LINE);
	snprintf(lightDisplayLine, sizeof(lightDisplayLine), "   %c _ _ _ _ _ _|_ _ _ _ _ _ %c     ", lights[1], lights[2]);
	CLI_Transmit((uint8_t *)lightDisplayLine);

	clear_line(INTERSECTION_SOUTH_LIGHT_DISPLAY_LINE);
	snprintf(lightDisplayLine, sizeof(lightDisplayLine), "                %c                  ", lights[3]);
	CLI_Transmit((uint8_t *)lightDisplayLine);

	move_cursor_to_line(current_line, current_column);
}

// Transmits data to the status window
void CLI_Transmit_Status_Window(uint8_t *pData)
{
	// Clear the status window
	for (unsigned i = INTERSECTION_DISPLAY_LENGTH + 1; i <= SCROLLABLE_REGION_STARTING_LINE - 1; i++)
	{
		clear_line(i);
	}

	// Move cursor back up to the top of the status window and print
	move_cursor_to_line(INTERSECTION_DISPLAY_LENGTH + 1, 0);
	CLI_Transmit(pData);

	// Move the cursor back to the line it was on
	current_line += 1;
	move_cursor_to_line(current_line, 0);
	CLI_Transmit((uint8_t *)"cmd>");
}

// Prepares the status window and scrollable command window
void prepare_CLI(void)
{
	CLI_Transmit((uint8_t *)clearScreenSequence);
	CLI_Transmit((uint8_t *)scrollSequence);

	for (unsigned i = 0; i < INTERSECTION_DISPLAY_LENGTH; i++)
	{
		move_cursor_to_line(i + 1, 0);
		CLI_Transmit(intersectionDisplay[i]);
	}

	Update_Intersection_State((uint8_t *)"Normal");
	// Update_Intersection_Timer(3);
	Update_Intersection_Lights((uint8_t[]){'T', 'T', 'T', 'T'});

	// Outputting the first "cmd> "
	move_cursor_to_line(SCROLLABLE_REGION_STARTING_LINE, 0);
	CLI_Transmit((uint8_t *)"cmd> ");

	CLI_Transmit((uint8_t *)cursorPositionSequence);
}

// Handles the users input, and displays a response message in the status window
void process_command(uint8_t *user_input)
{
	current_column = SCROLLABLE_REGION_STARTING_COLUMN;
	uint8_t *message;

	// Convert user_input to all lowercase
	for (int i = 0; user_input[i] != '\0'; i++)
	{
		user_input[i] = tolower(user_input[i]);
	}

	if (strcmp((char *)user_input, "help") == 0 || strcmp((char *)user_input, "h") == 0)
	{
		message = (uint8_t *)"Command Descriptions:\r\n"
		"Normal (N): Changes the state to normal\r\n"
		"Maintenance (M): Changes the state to maintenance\r\n"
		"Emergency (E): Changes the state to emergency vehicle";
	}
	else if (strcmp((char *)user_input, "maintenance") == 0 || strcmp((char *)user_input, "m") == 0)
	{
		message = (uint8_t *)"State has been changed to maintenance";
	}
	else if (strcmp((char *)user_input, "emergency") == 0 || strcmp((char *)user_input, "e") == 0)
	{
		message = (uint8_t *)"State has been changed to emergency vehicle";
	}
	else if (strcmp((char *)user_input, "normal") == 0 || strcmp((char *)user_input, "n") == 0)
	{
		message = (uint8_t *)"State has been changed to normal";
	}
	else
	{
		message = (uint8_t *)"Invalid command (use command 'Help' to see available commands)";
	}

	CLI_Transmit_Status_Window((uint8_t *)message);
}
