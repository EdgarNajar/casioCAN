/**
 * @file    app_bsp.h
 * @brief   **Auxiliar file**
 *
 * Auxiliar file
 *
 * @note    None
 */
#ifndef BSP_H_
#define BSP_H_

    #include "stm32g0xx.h"
    #include <stdint.h>

/**
  * @brief   Structure for time data
  */
typedef struct _APP_TmTypeDef 
{
    uint32_t tm_sec;         /*!< seconds,  range 0 to 59          */
    uint32_t tm_min;         /*!< minutes, range 0 to 59           */
    uint32_t tm_hour;        /*!< hours, range 0 to 23             */
    uint32_t tm_mday;        /*!< day of the month, range 1 to 31  */
    uint32_t tm_mon;         /*!< month, range 0 to 11             */
    uint32_t tm_year;        /*!< years in rage 1900 2100          */
    uint32_t tm_wday;        /*!< day of the week, range 0 to 6    */
    uint32_t tm_yday;        /*!< day in the year, range 0 to 365  */
    uint32_t tm_isdst;       /*!< daylight saving time             */
}APP_TmTypeDef;

/**
  * @brief   Structure for handle time data
  */
typedef struct _APP_MsgTypeDef
{
    uint8_t msg;          /*!< Store the message type to send    */
    APP_TmTypeDef tm;     /*!< time and date in stdlib tm format */
}APP_MsgTypeDef;

/**
  * @brief   Enum for kind af message
  */
typedef enum{
    SERIAL_MSG_NONE = 0u, /*!< no serial message    */
    SERIAL_MSG_TIME,      /*!< time serial message  */
    SERIAL_MSG_DATE,      /*!< date serial message  */
    SERIAL_MSG_ALARM      /*!< alarm serial message */
}APP_Messages;

/**
  * @brief   Enum for states control
  */
typedef enum{
    STATE_IDLE = 1u, /*!< wait for a message    */
    STATE_MESSAGE,   /*!< message recieve       */
    STATE_TIME,      /*!< time message          */
    STATE_DATE,      /*!< date message          */
    STATE_ALARM,     /*!< alarm message         */
    STATE_OK,        /*!< message recieve ok    */
    STATE_ERROR      /*!< message recieve error */
}APP_States;

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
extern APP_States STATE_CONTROL;

#endif

