/**
 * @file    app_bsp.h
 * @brief   **Structures, enums and variables**
 *
 * File with the declaration of structures and enums to storage
 * time, date and alarm, global variables to initialize the CAN protocol
 * and to access the state machine for message processing
 *
 * @note    None
 */
#ifndef BSP_H_
#define BSP_H_

    #include "stm32g0xx.h"
    #include <stdint.h>
    /* cppcheck-suppress misra-c2012-21.6 ; Standar library needed for use of semi hosting */
    #include <stdio.h>

/**
  * @brief   Structure for time data
  */
typedef struct _APP_TmTypeDef 
{
    uint32_t tm_sec;         /*!< Seconds,  range 0 to 59          */
    uint32_t tm_min;         /*!< Minutes, range 0 to 59           */
    uint32_t tm_hour;        /*!< Hours, range 0 to 23             */
    uint32_t tm_mday;        /*!< Day of the month, range 1 to 31  */
    uint32_t tm_mon;         /*!< Month, range 0 to 11             */
    uint32_t tm_year;        /*!< Years in rage 1900 2100          */
    uint32_t tm_wday;        /*!< Day of the week, range 0 to 6    */
    uint32_t tm_yday;        /*!< Day in the year, range 0 to 365  */
    uint32_t tm_isdst;       /*!< Daylight saving time             */
}APP_TmTypeDef;

/**
  * @brief   Structure for handle time data
  */
typedef struct _APP_MsgTypeDef
{
    uint8_t msg;          /*!< Store the message type to send    */
    APP_TmTypeDef tm;     /*!< Time and date in stdlib tm format */
}APP_MsgTypeDef;

/**
  * @brief   Enum for kind af message
  */
typedef enum
/* cppcheck-suppress misra-c2012-2.4 ; To supress declaration of variable type enum */
{
    SERIAL_MSG_NONE = 0u, /*!< No serial message    */
    SERIAL_MSG_TIME,      /*!< Time serial message  */
    SERIAL_MSG_DATE,      /*!< Date serial message  */
    SERIAL_MSG_ALARM      /*!< Alarm serial message */
}APP_Messages;

/**
  * @brief   Enum for states control
  */
typedef enum
{
    STATE_IDLE = 1u, /*!< Wait for a message    */
    STATE_MESSAGE,   /*!< Message recieve       */
    STATE_TIME,      /*!< Time message          */
    STATE_DATE,      /*!< Date message          */
    STATE_ALARM,     /*!< Alarm message         */
    STATE_OK,        /*!< Message recieve ok    */
    STATE_ERROR      /*!< Message recieve error */
}APP_States;

/** 
  * @defgroup Heartbeat
  @{ */
#define TOGGLE_HEART (uint32_t)300  /*!< Time to toggle the heart(GPIOA PIN5) */
/**
  @} */

/** 
  * @defgroup Parameters for window watchdog
  @{ */
#define VAL_WINDOW   (uint32_t)108  /*!< Value to the window of the window watchdog  */
#define VAL_COUNTER  (uint32_t)115  /*!< Value to the counter of the window watchdog */
#define REFRESH_WWDG (uint32_t)195  /*!< Time to refresh the window watchdog         */
/**
  @} */

/**
 * @brief  Structure type variable for time data
 */
extern APP_MsgTypeDef MSGHandler;

/**
 * @brief  Structure type variable for CAN handler
 */
extern FDCAN_HandleTypeDef CANHandler;

/**
 * @brief  Is used to move in state machine
 */
extern APP_States state_control;

/**
 * @brief  To determine the type of data that changed
 */
extern uint8_t changes;

/**
 * @brief  To store milisecods for display
 */
extern uint32_t tick_display;

/**
 * @brief  To set the default date and time
 */
extern uint8_t default_data;

extern void initialise_monitor_handles(void);

#endif

