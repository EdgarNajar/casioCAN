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
#include "hel_lcd.h"
/**
 * @brief   **CPU's microcontrollers speed**
 *
 * Increase the clock speed of the microcontroller to 64MHz.
 * Initializes the RCC Oscillators according to the specified parameters 
 * in the RCC_OscInitTypeDef structure.
 *
 * @note    The frequency set is 64MHz with the internal 16MHz HSI oscilator. 
 *          According to the formulas:
 *          fVCO = fPLLIN x ( N / M ) = 16MHz x (8 / 1) = 128MHz
 *          fPLLP = fVCO / P = 128MHz / 2 = 64MHz
 *          fPLLQ = fVCO / Q = 128MHz / 2 = 64MHz
 *          fPLLR = fVCO / R = 128MHz / 2 = 64MHz
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_MspInit( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Configure the main internal regulator output voltage */
    Status = HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, PWR_CONVOLSCA_RET_ERROR );

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN            = 8;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    Status = HAL_RCC_OscConfig( &RCC_OscInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_OSCCON_RET_ERROR );
    
    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV2;
    Status = HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_CLOCKCON_RET_ERROR );

    // RCC_OscInitTypeDef        RCC_OscInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
        
    /* Enable backup domain */
    Status = HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, PWR_CONVOLSCA_RET_ERROR );

    HAL_PWR_EnableBkUpAccess();

    __HAL_RCC_LSEDRIVE_CONFIG( RCC_LSEDRIVE_LOW );

    /* Reset previous RTC source clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_NONE;
    Status = HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_PERCLKCON_RET_ERROR );

    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    Status = HAL_RCC_OscConfig( &RCC_OscInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_OSCCON_RET_ERROR );

    /* Set LSE as source clock */
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    Status = HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_PERCLKCON_RET_ERROR );
        
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
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
    Status = HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, PWR_CONVOLSCA_RET_ERROR );

    HAL_PWR_EnableBkUpAccess();

    __HAL_RCC_LSEDRIVE_CONFIG( RCC_LSEDRIVE_LOW );
    
    /* Reset previous RTC source clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_NONE;
    Status = HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_PERCLKCON_RET_ERROR );

    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    Status = HAL_RCC_OscConfig( &RCC_OscInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_OSCCON_RET_ERROR );

    /* Set LSE as source clock */
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    Status = HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, RCC_PERCLKCON_RET_ERROR );
        
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
}

/**
 * @brief   **SPI**
 *
 * Function to initialize the SPI pins
 *
 * @param   hspi [in] Pointer to handle the SPI
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
/* cppcheck-suppress misra-c2012-2.7 ; function defined in HAL library */
void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi )
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __GPIOD_CLK_ENABLE();
    __SPI1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_SPI1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief   **LCD**
 *
 * Function to initialize the LCD pins
 *
 * @param   hlcd [in] Pointer to handle the LCD
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in hel_lcd.h file */
/* cppcheck-suppress misra-c2012-8.6 ; function defined in hel_lcd.h file */
void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __GPIOD_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin   = hlcd->RstPin | hlcd->CsPin | hlcd->RsPin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
    HAL_GPIO_WritePin( GPIOD, hlcd->CsPin, SET );
    
    GPIO_InitStruct.Pin   = hlcd->BklPin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
}