#include <stdint.h>

#include "states_m.h"

// Function prototypes for our inline functions
static inline void check_button_input(void);
static inline void play_sequence(uint16_t sequence_length);
static inline void process_user_input(uint16_t sequence_length);


// External declarations for global variables used across files
uint8_t right_digit;
uint8_t left_digit;
extern buttons button;
extern simon_stage stage;

