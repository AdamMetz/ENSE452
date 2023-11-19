#include <stdint.h>

#define NUM_LIGHTS 4

// Enum for light states
typedef enum
{
	GREEN,
	YELLOW,
	RED,
	TURN,
} LightState;

void updateLights(uint8_t lights_data[], LightState new_state);
