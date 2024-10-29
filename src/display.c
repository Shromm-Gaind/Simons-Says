#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t left_byte = DISP_OFF;
volatile uint8_t right_byte = DISP_OFF;

// Indexed digits encoded within hexadecimal values.
volatile uint8_t segments[] = {0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01, 0xFF};

// Function: update_display
// Description: Updates the display bytes for the left and right digits
// Parameters:
//  - left: 7-segment encoded value for the left digit
//  - right: 7-segment encoded value for the right digit
void update_display(const uint8_t left, const uint8_t right)
{
    left_byte = left | DISP_LHS;
    right_byte = right;
}

// Function: display_segment
// Description: Updates the display to show a specific segment based on the step
// Parameters:
//  - step: Index of the segment to be displayed (0 to 3)
void display_segment(uint8_t step)
{
    switch (step)
    {
    case 0:
        update_display(DISP_BAR_LEFT, DISP_OFF); // Displays pattern: "|   "
        break;
    case 1:
        update_display(DISP_BAR_RIGHT, DISP_OFF); // Displays pattern: " |  "
        break;
    case 2:
        update_display(DISP_OFF, DISP_BAR_LEFT); // Displays pattern: "  | "
        break;
    case 3:
        update_display(DISP_OFF, DISP_BAR_RIGHT); // Displays pattern: "   |"
        break;
    default:
        update_display(DISP_OFF, DISP_OFF);
        break;
    }
}

// Function: extract_digits
// Description: Separates an integer into its tens and units digits
// Parameters:
//  - number: The number to be split into digits
//  - left_digit: Pointer to store the tens digit
//  - right_digit: Pointer to store the units digit
void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit)
{
    uint8_t tens_count;

    while (number > 10)
    {
        number -= 10;
        tens_count++;
    }

    if (tens_count < 1)
    {
        *left_digit = 10; // Setting as ten will leave the display blank.
    }
    else
    {
        *left_digit = tens_count;
    }

    *right_digit = number;
}



// Interrupt Service Routine: SPI0_INT_vect
// Description: Handles SPI interrupt, latching the display values
ISR(SPI0_INT_vect)
{
    // Create rising edge on DISP LATCH
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;

    // Clear the SPI interrupt flag
    SPI0.INTFLAGS = SPI_IF_bm;
}