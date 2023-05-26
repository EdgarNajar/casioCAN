/**
 * @file    app_clock.h
 * @brief   **Clock calendar**
 *
 * In this file the RTC peripheral will be configured to keep track of the timing and date,
 * the data will be display within a state machine and a default time and date will be set 
 * to start the clock
 * 
 * @note    none
 */
#ifndef APP_CLOCK_H__
#define APP_CLOCK_H__

#include "app_bsp.h"

#define WAIT_MESSAGE (uint8_t)0
#define CHANGE_TIME  (uint8_t)1
#define CHANGE_DATE  (uint8_t)2
#define CHANGE_ALARM (uint8_t)3
#define DISPLAY      (uint8_t)4

#define val_AsynchPrediv (uint8_t)127
#define val_SynchPrediv  (uint8_t)255

#define def_Hours      (uint8_t)0x23
#define def_Minutes    (uint8_t)0x59
#define def_Seconds    (uint8_t)0x50
#define def_SubSeconds (uint8_t)0x00

#define def_Date    (uint8_t)0x31
#define def_YearLSB (uint8_t)0x23
#define def_YearMSB (uint8_t)20

#define def_Alarm_Hour    (uint8_t)0x20
#define def_Alarm_Minutes (uint8_t)0x00
#define def_Alarm_Seconds (uint8_t)0x00

#define one_second (uint8_t)1000

#define num_0 (uint8_t)0
#define num_1 (uint8_t)1

extern void Clock_Init( void );
extern void Clock_Task( void );

#endif