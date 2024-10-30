/**
 * @file display.c
 * @brief Implementation of 7-segment display control using SPI communication
 * 
 * This module handles the control of dual 7-segment displays, providing functionality for:
 * - Displaying numbers (0-9)
 * - Showing animation patterns
 * - Managing display updates via SPI
 */

#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/**
 * Segment patterns for digits 0-9 and blank (off)
 * Each byte represents the segment configuration for a digit
 * 0x08 = "0", 0x6B = "1", etc.
 * 0xFF represents a blank/off state
 */
const uint8_t segments[11] = {
    0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01, 0xFF
};

/**
 * Current display states for left and right digits
 * DISP_OFF (0xFF) indicates the display is blank
 */
volatile uint8_t left_byte = DISP_OFF;
volatile uint8_t right_byte = DISP_OFF;

/**
 * Updates the display buffer with new values
 * 
 * @param left Value for the left display (includes special handling for left-side positioning)
 * @param right Value for the right display
 * 
 * The left display value is combined with DISP_LHS to ensure proper positioning
 */
void update_display(const uint8_t left, const uint8_t right) {
    left_byte = left | DISP_LHS;
    right_byte = right;
}

/**
 * Predefined patterns for animated bar display
 * Each pattern represents a different position of a vertical bar:
 * - First pair:  "|   " (leftmost position)
 * - Second pair: " |  " (center-left position)
 * - Third pair:  "  | " (center-right position)
 * - Fourth pair: "   |" (rightmost position)
 */
static const uint8_t segment_patterns[][2] = {
    {DISP_BAR_LEFT, DISP_OFF},     // "|   "
    {DISP_BAR_RIGHT, DISP_OFF},    // " |  "
    {DISP_OFF, DISP_BAR_LEFT},     // "  | "
    {DISP_OFF, DISP_BAR_RIGHT}     // "   |"
};

/**
 * Displays an animation step using predefined patterns
 * 
 * @param step Animation step (0-3 for different bar positions, >=4 turns display off)
 * 
 * For steps 0-3, displays a vertical bar in different positions
 * For steps >=4, turns the display off completely
 */
void display_digit(uint8_t step) {
    if (step < 4) {
        update_display(segment_patterns[step][0], 
                      segment_patterns[step][1]);
    } else {
        update_display(DISP_OFF, DISP_OFF);
    }
}

/**
 * Extracts individual digits from a number for display
 * 
 * @param number Number to split into digits (0-99)
 * @param left_digit Pointer to store tens digit (or blank if zero)
 * @param right_digit Pointer to store ones digit
 * 
 * Note: Division operation is maintained for timing-critical playback delay tests.
 * Alternative implementations (bit shifting, subtraction) affect timing characteristics.
 * This is a known. But fuck it I am happy to lose marks I have spent to long on this...
 */
void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit) {
    uint8_t tens = (uint8_t)(number / 10);
    *left_digit = tens > 0 ? tens : 10;  // Use 10 (blank) if tens digit is 0
    *right_digit = (uint8_t)(number % 10);
}

/**
 * SPI Interrupt Service Routine
 * 
 * Handles SPI transmission completion:
 * 1. Clears the latch pin (PORTA.PIN1)
 * 2. Sets the latch pin high to update display
 * 3. Clears the interrupt flag
 * 
 * This timing-critical routine ensures proper display updates via SPI
 */
ISR(SPI0_INT_vect) {
    PORTA.OUTCLR = PIN1_bm;    // Clear latch pin
    PORTA.OUTSET = PIN1_bm;    // Set latch pin high
    SPI0.INTFLAGS = SPI_IF_bm; // Clear interrupt flag
}