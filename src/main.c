#include <avr/interrupt.h>
#include <util/delay.h>

#include "initialisation.h"
#include "timer.h"
#include "states_m.h"
#include "uart.h"
#include "input.h"
#include "lsfr.h"
#include "main.h"
#include "buzzer.h"
#include "display.h"


// Initialise state machines.
buttons button = COMPLETE;
simon_stage stage = START;


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
            display_digit(step);
            half_of_delay();
            buzzer_off();
            display_digit(4);
            half_of_delay();
        }
        state_sequence = seed;
        stage = INPUT;
    }
}


// Modified version of your process_user_input function
static inline void process_user_input(uint16_t sequence_length) {
    if (user_input) {
        if (!user_correct) {
            user_correct = 1;    
            input_count = 0;     
            stage = FAIL;
            uart_puts("GAME OVER\n");
            send_score(sequence_length - 1);
            uart_putc('\n');
        } else {
            if (input_count == sequence_length) {
                input_count = 0;  
                stage = SUCCESS;
                uart_puts("SUCCESS\n");
                send_score(sequence_length);
                uart_putc('\n');
            }
        }
        user_input = 0;
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

        switch (stage)
        {
        case START:
            sequence_length = 1; // Unitialise the sequence length to 1 on reset/initialisation.
            stage = START_SEQUENCE;
            break;
        case START_SEQUENCE:
            state_sequence = seed; // Initialise state to recreate the same sequence of steps as re_init_state.
            calculate_playback_delay();
            play_sequence(sequence_length);
            break;
        case INPUT:
            switch (button)
            {
            case COMPLETE:
                check_button_input();
                break;
            case S1:
                button_press(0);
                break;
            case S2:
                button_press(1);
                break;
            case S3:
                button_press(2);
                break;
            case S4:
                button_press(3);
                break;
            default:
                button = COMPLETE;
                break;
            }
            process_user_input(sequence_length);
            break;
        case SUCCESS:
            update_display(PATTERN_SUCCESS_LEFT, PATTERN_SUCCESS_RIGHT); // Success pattern
            delay();
            display_digit(4); // Display off.
            sequence_length++;
            stage = START_SEQUENCE;
            break;
        case FAIL:
            update_display(PATTERN_FAIL_LEFT, PATTERN_FAIL_RIGHT); // Fail pattern
            delay();
            extract_digits(sequence_length, &left_digit, &right_digit); // Extract digits from sequence length
            update_display(segments[left_digit], segments[right_digit]);
            delay();
            display_digit(4); // Display off.
            delay();
            uart_puts("Enter name: ");  // Add name prompt here after all displays and delays
            SEQUENCE(&state_sequence, &step, &result); // Get next step to re-initialise to.
            seed = state_sequence;        // Re-initialise sequence to where it was left off.
            stage = START;
            break;
        default:
            stage = START;
            break;
        }
    }
}