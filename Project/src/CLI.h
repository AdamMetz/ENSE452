#include <stdint.h>
void CLI_Transmit(uint8_t *pData);
void CLI_Receive(uint8_t *pData, uint16_t Size);
void prompt_user(void);
void process_command(uint8_t *user_input);
void debug_return_characters(uint32_t received_byte);
void prepare_CLI(void);
void move_cursor_to_line(unsigned line, unsigned column);
void CLI_Transmit_Status_Window(uint8_t *pData);
void CLI_Update_Frequency_Status(unsigned frequency);