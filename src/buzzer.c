#include "buzzer.h"

#include <avr/io.h>
#include <stdint.h>

volatile int8_t frequency = 0;


uint32_t period_map(uint8_t tone) {
    // Base periods for tones, bit-shifted by 3 because of trauncation in tests
    static const uint32_t base_periods[4] = {36024, 42840, 26984, 72056};

    return base_periods[tone] >> (frequency + 3); // Adjusted period for scaling
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


void buzzer_on(uint8_t tone) {
    uint32_t tone_period = period_map(tone); // Retrieve period for given tone
    TCA0.SINGLE.PERBUF = tone_period;        // Set period for PWM
    TCA0.SINGLE.CMP0BUF = tone_period >> 1;  // Set 50% duty cycle
}


void buzzer_off(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
}