/*
 * pwm.c
 *
 *  Created on: Feb 18, 2023
 *      Author: elass
 */

#include <state_machine.h>
#include <SI_EFM8BB52_Register_Enums.h>                  // SFR declarations
#include "pwm.h"
#include "command_handler.h"

void
pwm_set(uint16_t channel0, uint16_t channel1)
{
  // Save the SFR page and switch to SFR page 0x10.  This is needed because the
  // PWM peripheral's SFRs only exist on page 0x10.  For further information,
  // see the EFM8BB52 Reference Manual, Section 3. Special Function Registers.
  uint8_t sfrpage_prev = SFRPAGE;
  SFRPAGE = 0x10;

  // In order to prevent glitches, we use the synchronous update mechanism
  // provided by the buffer registers (PWMCPUDxn) instead of directly writing to
  // the compare registers (PWMCPxn).
  // The SYNCUPD flag is cleared before the writes, preventing the peripheral
  // from reading the buffer registers.  Once the compare values have been
  // written, the SYNCUPD flag is set and the peripheral begins overwriting its
  // compare registers with the contents of these update registers each time it
  // overflows back to 0.
  PWMCFG0 &= ~PWMCFG0_SYNCUPD__FMASK;
  PWMCPUDL0 = channel0 & 0xff;
  PWMCPUDH0 = (channel0 >> 8) & 0xff;
  PWMCPUDL1 = channel1 & 0xff;
  PWMCPUDH1 = (channel1 >> 8) & 0xff;
  PWMCFG0 |= PWMCFG0_SYNCUPD__CH0CH1CH2;

  // Restore the prior SFR page.
  SFRPAGE = sfrpage_prev;
}


/*
Reference equation with normalized (0 to 1) lightness (L) and normalized luminance (Y):

L = Y * 9.033                 Y < 0.008856
L = Y^(1/3) * 1.16-0.16       else

Inverted, yields:

Y = L/9.033                   L < 0.08
Y = ((L+0.16)/1.16)^3         else

For 8-bit L and and 16-bit Y:

Y = 28 * L                      L < 20
Y = (L >> 3 + 6 )^3                 else
*/
uint16_t code pwm_from_lightness[256] = {1,28,57,85,113,142,170,198,227,255,283,312,340,368,397,425,453,482,510,538,567,595,
    625,655,686,718,751,785,821,857,894,933,972,1012,1054,1097,1141,1186,1232,1279,1328,1378,1429,1481,1535,1590,1646,1703,
    1762,1822,1883,1946,2010,2076,2143,2211,2281,2353,2425,2500,2575,2653,2731,2812,2894,2977,3062,3149,3237,3327,3419,3512,
    3607,3704,3802,3902,4004,4108,4213,4320,4429,4540,4652,4767,4883,5001,5121,5243,5367,5493,5621,5751,5882,6016,6152,6290,
    6429,6571,6715,6861,7009,7160,7312,7466,7623,7782,7943,8106,8272,8439,8609,8782,8956,9133,9312,9493,9677,9863,10052,10243,
    10436,10632,10830,11031,11234,11439,11647,11858,12071,12286,12505,12725,12949,13175,13403,13634,13868,14104,14343,14585,
    14830,15077,15327,15580,15835,16093,16354,16618,16885,17154,17427,17702,17980,18261,18545,18832,19121,19414,19710,20009,
    20310,20615,20923,21233,21547,21864,22184,22507,22834,23163,23496,23831,24170,24512,24858,25206,25558,25913,26271,26633,
    26998,27366,27737,28112,28491,28872,29257,29646,30038,30433,30832,31234,31640,32049,32461,32878,33297,33721,34148,34578,
    35012,35450,35891,36336,36785,37237,37693,38153,38617,39084,39555,40030,40508,40990,41477,41966,42460,42958,43459,43965,
    44474,44987,45505,46026,46551,47080,47613,48150,48691,49236,49785,50339,50896,51457,52023,52593,53166,53744,54327,54913,
    55503,56098,56697,57300,57908,58520,59136,59756,60381,61010,61643,62281,62923,63570,64221,64876};
uint8_t dummy = 1;
uint8_t current_lightness = 0;
void button(){
  if ((P3 & P3_B0__BMASK) != P3_B0__BMASK){
    if (dummy == 0){
        dummy = 1;
        current_lightness = 255;
    }
    else{
        dummy = 0;
        current_lightness = 0;
    }
    update_pwm();
  }

}

void increase_lightness(){

    if(current_lightness ==254){
        current_event = EV_STARTED;
        return;
    }
    current_lightness += 2;
}


void set_lightness(uint8_t lightness){
  current_lightness = lightness;
  update_pwm();
}

uint8_t set_lightness_command(uint8_t* args){
  current_lightness = args[0];
  update_pwm();
  return SUCCESSFUL;
}

void update_pwm(){
  uint16_t pwm;
  pwm = pwm_from_lightness[current_lightness]/10;
  pwm_set(pwm + 150, pwm + 150);
}
