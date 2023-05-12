/**
 * @file    app_serial.h
 * @brief   **structures, enums and function prototypes**
 *
 * declaration of all structures, enums and function prototypes to initialize the CAN port
 *
 * @note    none
 */
 
#ifndef APP_SERIAL_H__
#define APP_SERIAL_H__

/**
  * @brief   structure for time data
  */

typedef struct _APP_TmTypeDef 
{
    uint32_t tm_sec;         /* seconds,  range 0 to 59          */
    uint32_t tm_min;         /* minutes, range 0 to 59           */
    uint32_t tm_hour;        /* hours, range 0 to 23             */
    uint32_t tm_mday;        /* day of the month, range 1 to 31  */
    uint32_t tm_mon;         /* month, range 0 to 11             */
    uint32_t tm_year;        /* years in rage 1900 2100          */
    uint32_t tm_wday;        /* day of the week, range 0 to 6    */
    uint32_t tm_yday;        /* day in the year, range 0 to 365  */
    uint32_t tm_isdst;       /* daylight saving time             */
}APP_TmTypeDef;

/**
  * @brief   structure for handle time data
  */

typedef struct _APP_MsgTypeDef
{
    uint8_t msg;          /*!< Store the message type to send */
    APP_TmTypeDef tm;     /*!< time and date in stdlib tm format */
}APP_MsgTypeDef;

/**
  * @brief   enum for kind af message
  */

typedef enum{
    SERIAL_MSG_NONE = 0,
    SERIAL_MSG_TIME,
    SERIAL_MSG_DATE,
    SERIAL_MSG_ALARM
}APP_Messages;

/** 
  * @defgroup months, months of the year in hexa
  @{ */
#define JANUARY   (uint8_t)0x01  /*!< january in hexa */
#define FEBRUARY  (uint8_t)0x02  /*!< february in hexa */
#define MARCH     (uint8_t)0x03  /*!< march in hexa */
#define APRIL     (uint8_t)0x04  /*!< april in hexa */
#define MAY       (uint8_t)0x05  /*!< may in hexa */
#define JUNE      (uint8_t)0x06  /*!< june in hexa */
#define JULY      (uint8_t)0x07  /*!< july in hexa */
#define AUGUST    (uint8_t)0x08  /*!< auust in hexa */
#define SEPTEMBER (uint8_t)0x09  /*!< september in hexa */
#define OCTOBER   (uint8_t)0x10  /*!< october in hexa */
#define NOVEMBER  (uint8_t)0x11  /*!< november in hexa */
#define DECEMBER  (uint8_t)0x12  /*!< december in hexa */
/**
  @} */

/**
  * @brief   enum for states control
  */

typedef enum{
    STATE_IDLE = 1,
    STATE_MESSAGE,
    STATE_TIME,
    STATE_DATE,
    STATE_ALARM,
    STATE_OK,
    STATE_ERROR
}APP_States;

/**
 * @brief  structure type variable for time data
 */

extern APP_MsgTypeDef MSGHandler;

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif