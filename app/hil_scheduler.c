/**
 * @file    hil_scheduler.c
 * @brief   ****
 *
 * 
 * 
 * @note    None
 */
#include "hil_scheduler.h"

void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler )
{

}

uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period )
{

}

uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{

}

uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{

}

uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t period )
{

}

void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler )
{

}