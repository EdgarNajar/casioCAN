/**
 * @file    hil_scheduler.c
 * @brief   ****
 *
 * 
 * 
 * @note    None
 */
#include "hil_scheduler.h"

/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler )
{
    
}

/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period )
{
    uint8_t Task_ID = NO_TASK;

    if( (Period > (hscheduler->tick)) && (Period % (hscheduler->tick) == MULTIPLE) )
    {
        hscheduler->taskPtr->elapsed  = NUM_0;
        hscheduler->taskPtr->initFunc = InitPtr;
        hscheduler->taskPtr->period   = Period;
        hscheduler->taskPtr->taskFunc = TaskPtr;
        (hscheduler->tasksCount)++;
        Task_ID = hscheduler->tasksCount;
    }

    return Task_ID;
}
/* By default after registering a task this will start running within the scheduler, 
this function can prevent it from being dispatched. The second parameter indicates 
the task to be stopped, which is a number from 1 to n task registered. Number zero is forbidden. 
the function will return a TRUE if the task was stopped otherwise returns FALSE */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
    uint8_t Task_stopped = FALSE;

    return Task_stopped;
}

/* Once a task is stopped using the function HIL_SCHEDULER_StopTask, it can be active again 
using this function. The second parameter indicates the task to be started, which is a number 
from 1 to n task registered. Number zero is forbidden. the function will return a TRUE if the 
task was stopped otherwise returns FALSE */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
    uint8_t Task_start = FALSE;

    return Task_start;
}

/* The new periodicity shall be a multiple of the tick value otherwise won’t be affected 
by the new period. The second parameter indicates the task to be started, which is a number 
from 1 to n task registered. Number zero is forbidden. the function will return a TRUE if the 
task was stopped otherwise returns FALSE. */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t Period )
{
    uint8_t Task_Period = FALSE;

    if( (Period > (hscheduler->tick)) && (Period % (hscheduler->tick) == MULTIPLE) )
    {
        Task_Period = TRUE;
        ((hscheduler->taskPtr) + task)->period = Period;
    }

    return Task_Period;
}

/* This is the function in charge of running the task init functions one single time 
and actual run each registered task according to their periodicity in an infinite loop, 
the function will never return at least something wrong happens, but this will be considered 
a malfunction. A timer will need to accomplish this purpose, for practical reasons, we can use 
the Systick Timer and the functions that come with the HAL library to control it. */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler )
{

}