/**
 * @file    main.c
 * @brief   **Main file with the calls to all functions nedded**
 *
 * This file contains the implementation of the functions nedded to run a clock
 * using CAN protocol 
 *
 * @note    None
 *
 */
#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "app_display.h"
#include "hil_scheduler.h"

static void heart_init( void );
static void heart_beat( void );
static void dog_init( void );
static void peth_the_dog( void );

/**
* @brief  Structure type variable to initialize the window watchdog
*/
static WWDG_HandleTypeDef hwwdg;

/**
 * @brief  Struct type variable to functional safety
 */
HAL_StatusTypeDef Status;

/**
 * @brief   **Implementation of the main program**
 *
 * Contains the calls to all the functions nedded to run the clock
 *
 * @note None
 *
 */
int main( void )
{
    static Scheduler_HandleTypeDef SchedulerHandler;
    Task_TypeDef hTasks[ TASKS_NUMBER ];

    HAL_Init();

    SchedulerHandler.taskPtr    = hTasks;
    SchedulerHandler.tasks      = TASKS_NUMBER;
    SchedulerHandler.tasksCount = NUM_0;
    SchedulerHandler.tick       = SCHEDULER_TICK;
    HIL_SCHEDULER_Init( &SchedulerHandler );

    (void)HIL_SCHEDULER_RegisterTask( &SchedulerHandler, Serial_Init, Serial_Task, TEN_MS );
    (void)HIL_SCHEDULER_RegisterTask( &SchedulerHandler, Clock_Init, Clock_Task, FIFTY_MS );
    (void)HIL_SCHEDULER_RegisterTask( &SchedulerHandler, heart_init, heart_beat, TOGGLE_HEART );
    (void)HIL_SCHEDULER_RegisterTask( &SchedulerHandler, dog_init, peth_the_dog, REFRESH_WWDG );
    (void)HIL_SCHEDULER_RegisterTask( &SchedulerHandler, Display_Init, Display_Task, HUNDRED_MS );
    HIL_SCHEDULER_Start( &SchedulerHandler );
}

/**
 * @brief   **Function to initialize the heartbeat**
 *
 * Parameters to initialize the pin5 in port A, in mode push-pull,
 * with no pull-up or pull-down and low frecuency speed
 *
 * @note None
 */
void heart_init( void )
{
    __HAL_RCC_GPIOA_CLK_ENABLE();     

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin   = GPIO_PIN_5;  
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
}

/**
 * @brief   **Heartbeat**
 *
 * This function check for the millisecons transcurrent 
 * to toggle a led every 300 millisecons in the pin 5 of port GPIOA. 
 *
 * @note    None
 */
void heart_beat( void )
{
    HAL_GPIO_TogglePin( GPIOA, GPIO_PIN_5 );
}

/**
 * @brief   **Function to initialize the window watchdog**
 *
 * Initialize all parameters needed to operate the window watchdog
 * between 311 milliseconds and 196 milliseconds.
 *
 * @param   hwwdg    [out] Structure type variable to initialize window watchdog
 *
 * @note    The formula to calculate the timeout value is given by:
 *          tWWDG = tPCLK*4096*(2^WDGTB[1:0])*(T[5:0]+1)      (ms)
 * 
 *          where:
 *          tWWDG: WWDG timeout
 *          tPCLK: APB clock period measured in ms
 *          4096: value corresponding to internal divider
 *          
 *          counter = (1/32000000)(4096)(128)(127-115) = 196ms
 *                                                
 *          window  = (1/32000000)(4096)(128)(127-108) = 311ms
 */
void dog_init( void )
{
    __HAL_RCC_WWDG_CLK_ENABLE();

    hwwdg.Instance       = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_128;
    hwwdg.Init.Window    = VAL_WINDOW;
    hwwdg.Init.Counter   = VAL_COUNTER;
    hwwdg.Init.EWIMode   = WWDG_EWI_DISABLE;
    
    Status = HAL_WWDG_Init( &hwwdg );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, WWDG_RET_ERROR );
}

/**
 * @brief   **Function to refresh the parameters of the window watchdog**
 *
 * This function refresh the window watchdog, 
 * the milliseconds are established in REFRESH_WWDG
 *
 * @note    None
 */
void peth_the_dog( void )
{
        Status = HAL_WWDG_Refresh( &hwwdg );
        /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
        assert_error( Status == HAL_OK, WWDG_REFRESH_RET_ERROR );
}

/**
 * @brief   **Safe state**
 *
 * This function is the safe state where all the necessary steps will be taken 
 * in order to avoid any fatal functionality for the system itself and the user.
 * First we disable all maskable interrupts, then the CAN pins, the SPI pins, 
 * the LCD pins, the LCD backlight pin and the Heartbeat pin will be set as inputs
 * to avoid any issue with short circuit, next all timers included the WWDG will be disabled
 * and leds connected to port C will be used to output the error code, finally we
 * just wait for the user to press the reset button to start running the program again.
 * 
 * @param   file  [in] In wich file the error was detected
 * @param   line  [in] In wich line the error was detected
 * @param   error [in] Wich error was detected
 *
 * @note    None
 */
void safe_state( uint8_t *file, uint32_t line, uint8_t error )
{
    UNUSED( file );
    UNUSED( line );

    /* Disable all maskable interrupts */
    __disable_irq();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
    __HAL_RCC_PWR_CLK_DISABLE();
    __HAL_RCC_RTC_DISABLE();
    __HAL_RCC_RTCAPB_CLK_DISABLE();
    __HAL_RCC_FDCAN_CLK_DISABLE();
    // __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_SPI1_CLK_DISABLE();
    // __HAL_RCC_GPIOB_CLK_DISABLE();
    // __HAL_RCC_GPIOA_CLK_DISABLE();     
    
    /* Set all outputs to a safe state, you must think what will be the so called safe state 
    for the pins and peripherals */

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* CAN pins */
    GPIO_InitTypeDef GpioCanStruct;

    GpioCanStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    GpioCanStruct.Mode      = GPIO_MODE_INPUT;
    GpioCanStruct.Pull      = GPIO_NOPULL;
    GpioCanStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOD, &GpioCanStruct );

    /* SPI pins */
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin   = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* LCD pins */
    GPIO_InitStruct.Pin   = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
    
    /* LCD backlight pin */
    GPIO_InitStruct.Pin   = GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );

    /* Heartbeat pin */
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    
    /* Disable all timers included the WWDG */

    /* Output the error code using the leds connected to port C */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin   = PORTC; 
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;     
    GPIO_InitStruct.Pull  = GPIO_NOPULL;             
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    
    
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
    
    HAL_GPIO_WritePin( GPIOC, PORTC , RESET );
    HAL_GPIO_WritePin( GPIOC, error , SET );

    while( 1 )
    {
        /* Waiting for the user to press the reset button */
    }
}