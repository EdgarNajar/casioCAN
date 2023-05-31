/**
 * @file    app_msps.c
 * @brief   **Clock frequencies and peripherals configuration for CasioCAN**
 *
 * File with the functions of the auxiliar initializations of HAL library,
 * here will be the declaration and configuration of the pins to control
 * each part of the clock, such as the FDCAN pins
 *
 * @note    None
 */
#include "app_bsp.h"

/**
 * @brief   **CPU's microcontrollers speed**
 *
 * Increase the clock speed of the microcontroller to 64MHz
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_MspInit( void )
{
    
}

/**
 * @brief   **CAN pin configuration**
 *
 * Function with the pin configuration of GPIOD pin 0(rx) and pin 1(tx)
 * in altern mode for FDCAN1
 *
 * @note None
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

/**
 * @brief   **RTC configuration**
 *
 * Function with the configuration for RTC peripheral 
 * to run with the external ion board LSE cristal
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
/* cppcheck-suppress misra-c2012-2.7 ; function defined in HAL library */
void HAL_RTC_MspInit( RTC_HandleTypeDef* hrtc ) 
{
    RCC_OscInitTypeDef        RCC_OscInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* Enable backup domain */
    HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG( RCC_LSEDRIVE_LOW );

    /* Reset previous RTC source clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection    = RCC_RTCCLKSOURCE_NONE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    
    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    HAL_RCC_OscConfig( &RCC_OscInitStruct );

    /* Set LSE as source clock */
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
      
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
}