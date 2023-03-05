
#include <SI_EFM8BB52_Register_Enums.h>
#include <math.h>
#include "smb_0.h"
#include "rtc_driver.h"
#include "pwm.h"
#include "state_machine.h"
#include "command_handler.h"

#define M_PI 3.1415926535897f

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
void SMB0_errorCb(SMB0_TransferError_t error)
{
  // Disable state machine and put lamp in red (to be implemented later)
}

// For slave transmission (irrelevant as chip is master-only)
void SMB0_commandReceivedCb()
{

}

// Send clock pulses to the RTC module to reset its i2c interface
void rtc_reset()
{
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




uint8_t code days_per_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

static uint8_t days_of_month(uint8_t month, uint8_t year)
{
  // If February and leap year, add one day to standard days of month
  if((month == 2) && (year % 4 == 0))
    return days_per_month[month-1]+1;

  return days_per_month[month-1];
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

// hours as a function of day
double xdata sunrise_time;
double xdata latitude = 30.266666;
void update_sunrise_time()
{
  uint8_t day = rtc_data.date;
  double lat = latitude * M_PI/180;
  double delta;
  uint8_t i;

  for (i = 1; i < rtc_data.month; i++)
    day += days_of_month(i, rtc_data.year);

  delta = -23.44*cos((2*M_PI/365)*(day - 1 - 10))*(M_PI/180);
  sunrise_time = -12.f*acos(-tan(lat)*tan(delta))/M_PI+12.f;
}

void upload_time_to_RTC()
{
  uint8_t i;

  // Uploading to RTC means data is now valid
  rtc_data.invalid = false;
  set_error_LED(rtc_data.invalid);
  update_sunrise_time();

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


void read_time_from_RTC()
{

  // Read data from RTC module
  transfer_complete = false;
  rtc_send_buffer[0] = 0x00;
  SMB0_transfer(RTC_DEVICE_ADDRESS << 1, rtc_send_buffer, rtc_receive_buffer, 1, RTC_RECEIVE_BUFFER_SIZE);

  // Wait for transfer to end
  while(!transfer_complete);

  // Load rtc data from buffer
  rtc_data.seconds = BCD_to_HEX(rtc_receive_buffer[RTC_SECONDS_REG]);
  rtc_data.minutes = BCD_to_HEX(rtc_receive_buffer[RTC_MINUTES_REG]);
  rtc_data.hours = BCD_to_HEX(rtc_receive_buffer[RTC_HOURS_REG]);
  rtc_data.date = BCD_to_HEX(rtc_receive_buffer[RTC_DATE_REG]);
  rtc_data.month = BCD_to_HEX(rtc_receive_buffer[RTC_MONTH_REG]);
  rtc_data.year = BCD_to_HEX(rtc_receive_buffer[RTC_YEAR_REG]);
  rtc_data.invalid = rtc_receive_buffer[RTC_STATUS_REG] >> RTC_STATUS_REG_OSF__SHIFT;

  // Update status LED based on if the rtc data is valid
  set_error_LED(rtc_data.invalid);

  // Update sunrise time
  update_sunrise_time();
}


bool is_time_valid()
{
  return !rtc_data.invalid;
}

void clock_tick()
{


}

uint8_t get_datetime_command(uint8_t* info_bytes){
  info_bytes[0] = SUCCESSFUL;
  info_bytes[1] = rtc_data.seconds;
  info_bytes[2] = rtc_data.minutes;
  info_bytes[3] = rtc_data.hours;
  info_bytes[4] = rtc_data.date;
  info_bytes[5] = rtc_data.month;
  info_bytes[6] = rtc_data.year;
  return 7;
}

uint8_t set_datetime_command(uint8_t* info_bytes){
  rtc_data.seconds = info_bytes[1];
  rtc_data.minutes = info_bytes[2];
  rtc_data.hours = info_bytes[3];
  rtc_data.date = info_bytes[4];
  rtc_data.month = info_bytes[5];
  rtc_data.year = info_bytes[6];

  // Upload time to RTC module
  upload_time_to_RTC();

  // Successful response
  info_bytes[0] = SUCCESSFUL;
  return 1;
}



uint8_t get_sunrise_time_command(uint8_t* info_bytes){
  double minutes;
  double seconds;
  info_bytes[0] = SUCCESSFUL;
  info_bytes[1] = (int)sunrise_time;
  minutes = 60*(sunrise_time-info_bytes[1]);
  info_bytes[2] = (int)minutes;
  seconds = 60*(minutes-info_bytes[2]);
  info_bytes[3] = (int)seconds;
  return 4;
}



// SQW 1 Hz interrupt from RTC module to update datetime
SI_INTERRUPT (INT0_ISR, INT0_IRQn)
  {
    // Signal tick event
    event_queue_add_event(EV_CLOCK_TICK);

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
    if(++rtc_data.date != days_of_month(rtc_data.month, rtc_data.year)+1)
      return;

    // Update sunrise time
    update_sunrise_time();

    rtc_data.date = 1;
    if(++rtc_data.month != 13)
      return;

    rtc_data.month = 1;
    if(++rtc_data.year != 100)
      return;

    rtc_data.invalid = true;
    set_error_LED(rtc_data.invalid);
  }
