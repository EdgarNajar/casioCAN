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
 * @brief  Variable to control the state machine of display
 */
uint8_t display_lcd = DISPLAY_TIME;

/**
 * @brief   **Initialization of the LCD**
 *
 * This function initialize the ports to start working with the LCD and to
 * configure the spi in master mode, full-duplex comunication, clock polarity high
 * and phase in falling edge
 *
 * @param   hlcd [in/out] Structure type function to handle the LCD
 *
 * @note None
 */
void Display_Init( void )
{
    uint8_t x;

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
    HAL_SPI_Init( &SpiHandle );

    hlcd.RstPort    = GPIOD;
    hlcd.RstPin     = GPIO_PIN_2;
    hlcd.RsPort     = GPIOD;
    hlcd.RsPin      = GPIO_PIN_4;
    hlcd.CsPort     = GPIOD;
    hlcd.CsPin      = GPIO_PIN_3;
    hlcd.BklPort    = GPIOB;
    hlcd.BklPin     = GPIO_PIN_4;
    hlcd.SpiHandler = &SpiHandle;
    x = HEL_LCD_Init( &hlcd );

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
    switch( display_lcd )
    {
        case DISPLAY_TIME:
            display_lcd = DISPLAY_DATE;
            
            Display_TimeString( &ClockMsg );
            break;

        case DISPLAY_DATE:
            display_lcd = DISPLAY_TIME;

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
    char buffer_time[NUM_16];
    uint8_t x;

    buffer_time[NUM_0] = (tm->tm.tm_hour / NUM_10) + NUM_48;
    buffer_time[NUM_1] = (tm->tm.tm_hour % NUM_10) + NUM_48;
    buffer_time[NUM_2] = ':';
    buffer_time[NUM_3] = (tm->tm.tm_min / NUM_10) + NUM_48;
    buffer_time[NUM_4] = (tm->tm.tm_min % NUM_10) + NUM_48;
    buffer_time[NUM_5] = ':';
    buffer_time[NUM_6] = (tm->tm.tm_sec / NUM_10) + NUM_48;
    buffer_time[NUM_7] = (tm->tm.tm_sec % NUM_10) + NUM_48;

    x = HEL_LCD_SetCursor( &hlcd, ROW_TWO, COL_3 );
    x = HEL_LCD_String( &hlcd, &buffer_time[NUM_0] );
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
    char buffer_date[NUM_16];
    uint8_t x;
    
    char months[NUM_12][NUM_3] = { {'J','A','N'}, {'F','E','B'}, {'M','A','R'}, {'A','P','R'}, {'M','A','Y'}, {'J','U','N'}, 
                                   {'J','U','L'}, {'A','U','G'}, {'S','E','P'}, {'O','C','T'}, {'N','O','V'}, {'D','E','C'} };

    char days_w[NUM_7][NUM_2] = { {'M','O'}, {'T','U'}, {'W','E'}, {'T','H'}, {'F','R'}, {'S','A'}, {'S','U'} };
    
    for(uint8_t x = NUM_0; x < NUM_3; x++)
    {
        buffer_date[x] = months[(tm->tm.tm_mon)-(uint32_t)1][x];
    }

    buffer_date[NUM_3]  = ',';
    buffer_date[NUM_4]  = (tm->tm.tm_mday / NUM_10) + NUM_48;
    buffer_date[NUM_5]  = (tm->tm.tm_mday % NUM_10) + NUM_48;
    buffer_date[NUM_6]  = ' ';
    buffer_date[NUM_7]  = (tm->tm.tm_yday / HEX_10) + NUM_48;
    buffer_date[NUM_8]  = (tm->tm.tm_yday % HEX_10) + NUM_48;
    buffer_date[NUM_9]  = (tm->tm.tm_year / NUM_10) + NUM_48;
    buffer_date[NUM_10] = (tm->tm.tm_year % NUM_10) + NUM_48;
    buffer_date[NUM_11] = ' ';

    for(uint8_t x = NUM_0; x < NUM_2; x++)
    {
        buffer_date[x+(uint8_t)12] = days_w[(tm->tm.tm_wday)-(uint32_t)1][x];
    }

    x = HEL_LCD_SetCursor( &hlcd, ROW_ONE, COL_1 );
    x = HEL_LCD_String( &hlcd, &buffer_date[NUM_0] );
}
