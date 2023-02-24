/*
 * timer_4.c
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */
#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations
#include "timer_4.h"

// Millisecond delay timer based on ticks of timer 4. Accurate for ms_delay >> 1
void timer_4_delay_ms(uint32_t ms_delay){

 uint32_t timer_4_cycles_seen = 0;
 uint8_t timer_4_previous_value;

 // Switch to the timer 4 SFR page
 uint8_t SFRPAGE_save = SFRPAGE;
 SFRPAGE = 0x10;

 timer_4_previous_value = TMR4L;

 // Stay in loop till enough ms cycles are seen
 while(timer_4_cycles_seen != ms_delay){

     // Wait for timer 4 counter to change, then store its new value and add one
     // cycle to the ms counter
     while(timer_4_previous_value == TMR4L);
     timer_4_previous_value = TMR4L;
     timer_4_cycles_seen += 1;
 }

 // Switch back to the original SFR page
 SFRPAGE = SFRPAGE_save;
}
