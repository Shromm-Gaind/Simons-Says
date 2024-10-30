#include <avr/io.h>
#include <avr/interrupt.h>
#include "input.h"
#include "timer.h"
#include "display.h"
#include "states_m.h"
#include "lsfr.h"
#include "buzzer.h"
#include "spi.h"



// Variable definitions
uint8_t pb_sample = 0xFF;
uint8_t pb_sample_r = 0xFF;
volatile uint8_t pb_debounced_state = 0xFF;
uint8_t pb_changed;
uint8_t pb_falling;
uint8_t pb_rising;
volatile uint8_t key_pressed = 0;
uint8_t pb_released = 0;
uint8_t pushbutton_received = 0;

uint8_t user_input = 0;
uint8_t input_count = 0;
uint8_t user_correct = 1;


button_pin mapped_array[4] = {
    {PIN4_bm, S1},
    {PIN5_bm, S2},
    {PIN6_bm, S3},
    {PIN7_bm, S4}
};



// Function: pb_debounce
// Description: Debounces the push buttons using a vertical counter method.
void pb_debounce(void)
{
    // Vertical counter bits.
    static uint8_t count0 = 0; // Counter bit 0
    static uint8_t count1 = 0; // Counter bit 1

    uint8_t pb_sample = PORTA.IN; // Read the current state of the push buttons

    uint8_t pb_changed = (pb_sample ^ pb_debounced_state); // Detect changes

    // Vertical counter update.
    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;

    pb_debounced_state ^= (count0 & count1); // Update the debounced state
}


void check_edge(void)
{
    pb_sample_r = pb_sample;
    pb_sample = pb_debounced_state;

    pb_changed = pb_sample_r ^ pb_sample;

    pb_falling = pb_changed & pb_sample_r;
    pb_rising = pb_changed & pb_sample;
}


void button_press(uint8_t button_index) 
{
    const uint8_t button_pin = mapped_array[button_index].pin;
    
    buzzer_on(button_index);
    display_segment(button_index);

    if (step != button_index) {
        user_correct = 0;
    }

    if (!pb_released) {
        if ((pb_rising & button_pin) || !pushbutton_received) {
            pb_released = 1;
            pushbutton_received = 0;
        }
    } else if (playback_timer >= (playback_delay >> 1)) {
        buzzer_off();
        display_segment(4);
        user_input = 1;
        pb_released = 0;
        button = COMPLETE;
    }
}


// ISR: TCB1_INT_vect
// Description: Timer interrupt service routine triggered every 5ms.
ISR(TCB1_INT_vect)
{
    pb_debounce();
    spi_write();   // Write data to SPI


    TCB1.INTFLAGS = TCB_CAPT_bm; // Clear interrupt flag
}
