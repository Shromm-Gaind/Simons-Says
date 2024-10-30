#include <avr/interrupt.h>
#include <util/delay.h>

#include "initialisation.h"
#include "timer.h"
#include "types.h"
#include "uart.h"
#include "input.h"
#include "lsfr.h"
#include "main.h"
#include "buzzer.h"
#include "display.h"


// Initialise state machines.
buttons button = WAIT;
gameplay_stages gameplay_stage = INIT;


static inline void check_button_input(void) {
    for (int i = 0; i < 4; i++) {
        if ((pb_falling | key_pressed) & mapped_array[i].pin) {
            SEQUENCE(&state_sequence, &step, &result);
            key_pressed = 0;
            playback_timer = 0;
            input_count++;
            button = mapped_array[i].button;
        }

        if (pb_falling & mapped_array[i].pin) {
            pushbutton_received = 1;
        }
    }
}

static inline void play_sequence(uint16_t sequence_length) {
    if (adc_ready_flag) {   
        for (int i = 0; i < sequence_length; i++) {
            SEQUENCE(&state_sequence, &step, &result);
            buzzer_on(step);
            display_segment(step);
            half_of_delay();
            buzzer_off();
            display_segment(4);
            half_of_delay();
        }
        state_sequence = seed;
        gameplay_stage = PLAYER;
    }
}

static inline void process_user_input(uint16_t sequence_length) {
    if (user_input) {
        if (!user_correct) {
            user_correct = 1;    // Reset flag
            input_count = 0;     // Reset flag
            gameplay_stage = FAIL;
        } else {
            if (input_count == sequence_length) {
                input_count = 0;  // Reset count
                gameplay_stage = SUCCESS;
            }
        }
        user_input = 0;  // Reset user input flag
    }
}



int main(void)
{
    cli(); // Disable interrupts for initialisation functions
    INIT_ALL_SYSTEMS();
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
            state_sequence = seed; // Initialise state to recreate the same sequence of steps as re_init_state.
            calculate_playback_delay();
            play_sequence(sequence_length);
            break;
        case PLAYER:
            switch (button)
            {
            case WAIT:
                check_button_input();
                break;
            case BUTTON1:
                button_press(0);
                break;
            case BUTTON2:
                button_press(1);
                break;
            case BUTTON3:
                button_press(2);
                break;
            case BUTTON4:
                button_press(3);
                break;
            default:
                button = WAIT;
                break;
            }
            process_user_input(sequence_length);
            break;
        case SUCCESS:
            update_display(PATTERN_SUCCESS_LEFT, PATTERN_SUCCESS_RIGHT); // Success pattern
            delay();
            display_segment(4); // Display off.
            sequence_length++;
            gameplay_stage = SIMON;
            break;
        case FAIL:
            update_display(PATTERN_FAIL_LEFT, PATTERN_FAIL_RIGHT); // Fail pattern
            delay();
            extract_digits(sequence_length, &left_digit, &right_digit); // Extract digits from the sequence length (user's score) to be displayed.
            update_display(segments[left_digit], segments[right_digit]);
            delay();
            display_segment(4); // Display off.
            delay();
            SEQUENCE(&state_sequence, &step, &result); // Get next step to re-initialise to.
            seed = state_sequence;        // Re-initialise sequence to where it was left off.
            gameplay_stage = INIT;
            break;
        default:
            gameplay_stage = INIT;
            break;
        }
    }
}