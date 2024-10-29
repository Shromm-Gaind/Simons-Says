#include <avr/io.h>
#include <avr/interrupt.h>

// Linear Shift Feedback Register (LSFR) constants and variables
extern uint32_t mask;
extern uint32_t seed;
extern uint32_t state_sequence;
extern uint8_t step;
extern uint8_t result;

// Function: LSFR
// Description: Produces a deterministic pseudo-random number.
// Parameters:
//  - state: Pointer to the current state of the LSFR
//  - step: Pointer to the variable to store the next step
//  - result: Pointer to store the least significant bit of the state
void SEQUENCE(uint32_t *state, uint8_t *step, uint8_t *result);
