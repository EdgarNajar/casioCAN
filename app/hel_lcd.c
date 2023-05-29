/**
 * @file    hel_lcd.
 * @brief   **LCD driver**
 *
 * this is a driver that configures and controls an alphanumeric LCD through a spi serial interface
 *
 * @note    none
 */

#include "hel_lcd.h"

#define wakeup                 0x30
#define function_set           0x39
#define internal_osc_frequency 0x14
#define power_control          0x56
#define follower_control       0x6D
#define contrast_cmd           0x70
#define display_on             0x0C
#define entry_mode             0x06
#define clear_screen           0x01

/** 
  * @defgroup state of the backlight of the LCD
  @{ */
#define LCD_ON     (uint8_t)1  /*!< turn on backlight */
#define LCD_OFF    (uint8_t)2  /*!< turn off backlight */
#define LCD_TOGGLE (uint8_t)3  /*!< toggle backlight */
/**
  @} */

/**
 * @brief   **LCD initialization function**
 *
 * this function sends the secuence to initialize the comunication with the LCD
 *
 * @param   hlcd [in] structure type pointer to handle te LCD
 *
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd )
{
    HEL_LCD_MspInit( hlcd );

    /* chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    /* reset */
    HAL_GPIO_WritePin( hlcd->RstPort, hlcd->RstPin, RESET );
    HAL_Delay(2);
    /* reset */
    HAL_GPIO_WritePin( hlcd->RstPort, hlcd->RstPin, SET );
    HAL_Delay(20);
    HEL_LCD_Command( hlcd, wakeup );
    HAL_Delay(2);
    HEL_LCD_Command( hlcd, wakeup );
    HEL_LCD_Command( hlcd, wakeup );
    HEL_LCD_Command( hlcd, function_set );
    HEL_LCD_Command( hlcd, internal_osc_frequency );
    HEL_LCD_Command( hlcd, power_control );
    HEL_LCD_Command( hlcd, follower_control );
    HAL_Delay(200);
    HEL_LCD_Command( hlcd, contrast_cmd );
    HEL_LCD_Command( hlcd, display_on );
    HEL_LCD_Command( hlcd, entry_mode );
    HEL_LCD_Command( hlcd, clear_screen );
    HAL_Delay(10);

    return 1;
}

/**
 * @brief   **Sub-initialization function for the pins used by the LCD**
 *
 * Provide the proper and detailed description for your function, you can use as many
 * lines as you want but keep each line less than 100 characteres
 *
 * @param   <param name>[in] Parameer description based on what does
 * @param   <param name>[out] add one line per paramter and indicates if input or output
 *
 * @retval  Describe the return value and its potential values, 
 *          if void type None
 *
 * @note This is optional, just in case something very special needs to be take into account
 */

__weak void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    UNUSED(hlcd);
}

/**
 * @brief   **command to the LCD**
 *
 * function to send a command to the LCD
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   cmd [in] command to send
 * 
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
    /*command mode*/
    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, RESET );
    /*chip select on*/
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );
    /*send command*/
    HAL_SPI_Transmit( hlcd->SpiHandler, &cmd, sizeof(cmd), 5000 );
    /*chip select off*/
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    // HAL_Delay(30);
    return 1;
}

/**
 * @brief   **data to the LCD**
 *
 * function to send a characters to the LCD using the SPI
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   data [in] data to send to the LCD
 *
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{
    /*data mode*/
    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, SET );
    /*chip select on*/
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );
    /*send command*/
    HAL_SPI_Transmit( hlcd->SpiHandler, &data, 1, 5000 );
    /*chip select off*/
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    // HAL_Delay(30);

    return 1;
}

/**
 * @brief   **string to the LCD**
 *
 * function to send a string of characters to the LCD using the SPI
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   str [in] pointer to char
 *
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_String( LCD_HandleTypeDef *hlcd, char *str )
{
    uint8_t i = 0;

    while( str[i] != '\0' )
    {
        HEL_LCD_Data( hlcd, str[i] );
        i++;
    }

    return 1;
}

/**
 * @brief   **function to locate the LCD cursor**
 *
 * this function recieves the coordinates of the LCD on which to start writing
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   row [in] row in wich write
 * @param   col [in] col in wich write
 *
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
    uint8_t cursor[2] = {0x80, 0xC0}; 
    uint8_t address;

    address = cursor[row] + col;
    
    /*second row*/
    HEL_LCD_Command( hlcd, address );

    return 1;
}

/**
 * @brief   **function to control the LCD backlight**
 *
 * this function changes the state of the backlight of the LCD
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   state [in] Specifies the state of the backlight which can be OFF, ON or TOGGLE
 *
 * @retval  none
 *
 * @note none
 */

void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state )
{
    if( state == LCD_ON )
    {
        HAL_GPIO_WritePin( hlcd->BklPort, hlcd->BklPin, SET );
    }
    else if( state == LCD_OFF )
    {
        HAL_GPIO_WritePin( hlcd->BklPort, hlcd->BklPin, RESET );
    }
    else if( state == LCD_TOGGLE )
    {
        HAL_GPIO_TogglePin( hlcd->BklPort, hlcd->BklPin );
    }
    else{}
}

/**
 * @brief   **function to control the contrast level of the LCD**
 *
 * the controller of the LCD has an internal register in which the contrast level 
 * can be established through 4 bits
 *
 * @param   hlcd [in] structure type pointer to handle the LCD
 * @param   contrast [in] specifies one of the 16 levels to the LCD contrast
 *
 * @retval  which SPI functions return
 *
 * @note none
 */

uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast )
{
    /*constrast*/
    HEL_LCD_Command( hlcd, (contrast | 0x70u ) );

    return 1;
}
