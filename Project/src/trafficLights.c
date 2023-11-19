#include "trafficLights.h"

void updateLights(uint8_t lights_data[], LightState new_state)
{
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		switch (new_state)
		{
		case GREEN:
			lights_data[i] = (lights_data[i] == 'T') ? 'G' : lights_data[i];
			break;
		case YELLOW:
			lights_data[i] = (lights_data[i] == 'G') ? 'Y' : lights_data[i];
			break;
		case TURN:
			lights_data[i] = (lights_data[i] == 'Y') ? 'R' : (lights_data[i] == 'R') ? 'T'
																					 : lights_data[i];
			break;
		case RED:
			break;
		}
	}
}
