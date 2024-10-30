/**
 * @file spi.c
 * @brief Optimized SPI communication for dual-digit display
 * 
 * This module implements efficient SPI communication to drive a multiplexed
 * dual-digit display. The implementation uses several optimization techniques
 * to ensure consistent timing and minimal overhead.
 * Optimization Strategy:
 * 1. Register variable for side tracking - keeps value in register
 * 2. Static state preservation - maintains multiplexing sequence
 * 3. Branchless selection - avoids pipeline stalls
 * 4. Bitwise XOR toggle - efficient state switching
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"

/**
 * Writes display data to SPI bus with optimized multiplexing
 * 
 * Implementation details:
 * - Uses register variable for current_side to ensure fast access
 * - Maintains display side state between calls
 * - Uses branchless operations for predictable timing
 * - Toggles display side using XOR for efficiency
 * 
 * Memory access pattern:
 * Cycle 1: Read saved_side -> register
 * Cycle 2: Select byte based on side
 * Cycle 3: Write to SPI
 * Cycle 4: Toggle side with XOR
 * Cycle 5: Save new side
 */
void spi_write(void) {
    /* Keep side tracking in register for speed */
    register display_side_t current_side asm("r16");
    static display_side_t saved_side = SIDE_LEFT;
    
    /* Load previous state - single cycle operation */
    current_side = saved_side;
    
    /* Select and write byte - branchless operation */
    SPI0.DATA = current_side ? right_byte : left_byte;
    
    /* Toggle side for next write - single cycle XOR */
    saved_side = current_side ^ 1;
}