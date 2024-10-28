
#include <avr/io.h>
#include <avr/interrupt.h>

// Function: spi_write
// Description: Writes data to the SPI bus, alternating between left and right bytes.
void spi_write(void);