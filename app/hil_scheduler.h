/**
 * @file    hil_scheduler.h
 * @brief   ****
 *
 * 
 * 
 * @note    None
 */
#ifndef HIL_SCHEDULER_H__
#define HIL_SCHEDULER_H__

#include "app_bsp.h"

void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler );
uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period );
uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t period );
void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler );

typedef struct _scheduler
{
    uint32_t tasks;         /*number of task to handle*/
    uint32_t tick;          /*the time base in ms*/
    uint32_t tasksCount;    /*internal task counter*/
    Task_TypeDef *taskPtr;  /*Pointer to buffer for the TCB tasks*/
    //Add more elements if required
}Scheduler_HandleTypeDef;

typedef struct _task
{
    uint32_t period;          /*How often the task shopud run in ms*/
    uint32_t elapsed;         /*the cuurent elapsed time*/
    void (*initFunc)(void);   /*pointer to init task function*/
    void (*taskFunc)(void);   /*pointer to task function*/
    //Add more elements if required
}Task_TypeDef;

#endif
