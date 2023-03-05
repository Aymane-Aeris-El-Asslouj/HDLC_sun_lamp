#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_
#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations

#define STATE_MACHINE_PERIOD 2 // Period of state machine in ms

#define EVENT_QUEUE_SIZE 10

// List of possible events
typedef enum {
    EV_ANY,
    EV_STARTED,
    EV_UART_COMMAND,
    EV_UART_DONE,
    EV_ANTENNA_COMMAND,
    EV_ANTENNA_DONE,
    EV_CLOCK_TICK,
    EV_NONE
} event_t;

// Current event
event_t fetch_event();

bool event_queue_is_not_full();
void event_queue_add_event(event_t event);

// Single iteration of the state machine system with up to one transition.
void state_machine_run_iteration(event_t event);

#endif
