#include <stdint.h>
void CLI_Transmit(uint8_t *pData);
void CLI_Receive(uint8_t *pData, uint16_t Size);
void prompt_user(void);
void process_command(uint8_t *user_input);
void prepare_CLI(void);
void move_cursor_to_line(unsigned line, unsigned column);
void CLI_Transmit_Status_Window(uint8_t *pData);
void clear_line(unsigned line);
void update_intersection_state(uint8_t *state);
void update_intersection_timer(uint8_t time);
void update_intersection_lights(uint8_t lights[4]);
void update_intersection_pedestrian(uint8_t pedestrian_data[2]);
void vConsoleTask(void);
void vCLITask(void);
