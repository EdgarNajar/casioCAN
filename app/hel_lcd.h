/**
 * @file    hel_lcd.h
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
 
#ifndef HEL_LCD_H__
#define HEL_LCD_H__

#include "app_bsp.h"

/** 
  * @defgroup LCD instruction code
  @{ */
#define WAKEUP                 (uint8_t)0x30  /*!< Wakeup code                 */
#define FUNCTION_SET           (uint8_t)0x39  /*!< Function set code           */
#define INTERNAL_OSC_FRECUENCY (uint8_t)0x14  /*!< Internal osc frequency code */
#define POWER_CONTROL          (uint8_t)0x56  /*!< Power control code          */
#define FOLLOWER_CONTROL       (uint8_t)0x6D  /*!< Follower control code       */
#define CONTRAST_CMD           (uint8_t)0x70  /*!< Contrast code               */
#define DISPLAY_ON             (uint8_t)0x0C  /*!< Display on code             */
#define ENTRY_MODE             (uint8_t)0x06  /*!< Entry mode code             */
#define CLEAR_SCREEN           (uint8_t)0x01  /*!< Clear screen code           */
/**
  @} */

/** 
  * @defgroup ROW instruction code
  @{ */
#define ROW_ONE (uint8_t)0x80  /*!< Move cursor to row one */
#define ROW_TWO (uint8_t)0xC0  /*!< Move cursor to row two */
/**
  @} */

/** 
  * @defgroup Columns instruction code
  @{ */
#define COL_0 (uint8_t)0x00  /*!< Column 0 */
#define COL_1 (uint8_t)0x01  /*!< Column 1 */
#define COL_3 (uint8_t)0x03  /*!< Column 3 */
#define COL_9 (uint8_t)0x09  /*!< Column 9 */
/**
  @} */

/** 
  * @defgroup State of the backlight of the LCD
  @{ */
#define LCD_ON     (uint8_t)1  /*!< Turn on backlight  */
#define LCD_OFF    (uint8_t)2  /*!< Turn off backlight */
#define LCD_TOGGLE (uint8_t)3  /*!< Toggle backlight   */
/**
  @} */

/**
  * @brief   Structure for LCD control
  */
typedef struct
{
    SPI_HandleTypeDef   *SpiHandler;    /*!< SPI handler address of the spi to use with the LCD */
    GPIO_TypeDef        *RstPort;       /*!< Port where the pin to control the LCD reset pin is */
    uint32_t            RstPin;         /*!< Pin to control the LCD reset pin */
    GPIO_TypeDef        *RsPort;        /*!< Port where the pin to control the LCD RS pin */
    uint32_t            RsPin;          /*!< Pin to control the LCD RS pin */
    GPIO_TypeDef        *CsPort;        /*!< Port where the pin to control the LCD chip select is */
    uint32_t            CsPin;          /*!< Pin to control the LCD chip select pin */
    GPIO_TypeDef        *BklPort;       /*!< Port where the pin to control the LCD backlight is */
    uint32_t            BklPin;         /*!< Pin to control the LCD backlight pin */
} LCD_HandleTypeDef;

extern uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd );
extern void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd );
extern uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );
extern uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );
extern uint8_t HEL_LCD_String( LCD_HandleTypeDef *hlcd, char *str );
extern uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );
extern void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state );
extern uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast );
     
#endif