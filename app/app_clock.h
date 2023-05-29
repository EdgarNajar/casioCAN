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

/** 
  * @defgroup Name of states of state machine
  @{ */
#define WAIT_MESSAGE (uint8_t)0  /*!< State wait message */
#define CHANGE_TIME  (uint8_t)1  /*!< State change time  */
#define CHANGE_DATE  (uint8_t)2  /*!< State change date  */
#define CHANGE_ALARM (uint8_t)3  /*!< State change alarm */
#define DISPLAY      (uint8_t)4  /*!< State display      */
/**
  @} */

/** 
  * @defgroup Values for RTC initialization
  @{ */
#define val_AsynchPrediv (uint8_t)127  /*!< Value for AsynchPrediv */
#define val_SynchPrediv  (uint8_t)255  /*!< Value for SynchPrediv  */
/**
  @} */

/** 
  * @defgroup Valies for default data
  @{ */
#define def_Hours         (uint8_t)0x23  /*!< Default hours         */
#define def_Minutes       (uint8_t)0x59  /*!< Default minutes       */
#define def_Seconds       (uint8_t)0x50  /*!< Default seconds       */
#define def_SubSeconds    (uint8_t)0x00  /*!< Default subseconds    */
#define def_Date          (uint8_t)0x31  /*!< Default date          */
#define def_YearLSB       (uint8_t)0x23  /*!< Default year MSB      */
#define def_YearMSB       (uint8_t)20    /*!< Default year LSB      */
#define def_Alarm_Hour    (uint8_t)0x20  /*!< Default alarm hour    */
#define def_Alarm_Minutes (uint8_t)0x00  /*!< Default alarm minutes */
#define def_Alarm_Seconds (uint8_t)0x00  /*!< Default alarm seconds */
/**
  @} */

/** 
  * @defgroup Amount of miliseconds to display
  @{ */
#define one_second (uint8_t)1000  /*!< Define for one second */
/**
  @} */

/** 
  * @defgroup Defines for numbers
  @{ */
#define num_0 (uint8_t)0  /*!< Number 0 */
#define num_1 (uint8_t)1  /*!< Number 1 */
/**
  @} */

extern void Clock_Init( void );
extern void Clock_Task( void );

extern void initialise_monitor_handles(void);

#endif