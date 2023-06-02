/**
 * @file    app_display.c
 * @brief   **file to control the LCD**
 *
 * this file will control the LCD and the way the information is displayed in the LCD
 *
 * @note    none
 */

#include "app_bsp.h"
#include "app_serial.h"
#include "app_display.h"
#include "hel_lcd.h"

#define row_1 0
#define row_2 1

/** 
  * @defgroup state of the backlight of the LCD
  @{ */
#define LCD_ON     1  /*!< turn on backlight */
#define LCD_OFF    2  /*!< turn off backlight */
#define LCD_TOGGLE 3  /*!< toggle backlight */
/**
  @} */

/**
 * @brief  struct type variable to handle the LCD
 */

static LCD_HandleTypeDef hlcd;

/**
 * @brief  struct type variable to clock messages
 */

APP_MsgTypeDef ClockMsg;

/**
 * @brief  struct type variable to handle the SPI
 */

SPI_HandleTypeDef SpiHandle;

/**
 * @brief  variable to control the state machine of display
 */

uint8_t display_lcd = DISPLAY_TIME;

/**
 * @brief  variable to store the string of time to the LCD
 */

char buffer_time[16];

/**
 * @brief  variable to store the string of date to the LCD
 */

char buffer_date[16];

/**
 * @brief   **initialization of the LCD**
 *
 * this function initialize the ports to start working with the LCD and
 * configure the spi in master mode, full-duplex comunication, clock polarity high
 * and phase in falling edge
 *
 * @param   hlcd [in/out] structure type function to handle the LCD
 *
 * @retval  none
 *
 * @note none
 */

void Display_Init( void )
{
    /* Configuring the spi in master mode, full-duplex comunication, clock polarity high
    and phase in falling edge */
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
    /* aplicamos la configuracion al spi 1 pero antes nos aseguramos 
    que el esclavo este deshabilitado pin C8 en alto 
    HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3, SET );*/
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
    HEL_LCD_Init( &hlcd );

    HEL_LCD_Backlight( &hlcd, LCD_ON );
}

/**
 * @brief   **display message processing**
 *
 * implementation of the state machine in charge of messages processing 
 * from the clock task and display the time and date
 *
 * @retval  none
 *
 * @note none
 */

void Display_Task( void )
{    
    switch( display_lcd )
    {
        case DISPLAY_TIME:
            display_lcd = DISPLAY_DATE;
            
            Display_TimeString( &ClockMsg );
            HEL_LCD_SetCursor( &hlcd, row_2, 0x03 );
            HEL_LCD_String( &hlcd, &buffer_time[0] );
            break;

        case DISPLAY_DATE:
            display_lcd = DISPLAY_TIME;

            Display_DateString( &ClockMsg );
            HEL_LCD_SetCursor( &hlcd, row_1, 0x01 );
            HEL_LCD_String( &hlcd, &buffer_date[0] );
            break;

        default :
            break;
    }
}

/**
 * @brief   **display time**
 *
 * to make the string of time to be send to the LCD
 *
 * @param   tm [in] structure type variable with the time to display
 * @param   buffer_time [out] variable to store the string of time to the LCD
 * 
 * @retval  none
 *
 * @note none
 */

static void Display_TimeString( APP_MsgTypeDef *tm )
{
    buffer_time[0] = (tm->tm.tm_hour / 10) + 48;
    buffer_time[1] = (tm->tm.tm_hour % 10) + 48;
    buffer_time[2] = ':';
    buffer_time[3] = (tm->tm.tm_min / 10) + 48;
    buffer_time[4] = (tm->tm.tm_min % 10) + 48;
    buffer_time[5] = ':';
    buffer_time[6] = (tm->tm.tm_sec / 10) + 48;
    buffer_time[7] = (tm->tm.tm_sec % 10) + 48;
}

/**
 * @brief   **display date**
 *
 * to make the string of date to be send to the LCD
 *
 * @param   tm [in] structure type variable with the time to display
 * @param   buffer_date [out] variable to store the string of date to the LCD
 * 
 * @retval  none
 *
 * @note none
 */

static void Display_DateString( APP_MsgTypeDef *tm )
{
    char months[12][3] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
                           "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

    char days_w[7][2] = { "MO", "TU", "WE", "TH", "FR", "SA", "SU" };
    
    for(char x = 0; x < 3u; x++)
    {
        buffer_date[x] = months[(tm->tm.tm_mon)-1][x];
    }

    buffer_date[3]  = ',';
    buffer_date[4]  = (tm->tm.tm_mday / 10) + 48;
    buffer_date[5]  = (tm->tm.tm_mday % 10) + 48;
    buffer_date[6]  = ' ';
    buffer_date[7]  = (tm->tm.tm_yday / 0x10) + 48;
    buffer_date[8]  = (tm->tm.tm_yday % 0x10) + 48;
    buffer_date[9]  = (tm->tm.tm_year / 10) + 48;
    buffer_date[10] = (tm->tm.tm_year % 10) + 48;
    buffer_date[11] = ' ';

    for(char x = 0; x < 2; x++)
    {
        buffer_date[x+12] = days_w[(tm->tm.tm_wday)-1][x];
    }
}
