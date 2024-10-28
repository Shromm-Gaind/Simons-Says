#include "buzzer.h"

#include <avr/io.h>
#include <stdint.h>

volatile int8_t octave = 0;

// Function: buzzer_on
// Description: Turns the buzzer on to a given frequency with a 50% duty cycle.
// Parameters:
//  - tone: Index of the tone to be played (0 to 3)
void buzzer_on(uint8_t tone)
{
    // Predefined periods, scaled down by 3 left bit shifts, to prevent negative bitshifts.
    static const uint32_t periods[4] = {36024, 42840, 26984, 72056};

    uint16_t period = periods[tone] >> (octave + 3); // Adjust for scaling.

    TCA0.SINGLE.PERBUF = period;
    TCA0.SINGLE.CMP0BUF = period >> 1; // 50% duty cycle
}

// Function: increase_octave
// Description: Increases the octave by one step, ensuring the octave doesn't exceed the limit (3).
void increase_octave(void)
{
    if (octave < MAX_OCTAVE)
        octave++;
}

// Function: decrease_octave
// Description: Decreases the octave by one step.
void decrease_octave(void)
{
    if (octave > MIN_OCTAVE)
        octave--;
}

// Function: buzzer_off
// Description: Decreases the octave by one step, ensuring the octave doesn't exceed the limit (-3).
void buzzer_off(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
}