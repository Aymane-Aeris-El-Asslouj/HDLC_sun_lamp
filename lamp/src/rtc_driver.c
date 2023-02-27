
#include <SI_EFM8BB52_Register_Enums.h>
#include "smb_0.h"
#include "rtc_driver.h"



static SI_SEGMENT_VARIABLE(rtc_send_buffer[RTC_SEND_BUFFER_SIZE], uint8_t, SI_SEG_XDATA);
static SI_SEGMENT_VARIABLE(rtc_receive_buffer[RTC_RECEIVE_BUFFER_SIZE], uint8_t, SI_SEG_XDATA);


// datetime struct
static xdata rtc_time_data rtc_data;

// Flag to indicate end of RTC transfer
bool transfer_complete = false;

// Communication with RTC done
void SMB0_transferCompleteCb()
{
  transfer_complete = true;
}

// Error during communication with RTC
void SMB0_errorCb(SMB0_TransferError_t error){
  // Disable state machine and put lamp in red (to be implemented later)
}

// For slave transmission (irrelevant as chip is master-only)
void SMB0_commandReceivedCb(){

}

// Send clock pulses to the RTC module to reset its i2c interface
void rtc_reset(){
  uint8_t i;
  // Disable SMB0's access to the pins
  XBR2 &= ~XBR2_XBARE__BMASK; // disable crossbar
  SCL_PnSKIP |= SCL_PnSKIP_Bm__BMASK; // skip SCL pin
  SDA_PnSKIP |= SDA_PnSKIP_Bm__BMASK; // skip SDA pin
  SCL_PnMDIN |= SCL_PnMDIN_Bm__BMASK; // put SCL pin into digital mode
  SDA_PnMDIN |= SDA_PnMDIN_Bm__BMASK; // put SDA pin into digital mode
  XBR0 &= ~XBR0_SMB0E__BMASK; // Disable SMB0 crossbar access
  XBR2 |= XBR2_XBARE__BMASK; // enable crossbar

  // Send clock pulses till the SDA line is allowed to go up
  while(!SDA_PnBm){
      SCL_PnBm = 0;
      for(i=0; i < 0xFF; i++);
      SCL_PnBm = 1;
      for(i=0; i < 0xFF; i++);
  }
  // Reenable SMB0's access to the pins
  XBR2 &= ~XBR2_XBARE__BMASK; // disable crossbar
  SCL_PnSKIP &= ~SCL_PnSKIP_Bm__BMASK; // unskip SCL pin
  SDA_PnSKIP &= ~SDA_PnSKIP_Bm__BMASK; // unskip SDA pin
  XBR0 |= XBR0_SMB0E__BMASK; // enable SMB0 crossbar access
  XBR2 |= XBR2_XBARE__BMASK; // enable crossbar
}



// Convert 8-bit binary numbers from bcd to hex format
static uint8_t BCD_to_HEX(uint8_t number)
{
  return (number / 0x10) *10  + (number % 0x10);
}

// Convert 8-bit binary numbers from bcd to hex format
static uint8_t HEX_TO_BCD(uint8_t number)
{
  return (number/10)*0x10 + (number % 10);
}

void upload_time_to_RTC(){
  uint8_t i;

  // Clear send buffer
  for(i = 0; i < RTC_SEND_BUFFER_SIZE; i++)
      rtc_send_buffer[i] = 0;


  // Write date-time to RTC send buffer
  rtc_send_buffer[0] = 0x00; // Target starting register address
  rtc_send_buffer[RTC_SECONDS_REG+1] = HEX_TO_BCD(rtc_data.seconds);
  rtc_send_buffer[RTC_MINUTES_REG+1] = HEX_TO_BCD(rtc_data.minutes);
  rtc_send_buffer[RTC_HOURS_REG+1] = HEX_TO_BCD(rtc_data.hours);
  rtc_send_buffer[RTC_DATE_REG+1] = HEX_TO_BCD(rtc_data.date);
  rtc_send_buffer[RTC_MONTH_REG+1] = HEX_TO_BCD(rtc_data.month);
  rtc_send_buffer[RTC_YEAR_REG+1] = HEX_TO_BCD(rtc_data.year);
  // Alarm and day of week registers are set to 0x00 as they are irrelevant
  // Control register is set to 0x00 for 1Hz cycle on SQW pin
  // Status register is set to 0x00 to clear OSF bit
  // Aging offset register is set to 0x00 to have normal operation

  // Write send buffer to RTC module
  transfer_complete = false;
  SMB0_transfer(RTC_DEVICE_ADDRESS << 1, rtc_send_buffer, NULL, RTC_SEND_BUFFER_SIZE, 0);

  // Wait for transfer to end
  while(!transfer_complete);
}


void read_time_from_RTC(){
  uint8_t pm;


  // Read data from RTC module
  transfer_complete = false;
  rtc_send_buffer[0] = 0x00;
  SMB0_transfer(RTC_DEVICE_ADDRESS << 1, rtc_send_buffer, rtc_receive_buffer, 1, RTC_RECEIVE_BUFFER_SIZE);

  // Wait for transfer to end
  while(!transfer_complete);

  rtc_data.seconds = BCD_to_HEX(rtc_receive_buffer[RTC_SECONDS_REG] & RTC_SECONDS_MASK);
  rtc_data.minutes = BCD_to_HEX(rtc_receive_buffer[RTC_MINUTES_REG] & RTC_MINUTES_MASK);

  rtc_data.hours = rtc_receive_buffer[RTC_HOURS_REG] & 0x7F;

  // 12 hours mode
  if(rtc_data.hours & RTC_HOURS_REG_MODE_BIT__BMASK){
      pm = rtc_data.hours & RTC_HOURS_REG_PM_BIT__BMASK;
      rtc_data.hours = BCD_to_HEX(rtc_data.hours & RTC_HOURS12_MASK);

      if(rtc_data.hours == 12)
        {
          if(!pm)
              rtc_data.hours = 0; // 12AM is 00
        }

      else if(pm)
          rtc_data.hours += 12;
  }
  // 24 hour mode
  else
      rtc_data.hours = BCD_to_HEX(rtc_data.hours & RTC_HOURS24_MASK);

  rtc_data.date = BCD_to_HEX(rtc_receive_buffer[RTC_DATE_REG] & RTC_DATE_MASK);
  rtc_data.month = BCD_to_HEX(rtc_receive_buffer[RTC_MONTH_REG] & RTC_MONTH_MASK);
  rtc_data.year = BCD_to_HEX(rtc_receive_buffer[RTC_YEAR_REG]);
  rtc_data.invalid = rtc_receive_buffer[RTC_STATUS_REG] >> RTC_STATUS_REG_OSF__SHIFT;

  // If time is valid, enable SQW INT0 interrupt to keep up with time
  IE_EX0 = is_time_valid();
}

uint8_t code days_per_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

static uint8_t days_of_month()
{
  // If February and leap year, add one day to standard days of month
  if((rtc_data.month == 2) && (rtc_data.year % 4 == 0))
    return days_per_month[rtc_data.month-1]+1;

  return days_per_month[rtc_data.month-1];
}


bool is_time_valid(){
  return !rtc_data.invalid;
}

// SQW 1 Hz interrupt from RTC module to update datetime
SI_INTERRUPT (INT0_ISR, INT0_IRQn)
  {
    // Blink led (for visual debugging purposes, will be removed)
    P1_B4 = !P1_B4;

    // Update datetime (trickle down modification)

    if(++rtc_data.seconds != 60)
      return;

    rtc_data.seconds = 0;
    if(++rtc_data.minutes != 60)
      return;

    rtc_data.minutes = 0;
    if(++rtc_data.hours != 24)
      return;

    rtc_data.hours = 0;
    if(++rtc_data.date != days_of_month())
      return;

    rtc_data.date = 1;
    if(++rtc_data.month != 13)
      return;

    rtc_data.month = 1;
    rtc_data.year++;
  }
