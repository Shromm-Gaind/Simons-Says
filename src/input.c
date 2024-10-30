/**
 * @file input.c
 * @brief Implementation of button input processing and debouncing
 * 
 * This module handles:
 * - Button debouncing using vertical counter method
 * - Edge detection for button presses/releases
 * - Button-to-action mapping
 * - Sequence matching for game logic
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "input.h"
#include "timer.h"
#include "display.h"
#include "states_m.h"
#include "lsfr.h"
#include "buzzer.h"
#include "spi.h"

/**
 * Button state tracking variables:
 * pb_sample: Current button state sample
 * pb_sample_r: Previous button state sample
 * pb_debounced_state: Stable button state after debouncing
 * pb_changed: Mask of buttons that changed state
 * pb_falling: Mask of buttons that were just pressed
 * pb_rising: Mask of buttons that were just released
 * button_active: Flag indicating active button processing
 * pb_released: Flag indicating button release detected
 * pushbutton_received: Flag for button input acknowledgment
 */
uint8_t pb_sample = 0xFF;
uint8_t pb_sample_r = 0xFF;
volatile uint8_t pb_debounced_state = 0xFF;
uint8_t pb_changed;
uint8_t pb_falling;
uint8_t pb_rising;
volatile uint8_t button_active = 0;
uint8_t pb_released = 0;
uint8_t pushbutton_received = 0;

/**
 * Game state variables:
 * player_input: Flag indicating player has provided input
 * sequence_position: Current position in the sequence being matched
 * sequence_matched: Flag indicating if player's input matches expected sequence
 */
uint8_t player_input = 0;
uint8_t sequence_position = 0;
uint8_t sequence_matched = 1;

/**
 * Button mapping structure array
 * Maps physical pins to logical button identifiers
 * S1-S4 correspond to game buttons
 */
button_pin mapped_array[4] = {
    {PIN4_bm, S1},
    {PIN5_bm, S2},
    {PIN6_bm, S3},
    {PIN7_bm, S4}
};

/**
 * Implements button debouncing using vertical counter method
 * 
 * The vertical counter method uses two counter bits per input to
 * filter out noise and provide stable button states. A button state
 * change is only registered after multiple consistent samples.
 */
void pb_debounce(void) {
    static uint8_t count0 = 0; // Counter bit 0 for vertical counting
    static uint8_t count1 = 0; // Counter bit 1 for vertical counting

    uint8_t pb_sample = PORTA.IN;    // Sample current button states
    uint8_t pb_changed = (pb_sample ^ pb_debounced_state); // Detect changes

    /* Update vertical counter bits */
    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;

    /* Update debounced state when both counter bits indicate stable change */
    pb_debounced_state ^= (count0 & count1);
}

/**
 * Detects edge transitions (press/release) for buttons
 * 
 * Compares current and previous button states to detect:
 * - Rising edges (button releases)
 * - Falling edges (button presses)
 * Updates global state variables for edge detection
 */
void check_edge(void) {
    pb_sample_r = pb_sample;         // Save previous sample
    pb_sample = pb_debounced_state;  // Get current sample

    pb_changed = pb_sample_r ^ pb_sample;  // Detect any changes

    pb_falling = pb_changed & pb_sample_r; // Detect press (falling edge)
    pb_rising = pb_changed & pb_sample;    // Detect release (rising edge)
}

/**
 * Processes a button press event
 * 
 * @param button_index Index of the button being processed (0-3)
 * 
 * Handles:
 * - Button sound feedback
 * - Display update
 * - Sequence matching
 * - Button release detection
 * - State transitions
 */
void button_press(uint8_t button_index) {
    const uint8_t button_pin = mapped_array[button_index].pin;
    
    /* Activate feedback for button press */
    buzzer_on(button_index);
    display_digit(button_index);

    /* Check if pressed button matches sequence */
    if (step != button_index) {
        sequence_matched = 0;
    }

    /* Handle button release and state transition */
    if (!pb_released) {
        if ((pb_rising & button_pin) || !pushbutton_received) {
            pb_released = 1;
            pushbutton_received = 0;
        }
    } else if (playback_timer >= (playback_delay >> 1)) {
        buzzer_off();
        display_digit(4);
        player_input = 1;
        pb_released = 0;
        button = COMPLETE;
    }
}

/**
 * Timer interrupt service routine (5ms interval)
 * 
 * Handles periodic tasks:
 * - Button debouncing
 * - SPI display updates
 * 
 * Note: Triggered by TCB1 timer every 5ms
 */
ISR(TCB1_INT_vect) {
    pb_debounce();    // Update button debouncing
    spi_write();      // Update display via SPI
    
    TCB1.INTFLAGS = TCB_CAPT_bm;  // Clear interrupt flag
}