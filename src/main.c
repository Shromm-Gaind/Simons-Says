/**
 * @file main.c
 * @brief Main control logic for Simon Says game implementation
 * 
 * This module implements the core game logic including:
 * - Game state management
 * - Sequence playback and verification
 * - Button input processing
 * - Score tracking and display
 * - Success/failure handling
 */

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

/**
 * State machine enums for game control:
 * button: Tracks current button state/processing
 * stage: Tracks overall game progression
 */
buttons button = COMPLETE;
simon_stage stage = START;

/**
 * Processes button input events and updates game state
 * 
 * Checks for button press events and:
 * - Generates next sequence step
 * - Resets playback timer
 * - Updates sequence position
 * - Sets active button state
 */
static inline void check_button_input(void) {
    for (int i = 0; i < 4; i++) {
        /* Check for new button press or active button */
        if ((pb_falling | button_active) & mapped_array[i].pin) {
            SEQUENCE(&state_sequence, &step, &result);  // Generate next step
            button_active = 0;
            playback_timer = 0;
            sequence_position++;
            button = mapped_array[i].button;
        }

        /* Register button press */
        if (pb_falling & mapped_array[i].pin) {
            pushbutton_received = 1;
        }
    }
}

/**
 * Plays back the sequence for player to memorize
 * 
 * @param sequence_length Current length of sequence to play
 * 
 * For each step in sequence:
 * - Generates next sequence step
 * - Activates corresponding buzzer and display
 * - Adds delay between steps
 * - Resets sequence state for player input
 */
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
        state_sequence = seed;  // Reset sequence for player input
        stage = INPUT;
    }
}

/**
 * Processes player's input sequence and determines success/failure
 * 
 * @param sequence_length Current sequence length being matched
 * 
 * Handles:
 * - Sequence matching verification
 * - Game over on mismatch
 * - Success when full sequence matched
 * - Score reporting via UART
 */
static inline void process_user_input(uint16_t sequence_length) {
    if (player_input) {
        if (!sequence_matched) {
            /* Handle sequence mismatch */
            sequence_matched = 1;    
            sequence_position = 0;     
            stage = FAIL;
            uart_puts("GAME OVER\n");
            send_score(sequence_length - 1);
            uart_putc('\n');
        } else {
            /* Check for complete sequence match */
            if (sequence_position == sequence_length) {
                sequence_position = 0;  
                stage = SUCCESS;
                uart_puts("SUCCESS\n");
                send_score(sequence_length);
                uart_putc('\n');
            }
        }
        player_input = 0;
    }
}

/**
 * Main program entry point and game loop
 * 
 * Implements the main state machine controlling game flow:
 * START: Initialize game parameters
 * START_SEQUENCE: Play current sequence
 * INPUT: Process player input
 * SUCCESS: Handle successful sequence completion
 * FAIL: Handle incorrect sequence input
 * 
 * Also manages button state machine for input processing
 *  * State Machine Diagram:
 * 
 * +-------------------+     Initialize     +----------------------+
 * |                   |---------------->   |                      |
 * |       START       |                   |    START_SEQUENCE    |
 * |                   |     Fail          |                      |
 * +-------------------+   <----------     +----------------------+
 *          ^                              |    1. Reset sequence |
 *          |                              |    2. Calculate delay|
 *          |                              |    3. Play pattern  |
 *          |                              v                      |
 * +-------------------+     Success     +----------------------+
 * |      FAILURE      |                |         INPUT        |
 * |-------------------|   <----------  |----------------------|
 * | 1. Show pattern   |                | 1. Process buttons   |
 * | 2. Display score  |                | 2. Check sequence    |
 * | 3. Get user name  |                | 3. Verify matching   |
 * | 4. Update seed    |                |                      |
 * +-------------------+                +----------------------+
 *                                              |
 *                       +----------------------+
 *                       |
 *                       v
 * +-------------------+     Increment    +----------------------+
 * |      SUCCESS      |     Length      |    START_SEQUENCE    |
 * |-------------------|  ------------->  |----------------------|
 * | 1. Show pattern   |                 |    (Next Level)      |
 * | 2. Display score  |                 |                      |
 * +-------------------+                 +----------------------+
 * 
 * Button State Machine:
 * 
 * +------------+    Press    +------------+    Release    +-----------+
 * | COMPLETE   |-----------> |  S1-S4     |-------------> | COMPLETE  |
 * | (Waiting)  |            | (Active)    |   after      | (Ready)   |
 * +------------+            +------------+   delay       +-----------+
 *      ^                    | 1. Sound   |
 *      |                    | 2. Display |
 *      +--------------------| 3. Check   |
 *           Complete        +------------+
 */
int main(void) {
    cli();               // Disable interrupts for initialization
    INIT_ALL_SYSTEMS();
    sei();               // Enable interrupts

    uint16_t sequence_length;

    while (1) {
        check_edge();    // Check for button edge transitions

        switch (stage) {
        case START:
            sequence_length = 1;    // Initialize sequence length
            stage = START_SEQUENCE;
            break;

        case START_SEQUENCE:
            state_sequence = seed;  // Reset sequence generator
            calculate_playback_delay();
            play_sequence(sequence_length);
            break;

        case INPUT:
            /* Handle button input state machine */
            switch (button) {
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
            /* Display success pattern and increment sequence */
            update_display(PATTERN_SUCCESS_LEFT, PATTERN_SUCCESS_RIGHT);
            delay();
            display_digit(4);
            sequence_length++;
            stage = START_SEQUENCE;
            break;

        case FAIL:
            /* Display failure pattern and score */
            update_display(PATTERN_FAIL_LEFT, PATTERN_FAIL_RIGHT);
            delay();
            extract_digits(sequence_length, &left_digit, &right_digit);
            update_display(segments[left_digit], segments[right_digit]);
            delay();
            display_digit(4);
            delay();
            uart_puts("Enter name: ");
            
            /* Update sequence seed for next game */
            SEQUENCE(&state_sequence, &step, &result);
            seed = state_sequence;
            stage = START;
            break;

        default:
            stage = START;
            break;
        }
    }
}