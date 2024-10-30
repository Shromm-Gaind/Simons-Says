#include <avr/io.h>
#include <avr/interrupt.h>

#include "display.h"

// Function: spi_write
// Description: Writes data to the SPI bus, alternating between left and right bytes.
void spi_write(void)
{
    // Use a register variable to ensure the compiler keeps this in a register
    register display_side_t current_side asm("r16");
    static display_side_t saved_side = SIDE_LEFT;
    
    // Load the saved state
    current_side = saved_side;
    
    // Direct assignment using ternary operator - typically compiles to branchless code
    SPI0.DATA = current_side ? right_byte : left_byte;
    
    // Toggle using bitwise XOR - typically more efficient than logical NOT
    saved_side = current_side ^ 1;
}