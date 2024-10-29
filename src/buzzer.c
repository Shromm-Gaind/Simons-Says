#include "buzzer.h"

#include <avr/io.h>
#include <stdint.h>

volatile int8_t octave = 0;


// Function: period_map
// Description: Returns the appropriate period for each tone, potentially adjusted based on the octave.
uint32_t period_map(uint8_t tone) {
    // Base periods for tones, bit-shifted by 3 because of trauncation in tests
    static const uint32_t base_periods[4] = {36024, 42840, 26984, 72056};

    // Calculate tone period based on octave shift (left-shift for higher octaves)
    return base_periods[tone] >> (octave + 3); // Adjusted period for scaling
}

// Function: buzzer_on
// Description: Turns the buzzer on to a given frequency with a 50% duty cycle.
// Parameters:
//  - tone: Index of the tone to be played (0 to 3)
void buzzer_on(uint8_t tone) {
    uint32_t tone_period = period_map(tone); // Retrieve period for given tone
    TCA0.SINGLE.PERBUF = tone_period;        // Set period for PWM
    TCA0.SINGLE.CMP0BUF = tone_period >> 1;  // Set 50% duty cycle
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