/**
 * @file    hel_lcd.
 * @brief   **LCD driver**
 *
 * This is a driver that configures and controls an alphanumeric LCD through SPI,
 * The LCD-SPI board has an alphanumeric LCD NHD-C0216CZ-NSW-BBW-3V3 from Newhaven, 
 * which is capable of displaying 2x16 lines of characters. Internally, 
 * the display has the Sitronix ST7032 controller in charge of keeping the characters displayed 
 * on the display without the need to continually refresh the screen. The operating voltage 
 * of the board is 3.3v with backlight included.
 *
 * @note    None
 */
#include "hel_lcd.h"

/**
 * @brief   **LCD initialization function**
 *
 * This function sends the secuence code to initialize the comunication with the LCD.
 * First the select chip is turned on low, then a reset is done by toggling the reset pin. 
 * It is configured for 8 bits and operation in two lines with function set code, 
 * the internal oscillation frequency, the power control mode, the follower control 
 * and the contrast are configured to finally clean the screen.
 *
 * @param   hlcd [in] Structure type pointer to handle te LCD
 *
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
 */
uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd )
{
    HEL_LCD_MspInit( hlcd );

    /* Chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    /* Reset */
    HAL_GPIO_WritePin( hlcd->RstPort, hlcd->RstPin, RESET );
    HAL_Delay(2);
    /* Reset */
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
 * @brief   **Sub-initialization function**
 *
 * This is a weak function for user initialization of the pins to configure and 
 * control the LCD using SPI.
 *
 * @param   hlcd [in] Structure type pointer to handle te LCD
 *
 * @note None
 */
__weak void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    UNUSED(hlcd);
}

/**
 * @brief   **Command to the LCD**
 *
 * Function to send a commands to the LCD using SPI.
 * First the RS pin is set to low to select the command mode, then the chip select pin
 * is set to low to select the LCD, and with the help of the function HAL_SPI_Transmit
 * the data will be transmitted, last, the chip select pin is set to high.
 *
 * @param   hlcd [in] Structure type pointer to handle the LCD
 * @param   cmd  [in] Command to send
 * 
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
 */
uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
    /* Command mode */
    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, RESET );
    /* Chip select on */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );
    /* send command */
    HAL_SPI_Transmit( hlcd->SpiHandler, &cmd, sizeof(cmd), 5000 );
    /* chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    // HAL_Delay(30);
    return 1;
}

/**
 * @brief   **Data to the LCD**
 *
 * Function to send characters to the LCD using the SPI.
 * First the RS pin is set to high to select the data mode, then the chip select pin
 * is set to low to select the LCD, and with the help of the function HAL_SPI_Transmit
 * the data will be transmitted, last, the chip select pin is set to high.
 *
 * @param   hlcd [in] Structure type pointer to handle the LCD
 * @param   data [in] Data to send to the LCD
 *
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
 */
uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{
    /* Data mode */
    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, SET );
    /* Chip select on */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );
    /* Send data */
    HAL_SPI_Transmit( hlcd->SpiHandler, &data, 1, 5000 );
    /* Chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    // HAL_Delay(30);
    return 1;
}

/**
 * @brief   **String to the LCD**
 *
 * Function to send a string of characters to the LCD using the SPI.
 * In the while loop is evaluated when the string to send reach the null character,
 * who indicates the end of the string, the HEL_LCD_Data is used to send the data.
 *
 * @param   hlcd [in] Structure type pointer to handle the LCD
 * @param   str  [in] Pointer to string
 *
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
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
 * @brief   **Function to locate the LCD cursor**
 *
 * This function recieves the coordinates of the LCD on which to start writing,
 * and sends the command with the function HEL_LCD_Command.
 *
 * @param   hlcd [in] Structure type pointer to handle the LCD
 * @param   row  [in] Row in wich write
 * @param   col  [in] Column in wich write
 *
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
 */
uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
    uint8_t cursor[2] = {0x80, 0xC0}; 
    uint8_t address;

    address = cursor[row] + col;
    
    /* Second row */
    HEL_LCD_Command( hlcd, address );

    return 1;
}

/**
 * @brief   **Function to control the LCD backlight**
 *
 * This function changes the state of the backlight of the LCD
 *
 * @param   hlcd  [in] Structure type pointer to handle the LCD
 * @param   state [in] Specifies the state of the backlight which can be OFF, ON or TOGGLE
 *
 * @note None
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
 * @brief   **Function to control the contrast level of the LCD**
 *
 * The controller of the LCD has an internal register in which the contrast level 
 * can be established through 4 bits with the help of the function HEL_LCD_Command.
 *
 * @param   hlcd     [in] Structure type pointer to handle the LCD
 * @param   contrast [in] Specifies one of the 16 levels to the LCD contrast
 *
 * @retval  Since the function make use of the HAL SPI library functions 
 *          it is necessary to return what the SPI functions returns, 
 *          we will take this as an indication of a successful operation or not.
 *
 * @note None
 */
uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast )
{
    /* Constrast */
    HEL_LCD_Command( hlcd, (contrast | 0x70u ) );

    return 1;
}
