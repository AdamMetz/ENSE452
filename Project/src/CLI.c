#include "CLI.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xStateQueue;
extern QueueHandle_t xTrafficLightQueue;
extern QueueHandle_t xConsolePedestrianQueue;
extern QueueHandle_t xCLIQueue;

#define INTERSECTION_DISPLAY_LENGTH 12

#define INTERSECTION_STATE_DISPLAY_LINE 1

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

// Default intersection display value
static uint8_t intersectionDisplay[INTERSECTION_DISPLAY_LENGTH][100] = {
	"   State: Emergency Vehicle        ",
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

// Updates the display of the state in the intersection (Normal, Maintenance, Emergency Vehicle)
void update_intersection_state(uint8_t *state)
{
	char stateDisplayLine[50];
	clear_line(INTERSECTION_STATE_DISPLAY_LINE);
	snprintf(stateDisplayLine, sizeof(stateDisplayLine), "   State: %.20s        ", state);
	CLI_Transmit((uint8_t *)stateDisplayLine);
	move_cursor_to_line(current_line, current_column);
}

// Updates the display of pedestrians waiting/crossing/done crossing the intersection
// Params:
//   pedestrian_data[2]:
//     pedestrian_data[0]: Contains the location of the pedestrians
//       0 = Top/Bottom crossings
//       1 = Side crossings
//       2 = Both crossings
//     pedestrian_data[1]: Contains the new state of the crossing
//       0 = Waiting to cross
//       1 = Crossing
//       2 = Done crossing
void update_intersection_pedestrian(uint8_t pedestrian_data[2])
{
	char pedestrianDisplayLine[50];
	char newPedestrianState = ' ';

	// Get the character associated with the new pedestrian crossing state
	if (pedestrian_data[1] == 0)
		newPedestrianState = 'W';
	else if (pedestrian_data[1] == 1)
		newPedestrianState = 'P';
	else if (pedestrian_data[1] == 2)
		newPedestrianState = ' ';

	// Update the top/bottom crossing display
	if (pedestrian_data[0] == 0 || pedestrian_data[0] == 2)
	{
		clear_line(5);
		snprintf(pedestrianDisplayLine, sizeof(pedestrianDisplayLine), "     _____|%c    |    %c|_____       ", newPedestrianState, newPedestrianState);
		CLI_Transmit((uint8_t *)pedestrianDisplayLine);

		clear_line(10);
		snprintf(pedestrianDisplayLine, sizeof(pedestrianDisplayLine), "          |%c    |    %c|            ", newPedestrianState, newPedestrianState);
		CLI_Transmit((uint8_t *)pedestrianDisplayLine);
	}

	// Update the sides crossing display
	if (pedestrian_data[0] == 1 || pedestrian_data[0] == 2)
	{
		clear_line(6);
		snprintf(pedestrianDisplayLine, sizeof(pedestrianDisplayLine), "          %c     |     %c            ", newPedestrianState, newPedestrianState);
		CLI_Transmit((uint8_t *)pedestrianDisplayLine);

		clear_line(9);
		snprintf(pedestrianDisplayLine, sizeof(pedestrianDisplayLine), "     _____%c     |     %c_____       ", newPedestrianState, newPedestrianState);
		CLI_Transmit((uint8_t *)pedestrianDisplayLine);
	}

	move_cursor_to_line(current_line, current_column);
}

// Updates the display of each of the lights in the intersection
void update_intersection_lights(uint8_t lights[4])
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

	update_intersection_state((uint8_t *)"Normal");
	update_intersection_lights((uint8_t[]){'T', 'T', 'T', 'T'});

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
		message = (uint8_t *)"State has been changed to maintenance, all lights are now blinking red";
		update_intersection_state((uint8_t *)"Maintenance");
		xQueueSend(xStateQueue, (uint8_t *)"M", portMAX_DELAY);
	}
	else if (strcmp((char *)user_input, "emergency") == 0 || strcmp((char *)user_input, "e") == 0)
	{
		message = (uint8_t *)"State has been changed to emergency vehicle, all lights are now red";
		update_intersection_state((uint8_t *)"Emergency Vehicle");
		xQueueSend(xStateQueue, (uint8_t *)"E", portMAX_DELAY);
	}
	else if (strcmp((char *)user_input, "normal") == 0 || strcmp((char *)user_input, "n") == 0)
	{
		message = (uint8_t *)"State has been changed to normal";
		update_intersection_state((uint8_t *)"Normal");
		xQueueSend(xStateQueue, (uint8_t *)"N", portMAX_DELAY);
	}
	else
	{
		message = (uint8_t *)"Invalid command (use command 'Help' to see available commands)";
	}

	CLI_Transmit_Status_Window((uint8_t *)message);
}

void vConsoleTask(void)
{
	uint8_t received_data[4];
	uint8_t pedestrian_data[2];
	for (;;)
	{
		if (xQueueReceive(xTrafficLightQueue, &received_data, 5) == pdTRUE)
		{
			update_intersection_lights(received_data);
		}
		if (xQueueReceive(xConsolePedestrianQueue, &pedestrian_data, 5) == pdTRUE)
		{
			update_intersection_pedestrian(pedestrian_data);
		}
	}
}

void vCLITask(void)
{
	uint8_t received_data[100];
	for (;;)
	{
		if (xQueueReceive(xCLIQueue, &received_data, 25) == pdTRUE)
		{
			process_command(received_data);
		}
	}
}
