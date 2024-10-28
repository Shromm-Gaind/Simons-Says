#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t left_byte = DISP_OFF;
volatile uint8_t right_byte = DISP_OFF;

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