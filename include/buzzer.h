#include <stdint.h>

#define max_frequency 3
#define min_frequency -3

void buzzer_on(const uint8_t tone);
void buzzer_off(void);
void decrease_frequency(void);
void increase_frequency(void);
uint32_t period_map(uint8_t tone);