#include "state_machine.h"
#include "pwm.h"
#include "command_handler.h"

#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations

// List of state of the state machine
typedef enum{
  ST_INIT, ST_START, ST_IDLE, ST_DEBOUNCE, ST_BUTTON,
} state_t;

// Meely state machine transition table element. Events trigger state transition
// accompanied by an event handler function.
typedef struct {
    state_t current_state;
    event_t event;
    state_t next_state;
    void (*event_handler)(void);
} stateTransMatrixRow_t;

// Meely state machine transition table. A single transition happens per cycle, so
// event transitions are ordered by priority up to down. EV_ANY is the wildcard
// event which always triggers, so it should be at the lowest priority.
void empty(){}
stateTransMatrixRow_t stateTransMatrix[] = {
    { ST_INIT,     EV_ANY,               ST_START,     &empty},
    { ST_START,    EV_STARTED,           ST_IDLE,      &empty},
    { ST_START,    EV_ANY,               ST_START,     &increase_lightness},
    { ST_IDLE,     EV_COMMAND,           ST_IDLE,      &command_handler},
    { ST_IDLE,     EV_BUTTON_PUSHED,     ST_DEBOUNCE,  &empty},
    { ST_DEBOUNCE, EV_ANY,               ST_IDLE,      &button}
};

// Single iteration of the state machine system with up to one transition.
state_t current_state;
void state_machine_run_iteration(event_t event) {
  int i;
  // Iterate through the state transition table
  for(i = 0; i < sizeof(stateTransMatrix)/sizeof(stateTransMatrix[0]); i++) {
   // Look for a transition with the current state and event
    if(stateTransMatrix[i].current_state == current_state) {
            if((stateTransMatrix[i].event == event) || (stateTransMatrix[i].event == EV_ANY)) {

                // Transition to the next state
                current_state =  stateTransMatrix[i].next_state;

                // Call the function associated with transition
                stateTransMatrix[i].event_handler();

                // break so no more transitions happen till the next cycle
                break;
            }
        }
    }
}

// Fetch current event and remove it (consumes current event)
event_t current_event = EV_NONE;
event_t fetch_event(){
    event_t event = current_event;
    current_event = EV_NONE;
    return event;
}
