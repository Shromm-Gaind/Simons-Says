#include <stdint.h>

extern volatile uint8_t left_byte;
extern volatile uint8_t right_byte;

// Segment display hexadecimal values for digits
extern const uint8_t segments[11];


#define DISP_SEG_B 0b01101111
#define DISP_SEG_C 0b01111011
#define DISP_SEG_E 0b01111110
#define DISP_SEG_F 0b00111111

#define DISP_BAR_LEFT (DISP_SEG_E & DISP_SEG_F)
#define DISP_BAR_RIGHT (DISP_SEG_B & DISP_SEG_C)

#define DISP_OFF 0b01111111

#define DISP_LHS (1 << 7)

#define PATTERN_SUCCESS_LEFT  0x00  // Binary: 00000000
#define PATTERN_SUCCESS_RIGHT 0x00  // Binary: 00000000

#define PATTERN_FAIL_LEFT    0x77   // Binary: 01110111
#define PATTERN_FAIL_RIGHT   0x77   // Binary: 01110111

typedef enum {
    SIDE_LEFT = 0,
    SIDE_RIGHT = 1
} display_side_t;


void update_display(const uint8_t left, const uint8_t right);
void display_digit(uint8_t step);

void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit);