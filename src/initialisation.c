#include "initialisation.h"
#include <avr/io.h>

// Function: button_init
// Description: Initializes the buttons by enabling pull-up resistors.
void button_init(void)
{
    // Enable pull-up resistors for push buttons S1 to S4
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm; // Button S1
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm; // Button S2
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // Button S3
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // Button S4
}

// Function: pwm_init
// Description: Initializes the PWM for the buzzer.
void pwm_init(void)
{
    // Set buzzer pin as output.
    PORTB.DIRSET = PIN0_bm;

    // Set pre-scaler as 2 to avoid truncation when setting tones.
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc;

    // Configure single slope PWM mode and enable output to the buzzer.
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;

    // Initialize the buzzer to be off.
    TCA0.SINGLE.PER = 0;
    TCA0.SINGLE.CMP0 = 5051;

    // Enable the timer.
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

// Function: spi_init
// Description: Initializes the SPI interface.
void spi_init(void)
{
    // Configure DISP LATCH (PA1) as output and clear it.
    PORTA.OUTCLR = PIN1_bm;
    PORTA.DIRSET = PIN1_bm;

    // Configure SPI CLK (PC0) and SPI MOSI (PC2) as outputs.
    PORTC.DIRSET = PIN0_bm | PIN2_bm;

    // Select alternate pin configuration for SPI.
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;

    SPI0.CTRLB = SPI_SSD_bm;
    SPI0.INTCTRL = SPI_IE_bm; // Enable interrupts.

    // Enable SPI as master.
    SPI0.CTRLA = SPI_MASTER_bm | SPI_ENABLE_bm;
}

// Function: adc_init
// Description: Initializes the ADC for potentiometer readings.
void adc_init(void)
{
    // Enable the ADC.
    ADC0.CTRLA = ADC_ENABLE_bm;

    // Set clock prescaler to 2 to avoid truncation when setting tones.
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;

    // Configure ADC with 4 CLK_PER cycles, select VDD as reference.
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc;

    // Set sample duration to 64.
    ADC0.CTRLE = 64;

    // Left adjust result (not free running).
    ADC0.CTRLF = ADC_LEFTADJ_bm;

    // Select AIN2 (potentiometer R1) as input.
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;

    // Configure for 8-bit resolution, single-ended mode.
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc;
}

// Function: timers_init
// Description: Initializes two timers to be used for triggering interrupts.
void timers_init(void)
{
    // Timer for 5ms intervals.
    TCB1.CCMP = 16667;               // Set compare match value
    TCB1.CTRLB = TCB_CNTMODE_INT_gc; // Set count mode to interrupt
    TCB1.INTCTRL = TCB_CAPT_bm;      // Enable interrupt
    TCB1.CTRLA = TCB_ENABLE_bm;      // Enable timer

    // Timer for 1ms intervals.
    TCB0.CNT = 0;               // Reset counter
    TCB0.CCMP = 3333;           // Set compare match value
    TCB0.INTCTRL = TCB_CAPT_bm; // Enable interrupt
    TCB0.CTRLA = TCB_ENABLE_bm; // Enable timer
}

// Function: uart_init
// Description: Initializes the UART for serial communication.
void uart_init(void)
{
    // Set PB2 as output for USART0 TXD.
    PORTB.DIRSET = PIN2_bm;

    // Set baud rate to 9600 at 3.3 MHz clock.
    USART0.BAUD = 1389;

    // Enable receive complete interrupt.
    USART0.CTRLA = USART_RXCIE_bm;

    // Enable transmitter and receiver.
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}