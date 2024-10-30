#include "buzzer.h"

#include <avr/io.h>
#include <stdint.h>

volatile int8_t frequency = 0;


// Define the array with corresponding notes
const uint32_t base_periods[4] = {
    [E_HIGH] = 36024,   // E high note
    [C_SHARP] = 42840,  // C# note
    [A] = 26984,        // A note
    [E_LOW] = 72056     // E low note
};

uint32_t period_map(Note note) {
    return base_periods[note] >> (frequency + scaling_factor);
}


void increase_frequency(void)
{
    if (frequency < max_frequency)
        frequency++;
}


void decrease_frequency(void)
{
    if (frequency > min_frequency)
        frequency--;
}


void buzzer_on(Note note) {
    uint32_t tone_period = period_map(note);
    TCA0.SINGLE.PERBUF = tone_period;
    TCA0.SINGLE.CMP0BUF = tone_period >> 1;
}


void buzzer_off(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
}