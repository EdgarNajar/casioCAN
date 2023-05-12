/**
 * @file    app_ints.c
 * @brief   **file of interrupt functions**
 *
 * file of interrupt functions
 *
 * @note    check startup_stm32g0b1.S file
 */
 
#include "app_bsp.h"

/**
 * @brief  structure type variable for CAN handler
 */

extern FDCAN_HandleTypeDef CANHandler;

/**
 * @brief   **program entry point**
 *
 * program entry point
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void NMI_Handler( void )
{

}

/**
 * @brief   **program entry point**
 *
 * program entry point
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**
 * @brief   **program entry point**
 *
 * program entry point
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void SVC_Handler( void )
{

}

/**
 * @brief   **program entry point**
 *
 * program entry point
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void PendSV_Handler( void )
{

}

/**
 * @brief   **program entry point**
 *
 * program entry point
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

/**
 * @brief   **CAN interrupt handler**
 *
 * function to handle the CAN interrupt
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */

void TIM16_FDCAN_IT0_IRQHandler( void )
{
    HAL_FDCAN_IRQHandler( &CANHandler );
}