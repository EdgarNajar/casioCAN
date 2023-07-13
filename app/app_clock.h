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
#define CHANGE_TIME      (uint8_t)1  /*!< State change time  */
#define CHANGE_DATE      (uint8_t)2  /*!< State change date  */
#define CHANGE_ALARM     (uint8_t)3  /*!< State change alarm */
#define DISPLAY          (uint8_t)4  /*!< State display      */
/**
  @} */

/** 
  * @defgroup Values for RTC initialization
  @{ */
#define VAL_ASYNCHPREDIV (uint8_t)127  /*!< Value for AsynchPrediv */
#define VAL_SYNCHPREDIV  (uint8_t)255  /*!< Value for SynchPrediv  */
/**
  @} */

/** 
  * @defgroup Valies for default data
  @{ */
#define DEF_HOURS         (uint8_t)0x23  /*!< Default hours         */
#define DEF_MINUTES       (uint8_t)0x59  /*!< Default minutes       */
#define DEF_SECONDS       (uint8_t)0x50  /*!< Default seconds       */
#define DEF_SUBSECONDS    (uint8_t)0x00  /*!< Default subseconds    */
#define DEF_DATE          (uint8_t)0x31  /*!< Default date          */
#define DEF_YEARLSB       (uint8_t)0x23  /*!< Default year LSB      */
#define DEF_YEARMSB       (uint8_t)0x20  /*!< Default year MSB      */
#define DEF_ALARM_HOURS   (uint8_t)0x20  /*!< Default alarm hour    */
#define DEF_ALARM_MINUTES (uint8_t)0x00  /*!< Default alarm minutes */
#define DEF_ALARM_SECONDS (uint8_t)0x00  /*!< Default alarm seconds */
/**
  @} */

/** 
  * @defgroup Amount of miliseconds to display
  @{ */
#define ONE_SECOND (uint32_t)1000  /*!< Define for one second    */
#define FIFTY_MS   (uint32_t)50             /*!< Define for 50 miliseconds*/
/**
  @} */

/** 
  * @defgroup Defines for numbers
  @{ */
#define NUM_0 (uint8_t)0  /*!< Number 0 */
#define NUM_1 (uint8_t)1  /*!< Number 1 */
/**
  @} */

extern void Clock_Init( void );
extern void Clock_Task( void );
extern void Change_Display( void );

#endif