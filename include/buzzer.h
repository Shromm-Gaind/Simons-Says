// buzzer.h
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include "notes.h"  // Include the notes enum definition

#define max_frequency 3
#define min_frequency -3
#define scaling_factor 3

// The array declaration
extern const uint32_t base_periods[4];

// Function declarations
void buzzer_on(const Note note);
void buzzer_off(void);
void decrease_frequency(void);
void increase_frequency(void);
uint32_t period_map(Note note);

#endif  // BUZZER_H