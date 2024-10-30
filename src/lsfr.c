/**
 * @file lsfr.c
 * @brief Implementation of Linear Feedback Shift Register for random sequence generation
 * 
 * This module implements a Linear Feedback Shift Register (LFSR) to generate
 * pseudo-random sequences for game patterns. The LFSR uses a 32-bit state
 * with specific feedback taps to ensure maximum sequence length.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lsfr.h"

/**
 * LFSR state variables:
 * 
 * seed: Initial value for the LFSR (0x11638494)
 * state_sequence: Current state of the LFSR
 * step: Current 2-bit output value (0-3)
 * result: Latest single bit output from LFSR
 * 
 * The seed value is chosen to prevent the LFSR from entering a zero state,
 * ensuring proper sequence generation.
 */
uint32_t seed = 0x11638494;           // Initial seed value
uint32_t state_sequence;              // Current LFSR state
uint8_t step;                         // Current sequence step (0-3)
uint8_t result;                       // Latest LFSR output bit

/**
 * Generates the next value in the LFSR sequence
 * 
 * @param state Pointer to the current LFSR state
 * @param step Pointer to store the 2-bit output (0-3)
 * @param result Pointer to store the single bit result
 * 
 * Algorithm:
 * 1. Extract least significant bit as result
 * 2. Right shift the state by 1
 * 3. If result is 1, XOR the state with LSFR_MASK
 * 4. Extract 2 least significant bits as step value
 * 
 * The LSFR_MASK is defined to create maximum-length sequences,
 * typically using polynomials like x^32 + x^31 + x^29 + x + 1
 */
void SEQUENCE(uint32_t *state, uint8_t *step, uint8_t *result) {
    *result = *state & 1u;            // Extract LSB
    *state >>= 1;                     // Shift right by 1
    
    if (*result) {
        *state ^= LSFR_MASK;          // Apply feedback polynomial
    }
    *step = *state & 0x3u;            // Extract 2 LSBs for step value
}