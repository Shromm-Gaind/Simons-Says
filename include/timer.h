#include <stdint.h>

volatile uint16_t playback_delay;
volatile uint16_t playback_timer;
void calculate_playback_delay(void);
void delay(void);
void half_of_delay(void);
uint8_t adc_ready_flag;