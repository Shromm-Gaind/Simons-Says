#include <avr/io.h>
#include <avr/interrupt.h>


// Linear Shift Feedback Register (LSFR) variables:
uint32_t mask = 0xE2024CAB;
uint32_t re_init_state = 0x11638494;
uint32_t state_lsfr;
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
    *result = *state & 1;
    *state >>= 1;

    if (*result)
    {
        *state ^= mask;
    }
    *step = (uint8_t)(*state & 0b11);
}

