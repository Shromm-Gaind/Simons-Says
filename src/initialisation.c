/**
 * @file initialisation.c
 * @brief Initialization routines for microcontroller peripherals
 * 
 * This module contains initialization functions for:
 * - Timer configuration for interrupt generation
 * - ADC setup for potentiometer reading
 * - Button input configuration
 * - SPI interface for display control
 * - PWM setup for buzzer control
 * - UART configuration for serial communication
 */

#include "initialisation.h"
#include <avr/io.h>

/**
 * Initializes system timers for interrupt generation
 * 
 * Configures two TCB timers:
 * TCB1: 5ms interval timer
 * - CCMP = 16667 for 5ms period at 3.3MHz
 * - Configured in interrupt mode
 * 
 * TCB0: 1ms interval timer
 * - CCMP = 3333 for 1ms period at 3.3MHz
 * - Used for system timing
 */
void timers_init(void) {
    /* Configure TCB1 for 5ms intervals */
    TCB1.CCMP = 16667;               // Set compare match value (5ms @ 3.3MHz)
    TCB1.CTRLB = TCB_CNTMODE_INT_gc; // Configure for interrupt mode
    TCB1.INTCTRL = TCB_CAPT_bm;      // Enable capture interrupt
    TCB1.CTRLA = TCB_ENABLE_bm;      // Start the timer

    /* Configure TCB0 for 1ms intervals */
    TCB0.CNT = 0;                    // Initialize counter to 0
    TCB0.CCMP = 3333;                // Set compare match value (1ms @ 3.3MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;      // Enable capture interrupt
    TCB0.CTRLA = TCB_ENABLE_bm;      // Start the timer
}

/**
 * Initializes ADC for potentiometer reading
 * 
 * Configuration details:
 * - 8-bit resolution, single-ended mode
 * - VDD reference voltage
 * - Input channel: AIN2 (potentiometer)
 * - Left-adjusted result for 8-bit readings
 * - DIV2 prescaler for optimal tone setting
 */
void adc_init(void) {
    ADC0.CTRLA = ADC_ENABLE_bm;                     // Enable ADC
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;                 // Set prescaler to 2 for accurate tone generation
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) |          // 4 clock cycles timebase
                 ADC_REFSEL_VDD_gc;                 // Use VDD as reference
    ADC0.CTRLE = 64;                               // Set sample duration
    ADC0.CTRLF = ADC_LEFTADJ_bm;                   // Left adjust for 8-bit reads
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;             // Select potentiometer input
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc;        // 8-bit resolution, single-ended
}

/**
 * Initializes push buttons with internal pull-up resistors
 * 
 * Configures buttons S1-S4 on PORTA pins 4-7:
 * - S1: PA4 (Button 1)
 * - S2: PA5 (Button 2)
 * - S3: PA6 (Button 3)
 * - S4: PA7 (Button 4)
 */
void button_init(void) {
    /* Enable pull-up resistors for all buttons */
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm; // S1
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm; // S2
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // S3
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // S4
}

/**
 * Initializes SPI interface for display communication
 * 
 * Configuration:
 * - DISP LATCH: PA1 (output)
 * - SPI CLK: PC0 (output)
 * - SPI MOSI: PC2 (output)
 * - Uses alternate pin configuration
 * - Master mode with interrupt enable
 */
void spi_init(void) {
    /* Configure display latch pin */
    PORTA.OUTCLR = PIN1_bm;          // Initialize latch low
    PORTA.DIRSET = PIN1_bm;          // Set as output

    /* Configure SPI pins */
    PORTC.DIRSET = PIN0_bm | PIN2_bm; // Set CLK and MOSI as outputs
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc; // Use alternate pin configuration

    /* Configure SPI peripheral */
    SPI0.CTRLB = SPI_SSD_bm;         // Disable slave select
    SPI0.INTCTRL = SPI_IE_bm;        // Enable SPI interrupts
    SPI0.CTRLA = SPI_MASTER_bm |     // Configure as master
                 SPI_ENABLE_bm;       // Enable SPI
}

/**
 * Initializes PWM for buzzer control
 * 
 * Configuration:
 * - Uses TCA0 in single-slope PWM mode
 * - Output on PB0 (buzzer pin)
 * - DIV2 prescaler for accurate frequency generation
 * - Initially configured with output disabled
 */
void pwm_init(void) {
    PORTB.DIRSET = PIN0_bm;          // Set buzzer pin as output

    /* Configure Timer/Counter A */
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc; // Set prescaler to 2
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | // Single-slope PWM mode
                        TCA_SINGLE_CMP0EN_bm;               // Enable compare output

    /* Initialize with buzzer off */
    TCA0.SINGLE.PER = 0;
    TCA0.SINGLE.CMP0 = 0;

    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // Enable timer
}

/**
 * Initializes UART for serial communication
 * 
 * Configuration:
 * - Baud rate: 9600 bps (BAUD = 1389 for 3.3MHz clock)
 * - Uses PB2 for TXD
 * - Enables both transmitter and receiver
 * - Enables receive complete interrupt
 */
void uart_init(void) {
    PORTB.DIRSET = PIN2_bm;          // Set TXD pin as output
    USART0.BAUD = 1389;              // Set baud rate to 9600 @ 3.3MHz
    USART0.CTRLA = USART_RXCIE_bm;   // Enable receive interrupt
    USART0.CTRLB = USART_RXEN_bm |   // Enable receiver
                   USART_TXEN_bm;     // Enable transmitter
}