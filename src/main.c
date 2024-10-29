#include <avr/interrupt.h>
#include "buzzer.h"
#include "display.h"
#include <util/delay.h>
#include <stdlib.h> // Include for itoa

#include "initialisation.h"
#include "timer.h"
#include "types.h"
#include "uart.h"
#include "input.h"
#include "lsfr.h"


// Display variables:
uint8_t right_digit;
uint8_t left_digit;

// Initialise state machines.
buttons button = WAIT;
gameplay_stages gameplay_stage = INIT;



int main(void)
{
    cli(); // Disable interrupts for initialisation functions
    adc_init();
    button_init();
    spi_init();
    pwm_init();
    timers_init();
    uart_init();
    sei(); // Enable interrupts

    uint16_t sequence_length;

    while (1)
    {
        check_edge();

        switch (gameplay_stage)
        {
        case INIT:
            sequence_length = 1; // Unitialise the sequence length to 1 on reset/initialisation.
            gameplay_stage = SIMON;
            break;
        case SIMON:
            state_lsfr = re_init_state; // Initialise state to recreate the same sequence of steps as re_init_state.
            prepare_delay();
            if (delay_ready)
            {   
                for (int i = 0; i < sequence_length; i++)
                {
                    LSFR(&state_lsfr, &step, &result); // Create new step
                    buzzer_on(step);
                    display_segment(step);
                    half_playback_delay(); // Half delay
                    buzzer_off();
                    display_segment(4);    // Display off.
                    half_playback_delay(); // Half delay
                }
                state_lsfr = re_init_state; // Re-initialise state to recreate the same sequence, for the user's, as displayed by Simon.
                gameplay_stage = PLAYER;
            }
            break;
        case PLAYER:
            switch (button)
            {
            case WAIT:
                for (int i = 0; i < 4; i++) // Loop through each button/key.
                {
                    if ((pb_falling | key_pressed) & arr[i].pin) // Check if pushbutton or key pressed.
                    {
                        LSFR(&state_lsfr, &step, &result); // Update the step to compare the user's input to.
                        key_pressed = 0;                   // Rest key pressed flag bitmask.
                        elapsed_time = 0;                  // Start timer.
                        input_count++;                     // Log input.
                        button = arr[i].button;            // Change states.
                    }

                    // Check if the input came from the pushbutton.
                    if (pb_falling & arr[i].pin)
                    {
                        pushbutton_received = 1;
                    }
                }
                break;
            case BUTTON1:
                handle_button(0);
                break;
            case BUTTON2:
                handle_button(1);
                break;
            case BUTTON3:
                handle_button(2);
                break;
            case BUTTON4:
                handle_button(3);
                break;
            default:
                button = WAIT;
                break;
            }

            if (user_input) // If user input is detected.
            {
                if (!user_correct)
                {
                    user_correct = 1; // Reset flag.
                    input_count = 0;  // Reset flag.
                    gameplay_stage = FAIL;
                }
                else
                {
                    if (input_count == sequence_length) // If the number of inputs matches the sequence length.
                    {
                        input_count = 0; // Reset count.
                        gameplay_stage = SUCCESS;
                    }
                }
                user_input = 0; // Reset user input flag.
            }
            break;
        case SUCCESS:
            update_display(0, 0); // Success pattern.
            playback_delay();
            display_segment(4); // Display off.
            sequence_length++;
            gameplay_stage = SIMON;
            break;
        case FAIL:
            update_display(0b01110111, 0b01110111); // Fail pattern.
            playback_delay();
            extract_digits(sequence_length, &left_digit, &right_digit); // Extract digits from the sequence length (user's score) to be displayed.
            update_display(segs[left_digit], segs[right_digit]);
            playback_delay();
            display_segment(4); // Display off.
            playback_delay();
            LSFR(&state_lsfr, &step, &result); // Get next step to re-initialise to.
            re_init_state = state_lsfr;        // Re-initialise sequence to where it was left off.
            gameplay_stage = INIT;
            break;
        default:
            gameplay_stage = INIT;
            break;
        }
    }
}