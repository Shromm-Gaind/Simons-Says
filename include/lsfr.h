#include <avr/io.h>
#include <avr/interrupt.h>

// Mask defined as specified
#define LSFR_MASK 0xE2024CABu

extern uint32_t seed;
extern uint32_t state_sequence;
extern uint8_t step;
extern uint8_t result;


void SEQUENCE(uint32_t *state, uint8_t *step, uint8_t *result);
