/**
 * @file    hil_scheduler.h
 * @brief   **Scheduler driver**
 *
 * This file contains a scheduler to set each task periodicity 
 * and ensure each run according to the time established.
 * 
 * @note    None
 */
#ifndef HIL_SCHEDULER_H__
#define HIL_SCHEDULER_H__

#include "app_bsp.h"

/** 
  * @defgroup Values to the scheduler
  @{ */
#define NO_TASK    (uint8_t)0   /*!< To indicate there is no task                */
#define NO_DATA    (uint8_t)0   /*!< To indicate there is no data                */
#define MULTIPLE   (uint32_t)0  /*!< To indicate the period is a mutiple of tick */
#define FALSE      (uint8_t)0   /*!< FALSE                                       */
#define TRUE       (uint8_t)1   /*!< TRUE                                        */
/**
  @} */

/** 
  * @defgroup Defines of numbers
  @{ */
#define STOP_TASK      (uint32_t)0  /*!< To indicate a task has been stopped */
#define START_TASK     (uint32_t)1  /*!< To indicate a task has been started */
/**
  @} */

/** 
  * @defgroup Parameters for TIM6
  @{ */
#define TIM6_PRESCALER (uint32_t)32000   /*!< Value of TIM6 prescaler */
#define TIM6_PERIOD    (uint32_t)0xFFFF  /*!< Value of TIM6 period    */
/**
  @} */

/**
  * @brief   Task control block
  */
typedef struct _task
{
    uint32_t period;          /*!< How often the task should run in ms */
    uint32_t elapsed;         /*!< The current elapsed time            */
    uint32_t StopStart;     /*!< Flag to stop or start a task        */
    void (*initFunc)(void);   /*!< Pointer to init task function       */
    void (*taskFunc)(void);   /*!< Pointer to task function            */
}Task_TypeDef;

/**
  * @brief   Scheduler control structure
  */
typedef struct _scheduler
{
    uint32_t tasks;         /*!< Number of task to handle            */
    uint32_t tick;          /*!< The time base in ms                 */
    uint32_t tasksCount;    /*!< Internal task counter               */
    Task_TypeDef *taskPtr;  /*!< Pointer to buffer for the TCB tasks */
}Scheduler_HandleTypeDef;

extern void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler );
extern uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period );
extern uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
extern uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
extern uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t Period );
extern void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler );

#endif
