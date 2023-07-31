/**
 * @file    app_display.c
 * @brief   **File to control the LCD**
 *
 * This file will control the LCD and the way the information is displayed in the LCD
 *
 * @note    None
 */
#include "app_display.h"
#include "hel_lcd.h"
#include "hil_queue.h"
#include "hil_scheduler.h"

static void Display_TimeString( APP_MsgTypeDef *tm );
static void Display_DateString( APP_MsgTypeDef *tm );
static void Display_StMachine( void );

/**
 * @brief  Struct type variable to handle the LCD
 */
static LCD_HandleTypeDef hlcd;

/**
 * @brief  Struct type variable to clock messages
 */
APP_MsgTypeDef ClockMsg;

/**
 * @brief  Struct type variable to handle the SPI
 */
SPI_HandleTypeDef SpiHandle;

/**
 * @brief  Struct type variable to handle TIM
 */
TIM_HandleTypeDef TIM14Handle;

/**
 * @brief  Struct type variable to handle the PWM
 */
TIM_OC_InitTypeDef sConfig;

/**
 * @brief  State of button
 */
uint8_t AlarmButton = NOT_PRESSED;

/**
 * @brief  Counter for one minute
 */
uint8_t CounterMin = NUM_0;

/**
 * @brief   **Initialization of the LCD**
 *
 * This function initialize the ports to start working with the LCD and to
 * configure the spi in master mode, full-duplex comunication, clock polarity high
 * and phase in falling edge, the SPI frequency to communicate with the LCD 
 * should be 5MHz minimum
 *
 * @param   hlcd [in/out] Structure type function to handle the LCD
 *
 * @note None
 */
void Display_Init( void )
{
    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;
    Status = HAL_SPI_Init( &SpiHandle );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, SPI_INIT_RET_ERROR );

    hlcd.RstPort    = GPIOD;
    hlcd.RstPin     = GPIO_PIN_2;
    hlcd.RsPort     = GPIOD;
    hlcd.RsPin      = GPIO_PIN_4;
    hlcd.CsPort     = GPIOD;
    hlcd.CsPin      = GPIO_PIN_3;
    hlcd.BklPort    = GPIOB;
    hlcd.BklPin     = GPIO_PIN_4;
    hlcd.SpiHandler = &SpiHandle;
    Status = HEL_LCD_Init( &hlcd );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, LCD_INIT_RET_ERROR );

    HEL_LCD_Backlight( &hlcd, LCD_ON );

    GPIO_InitTypeDef GPIO_InitStruct;               /*gpios initial structure*/
    
    __HAL_RCC_GPIOB_CLK_ENABLE();                   /*Enable clock on port B*/
  
    GPIO_InitStruct.Pin = GPIO_PIN_15;               /*pin to set as output*/
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;    /*input mode with interrupts enable on falling and rising edges*/    
    GPIO_InitStruct.Pull = GPIO_NOPULL;             /*no pull-up niether pull-down*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;    /*pin speed*/
    /*use the previous parameters to set configuration on pin B15*/
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
    
    /*Enable interrupt vector EXTI4_15_IRQ where the CPU will jump if a input change
    happens on pins 4 to 15*/
    HAL_NVIC_SetPriority( EXTI4_15_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( EXTI4_15_IRQn );
    
    /* Configuration of PWM */
    __HAL_RCC_TIM14_CLK_ENABLE();

    TIM14Handle.Instance           = TIM14;
    TIM14Handle.Init.Prescaler     = PRESCALER_TIM14;
    TIM14Handle.Init.Period        = PERIOD_TIM14;
    TIM14Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init( &TIM14Handle );

    sConfig.OCMode     = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse = PULSE_TIM14;
    HAL_TIM_PWM_ConfigChannel( &TIM14Handle, &sConfig, TIM_CHANNEL_1 );
    HAL_TIM_PWM_Start( &TIM14Handle, TIM_CHANNEL_1 );

}

/**
 * @brief   **Falling callback for pin C15**
 *
 * When the pin changes from high to low this function will be called 
 * by HAL_GPIO_EXTI_IRQHandler which is in turn called from the 
 * EXTI4_15_IRQHandler interrupt vector
 *
 * @param   GPIO_Pin     [in]   Pin to check
 * @param   AlarmButton  [out]  Flag of state of button
 * 
 * @note None
 */
/* */
/* cppcheck-suppress misra-c2012-2.7 ; function defined in HAL library */
void HAL_GPIO_EXTI_Falling_Callback( uint16_t GPIO_Pin )
{
    AlarmButton = IS_PRESSED;
}

/**
 * @brief   **Rising callback for pin C15**
 *
 * When the pin changes from low to high this function will be called 
 * by HAL_GPIO_EXTI_IRQHandler which is in turn called from the 
 * EXTI4_15_IRQHandler interrupt vector
 *
 * @param   GPIO_Pin     [in]   Pin to check
 * @param   AlarmButton  [out]  Flag of state of button
 * 
 * @note None
 */
/* cppcheck-suppress misra-c2012-2.7 ; function defined in HAL library */
void HAL_GPIO_EXTI_Rising_Callback( uint16_t GPIO_Pin )
{
    AlarmButton = IS_RELEASED;
}

/**
 * @brief   **Call of the display state machine**
 *
 * This function calls the display state machine every 100ms
 * with the help of queues, therefore it won't be execute all the time
 *
 * @note None
 */
void Display_Task( void )
{
    while( HIL_QUEUE_IsEmptyISR( &ClockQueue, SPI1_IRQn ) == NUM_0 )
    {
        /*Read the first message*/
        (void)HIL_QUEUE_ReadISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
        Display_StMachine();
    }
}

/**
 * @brief   **Display message processing**
 *
 * Implementation of the state machine in charge of messages processing 
 * from the clock task and display the time and date
 * 
 * @note  None
 */
void Display_StMachine( void )
{
    char alarm[NUM_14];

    switch( ClockMsg.msg )
    {
        case DISPLAY_MSG:
            Display_TimeString( &ClockMsg );
            Display_DateString( &ClockMsg );

            ClockMsg.msg = ALARM_ACTIVE;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case ALARM_ACTIVE:
            if( MSGHandler.alarm == ALARM_SET )
            {
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_Data( &hlcd, 'A' );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_DATA_RET_ERROR );
            }
            else
            {
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_Data( &hlcd, ' ' );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_DATA_RET_ERROR );
            }

            ClockMsg.msg = DISPLAY_ALERT;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case DISPLAY_ALERT:
            if( (MSGHandler.alarm == ALARM_TRIGGER) && (AlarmButton == NOT_PRESSED) )
            {
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_String( &hlcd, "    ALARM!!!" );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
                CounterMin++;
            }

            ClockMsg.msg = BUZZER;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case BUZZER:
            if( (CounterMin % NUM_2 == NUM_0) && ((MSGHandler.alarm == ALARM_TRIGGER) && (AlarmButton == NOT_PRESSED)) )
            {
                __HAL_TIM_SET_COMPARE( &TIM14Handle, TIM_CHANNEL_1, DUTY_CYCLE_50 );
            }
            else
            {
                __HAL_TIM_SET_COMPARE( &TIM14Handle, TIM_CHANNEL_1, DUTY_CYCLE_0 );
            }

            ClockMsg.msg = BLINK_BLACKLIGHT;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case BLINK_BLACKLIGHT:
            if( (CounterMin % NUM_2 == NUM_0) && ((MSGHandler.alarm == ALARM_TRIGGER) && (AlarmButton == NOT_PRESSED)) )
            {
                HEL_LCD_Backlight( &hlcd, LCD_OFF );
            }
            else
            {
                HEL_LCD_Backlight( &hlcd, LCD_ON );
            }

            ClockMsg.msg = ONE_MINUTE_PASSED;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case ONE_MINUTE_PASSED:
            if( CounterMin == NUM_60 )
            {
                CounterMin = NUM_0; 
                AlarmButton = NOT_PRESSED;
                MSGHandler.alarm = NO_ALARM;

                HEL_LCD_Backlight( &hlcd, LCD_ON );
                HAL_TIM_PWM_Stop( &TIM14Handle, TIM_CHANNEL_1 );
                Status = HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_A );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, RTC_DEACTICATE_ALARM_RET_ERROR );
            }

            ClockMsg.msg = BUTTON_PRESSED;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case BUTTON_PRESSED:
            alarm[NUM_0] = (ClockMsg.tm.tm_alarm_hour / NUM_10) + (uint8_t)'0';
            alarm[NUM_1] = (ClockMsg.tm.tm_alarm_hour % NUM_10) + (uint8_t)'0';
            alarm[NUM_2] = ':';
            alarm[NUM_3] = (ClockMsg.tm.tm_alarm_min / NUM_10) + (uint8_t)'0';
            alarm[NUM_4] = (ClockMsg.tm.tm_alarm_min % NUM_10) + (uint8_t)'0';

            if( (MSGHandler.alarm == ALARM_TRIGGER) && (AlarmButton == IS_PRESSED) )
            {
                MSGHandler.alarm = ALARM_DEACTIVATE;
            }
            else if( (MSGHandler.alarm == ALARM_SET) && (AlarmButton == IS_PRESSED) )
            {
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_String( &hlcd, "   ALARM=" );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_9 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_String( &hlcd, alarm );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
            }
            else if( (MSGHandler.alarm == NO_ALARM) && (AlarmButton == IS_PRESSED) )
            {
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_String( &hlcd, "ALARM NO CONFIG " );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
            }
            else
            {}

            ClockMsg.msg = BUTTON_RELEASED;
            (void)HIL_QUEUE_WriteISR( &ClockQueue, &ClockMsg, SPI1_IRQn );
            break;

        case BUTTON_RELEASED:
            if( (MSGHandler.alarm == ALARM_DEACTIVATE) && (AlarmButton == IS_RELEASED) )
            {
                AlarmButton = NOT_PRESSED;
                MSGHandler.alarm = NO_ALARM;
                HEL_LCD_Backlight( &hlcd, LCD_ON );
                Status = HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_A );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, RTC_DEACTICATE_ALARM_RET_ERROR );
            }
            else if( (MSGHandler.alarm == NO_ALARM) && (AlarmButton == IS_RELEASED) )
            {
                AlarmButton = NOT_PRESSED;
                Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_0 );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
                Status = HEL_LCD_String( &hlcd, "   " );
                /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
                assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
            }
            else if( (MSGHandler.alarm == ALARM_SET) && (AlarmButton == IS_RELEASED) )
            {
                AlarmButton = NOT_PRESSED;
            }
            else
            {
            }

            break;

        default:
            break;
    }
    
}

/**
 * @brief   **Display time**
 *
 * To make the string of time to be send to the LCD in format hh:mm:ss
 *
 * @param   tm          [in]  Structure type variable with the time to display
 * @param   buffer_time [out] Variable to store the string of time to the LCD
 * 
 * @note None
 */
static void Display_TimeString( APP_MsgTypeDef *tm )
{
    char buffer_time[NUM_14];

    buffer_time[NUM_0] = (tm->tm.tm_hour / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_1] = (tm->tm.tm_hour % NUM_10) + (uint8_t)'0';
    buffer_time[NUM_2] = ':';
    buffer_time[NUM_3] = (tm->tm.tm_min / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_4] = (tm->tm.tm_min % NUM_10) + (uint8_t)'0';
    buffer_time[NUM_5] = ':';
    buffer_time[NUM_6] = (tm->tm.tm_sec / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_7] = (tm->tm.tm_sec % NUM_10) + (uint8_t)'0';
    buffer_time[NUM_8] = ' ';
    buffer_time[NUM_9] = ' ';
    buffer_time[NUM_10] = ' ';
    buffer_time[NUM_11] = ' ';
    buffer_time[NUM_12] = ' ';

    Status = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_3 );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
    Status = HEL_LCD_String( &hlcd, &buffer_time[NUM_0] );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
}

/**
 * @brief   **Display date**
 *
 * To make the string of date to be send to the LCD in format mmm dd yyyy 
 *
 * @param   tm          [in]  Structure type variable with the time to display
 * @param   buffer_date [out] Variable to store the string of date to the LCD
 * 
 * @note  None
 */
static void Display_DateString( APP_MsgTypeDef *tm )
{
    char buffer_date[NUM_14];
    
    char *months[NUM_12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

    char *days_w[NUM_7] = { "MO ", "TU ", "WE ", "TH ", "FR ", "SA ", "SU " };

    for( uint8_t i = NUM_0; i < NUM_3; i++)
    {
        /* cppcheck-suppress misra-c2012-18.4 ; Nedded to move trough pointer */
        buffer_date[i]  = *(months[(tm->tm.tm_mon)-(uint32_t)1]+i);
    }
    buffer_date[NUM_3]  = ',';
    buffer_date[NUM_4]  = (tm->tm.tm_mday / NUM_10) + (uint8_t)'0';
    buffer_date[NUM_5]  = (tm->tm.tm_mday % NUM_10) + (uint8_t)'0';
    buffer_date[NUM_6]  = ' ';
    buffer_date[NUM_7]  = (tm->tm.tm_yday / HEX_10) + (uint8_t)'0';
    buffer_date[NUM_8]  = (tm->tm.tm_yday % HEX_10) + (uint8_t)'0';
    buffer_date[NUM_9]  = (tm->tm.tm_year / NUM_10) + (uint8_t)'0';
    buffer_date[NUM_10] = (tm->tm.tm_year % NUM_10) + (uint8_t)'0';
    buffer_date[NUM_11] = ' ';
    buffer_date[NUM_12] = *days_w[(tm->tm.tm_wday)-(uint32_t)1];
    /* cppcheck-suppress misra-c2012-18.4 ; Nedded to move trough pointer */
    buffer_date[NUM_13] = *(days_w[(tm->tm.tm_wday)-(uint32_t)1]+(uint32_t)1);

    Status = HEL_LCD_SetCursor( &hlcd, ROW_ONE, COL_1 );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, LCD_SETCUR_RET_ERROR );
    Status = HEL_LCD_String( &hlcd, &buffer_date[NUM_0] );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, LCD_STRING_RET_ERROR );
}
