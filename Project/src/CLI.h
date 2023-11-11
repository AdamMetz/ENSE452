#include <stdint.h>
void CLI_Transmit(uint8_t *pData);
void CLI_Receive(uint8_t *pData, uint16_t Size);
void prompt_user(void);
void process_command(uint8_t *user_input);
void prepare_CLI(void);
void move_cursor_to_line(unsigned line, unsigned column);
void CLI_Transmit_Status_Window(uint8_t *pData);
void clear_line(unsigned line);
void Update_Intersection_State(uint8_t *state);
void Update_Intersection_Timer(uint8_t time);
void Update_Intersection_Lights(uint8_t lights[4]);
