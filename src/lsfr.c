#include <avr/io.h>
#include <avr/interrupt.h>

#include "lsfr.h"


// Linear Shift Feedback Register (LSFR) variables:
uint32_t seed = 0x11638494;
uint32_t state_sequence;
uint8_t step;
uint8_t result;

// Function: Linear Shift Feedback Register
// Description: Produces a deterministic pseudo-random number.
// Parameters:
//  - state: Pointer to the current state of the LSFR
//  - step: Pointer to the variable to store the next step
//  - result: Pointer to store the least significant bit of the statevoid
void SEQUENCE(uint32_t *state, uint8_t *step, uint8_t *result)
{
    *result = *state & 1u;        // Use unsigned literal
    *state >>= 1;
    
    if (*result)
    {
        *state ^= LSFR_MASK;      // Use defined constant
    }
    *step = *state & 0x3u;        // Use 0x3u for consistency
}
