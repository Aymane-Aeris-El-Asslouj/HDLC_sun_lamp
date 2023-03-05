/*
 * command_handler.h
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */

#ifndef INC_COMMAND_HANDLER_H_
#define INC_COMMAND_HANDLER_H_
#include <stdint.h>
#include "pwm.h"

void uart_command_handler();

void antenna_command_handler();

void command_handler();


// Response fields
typedef enum {
    SUCCESSFUL,
    INVALID_ARGUMENTS,
    UNSUCCESSFUL,
    NO_COMMAND,
    UNKNOWN_COMMAND,
    INCORRECT_NUMBER_OF_ARGUMENTS
} command_response;


//


#endif /* INC_COMMAND_HANDLER_H_ */
