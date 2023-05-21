/**
 * @file    app_ints.c
 * @brief   **File of interrupt functions**
 *
 * File of interrupt functions
 *
 * @note    Check startup_stm32g0b1.S file
 */
#include "app_bsp.h"

/**
 * @brief   **Program entry point**
 *
 * Program entry point
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void NMI_Handler( void )
{

}

/**
 * @brief   **Program entry point**
 *
 * Program entry point
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**
 * @brief   **Program entry point**
 *
 * Program entry point
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void SVC_Handler( void )
{

}

/**
 * @brief   **Program entry point**
 *
 * Program entry point
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void PendSV_Handler( void )
{

}

/**
 * @brief   **Program entry point**
 *
 * Program entry point
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

/**
 * @brief   **CAN interrupt vector**
 *
 * Exception handler to the Default_Handler
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void TIM16_FDCAN_IT0_IRQHandler( void )
{
    HAL_FDCAN_IRQHandler( &CANHandler );
}