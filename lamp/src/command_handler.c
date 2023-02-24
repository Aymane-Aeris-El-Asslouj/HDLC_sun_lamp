/*
 * command_handler.c
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */
#include "command_handler.h"
#include "pwm.h"
#include "hdlc_l.h"
#include <SI_EFM8BB52_Register_Enums.h>

#define NUMBER_OF_COMMANDS 2


// List of commands
typedef enum {
    CM_NONE,
    CM_LIGHTNESS
} command_t;

// Command table item with name, number of arguments, and handler
typedef struct {
  command_t command;
  uint8_t command_arg_count;
  uint8_t (*command_handler)(uint8_t*);
} command_table_element;

uint8_t empty_command(uint8_t* dummy){return SUCCESSFUL;}



command_table_element command_table[NUMBER_OF_COMMANDS] = {
    // Command name,  number of arguments, command_handler
    {CM_NONE,         0,                   &empty_command},
    {CM_LIGHTNESS,   1,                   &set_lightness_command}
};

void command_handler(){

  // Command from computer
  uint8_t received_info_bytes[10];
  // received_info_bytes's size
  uint8_t received_info_bytes_size;

  // Response to command to computer
  uint8_t send_info_bytes[10];
  // Response's number of bytes
  uint8_t send_info_bytes_size;

  // Command opcode
  uint8_t command;

  // Make a copy of in_packet's info field into received_info_bytes
  hdlc_l_load_received_info_bytes(received_info_bytes, &received_info_bytes_size);

  // Error: No command
  if (received_info_bytes_size == 0){
      send_info_bytes[0] = NO_COMMAND;
      send_info_bytes_size = 1;
  }
  else{
      command = received_info_bytes[0];

      // Error: Unknown command
      if (command >= NUMBER_OF_COMMANDS){
          send_info_bytes[0] = UNKNOWN_COMMAND;
          send_info_bytes_size = 1;
      }

      // Error: Incorrect number of arguments for command
      else if (command_table[command].command_arg_count != received_info_bytes_size - 1){
          send_info_bytes[0] = INCORRECT_NUMBER_OF_ARGUMENTS;
          send_info_bytes_size = 1;
      }

       // Get command handler of the command and pass to it the arguments
      // which are the next bytes in the packet
      else{
          send_info_bytes[0] = (command_table[command].command_handler)(&(received_info_bytes[1]));
          send_info_bytes_size = 1;
      }
  }

  // Send packet
  hdlc_l_send_info_packet(send_info_bytes, send_info_bytes_size);

  // Re-enable UART0 so transmission starts
  IE |= IE_ES0__BMASK; // IE is on all SFR pages, so no need to change the SFR page
}
