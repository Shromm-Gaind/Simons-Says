#include <avr/io.h>
#include <avr/interrupt.h>
#include "types.h"

// Pushbutton handling variables
extern uint8_t pb_sample;
extern uint8_t pb_sample_r;
extern uint8_t pb_changed;
extern uint8_t pb_falling;
extern uint8_t pb_rising;
extern volatile uint8_t key_pressed;
extern uint8_t pb_released;
extern uint8_t pushbutton_received;
extern volatile uint8_t pb_debounced_state;


// Gameplay state variables
extern uint8_t user_input;
extern uint8_t input_count;
extern uint8_t user_correct;

// Function: check_edge
// Description: Checks for rising or falling edges from the pushbuttons.
void check_edge(void);

void pb_debounce(void);

// Struct for button-to-pin mappings
typedef struct {
    uint8_t pin;
    buttons button;
} button_pin;

// Button mapping array
extern button_pin arr[4];

// Function: extract_digits
// Description: Separates an integer into its tens and units digits
// Parameters:
//  - number: The number to be split into digits
//  - left_digit: Pointer to store the tens digit
//  - right_digit: Pointer to store the units digit
void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit);

// Segment display hexadecimal values for digits
extern volatile uint8_t segs[11];

// Function: handle_button
// Description: Handles button press events during the PLAYER stage
// Parameters:
//  - button_index: Index of the button pressed
void handle_button(uint8_t button_index);