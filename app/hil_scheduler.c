/**
 * @file    hil_scheduler.c
 * @brief   **Scheduler driver**
 *
 * This file contains a scheduler to set each task periodicity 
 * and ensure each run according to the time established.
 * 
 * @note    None
 */
#include "hil_scheduler.h"

/** 
  * @defgroup Defines of numbers
  @{ */
#define NUM_0      (uint32_t)0   /*!< Number 0  */
#define NUM_1      (uint32_t)1   /*!< Number 1  */
#define NUM_5      (uint32_t)5   /*!< Number 5  */
#define NUM_10     (uint32_t)10  /*!< Number 10 */
/**
  @} */

/**
 * @brief   **Initialization of scheduler**
 *
 * To initialize the scheduler this function checks for the
 * parameters pased to be valid
 * 
 * @param   hscheduler [in] Pointer to structure data
 *
 * @note None
 */
void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->taskPtr != NULL), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tasks != NO_DATA), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tasksCount == NO_DATA), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tick != NO_DATA), SQUEDULER_PAR_ERROR );
}

/**
 * @brief   **Register of task**
 *
 * The function will set the task TCB with the following parameters, the address of the function 
 * to hold the init routine for the given task and the address for the actual routine that will run 
 * as the task, plus the periodicity in milliseconds of the task to register, it will validate
 * if the periodicity is not less than the tick value and if it is a multiple.
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   InitPtr    [in] The address of the init function routine
 * @param   TaskPtr    [in] The address of the task function routine
 * @param   Period     [in] Periodicity in milliseconds
 *
 * @retval  The function return a Task ID which will be a number from 1 to n task registered 
 *          if the operation was a success, otherwise, it will return zero.
 * 
 * @note None
 */
uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (InitPtr != NULL), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (TaskPtr != NULL), SQUEDULER_PAR_ERROR );    
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Period != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t Task_ID = NO_TASK;

    if( (Period > (hscheduler->tick)) && (Period % (hscheduler->tick) == MULTIPLE) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) += (hscheduler->tasksCount);

        (hscheduler->taskPtr)->elapsed  = NUM_0;
        (hscheduler->taskPtr)->initFunc = InitPtr;
        (hscheduler->taskPtr)->period   = Period;
        (hscheduler->taskPtr)->taskFunc = TaskPtr;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) -= (hscheduler->tasksCount);
        (hscheduler->tasksCount)++;
        Task_ID = hscheduler->tasksCount;
    }

    return Task_ID;
}

/**
 * @brief   **Stop a task**
 *
 * This function will prevent a task for being dispatched
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   task       [in] Task to be stopped
 *
 * @retval  The function returns TRUE if the task was stopped otherwise returns FALSE
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (task != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t Task_stopped = FALSE;

    if( task <= (hscheduler->tasks) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to obtain task pointer */
        (hscheduler->taskPtr) += (task - NUM_1);
        (hscheduler->taskPtr)->StopStart = STOP_TASK;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) -= (task - NUM_1);
        Task_stopped = TRUE;
    }

    return Task_stopped;
}

/**
 * @brief   **Start a task**
 *
 * This function will start a task previously stopped
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   task       [in] Task to be started
 *
 * @retval  The function returns TRUE if the task was started otherwise returns FALSE
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (task != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t Task_start = FALSE;

    if( task <= (hscheduler->tasks) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to obtain task pointer */
        (hscheduler->taskPtr) += (task - NUM_1);
        (hscheduler->taskPtr)->StopStart = START_TASK;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) -= (task - NUM_1);
        Task_start = TRUE;
    }

    return Task_start;
}

/**
 * @brief   **Change a task periodicity**
 *
 * This function will change the periodicity of a given task, it shall be a multiple of the tick value
 * otherwise won’t be affected by the new period
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   task       [in] The address of the task function routine
 * @param   Period     [in] Periodicity in milliseconds
 *
 * @retval  The function returns TRUE if the periodicity of the task was changed otherwise returns FALSE.
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t Period )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (task != NO_DATA), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Period != NO_DATA), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tick != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t Task_Period = FALSE;

    if( (Period > (hscheduler->tick)) && (Period % (hscheduler->tick) == MULTIPLE) )
    {
        Task_Period = TRUE;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) += (task - NUM_1);
        (hscheduler->taskPtr)->period = Period;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) -= (task - NUM_1);
    }

    return Task_Period;
}

/**
 * @brief   **Run the different tasks**
 *
 * This function will run the task init functions one single time and run each registered task
 * according to their periodicity in an infinite loop. The Systick Timer and the functions that 
 * come with the HAL library to control it will be used to accomplish this purpose
 *  
 * @param   hscheduler [in] Pointer to structure data
 *
 * @note None
 */
void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->taskPtr != NULL), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tasks != NO_DATA), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->tick != NO_DATA), SQUEDULER_PAR_ERROR );

    TIM_HandleTypeDef TIM6_Handler = {NUM_0};
    TIM6_Handler.Instance          = TIM6;
    TIM6_Handler.Init.Prescaler    = TIM6_PRESCALER;
    TIM6_Handler.Init.CounterMode  = TIM_COUNTERMODE_UP;
    TIM6_Handler.Init.Period       = TIM6_PERIOD;
    HAL_TIM_Base_Init( &TIM6_Handler );
    HAL_TIM_Base_Start_IT( &TIM6_Handler );

    uint32_t x = NUM_0;
    uint32_t count;
    uint32_t TIM6_count1;
    uint32_t TIM6_count2;
    uint32_t TIM6_count3;
    uint32_t monitor;
    uint32_t errors[NUM_5] = { SCHEDULER_SERIAL_ERROR, SCHEDULER_CLOCK_ERROR, SCHEDULER_HEARTBEAT_ERROR, 
                               SCHEDULER_WWDG_ERROR, SCHEDULER_DISPLAY_ERROR };

    count = HAL_GetTick();

    while( x != (hscheduler->tasks) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) += x;
        (hscheduler->taskPtr)->initFunc();
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->taskPtr) -= x;
        x++;
    }

    TIM6_count1 = __HAL_TIM_GET_COUNTER( &TIM6_Handler );

    while( 1 )
    {
        if( (HAL_GetTick() - count) >= (hscheduler->tick) )
        {
            count = HAL_GetTick();

            for( uint32_t i = 0; i < (hscheduler->tasks) ; i++ )
            {
                /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
                (hscheduler->taskPtr) += i;

                if( ((hscheduler->taskPtr)->elapsed) >= ((hscheduler->taskPtr)->period) )
                {
                    TIM6_count2 = __HAL_TIM_GET_COUNTER( &TIM6_Handler );
                    TIM6_count3 = TIM6_count2 - TIM6_count1;
                    monitor = (hscheduler->taskPtr)->period + (((hscheduler->taskPtr)->period) / NUM_10);
                    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
                    assert_error( TIM6_count3 <= monitor, errors[i] );
                    TIM6_count1 = __HAL_TIM_GET_COUNTER( &TIM6_Handler );
                    (hscheduler->taskPtr)->taskFunc();
                    (hscheduler->taskPtr)->elapsed = NUM_0;
                }

                (hscheduler->taskPtr)->elapsed += hscheduler->tick;
                /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
                (hscheduler->taskPtr) -= i;
            }
        }
    }
}
