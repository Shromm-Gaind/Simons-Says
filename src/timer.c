#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "display.h"

#include "uart.h"
#include "input.h"

#include "spi.h"


void calculate_playback_delay(void)
{
    ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;

    adc_ready_flag = ADC0.INTFLAGS & ADC_RESRDY_bm; 

    if (adc_ready_flag)
    {
        
        uint8_t adc_result = ADC0.RESULT;

        ADC0.INTFLAGS = ADC_RESRDY_bm;

        playback_delay = (2000 + (56 * adc_result) - adc_result) >> 3;
    }
}


void delay(void)
{
    playback_timer = 0;
    while (playback_timer < playback_delay)
    {
    } 
}


void half_of_delay(void)
{
    playback_timer = 0;
    while (playback_timer < (playback_delay >> 1))
    {
    } 
}


ISR(TCB0_INT_vect)
{

    playback_timer++;              
    TCB0.INTFLAGS = TCB_CAPT_bm; 
}
