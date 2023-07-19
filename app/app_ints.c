/**
 * @file    app_ints.c
 * @brief   **File of interrupt vectors**
 *
 * File of the interrupt vectors, to be able to process the information recieved
 * through CAN protocol
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
 * Activates the CAN interrupt vector on TIM16
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void TIM16_FDCAN_IT0_IRQHandler( void )
{
    HAL_FDCAN_IRQHandler( &CANHandler );
}

/**
 * @brief   **RTC alarm interrupt vector**
 *
 * Activates the RTC alarm interrupt vector
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void RTC_TAMP_IRQHandler( void )
{
    HAL_RTC_AlarmIRQHandler( &hrtc );
}