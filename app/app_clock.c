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
static RTC_HandleTypeDef hrtc = {num_0};

/**
 * @brief  Variable to control changes in time data
 */
uint8_t changes = WAIT_MESSAGE;

/**
 * @brief  To store milisecods for display
 */
static uint32_t tick_display;

/**
 * @brief  To display the default date and time
 */
static uint8_t default_data = num_1;

/**
 * @brief   **Initialization of the RTC**
 *
 * It will monitor a message from the serial_task in the form of variable APP_MsgTypeDef 
 * and update the RTC with the new Time, Date or Alarm
 *
 * @param   hrtc [out] Structure type variable used to configure the RTC
 *
 * @note None
 */
void Clock_Init( void )
{
    /* Configuration RTC */
    hrtc.Instance             = RTC;
    hrtc.Init.HourFormat      = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv    = val_AsynchPrediv;
    hrtc.Init.SynchPrediv     = val_SynchPrediv;
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
 * @note None
 */
void Clock_Task( void )
{
    RTC_DateTypeDef  sDate  = {num_0};
    RTC_TimeTypeDef  sTime  = {num_0};
    RTC_AlarmTypeDef sAlarm = {num_0};

    if( default_data == num_1 )
    {
        default_data++;
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
        ClockMsg.tm.tm_yday = def_YearMSB;
        HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
    }
    else
    {}

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
            
            ClockMsg.tm.tm_yday = MSGHandler.tm.tm_yday;
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

            sAlarm.AlarmTime.Hours   = 0x12;
            sAlarm.AlarmTime.Minutes = 0x12;
            sAlarm.AlarmTime.Seconds = 0x12;
            sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
            sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
            sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
            sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
            sAlarm.AlarmDateWeekDay = 0x12;
            sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
            sAlarm.Alarm = RTC_ALARM_A;
            break;

        case DISPLAY:
            changes = WAIT_MESSAGE;
            /* Get the RTC current Time */
            HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN );
            /* Get the RTC current Date */
            HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN );

            break;

        default :
            break;
    }
}