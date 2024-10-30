/**
 * @file timer.c
 * @brief Implementation of timing and delay functions
 * 
 * This module handles:
 * - Variable delay calculations based on ADC input
 * - Timer interrupt handling
 * - Precise delay generation for game timing
 */

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "display.h"
#include "uart.h"
#include "input.h"
#include "spi.h"

/**
 * Calculates playback delay based on potentiometer reading
 * 
 * Process:
 * 1. Triggers immediate ADC conversion
 * 2. Checks for ADC result ready
 * 3. Reads ADC result
 * 4. Calculates delay using formula:
 *    delay = (2000 + (56 * adc_result) - adc_result) >> 3
 */
void calculate_playback_delay(void) {
    /* Start ADC conversion */
    ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;

    /* Check if conversion complete */
    adc_ready_flag = ADC0.INTFLAGS & ADC_RESRDY_bm; 

    if (adc_ready_flag) {
        /* Read and process ADC result */
        uint8_t adc_result = ADC0.RESULT;
        ADC0.INTFLAGS = ADC_RESRDY_bm;  // Clear ready flag

        /* Calculate delay with scaling */
        playback_delay = (2000 + (56 * adc_result) - adc_result) >> 3;
    }
}

/**
 * Implements a full-duration delay
 * 
 * Uses playback_timer incremented by timer interrupt
 * to create precise delays for game timing.
 * Blocks until playback_timer reaches playback_delay.
 */
void delay(void) {
    playback_timer = 0;
    while (playback_timer < playback_delay) {
        /* Wait for timer interrupt to increment counter */
    } 
}

/**
 * Implements a half-duration delay
 * 
 * Similar to full delay but uses half the duration
 * (playback_delay >> 1) for shorter timing intervals.
 * Used for creating gaps between sequence elements.
 */
void half_of_delay(void) {
    playback_timer = 0;
    while (playback_timer < (playback_delay >> 1)) {
        /* Wait for timer interrupt to increment counter */
    } 
}

/**
 * Timer Counter B0 Interrupt Service Routine
 * 
 * Triggered by TCB0 match/capture event
 * Increments playback_timer for delay timing
 * Called every 1ms based on TCB0 configuration
 */
ISR(TCB0_INT_vect) {
    playback_timer++;               // Increment timer counter
    TCB0.INTFLAGS = TCB_CAPT_bm;   // Clear interrupt flag
}