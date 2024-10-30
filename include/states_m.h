#ifndef STATES_M_H
#define STATES_M_H

// Enum for gameplay stages
typedef enum {
    INIT,
    SIMON,
    PLAYER,
    SUCCESS,
    FAIL
} gameplay_stages;

// Enum for button states
typedef enum {
    WAIT,
    BUTTON1,
    BUTTON2,
    BUTTON3,
    BUTTON4
} buttons;

// Declare external variables for button and gameplay stage
extern buttons button;
extern gameplay_stages gameplay_stage;

#endif // STATES_M_
