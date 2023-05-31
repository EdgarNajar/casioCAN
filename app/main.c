/**
 * @file    main.c
 * @brief   **Main file with the calls to all functions needed**
 *
 * This file contains the implementation of the functions needed to run a clock
 * using CAN protocol 
 *
 * @note    None
 *
 */
#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

static void heart_init( void );
static void heart_beat( void );
static void dog_init( void );
static void peth_the_dog( void );

/**
* @brief  Structure type variable to initialize the window watchdog
*/
static WWDG_HandleTypeDef hwwdg;

/**
* @brief  Variable to count the amount of milliseconds in function heart_beat
*/
static uint32_t tick_heart;

/**
* @brief  Variable to count the amount of milliseconds in function peth_the_dog
*/
static uint32_t tick_dog;

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
    HAL_Init();
    Serial_Init();
    Clock_Init();
    heart_init();
    dog_init();

    for( ;; )
    {
        Serial_Task();
        Clock_Task();
        heart_beat();
        peth_the_dog();
    }
}

/**
 * @brief   **Function to initialize the pin0 in port c**
 *
 * Farameters to initialize the pin5 in port A, in mode push-pull,
 * with no pull-up or pull-down and low frecuency speed
 *
 * @param   tick_heart [out] Equals to the value of function HAL_GetTick
 *
 * @note None
 */
void heart_init( void )
{
    __HAL_RCC_GPIOA_CLK_ENABLE();          

    GPIO_InitTypeDef GPIO_InitStruct;

    tick_heart = HAL_GetTick();
  
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
 * @param   tick_heart [in/out] Store the amount of millisecons transcurred
 *
 * @note    None
 */
void heart_beat( void )
{
    if( (HAL_GetTick() - tick_heart) >= TOGGLE_HEART )
    {
        tick_heart = HAL_GetTick();
        HAL_GPIO_TogglePin( GPIOA, GPIO_PIN_5 );
    }
}

/**
 * @brief   **Function to initialize the window watchdog**
 *
 * Initialize all parameters needed to operate the window watchdog
 * between 311 milliseconds and 196 milliseconds.
 *
 * @param   tick_dog [out] Equals to the value of function HAL_GetTick
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
    tick_dog = HAL_GetTick();

    __HAL_RCC_WWDG_CLK_ENABLE();

    hwwdg.Instance       = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_128;
    hwwdg.Init.Window    = VAL_WINDOW;
    hwwdg.Init.Counter   = VAL_COUNTER;
    hwwdg.Init.EWIMode   = WWDG_EWI_DISABLE;
    
    HAL_WWDG_Init( &hwwdg );
}

/**
 * @brief   **Function to refresh the parameters of the window watchdog**
 *
 * This function refresh the window watchdog every 220 miliseconds
 *
 * @param   tick_dog [in/out] Store the amount of millisecons transcurred
 *
 * @note    None
 */
void peth_the_dog( void )
{
    if( (HAL_GetTick() - tick_dog) >= REFRESH_WWDG )
    {
        tick_dog = HAL_GetTick();
        HAL_WWDG_Refresh( &hwwdg );
    }
}
