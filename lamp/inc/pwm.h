/*
 * pwm.h
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_


// ----------------------------------------------------------------------------
// pwm_set Routine
// ----------------------------------------------------------------------------
// Sets the two PWM channels' compare registers to the given 16-bit values.
// NOTE This doesn't enable the outputs or configure the peripheral in any other
// way.  See PWM_0_enter_DefaultMode_from_RESET() for details on that.
// ----------------------------------------------------------------------------
void pwm_set(uint16_t channel0, uint16_t channel1);

void lightness_start_pattern();

void set_lightness(uint8_t lightness);

uint8_t set_lightness_command(uint8_t* command_args_send_info_bytes);

void set_error_LED(bool state);

void update_pwm();

#endif /* INC_PWM_H_ */
