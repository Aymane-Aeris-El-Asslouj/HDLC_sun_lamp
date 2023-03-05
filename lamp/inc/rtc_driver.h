/*
 * rtc_driver.h
 *
 *  Created on: Feb 26, 2023
 *      Author: elass
 */

#ifndef INC_RTC_DRIVER_H_
#define INC_RTC_DRIVER_H_

// SCL/SDA pins and ports
#define SCL_PnBm P0_B0
#define SDA_PnBm P0_B1

#define SCL_PnSKIP P0SKIP
#define SDA_PnSKIP P0SKIP
#define SCL_PnMDIN P0MDIN
#define SDA_PnMDIN P0MDIN

#define SCL_PnSKIP_Bm__BMASK P0SKIP_B0__BMASK
#define SDA_PnSKIP_Bm__BMASK P0SKIP_B1__BMASK

#define SCL_PnMDIN_Bm__BMASK P0MDIN_B0__BMASK
#define SDA_PnMDIN_Bm__BMASK P0MDIN_B1__BMASK

// Covers all register values for RTC module till aging offset register.
#define RTC_RECEIVE_BUFFER_SIZE 17
// Covers initial register address for write and all register values
// for RTC module till aging offset register.
#define RTC_SEND_BUFFER_SIZE 1 + RTC_RECEIVE_BUFFER_SIZE

// RTC module special registers
#define RTC_SECONDS_REG 0x00
#define RTC_MINUTES_REG 0x01
#define RTC_HOURS_REG 0x02
#define RTC_DATE_REG 0x04
#define RTC_MONTH_REG 0x05
#define RTC_YEAR_REG 0x06
#define RTC_STATUS_REG 0x0F

#define RTC_STATUS_REG_OSF__SHIFT 7

#define RTC_DEVICE_ADDRESS 0x68

typedef struct{
  uint8_t invalid; // Checks OSF flag to know if RTC data is invalid (lost power)
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t date;
  uint8_t month;
  uint8_t year;
} rtc_time_data;

void rtc_reset();
void upload_time_to_RTC();
void read_time_from_RTC();
bool is_time_valid();
void clock_tick();
uint8_t get_datetime_command(uint8_t* info_bytes);
uint8_t set_datetime_command(uint8_t* info_bytes);
uint8_t get_sunrise_time_command(uint8_t* info_bytes);


#endif /* INC_RTC_DRIVER_H_ */
