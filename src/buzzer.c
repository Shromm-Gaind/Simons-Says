/**
 * @file buzzer.c
 * @brief Implementation of buzzer control functionality using AVR timer
 * 
 * This module provides functions to control a buzzer/speaker, allowing for:
 * - Playing different musical notes (E high, C#, A, E low)
 * - Adjusting frequency/pitch
 * - Turning the buzzer on and off
 */

#include "buzzer.h"
#include <avr/io.h>
#include <stdint.h>

/* Global frequency adjustment value, used to shift pitch up or down */
volatile int8_t frequency = 0;

/**
 * Base periods for different musical notes.
 * These values represent the timer periods needed to generate specific frequencies.
 * Lower period values produce higher frequencies/pitches.
 * scaled by three to avoid any turnication
 * 
 * (1 / F)   / (1 / (( 20MHz * 10^6 ) / 12) 
 * NOTE TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc; 
 * prescaler IS 2
 */
const uint32_t base_periods[4] = {
    [E_HIGH] = 36024,   // E high note 370Hz
    [C_SHARP] = 42840,  // C# note 311Hz
    [A] = 26984,        // A note 494Hz
    [E_LOW] = 72056     // E low note 185hz
};

/**
 * Maps a musical note to its actual timer period value, accounting for frequency adjustment
 * 
 * @param note The musical note to map
 * @return The calculated timer period for the note
 * 
 * The period is calculated by right-shifting the base period by (frequency + scaling_factor).
 * This allows for dynamic frequency adjustment while maintaining relative note relationships.
 */
uint32_t period_map(Note note) {
    return base_periods[note] >> (frequency + scaling_factor);
}

/**
 * Increases the frequency of all notes if not at maximum
 * 
 * Each increment effectively doubles the frequency (shifts right by 1)
 * Limited by max_frequency to prevent overflow/unplayable frequencies
 */
void increase_frequency(void) {
    if (frequency < max_frequency)
        frequency++;
}

/**
 * Decreases the frequency of all notes if not at minimum
 * 
 * Each decrement effectively halves the frequency (shifts left by 1)
 * Limited by min_frequency to prevent underflow/unplayable frequencies
 */
void decrease_frequency(void) {
    if (frequency > min_frequency)
        frequency--;
}

/**
 * Turns on the buzzer with the specified note
 * 
 * @param note The musical note to play
 * 
 * Configures the timer (TCA0) to generate the correct frequency:
 * - PERBUF sets the period of the waveform
 * - CMP0BUF sets the duty cycle to 50% (period >> 1)
 */
void buzzer_on(Note note) {
    uint32_t tone_period = period_map(note);
    TCA0.SINGLE.PERBUF = tone_period;
    TCA0.SINGLE.CMP0BUF = tone_period >> 1;
}

/**
 * Turns off the buzzer
 * 
 * Sets the compare buffer to 0, which results in a constant low output,
 * effectively silencing the buzzer
 */
void buzzer_off(void) {
    TCA0.SINGLE.CMP0BUF = 0;
}