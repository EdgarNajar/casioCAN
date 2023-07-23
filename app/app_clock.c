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
#include "hil_queue.h"
#include "hil_scheduler.h"

static void Clock_StMachine( void );

/**
 * @brief  Structure type variable to initialize the RTC
 */
RTC_HandleTypeDef hrtc;

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
 * @brief  To storage messages from clock
 */
QUEUE_HandleTypeDef ClockQueue;

/**
 * @brief  Struct type variable to handle the scheduler
 */
extern Scheduler_HandleTypeDef SchedulerHandler;

/**
 * @brief   **RTC peripheral**
 *
 * This function configures and initialize the RTC peripheral
 * in 24 hours format and no output signal enable
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
    hrtc.Init.AsynchPrediv    = VAL_ASYNCHPREDIV;
    hrtc.Init.SynchPrediv     = VAL_SYNCHPREDIV;
    hrtc.Init.OutPut          = RTC_OUTPUT_DISABLE;
    /* Initilize the RTC with 24 hour format and no output signal enble */
    Status = HAL_RTC_Init( &hrtc );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_INIT_RET_ERROR );

    /* Setting default time at 23:59:50 in BCD format */
    sTime.Hours   = DEF_HOURS;
    sTime.Minutes = DEF_MINUTES;
    sTime.Seconds = DEF_SECONDS;
    sTime.SubSeconds = DEF_SUBSECONDS;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    Status = HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_SETDEFTIME_RET_ERROR );

    /* Setting default date at Monday January 31, 2023 in BCD format */
    sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = DEF_DATE;
    sDate.Year = DEF_YEARLSB;
    MSGHandler.tm.tm_yday = DEF_YEARMSB;
    Status = HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_SETDEFDATE_RET_ERROR );

    sAlarm.AlarmTime.Hours   = DEF_ALARM_HOURS;
    sAlarm.AlarmTime.Minutes = DEF_ALARM_MINUTES;
    sAlarm.AlarmTime.Seconds = DEF_ALARM_SECONDS;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_TUESDAY;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    sAlarm.Alarm = RTC_ALARM_A;
    Status = HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_A );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_SETDEFALARM_RET_ERROR );

    MSGHandler.alarm = NO_ALARM;

    static APP_MsgTypeDef Clock_buffer[10];
    ClockQueue.Buffer   = Clock_buffer;
    ClockQueue.Elements = 10;
    ClockQueue.Size     = sizeof( APP_MsgTypeDef );
    HIL_QUEUE_Init( &ClockQueue );
}

/**
 * @brief   **Call of the clock state machine**
 *
 * This function calls the clock state machine every 50ms
 * with the help of queues, therefore it won't be execute all the time
 *
 * @param   SerialQueue  [in/out] To storage messages from serial
 * @param   MSGHandler   [in/out] Structure type variable for time data
 * 
 * @note None
 */
void Clock_Task( void )
{
    while( HIL_QUEUE_IsEmptyISR( &SerialQueue, RTC_TAMP_IRQn ) == NUM_0 )
    {
        /*Read the first message*/
        (void)HIL_QUEUE_ReadISR( &SerialQueue, &MSGHandler, RTC_TAMP_IRQn );
        Clock_StMachine();
    }
}

/**
 * @brief   **Calendar messages processing**
 *
 * This function will process the information established 
 * by default to display it, and wait for new time and date information 
 * to update the calendar and display the new information.
 *
 * @param   MSGHandler   [in/out] Structure type variable for time data
 * 
 * @note None
 */
void Clock_StMachine( void )
{
    switch( MSGHandler.msg )
    {
        case CHANGE_TIME:
            /* Setting time in BCD format */
            sTime.Hours   = MSGHandler.tm.tm_hour;
            sTime.Minutes = MSGHandler.tm.tm_min;
            sTime.Seconds = MSGHandler.tm.tm_sec;
            Status = HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BCD );
            /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
            assert_error( Status == HAL_OK, RTC_SETTIME_RET_ERROR );
            
            MSGHandler.msg = DISPLAY;
            (void)HIL_QUEUE_WriteISR( &SerialQueue, &MSGHandler, RTC_TAMP_IRQn );
            break;

        case CHANGE_DATE:
            /* Setting date in BCD format */
            sDate.WeekDay = MSGHandler.tm.tm_wday;
            sDate.Month   = MSGHandler.tm.tm_mon;
            sDate.Date    = MSGHandler.tm.tm_mday;
            sDate.Year    = MSGHandler.tm.tm_year;
            /* Set date */
            Status = HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BCD );
            /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
            assert_error( Status == HAL_OK, RTC_SETDATE_RET_ERROR );
            
            MSGHandler.msg = DISPLAY;
            (void)HIL_QUEUE_WriteISR( &SerialQueue, &MSGHandler, RTC_TAMP_IRQn );
            break;

        case CHANGE_ALARM:
            /* Setting alarm in BCD format */
            sAlarm.AlarmTime.Hours   = MSGHandler.tm.tm_alarm_hour;
            sAlarm.AlarmTime.Minutes = MSGHandler.tm.tm_alarm_min;
            Status = HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_A );
            /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
            assert_error( Status == HAL_OK, RTC_DEACTICATE_ALARM_RET_ERROR );
            Status = HAL_RTC_SetAlarm_IT( &hrtc, &sAlarm, RTC_FORMAT_BCD );
            /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
            assert_error( Status == HAL_OK, RTC_SETALARM_RET_ERROR );
            
            MSGHandler.alarm = ALARM_SET;
            MSGHandler.msg = DISPLAY;
            (void)HIL_QUEUE_WriteISR( &SerialQueue, &MSGHandler, RTC_TAMP_IRQn );
            break;

        case DISPLAY:
            Change_Display();
            break;
            
        default :
            break;
    }
}

/**
 * @brief   **Change the display**
 *
 * This function will get the time, data and alarm
 * and assin it to thestructure ClockMsg to be display
 *
 * @param   ClockMsg     [in/out] Structure type variable for time data
 * 
 * @note None
 */
void Change_Display( void )
{
    /* Get the RTC current Time */
    Status = HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_GETTIME_RET_ERROR );
    /* Get the RTC current Date */
    Status = HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RTC_GETDATE_RET_ERROR );
    /* Get the RTC current Alarm */
    if( (MSGHandler.alarm == ALARM_SET) && (AlarmButton == IS_PRESSED) )
    {
        Status = HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN );
        /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
        assert_error( Status == HAL_OK, RTC_GETTIME_RET_ERROR );
        Status = HAL_RTC_GetAlarm( &hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN );
        /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
        assert_error( Status == HAL_OK, RTC_GETALARM_RET_ERROR );
    }

    ClockMsg.tm.tm_mday = sDate.Date;
    ClockMsg.tm.tm_mon  = sDate.Month;
    ClockMsg.tm.tm_year = sDate.Year;
    ClockMsg.tm.tm_wday = sDate.WeekDay;

    ClockMsg.tm.tm_hour = sTime.Hours;
    ClockMsg.tm.tm_min  = sTime.Minutes;
    ClockMsg.tm.tm_sec  = sTime.Seconds;

    ClockMsg.tm.tm_alarm_hour = sAlarm.AlarmTime.Hours;
    ClockMsg.tm.tm_alarm_min  = sAlarm.AlarmTime.Minutes;

    ClockMsg.msg = DISPLAY;
    (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
}

/**
 * @brief   **RTC alarm event callback**
 *
 * The callback will change the state of the alarm to ALARM_TRIGGER
 * to indicate its activated
 *
 * @param   hrtc       [in]  Structure type variable to handle the RTC
 * @param   MSGHandler [out] Structure type variable for time data
 * 
 * @note  None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_RTC_AlarmAEventCallback( RTC_HandleTypeDef *hrtc )
{
    MSGHandler.alarm = ALARM_TRIGGER;
}
