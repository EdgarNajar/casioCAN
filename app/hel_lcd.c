/**
 * @file    hel_lcd.c
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
    Status = HEL_LCD_Command( hlcd, WAKEUP );
    HAL_Delay(2);
    Status = HEL_LCD_Command( hlcd, WAKEUP );
    Status = HEL_LCD_Command( hlcd, WAKEUP );
    Status = HEL_LCD_Command( hlcd, FUNCTION_SET );
    Status = HEL_LCD_Command( hlcd, INTERNAL_OSC_FRECUENCY );
    Status = HEL_LCD_Command( hlcd, POWER_CONTROL );
    Status = HEL_LCD_Command( hlcd, FOLLOWER_CONTROL );
    HAL_Delay(200);
    Status = HEL_LCD_Command( hlcd, CONTRAST_CMD );
    Status = HEL_LCD_Command( hlcd, DISPLAY_ON );
    Status = HEL_LCD_Command( hlcd, ENTRY_MODE );
    Status = HEL_LCD_Command( hlcd, CLEAR_SCREEN );
    HAL_Delay(10);
    
    return Status;
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
/* cppcheck-suppress misra-c2012-8.6 ; function defined in hel_lcd.h file */
/* cppcheck-suppress misra-c2012-8.7 ; function defined in hel_lcd.h file */
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
    Status = HAL_SPI_Transmit( hlcd->SpiHandler, &cmd, sizeof(cmd), 5000 );
    /* chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );

    return Status;
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
/* cppcheck-suppress misra-c2012-8.7 ; Function to be used in future applications */
uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{
    /* Data mode */
    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, SET );
    /* Chip select on */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );
    /* Send data */
    Status = HAL_SPI_Transmit( hlcd->SpiHandler, &data, 1, 5000 );
    /* Chip select off */
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );

    return Status;
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
    Status = HAL_ERROR;

    while( str[i] != '\0' )
    {
        Status = HEL_LCD_Data( hlcd, str[i] );
        i++;
    }

    return Status;
}

/**
 * @brief   **Function to locate the LCD cursor**
 *
 * This function recieves the row and column on which to start writing,
 * an array of two elements contains the instruction code for every row,
 * if the value of variable row is 0 then the element 0 of the array cursor
 * will be select, if the value of variable row is 1 then the element 1
 * of the array cursor will be select, the column value goes from 0 to F,
 * the two values get store in the variable address and then it's send 
 * with the function HEL_LCD_Command.
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
    uint8_t address;

    address = row + col;
    
    /* Second row */
    Status = HEL_LCD_Command( hlcd, address );

    return Status;
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
    Status = HEL_LCD_Command( hlcd, (contrast | CONTRAST_CMD ) );

    return Status;
}
