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
    
/**
  * @brief   Structure for time data
  */
typedef struct _APP_TmTypeDef 
{
    uint32_t tm_sec;         /*!< Seconds,  range 0 to 59          */
    uint32_t tm_min;         /*!< Minutes, range 0 to 59           */
    uint32_t tm_hour;        /*!< Hours, range 0 to 23             */
    uint32_t tm_alarm_min;   /*!< Minutes, range 0 to 59           */
    uint32_t tm_alarm_hour;  /*!< Hours, range 0 to 23             */
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
    uint8_t alarm;        /*!< Store the status of the alarm     */
    uint8_t msg;          /*!< Store the message type to send    */
    APP_TmTypeDef tm;     /*!< Time and date in stdlib tm format */
}APP_MsgTypeDef;

/** 
  * @defgroup Alarm status
  @{ */
#define NO_ALARM       (uint8_t)0  /*!< No alarm is set */
#define ALARM_SET      (uint8_t)1  /*!< An alarm is set */
#define ALARM_TRIGGER  (uint8_t)2  /*!< Alarm trigger   */
/**
  @} */

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
    STATE_TIME = 1,  /*!< Time message          */
    STATE_DATE,      /*!< Date message          */
    STATE_ALARM,     /*!< Alarm message         */
    STATE_OK,        /*!< Message recieve ok    */
    STATE_ERROR,     /*!< Message recieve error */
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
#define REFRESH_WWDG (uint32_t)250  /*!< Time to refresh the window watchdog         */
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
 * @brief  To store milisecods for display
 */
extern uint32_t tick_display;

/**
 * @brief  Struct type variable to handle the SPI
 */
extern SPI_HandleTypeDef SpiHandle;

/**
 * @brief  Structure type variable to initialize the RTC
 */
extern RTC_HandleTypeDef hrtc;

/**
 * @brief  Struct type variable to clock messages
 */
extern APP_MsgTypeDef ClockMsg;

/**
  * @brief   Structure of errors
  */
typedef enum _App_ErrorsCode
{/* cppcheck-suppress misra-c2012-2.4 ; To supress declaration of variable */
    WWDG_RET_ERROR = 1u,
    WWDG_REFRESH_RET_ERROR,
    CAN_INIT_RET_ERROR,
    CAN_CONFIL_RET_ERROR,
    CAN_CONFIGLOB_RET_ERROR,
    CAN_START_RET_ERROR,
    CAN_ACTNOT_RET_ERROR,
    CAN_GETMSG_RET_ERROR,
    CAN_ADDMSG_RET_ERROR,
    RTC_INIT_RET_ERROR,
    RTC_SETDEFTIME_RET_ERROR,
    RTC_SETDEFDATE_RET_ERROR,
    RTC_SETDEFALARM_RET_ERROR,
    RTC_DEACTICATE_ALARM_RET_ERROR,
    RTC_GETTIME_RET_ERROR,
    RTC_GETDATE_RET_ERROR,
    RTC_GETALARM_RET_ERROR,
    RTC_SETTIME_RET_ERROR,
    RTC_SETDATE_RET_ERROR,
    RTC_SETALARM_RET_ERROR,
    SPI_INIT_RET_ERROR,
    LCD_INIT_RET_ERROR,
    LCD_SETCUR_RET_ERROR,
    LCD_STRING_RET_ERROR,
    SPI_TRAN_RET_ERROR,
    PWR_CONVOLSCA_RET_ERROR,
    RCC_OSCCON_RET_ERROR,
    RCC_CLOCKCON_RET_ERROR,
    RCC_PERCLKCON_RET_ERROR,
    QUEUE_PAR_ERROR,
    SQUEDULER_PAR_ERROR,
    SCHEDULER_SERIAL_ERROR,
    SCHEDULER_CLOCK_ERROR,
    SCHEDULER_HEARTBEAT_ERROR,
    SCHEDULER_WWDG_ERROR,
    SCHEDULER_DISPLAY_ERROR
} App_ErrorsCode; /* cppcheck-suppress misra-c2012-2.3 ; To supress declaration of variable */

/** 
  * @defgroup Functional safety
  @{ */
#define assert_error(expr, error)         ((expr) ? (void)0U : safe_state((uint8_t *)__FILE__, __LINE__, (error)))  /*!< Macro to detect erros, wehere if expr is evaluated to false is an error */
/**
  @} */

/* External link to fucntion that will act as the safe state */
extern void safe_state(uint8_t *file, uint32_t line, uint8_t error);

/**
 * @brief  Struct type variable to functional safety
 */
extern HAL_StatusTypeDef Status;

/** 
  * @defgroup Port C
  @{ */
#define PORTC 0xFF  /*!< Pins 0 to 7 of port C  */
/**
  @} */

/**
  * @brief   Queue control structure
  */
typedef struct
{
    void     *Buffer;  /*!< Pointer to memory space use as buffer by Tail                               */
    uint32_t Elements; /*!< Number of items to store (queue size)                                       */
    uint8_t  Size;     /*!< Size of the type of elements to store                                       */
    uint32_t Head;     /*!< Pointer indicating the next space to write                                  */
    uint32_t Tail;     /*!< Pointer indicating the next space to read                                   */
    uint8_t  Empty;    /*!< Flag indicating if there are no items to read                               */
    uint8_t  Full;     /*!< Flag that indicates if it is not possible to continue writing more elements */
}QUEUE_HandleTypeDef;

/**
 * @brief  Struct type variable to handle the queue
 */
extern QUEUE_HandleTypeDef queueHandle;

/**
 * @brief  To storage messages from serial
 */
extern QUEUE_HandleTypeDef SerialQueue;

/**
 * @brief  To storage messages from clock
 */
extern QUEUE_HandleTypeDef ClockQueue;

/** 
  * @defgroup Defines to scheduler
  @{ */
#define TASKS_NUMBER   (uint32_t)5    /*!< Number of tasks to run  */
#define SCHEDULER_TICK (uint32_t)5    /*!< Tick for the scheduler  */
#define TIMERS_NUMBER  (uint32_t)1    /*!< Number of timers to run */
#define NUM32_0        (uint32_t)0    /*!< Number 0, 32 bits       */
#define NUM32_1        (uint32_t)1    /*!< Number 1, 32 bits       */
/**
  @} */

/** 
  * @defgroup Alarm Button
  @{ */
#define NOT_PRESSED  (uint8_t)0  /*!< The button is not pressed */
#define IS_PRESSED   (uint8_t)1  /*!< The button is pressed     */
#define IS_RELEASED  (uint8_t)2  /*!< The button is released    */
/**
  @} */

/**
 * @brief  Struct type variable to handle the SPI
 */
extern uint8_t AlarmButton;

#endif
