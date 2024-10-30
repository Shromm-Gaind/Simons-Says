#include <avr/interrupt.h>
#include "notes.h"   
#include "buzzer.h"
#include "states_m.h"
#include "uart.h"
#include "display.h"

buttons button;
simon_stage stage;
volatile uint8_t button_active;
// In your global variables (at top of file or in header)
volatile uint8_t reading_name = 0;
volatile uint8_t name_complete = 0;


// Interrupt Service Routine: USART0_RXC_vect
// Description: Handles USART0 receive complete interrupt
ISR(USART0_RXC_vect)
{
    // Read the received data from USART data register
    char rx_data = USART0.RXDATAL;

    if (reading_name) {
        if (rx_data == '\n') {
            reading_name = 0;
            name_complete = 1;
            uart_putc('\n');  // Echo the newline
        } else {
            uart_putc(rx_data);  // Echo the character
        }
        return;
    }

    // Determine the action based on the received character
    // Only take gameplay input if it's the user's turn.
    if (stage == INPUT)
    {
        switch (rx_data)
        {
        case '1':
        case 'q':
            button_active = PIN4_bm;
            break;
        case '2':
        case 'w':
            button_active = PIN5_bm;
            break;
        case '3':
        case 'e':
            button_active = PIN6_bm;
            break;
        case '4':
        case 'r':
            button_active = PIN7_bm;
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
            button_active = 0;
            break;
        }
    }
}

void uart_putc(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ;
    USART0.TXDATAL = c;
}


void uart_puts(char *string)
{

    while (*string)
    {
        uart_putc(*string++);
    }
}

// Function to send score as string
void send_score(uint16_t score)
{
    char score_str[6];  // Max 5 digits + null terminator
    uint8_t idx = 0;
    
    // Handle zero case
    if (score == 0) {
        uart_putc('0');
        return;
    }
    
    // Convert to string by repeatedly getting remainder
    while (score > 0) {
        score_str[idx++] = '0' + (score % 10);
        score = score / 10;  // Using assignment instead of division
    }
    
    // Send digits in reverse order
    while (idx > 0) {
        uart_putc(score_str[--idx]);
    }
}
