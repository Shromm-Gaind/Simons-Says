#include <stdint.h>

volatile uint8_t key_pressed;
void uart_putc(uint8_t);
void uart_puts(char *string);
uint8_t uart_getc(void);