#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// Keep the original volatile declaration to match header
const uint8_t segments[11] = {
    0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01, 0xFF
};

volatile uint8_t left_byte = DISP_OFF;
volatile uint8_t right_byte = DISP_OFF;

// Remove static inline to match header declaration
void update_display(const uint8_t left, const uint8_t right) {
    left_byte = left | DISP_LHS;
    right_byte = right;
}

// Define segment patterns using proper size types
static const uint8_t segment_patterns[][2] = {
    {DISP_BAR_LEFT, DISP_OFF},     // "|   "
    {DISP_BAR_RIGHT, DISP_OFF},    // " |  "
    {DISP_OFF, DISP_BAR_LEFT},     // "  | "
    {DISP_OFF, DISP_BAR_RIGHT}     // "   |"
};

void display_digit(uint8_t step) {
    if (step < 4) {
        update_display(segment_patterns[step][0], 
                      segment_patterns[step][1]);
    } else {
        update_display(DISP_OFF, DISP_OFF);
    }
}

void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit) {
    uint8_t tens = (uint8_t)(number / 10);
    *left_digit = tens > 0 ? tens : 10;
    *right_digit = (uint8_t)(number % 10);
}

ISR(SPI0_INT_vect) {
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;
    SPI0.INTFLAGS = SPI_IF_bm;
}