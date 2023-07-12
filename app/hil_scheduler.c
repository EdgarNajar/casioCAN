/**
 * @file    hil_scheduler.c
 * @brief   **Scheduler driver**
 *
 * This file contains a scheduler to set each task periodicity and ensure 
 * each one  run according to the time established. The version we will use 
 * is the Round Robin in wich the tasks run after the other considering a tick 
 * of time to accomplish each task.
 * We will make use of functions to initialize the scheduler, to register each task 
 * with the parameters necessary, to stop and start a task, to keep track of 
 * the periodicity of the tasks and to start the scheduler and run the tasks.
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
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->timerPtr != NULL), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->timers != NO_DATA), SQUEDULER_PAR_ERROR );
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

        (hscheduler->taskPtr)->elapsed   = NUM_0;
        (hscheduler->taskPtr)->initFunc  = InitPtr;
        (hscheduler->taskPtr)->period    = Period;
        (hscheduler->taskPtr)->taskFunc  = TaskPtr;
        (hscheduler->taskPtr)->StopStart = TRUE;
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
                    
                    if( (hscheduler->taskPtr)->StopStart == TRUE )
                    {
                        (hscheduler->taskPtr)->taskFunc();
                        (hscheduler->taskPtr)->elapsed = NUM_0;
                    }
                }

                /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
                (hscheduler->taskPtr) -= i;
            }

            for( uint32_t j = 0; j < (hscheduler->timers) ; j++ )
            {
                /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
                (hscheduler->timerPtr) += j;

                if( ((hscheduler->timerPtr)->Count) == NUM_0 )
                {
                    (hscheduler->timerPtr)->Count = (hscheduler->timerPtr)->Timeout;
                    (hscheduler->timerPtr)->callbackPtr();
                }
                
                if( ((hscheduler->timerPtr)->StartFlag) == TRUE )
                {
                    (hscheduler->timerPtr)->Count -= (hscheduler->tick);
                }

                /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
                (hscheduler->timerPtr) -= j;
            }
        }
    }
}

/**
 * @brief   **Register each timer**
 *
 * This function will register the timers to run every task of the scheduler,
 * the timer will count from a timeout value down to zero, and once its count ended 
 * the timer will stop and execute the function callback registered,in case 
 * no callback is needed a NULL will be passed. In order to accept a timer it has 
 * to be larger than the actual tick and multiple 
 * 
 * @param   hscheduler  [in] Pointer to structure data
 * @param   Timeout     [in] Value to initiate down counter
 * @param   CallbackPtr [in] Pointer to callback function
 *
 * @retval  The function returns an ID which is a value from 1 to the number of timer registers in the 
 *          scheduler, otherwise, it will return zero indicating the timer couldn't be registered
 * 
 * @note None
 */
uint8_t HIL_SCHEDULER_RegisterTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timeout, void (*CallbackPtr)(void) )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hscheduler->timerPtr != NULL), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (CallbackPtr != NULL), SQUEDULER_PAR_ERROR );    
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timeout != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t Timer_ID = NO_TASK;

    if( (Timeout > (hscheduler->tick)) && (Timeout % (hscheduler->tick) == MULTIPLE) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) += (hscheduler->timers);

        (hscheduler->timerPtr)->callbackPtr  = CallbackPtr;
        (hscheduler->timerPtr)->Timeout      = Timeout;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) -= (hscheduler->timers);
        (hscheduler->timers)++;
        Timer_ID = hscheduler->timers;
    }

    return Timer_ID;
}

/**
 * @brief   **Get current timer**
 *
 * This function get the current timer pending time in milliseconds
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   timer      [in] Current timer
 *
 * @retval  The function returns the current time of a timer or zero if not register
 *
 * @note None
 */
uint32_t HIL_SCHEDULER_GetTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timer != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t pending_time = FALSE;

    if( Timer <= (hscheduler->timers) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) += (Timer - NUM_1);
        pending_time = ((hscheduler->timerPtr)->Timeout) - (hscheduler->tick);
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) -= (Timer - NUM_1);
    }

    return pending_time;
}

/**
 * @brief   **Reload timer**
 *
 * This function will reload the time of a timer with a new value in milliseconds 
 * it will also start the timer, the function does not require the timer to stop first, 
 * but if the timer to reload has not been registered no action will be taken
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   timer      [in] Timer to reload
 * @param   Timeout    [in] Periodicity in milliseconds
 *
 * @retval  The function returns TRUE if the periodicity of the task was changed, otherwise returns FALSE
 *
 * @note None
 */
uint8_t HIL_SCHEDULER_ReloadTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer, uint32_t Timeout )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timer > NO_DATA) && (Timer <= (hscheduler->timers)), SQUEDULER_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timeout != NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t reload = FALSE;

    if( (Timeout > (hscheduler->tick)) && (Timeout % (hscheduler->tick) == MULTIPLE) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) += (Timer - NUM_1);
        (hscheduler->timerPtr)->Count = Timeout;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) -= (Timer - NUM_1);
        reload = TRUE;
    }

    return reload;
}

/**
 * @brief   **Start timer**
 *
 * This function start the timer registered when its called, 
 * the function will also serve as a mechanism of restart the timer 
 * from its timeout. If the timer to start has not been registered 
 * no action will be taken
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   timer      [in] Timer to start
 *
 * @retval  The function returns one as an indication of success, otherwise returns zero
 *
 * @note None
 */
uint8_t HIL_SCHEDULER_StartTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timer > NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t start_timer = FALSE;

    if( Timer <= (hscheduler->timers) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to obtain task pointer */
        (hscheduler->timerPtr) += (Timer - NUM_1);
        (hscheduler->timerPtr)->StartFlag = START_TIMER;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) -= (Timer - NUM_1);
        start_timer = TRUE;
    }

    return start_timer;
}

/**
 * @brief   **Stop timer**
 *
 * The function will indicate the timer should not be 
 * decremented during each tick occurrence. If the timer 
 * to stop has not been registered no action will be taken.
 * 
 * @param   hscheduler [in] Pointer to structure data
 * @param   Timer      [in] Timer to be stop
 *
 * @retval  It will return a zero as an indication of success, otherwise return one.
 *
 * @note None
 */
uint8_t HIL_SCHEDULER_StopTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (Timer > NO_DATA), SQUEDULER_PAR_ERROR );

    uint8_t stop_timer = FALSE;

    if( Timer <= (hscheduler->timers) )
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to obtain task pointer */
        (hscheduler->timerPtr) += (Timer - NUM_1);
        (hscheduler->timerPtr)->StartFlag = STOP_TIMER;
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform count */
        (hscheduler->timerPtr) -= (Timer - NUM_1);
        stop_timer = TRUE;
    }

    return stop_timer;
}
