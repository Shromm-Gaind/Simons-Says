/**
 * @file uart.c
 * @brief Implementation of UART communication and input processing
 * 
 * This module handles:
 * - Serial communication for game input
 * - Character echo and name entry
 * - Score reporting
 * - Button mapping from keyboard input
 */

#include <avr/interrupt.h>
#include "notes.h"   
#include "buzzer.h"
#include "states_m.h"
#include "uart.h"
#include "display.h"

/* Global state variables */
buttons button;                    // Current button state
simon_stage stage;                // Current game stage
volatile uint8_t button_active;   // Flag for active button press
volatile uint8_t reading_name;    // Flag for name entry mode
volatile uint8_t name_complete;   // Flag for completed name entry

/**
 * USART Receive Complete Interrupt Handler
 * 
 * Processes incoming serial data for:
 * 1. Name entry mode:
 *    - Echoes characters
 *    - Handles completion on newline
 * 
 * 2. Game input mode (during INPUT state):
 *    Primary controls:     Alternative controls:
 *    '1' or 'q' -> S1     ',' or 'k' -> Increase frequency
 *    '2' or 'w' -> S2     '.' or 'l' -> Decrease frequency
 *    '3' or 'e' -> S3
 *    '4' or 'r' -> S4
 * 
 * Note: A buffer implementation would be more robust for name entry,
 * but current implementation uses direct echo for simplicity.
 */
ISR(USART0_RXC_vect) {
    char rx_data = USART0.RXDATAL;

    /* Handle name entry mode */
    if (reading_name) {
        if (rx_data == '\n') {
            reading_name = 0;
            name_complete = 1;
            uart_putc('\n');      // Echo newline
        } else {
            uart_putc(rx_data);   // Echo character
        }
        return;
    }

    /* Process game input during INPUT state */
    if (stage == INPUT) {
        switch (rx_data) {
        /* Button 1 mappings */
        case '1':
        case 'q':
            button_active = PIN4_bm;
            break;
        
        /* Button 2 mappings */
        case '2':
        case 'w':
            button_active = PIN5_bm;
            break;
            
        /* Button 3 mappings */
        case '3':
        case 'e':
            button_active = PIN6_bm;
            break;
            
        /* Button 4 mappings */
        case '4':
        case 'r':
            button_active = PIN7_bm;
            break;
            
        /* Frequency control mappings */
        case ',':
        case 'k':
            increase_frequency();
            break;
        case '.':
        case 'l':
            decrease_frequency();
            break;
            
        /* Invalid input handling */
        default:
            button_active = 0;
            break;
        }
    }
}

/**
 * Transmits a single character via UART
 * 
 * @param c Character to transmit
 * 
 * Waits for transmit buffer to be empty before sending
 */
void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm))
        ;  // Wait for data register empty
    USART0.TXDATAL = c;
}

/**
 * Transmits a null-terminated string via UART
 * 
 * @param string Pointer to null-terminated string
 * 
 * Sends each character sequentially until null terminator
 */
void uart_puts(char *string) {
    while (*string) {
        uart_putc(*string++);
    }
}

/**
 * Converts and sends a score value as ASCII digits
 * 
 * @param score Score value to send (0-65535)
 * 
 * Algorithm:
 * 1. Special case for zero
 * 2. Extract digits by repeated division by 10
 * 3. Store digits in reverse order
 * 4. Send digits in correct order
 * 
 * Handles scores of any length up to 16-bit value
 */
void send_score(uint16_t score) {
    char score_str[6];           // Buffer for up to 5 digits plus null
    uint8_t idx = 0;
    
    /* Handle special case of zero */
    if (score == 0) {
        uart_putc('0');
        return;
    }
    
    /* Extract digits in reverse order */
    while (score > 0) {
        uint8_t digit = 0;
        uint16_t temp = score;
        while (temp >= 10) {
            temp -= 10;
            digit++;
        }
        score_str[idx++] = '0' + temp;
        score = digit;
    }
    
    /* Send digits in correct order */
    while (idx > 0) {
        uart_putc(score_str[--idx]);
    }
}