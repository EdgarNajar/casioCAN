/**
 * @file    app_clock.c
 * @brief   **Clock calendar**
 *
 * In this file the RTC peripheral will be configured to keep track of the timing and date,
 * the data will be display within a state machine and a default time and date will be set 
 * to start the clock
 *
 * @note    None
 */
#include "app_clock.h"

/**
 * @brief  Structure type variable to initialize the RTC
 */
static RTC_HandleTypeDef hrtc;

/**
 * @brief  Structure type valriable for user RTC date initialization
 */
static RTC_DateTypeDef  sDate;

/**
 * @brief  Structure type valriable for user RTC time initialization
 */
static RTC_TimeTypeDef  sTime;

/**
 * @brief  Structure type valriable for user RTC alarm initialization
 */
static RTC_AlarmTypeDef sAlarm;

/**
 * @brief  Variable to control changes in time data
 */
uint8_t changes = WAIT_MESSAGE;

/**
 * @brief  To store milisecods for display
 */
uint32_t tick_display;

/**
 * @brief   **RTC peripheral**
 *
 * This function configures and initialize the RTC peripheral
 * in 24 hours format and no output signal enble
 *
 * @param   hrtc         [out] Structure type variable used to configure the RTC
 * @param   tick_display [out] To store milisecods for display
 *
 * @note None
 */
void Clock_Init( void )
{
    initialise_monitor_handles();
    
    /* Configuration RTC */
    hrtc.Instance             = RTC;
    hrtc.Init.HourFormat      = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv    = val_AsynchPrediv;
    hrtc.Init.SynchPrediv     = val_SynchPrediv;
    hrtc.Init.OutPut          = RTC_OUTPUT_DISABLE;
    /* Initilize the RTC with 24 hour format and no output signal enble */
    HAL_RTC_Init( &hrtc );

    tick_display = HAL_GetTick();

    /* Setting default time at 23:59:50 in BCD format */
    sTime.Hours   = def_Hours;
    sTime.Minutes = def_Minutes;
    sTime.Seconds = def_Seconds;
    sTime.SubSeconds = def_SubSeconds;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );

    /* Setting default date at Monday January 31, 2023 in BCD format */
    sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = def_Date;
    sDate.Year = def_YearLSB;
    MSGHandler.tm.tm_yday = def_YearMSB;
    HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );

    sAlarm.AlarmTime.Hours   = def_Alarm_Hour;
    sAlarm.AlarmTime.Minutes = def_Alarm_Minutes;
    sAlarm.AlarmTime.Seconds = def_Alarm_Seconds;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    // sAlarm.AlarmDateWeekDay = 0x12;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm( &hrtc, &sAlarm, RTC_FORMAT_BCD );
}

/**
 * @brief   **Calendar messages processing**
 *
 * This function will process the information established 
 * by default to display it, and wait for new time and date information 
 * to update the calendar and display the new information.
 *
 * @param   changes      [out]    To control changes in time data
 * @param   default_data [in/out] To stablish the default data
 *
 * @note None
 */
void Clock_Task( void )
{
    switch( changes )
    {
        case WAIT_MESSAGE:
            if( MSGHandler.msg == CHANGE_TIME )
            {
                changes = CHANGE_TIME;
            }
            else if( MSGHandler.msg == CHANGE_DATE )
            {
                changes = CHANGE_DATE;
            }
            else if( MSGHandler.msg == CHANGE_ALARM )
            {
                changes = CHANGE_ALARM;
            }
            else if( (HAL_GetTick() - tick_display) >= one_second )
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
            HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );
            break;

        case CHANGE_DATE:
            changes = DISPLAY;
            
            /* Setting date in BCD format */
            sDate.WeekDay = MSGHandler.tm.tm_wday;
            sDate.Month   = MSGHandler.tm.tm_mon;
            sDate.Date    = MSGHandler.tm.tm_mday;
            sDate.Year    = MSGHandler.tm.tm_year;
            /* Set date */
            HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
            break;

        case CHANGE_ALARM:
            changes = DISPLAY;

            sAlarm.AlarmTime.Hours   = MSGHandler.tm.tm_hour;
            sAlarm.AlarmTime.Minutes = MSGHandler.tm.tm_min;
            HAL_RTC_SetAlarm( &hrtc, &sAlarm, RTC_FORMAT_BCD );
            break;

        case DISPLAY:
            changes = WAIT_MESSAGE;
            
            /* Get the RTC current Time */
            HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN );
            /* Get the RTC current Date */
            HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN );
            /* Get the RTC current Alarm */
            HAL_RTC_SetAlarm( &hrtc, &sAlarm, RTC_FORMAT_BIN );

            (void)printf("Time:  %02d:%02d:%02d\n\r", sTime.Hours, sTime.Minutes, sTime.Seconds);
            (void)printf("Date:  %02d/%02d/%ld%d\n\r", sDate.Month, sDate.Date, MSGHandler.tm.tm_yday, sDate.Year);
            (void)printf("Alarm: %02ld:%02ld\n\n\r", MSGHandler.tm.tm_hour, MSGHandler.tm.tm_min);
            break;

        default :
            break;
    }
}