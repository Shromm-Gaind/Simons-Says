void button_init(void);
void pwm_init(void);
void spi_init(void);
void timers_init(void);
void adc_init(void);
void uart_init(void);

#define INIT_ALL_SYSTEMS() \
    do {                   \
        adc_init();        \
        button_init();     \
        spi_init();        \
        pwm_init();        \
        timers_init();     \
        uart_init();       \
    } while (0)
