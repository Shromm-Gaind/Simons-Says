#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "display.h"
#include "types.h"
#include "lsfr.h"
#include "buzzer.h"

// Variables for pushbutton/key press handling:
uint8_t pb_sample = 0xFF;
uint8_t pb_sample_r = 0xFF;
volatile uint8_t pb_debounced_state = 0xFF;
uint8_t pb_changed;
uint8_t pb_falling;
uint8_t pb_rising;
volatile uint8_t key_pressed = 0;
uint8_t pb_released = 0;
uint8_t pushbutton_received = 0;

// Variables for gameplay state:
uint8_t user_input = 0;
uint8_t input_count = 0;
uint8_t user_correct = 1;



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

// Function: check_edge
// Description: Checks for rising or falling edges from the pushbuttons.

// Struct containing enum type buttons and their associated bitmasks.
typedef struct buttons_pins
{
    uint8_t pin;
    buttons button;
} button_pin;

// Array of mapped bitmasks and buttons.
button_pin arr[4] = {{PIN4_bm, BUTTON1}, {PIN5_bm, BUTTON2}, {PIN6_bm, BUTTON3}, {PIN7_bm, BUTTON4}};

// Function: extract_digits
// Description: Separates an integer into its tens and units digits
// Parameters:
//  - number: The number to be split into digits
//  - left_digit: Pointer to store the tens digit
//  - right_digit: Pointer to store the units digit
void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit)
{
    uint8_t tens_count;

    while (number > 10)
    {
        number -= 10;
        tens_count++;
    }

    if (tens_count < 1)
    {
        *left_digit = 10; // Setting as ten will leave the display blank.
    }
    else
    {
        *left_digit = tens_count;
    }

    *right_digit = number;
}

// Indexed digits encoded within hexadecimal values.
volatile uint8_t segs[] = {0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01, 0xFF};

// Function: handle_button
// Description: Handles button press events during the PLAYER stage
// Parameters:
//  - button_index: Index of the button pressed
void handle_button(uint8_t button_index)
{
    buzzer_on(button_index);
    display_segment(button_index);

    // Check if user's input was correct
    if (step != button_index)
    {
        user_correct = 0;
    }

    if (!pb_released)
    {
        if (pb_rising & arr[button_index].pin) // If the pushbutton was released.
        {
            pb_released = 1;
            pushbutton_received = 0; // Reset flag.
        }
        else if (!pushbutton_received) // If there was not a pushbutton input (UART input).
        {
            pb_released = 1;
        }
    }
    else
    {
        // If playback delay has elapsed since the button press.
        if (elapsed_time >= (playback_delay_ms >> 1))
        {
            buzzer_off();
            display_segment(4);
            user_input = 1;  // Set user input flage.
            pb_released = 0; // Reset pushbutton released flag.
            button = WAIT;
        }
    }
}
