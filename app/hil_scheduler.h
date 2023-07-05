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
#define NO_TASK      (uint8_t)0   /*!< To indicate there is no task                */
#define NO_DATA      (uint8_t)0   /*!< To indicate there is no data                */
#define MULTIPLE     (uint32_t)0  /*!< To indicate the period is a mutiple of tick */
#define FALSE        (uint8_t)0   /*!< FALSE                                       */
#define TRUE         (uint8_t)1   /*!< TRUE                                        */
#define STOP_TASK    (uint32_t)0  /*!< To indicate a task has been stopped         */
#define START_TASK   (uint32_t)1  /*!< To indicate a task has been started         */
#define STOP_TIMER   (uint32_t)0  /*!< To indicate a timer has been stopped        */
#define START_TIMER  (uint32_t)1  /*!< To indicate a timer has been started        */
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
    uint32_t StopStart;       /*!< Flag to stop or start a task        */
    void (*initFunc)(void);   /*!< Pointer to init task function       */
    void (*taskFunc)(void);   /*!< Pointer to task function            */
}Task_TypeDef;

/**
  * @brief   Timer control structure
  */
typedef struct _Timer_TypeDef
{
    uint32_t Timeout;          /*!< Timer timeout to decrement and reload when the timer is re-started */
    uint32_t Count;            /*!< Actual timer decrement count                                       */
    uint32_t StartFlag;        /*!< Flag to start timer count                                          */
    void(*callbackPtr)(void);  /*!< Pointer to callback function function                              */
} Timer_TypeDef;

/**
  * @brief   Scheduler control structure
  */
typedef struct _scheduler
{
    uint32_t tasks;          /*!< Number of task to handle            */
    uint32_t tick;           /*!< The time base in ms                 */
    uint32_t tasksCount;     /*!< Internal task counter               */
    Task_TypeDef *taskPtr;   /*!< Pointer to buffer for the TCB tasks */
    uint32_t timers;         /*!< Number of software timer to use     */
    Timer_TypeDef *timerPtr; /*!< Pointer to buffer timer array       */
}Scheduler_HandleTypeDef;

extern void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler );
extern uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period );
extern uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
extern uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task );
extern uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t Period );
extern void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler );

extern uint8_t HIL_SCHEDULER_RegisterTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timeout, void (*CallbackPtr)(void) );
extern uint32_t HIL_SCHEDULER_GetTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer );
extern uint8_t HIL_SCHEDULER_ReloadTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer, uint32_t Timeout );
extern uint8_t HIL_SCHEDULER_StartTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer );
extern uint8_t HIL_SCHEDULER_StopTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer );

#endif
