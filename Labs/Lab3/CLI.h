#include <stdint.h>
void CLI_Transmit(uint8_t *pData, uint16_t Size);
void CLI_Receive(uint8_t *pData, uint16_t Size);
void prompt_user(void);
void process_command(uint8_t *user_input);
void debug_return_characters(uint32_t received_byte);
