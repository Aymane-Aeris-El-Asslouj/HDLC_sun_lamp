/*
 * timer_4.h
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */

#ifndef INC_TIMER_4_H_
#define INC_TIMER_4_H_

// Millisecond delay timer based on ticks of timer 4. Accurate for ms_delay >> 1
void timer_4_delay_ms(uint32_t ms_delay);

#endif /* INC_TIMER_4_H_ */
