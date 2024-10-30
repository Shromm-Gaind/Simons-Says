#include <stdint.h>

volatile uint8_t button_active;
void uart_putc(uint8_t);
void uart_puts(char *string);
void send_score(uint16_t score);

