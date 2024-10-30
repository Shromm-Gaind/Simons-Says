#include <avr/interrupt.h>
#include "notes.h"   // For Note enum
#include "buzzer.h"  // For buzzer functions
#include "states_m.h"
#include "uart.h"
#include "display.h"

buttons button;
simon_stage stage;
volatile uint8_t key_pressed;

// Interrupt Service Routine: USART0_RXC_vect
// Description: Handles USART0 receive complete interrupt
ISR(USART0_RXC_vect)
{
    // Read the received data from USART data register
    char rx_data = USART0.RXDATAL;

    // Determine the action based on the received character
    // Only take gameplay input if it's the user's turn.
    if (stage == INPUT)
    {
        switch (rx_data)
        {
        case '1':
        case 'q':
            key_pressed = PIN4_bm;
            break;
        case '2':
        case 'w':
            key_pressed = PIN5_bm;
            break;
        case '3':
        case 'e':
            key_pressed = PIN6_bm;
            break;
        case '4':
        case 'r':
            key_pressed = PIN7_bm;
            break;
        case ',':
        case 'k':
            increase_frequency();
            break;
        case '.':
        case 'l':
            decrease_frequency();
            break;
        default:
            key_pressed = 0;
            break;
        }
    }
}

// Function: uart_putc
// Description: Outputs a character to UART.
void uart_putc(uint8_t c)
{
    // Wait until the transmit data register is empty
    while (!(USART0.STATUS & USART_DREIF_bm))
        ;
    USART0.TXDATAL = c;
}

// Function: uart_puts
// Description: Outputs a series of characters (string) to UART.
void uart_puts(char *string)
{
    // Output each character until the null terminator is reached.
    while (*string)
    {
        uart_putc(*string++);
    }
}