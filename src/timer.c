#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h> // Include for itoa

#include "display.h"

#include "uart.h"
#include "input.h"

#include "spi.h"

// Function: prepare_delay
// Description: Prepares the playback delay based on ADC result.
void calculate_playback_delay(void)
{
    // Start ADC conversion
    ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;

    adc_ready_flag = ADC0.INTFLAGS & ADC_RESRDY_bm; // Check if ADC conversion is complete

    if (adc_ready_flag)
    {
        // Read the result
        uint8_t adc_result = ADC0.RESULT;

        // Clear the Result Ready flag
        ADC0.INTFLAGS = ADC_RESRDY_bm;

        playback_delay = (2000 + (56 * adc_result) - adc_result) >> 3;
    }
}

// Function: playback_delay
// Description: Delays for the duration of playback_delay_ms.
void delay(void)
{
    playback_timer = 0;
    while (playback_timer < playback_delay)
    {
    } // Do nothing for the playback delay.
}

// Function: half_playback_delay
// Description: Delays for half the duration of playback_delay_ms.
void half_of_delay(void)
{
    playback_timer = 0;
    while (playback_timer < (playback_delay >> 1))
    {
    } // Do nothing for half the playback delay.
}

// ISR: TCB0_INT_vect
// Description: Timer interrupt service routine triggered every 1ms.
ISR(TCB0_INT_vect)
{

    playback_timer++;              // Increment elapsed time
    TCB0.INTFLAGS = TCB_CAPT_bm; // Clear interrupt flag
}
