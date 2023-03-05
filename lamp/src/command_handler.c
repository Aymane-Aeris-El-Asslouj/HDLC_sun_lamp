/*
 * command_handler.c
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */
#include <SI_EFM8BB52_Register_Enums.h>
#include "command_handler.h"
#include "pwm.h"
#include "hdlc_l.h"
#include "state_machine.h"
#include "rtc_driver.h"

#define NUMBER_OF_COMMANDS 6


// List of commands
typedef enum {
    CM_NONE,
    CM_LIGHTNESS,
    CM_DISCONNECT,
    CM_GET_DATETIME,
    CM_SET_DATETIME,
    CM_GET_SUNRISETIME
} command_t;

// Command table item with name, number of arguments, and handler
typedef struct {
  command_t command;
  uint8_t command_arg_count;
  uint8_t (*command_handler)(uint8_t* info_bytes);
  uint8_t end_of_communication;
} command_table_element;


uint8_t empty_command(uint8_t* command_args_send_info_bytes)
{
  command_args_send_info_bytes[0] = SUCCESSFUL;
  return 1;
}

command_table_element xdata command_table[NUMBER_OF_COMMANDS] = {
    // Command name,  number of arguments, command_handler
    {CM_NONE,         0,                   &empty_command,        true},
    {CM_LIGHTNESS,   1,                   &set_lightness_command, false},
    {CM_DISCONNECT,         0,                   &empty_command, true},
    {CM_GET_DATETIME,         0,                   &get_datetime_command, true},
    {CM_SET_DATETIME,         6,                   &set_datetime_command, true},
    {CM_GET_SUNRISETIME,         0,                   &get_sunrise_time_command, true}
};


void uart_command_handler(){
  command_handler();
}


void antenna_command_handler(){
  command_handler();
}

// Command from computer
uint8_t xdata info_bytes[10];
void command_handler(){

  // received_info_bytes's size
  uint8_t info_bytes_size;

  // Command opcode
  uint8_t command;

  // Whether communication is going to end after this command
  bool end_of_communication = true;

  // Make a copy of in_packet's info field into received_info_bytes
  hdlc_l_load_received_info_bytes(info_bytes, &info_bytes_size);

  // Error: No command
  if (info_bytes_size == 0){
      info_bytes[0] = NO_COMMAND;
      info_bytes_size = 1;
  }
  else{
      command = info_bytes[0];

      // Error: Unknown command
      if (command >= NUMBER_OF_COMMANDS){
          info_bytes[0] = UNKNOWN_COMMAND;
          info_bytes_size = 1;
      }

      // Error: Incorrect number of arguments for command
      else if (command_table[command].command_arg_count != info_bytes_size - 1){
          info_bytes[0] = INCORRECT_NUMBER_OF_ARGUMENTS;
          info_bytes_size = 1;
      }

       // Get command handler of the command and pass to it the arguments
      // which are the next bytes in the packet
      else{
          end_of_communication = command_table[command].end_of_communication;
          info_bytes_size = command_table[command].command_handler(&info_bytes[0]);
      }
  }

  if(end_of_communication)
    event_queue_add_event(EV_UART_DONE);


  // Send packet
  hdlc_l_send_info_packet(info_bytes, info_bytes_size);

  // Re-enable UART0 so transmission starts
  IE |= IE_ES0__BMASK; // IE is on all SFR pages, so no need to change the SFR page
}
