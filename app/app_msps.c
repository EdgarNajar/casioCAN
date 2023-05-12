/**
 * @file    app_msps.c
 * @brief   **auxiliar library functions**
 *
 * file with the functions of the auxiliar initializations of HAL library
 *
 * @note    none
 */
 
#include "app_bsp.h"
#include "app_serial.h"

/**
 * @brief   **CPU's microcontrollers speed**
 *
 * increase the clock speed of the microcontroller to 64MHz
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_MspInit( void )
{
    
}

/**
 * @brief   **CAN pin configuration**
 *
 * function with the pin configuration of GPIOD pin 0(rx) and pin 1(tx)
 * in altern mode for FDCAN1
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
/* cppcheck-suppress misra-c2012-2.7 ; function defined in HAL library */
void HAL_FDCAN_MspInit( FDCAN_HandleTypeDef *hfdcan )
{
    GPIO_InitTypeDef GpioCanStruct;

    __HAL_RCC_FDCAN_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GpioCanStruct.Mode      = GPIO_MODE_AF_PP;
    GpioCanStruct.Alternate = GPIO_AF3_FDCAN1;
    GpioCanStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    GpioCanStruct.Pull      = GPIO_NOPULL;
    GpioCanStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOD, &GpioCanStruct );
        
    HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
}
