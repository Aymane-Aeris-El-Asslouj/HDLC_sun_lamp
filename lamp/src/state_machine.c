#include "state_machine.h"
#include "pwm.h"
#include "rtc_driver.h"
#include "command_handler.h"

#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations

// List of state of the state machine
typedef enum{
  ST_INIT, ST_START, ST_IDLE, ST_UART, ST_ANTENNA,
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
stateTransMatrixRow_t xdata stateTransMatrix[] = {
    { ST_INIT,     EV_ANY,               ST_START,     &empty},
    { ST_START,    EV_STARTED,           ST_IDLE,      &empty},
    { ST_START,    EV_ANY,               ST_START,     &lightness_start_pattern},
    { ST_IDLE,     EV_UART_COMMAND,           ST_UART,      &uart_command_handler},
    { ST_UART,     EV_UART_COMMAND,           ST_UART,      &uart_command_handler},
    { ST_UART,     EV_UART_DONE,           ST_IDLE,      &empty},
    { ST_IDLE,     EV_ANTENNA_COMMAND,        ST_ANTENNA,      &antenna_command_handler},
    { ST_ANTENNA,     EV_ANTENNA_COMMAND,        ST_ANTENNA,      &antenna_command_handler},
    { ST_ANTENNA,     EV_ANTENNA_DONE,           ST_IDLE,      &empty},
    { ST_IDLE,     EV_CLOCK_TICK,           ST_IDLE,      &clock_tick},
};

// Single iteration of the state machine system with up to one transition.
state_t xdata current_state = ST_INIT;
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


uint8_t xdata event_queue_size = 0;
event_t xdata event_queue[EVENT_QUEUE_SIZE];

bool event_queue_is_not_full(){
  return event_queue_size != EVENT_QUEUE_SIZE;
}

void event_queue_add_event(event_t event){
  if(event_queue_is_not_full()){
      event_queue[event_queue_size] = event;
      event_queue_size++;
  }
}


// Pop event from queue
event_t fetch_event(){
  int i;
  event_t fetched_event;
    if (event_queue_size > 0)
      {
        fetched_event = event_queue[0];

        for (i = 0; i < event_queue_size-1; i++)
          event_queue[i] = event_queue[i+1];

        event_queue_size--;

        return fetched_event;
      }
    else
      return EV_NONE;
}
