#include "usart.h"

int main(void) {
    serial_open();
    
    while (1) {
        uint8_t data = getbyte();
        // Check for user hitting enter, to start a new line
        if (data == '\n' || data == '\r') {
            sendbyte('\n');
            sendbyte('\r');
        } else {
            sendbyte(data);
        }
    }

    // serial_close();
}
