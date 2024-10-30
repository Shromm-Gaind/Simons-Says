#ifndef INPUT_H
#define INPUT_H

#include <avr/io.h>
#include "states_m.h"

// Type definitions
typedef struct {
    uint8_t pin;
    buttons button;
} button_pin;


// Public function declarations
void check_edge(void);
void pb_debounce(void);
void button_press(uint8_t button_index);

// External variable declarations
extern uint8_t pb_sample;
extern uint8_t pb_sample_r;
extern volatile uint8_t pb_debounced_state;
extern uint8_t pb_changed;
extern uint8_t pb_falling;
extern uint8_t pb_rising;
extern volatile uint8_t button_active;
extern uint8_t pb_released;
extern uint8_t pushbutton_received;

extern uint8_t player_input;
extern uint8_t sequence_position;
extern uint8_t sequence_matched;

extern button_pin mapped_array[4];

#endif // INPUT_H