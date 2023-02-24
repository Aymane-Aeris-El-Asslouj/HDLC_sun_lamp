#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_
#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations

#define STATE_MACHINE_PERIOD 2 // Period of state machine in ms

// List of possible events
typedef enum {
    EV_ANY,
    EV_STARTED,
    EV_COMMAND,
    EV_BUTTON_PUSHED,
    EV_NONE
} event_t;

// Current event
extern event_t current_event;
event_t fetch_event();

// Single iteration of the state machine system with up to one transition.
void state_machine_run_iteration(event_t event);

#endif
