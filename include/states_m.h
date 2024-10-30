#ifndef STATES_M_H
#define STATES_M_H

// Enum for gameplay stages
typedef enum {
    START,
    START_SEQUENCE,
    INPUT,
    SUCCESS,
    FAIL
} simon_stage;

// Enum for button states
typedef enum {
    COMPLETE,
    S1,
    S2,
    S3,
    S4
} buttons;

// Declare external variables for button and gameplay stage
extern buttons button;
extern simon_stage stage;

#endif // STATES_M_H
