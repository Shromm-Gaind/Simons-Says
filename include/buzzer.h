#include <stdint.h>

#define MAX_OCTAVE 3
#define MIN_OCTAVE -3

void buzzer_on(const uint8_t tone);
void buzzer_off(void);
void decrease_octave(void);
void increase_octave(void);