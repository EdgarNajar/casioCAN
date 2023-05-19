/**
 * @file    app_clock.c
 * @brief   **Brief file description (just one line)**
 *
 * This application will give and display the time and date, it will use the internal 
 * Mcu RTC peripheral in order to keep track of the timing and date, implementing state machines
 *
 * @note    None
 */
 
#include "app_bsp.h"
#include "app_clock.h"

/**
 * @brief  Structure type variable to initialize the RTC
 */

static RTC_HandleTypeDef hrtc = {0};

/**
 * @brief  Variable to control changes in time data
 */

uint8_t changes = WAIT_MESSAGE;

static uint32_t tick_display;

static uint8_t default_data = 1u;

/**
 * @brief   **Initialization of the RTC**
 *
 * It will monitor a message from the serial_task in the form of variable APP_MsgTypeDef 
 * and update the RTC with the new Time, Date or Alarm
 *
 * @param   hrtc [out] Structure type variable used to configure the RTC
 *
 * @retval  None
 *
 * @note None
 */

void Clock_Init( void )
{
    /* Configuration RTC */
    hrtc.Instance             = RTC;
    hrtc.Init.HourFormat      = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv    = 127;
    hrtc.Init.SynchPrediv     = 255;
    hrtc.Init.OutPut          = RTC_OUTPUT_DISABLE;
    /* Initilize the RTC with 24 hour format and no output signal enble */
    HAL_RTC_Init( &hrtc );

    tick_display = HAL_GetTick();
}

/**
 * @brief   **Contains the state machine in charge of messages processing**
 *
 * It will process the data and displays it 
 *
 * @param   changes [out] To control changes in time data
 *
 * @retval  None
 *
 * @note None
 */

void Clock_Task( void )
{
    RTC_DateTypeDef sDate  = {0};
    RTC_TimeTypeDef sTime  = {0};
    

    if( default_data == 1u )
    {
        default_data++;
        /* Setting default time at 23:59:50 in BCD format */
        sTime.Hours   = 0x23;
        sTime.Minutes = 0x59;
        sTime.Seconds = 0x50;
        sTime.SubSeconds = 0x00;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );

        /* Setting default date at Monday January 31, 2023 in BCD format */
        sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
        sDate.Month = RTC_MONTH_JANUARY;
        sDate.Date = 0x31;
        sDate.Year = 0x23;
        ClockMsg.tm.tm_yday = 0x20;
        HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
    }

    switch( changes )
    {
        case WAIT_MESSAGE:
            if( MSGHandler.msg == (uint8_t)CHANGE_TIME )
            {
                changes = CHANGE_TIME;
            }
            else if( MSGHandler.msg == (uint8_t)CHANGE_DATE )
            {
                changes = CHANGE_DATE;
            }
            else if( MSGHandler.msg == (uint8_t)CHANGE_ALARM )
            {
                changes = CHANGE_ALARM;
            }
            else if( (HAL_GetTick() - tick_display) >= 1000 )
            {
                tick_display = HAL_GetTick();
                changes = DISPLAY;
            }
            else
            {}

            MSGHandler.msg = WAIT_MESSAGE;
            break;

        case CHANGE_TIME:
            changes = DISPLAY;            
            /* Setting time in BCD format */
            sTime.Hours   = MSGHandler.tm.tm_hour;
            sTime.Minutes = MSGHandler.tm.tm_min;
            sTime.Seconds = MSGHandler.tm.tm_sec;
            sTime.SubSeconds = 0x00;
            sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
            sTime.StoreOperation = RTC_STOREOPERATION_RESET;
            HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );
            break;

        case CHANGE_DATE:
            changes = DISPLAY;
            ClockMsg.tm.tm_yday = MSGHandler.tm.tm_yday;
            /* Setting date in BCD format */
            sDate.WeekDay = MSGHandler.tm.tm_wday;
            sDate.Month = MSGHandler.tm.tm_mon;
            sDate.Date = MSGHandler.tm.tm_mday;
            sDate.Year = MSGHandler.tm.tm_year;
            /* Set date */
            HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
            break;

        case CHANGE_ALARM:
            changes = DISPLAY;
            break;

        case DISPLAY:
            changes = WAIT_MESSAGE;
            /* Get the RTC current Time */
            HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN );
            /* Get the RTC current Date */
            HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN );

            ClockMsg.tm.tm_mday = sDate.Date;
            ClockMsg.tm.tm_mon  = sDate.Month;
            ClockMsg.tm.tm_year = sDate.Year;
            ClockMsg.tm.tm_wday = sDate.WeekDay;

            ClockMsg.tm.tm_hour = sTime.Hours;
            ClockMsg.tm.tm_min  = sTime.Minutes;
            ClockMsg.tm.tm_sec  = sTime.Seconds;

            ClockMsg.msg = changes;
            break;

        default :
            break;
    }
}