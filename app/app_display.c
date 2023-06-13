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

static void Display_TimeString( APP_MsgTypeDef *tm );
static void Display_DateString( APP_MsgTypeDef *tm );

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
}

/**
 * @brief   **Display message processing**
 *
 * Implementation of the state machine in charge of messages processing 
 * from the clock task and display the time and date
 *
 * @note  None
 */
void Display_Task( void )
{    
    static uint8_t display_lcd = DISPLAY_IDLE;

    switch( display_lcd )
    {
        case DISPLAY_IDLE:
            if( ClockMsg.msg >= DISPLAY_MSG )
            {
                display_lcd = DISPLAY_TIME;
            }
            break;

        case DISPLAY_TIME:
            display_lcd = DISPLAY_DATE;
            
            Display_TimeString( &ClockMsg );
            break;

        case DISPLAY_DATE:
            display_lcd = DISPLAY_IDLE;

            Display_DateString( &ClockMsg );
            break;

        default :
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
    char buffer_time[NUM_8];

    buffer_time[NUM_0] = (tm->tm.tm_hour / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_1] = (tm->tm.tm_hour % NUM_10) + (uint8_t)'0';
    buffer_time[NUM_2] = ':';
    buffer_time[NUM_3] = (tm->tm.tm_min / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_4] = (tm->tm.tm_min % NUM_10) + (uint8_t)'0';
    buffer_time[NUM_5] = ':';
    buffer_time[NUM_6] = (tm->tm.tm_sec / NUM_10) + (uint8_t)'0';
    buffer_time[NUM_7] = (tm->tm.tm_sec % NUM_10) + (uint8_t)'0';

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

    char *days_w[NUM_7] = { "MO", "TU", "WE", "TH", "FR", "SA", "SU" };

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
