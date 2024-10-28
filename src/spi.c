#include <avr/io.h>
#include <avr/interrupt.h>

#include "display.h"

// Function: spi_write
// Description: Writes data to the SPI bus, alternating between left and right bytes.
void spi_write(void)
{
    static uint8_t current_side = 0; // Current side to write (0 for left, 1 for right)

    if (current_side)
    {
        SPI0.DATA = left_byte; // Write left byte to SPI
    }
    else
    {
        SPI0.DATA = right_byte; // Write right byte to SPI
    }

    // Toggle the current side.
    current_side = !current_side;
}
