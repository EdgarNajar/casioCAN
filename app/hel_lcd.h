/**
 * @file    hel_lcd.h
 * @brief   **structures and function prototypes**
 *
 * declaration of all structures and function prototypes to initialize the LCD 
 *
 * @note    none
 */
 
#ifndef HEL_LCD_H__
#define HEL_LCD_H__

#include "app_bsp.h"

/**
  * @brief   structure for LCD
  */

typedef struct
{
    SPI_HandleTypeDef   *SpiHandler;    /* SPI handler address of the spi to use with the LCD */
    GPIO_TypeDef        *RstPort;       /* port where the pin to control the LCD reset pin is */
    uint32_t            RstPin;         /* pin to control the LCD reset pin */
    GPIO_TypeDef        *RsPort;        /* port where the pin to control the LCD RS pin */
    uint32_t            RsPin;          /* pin to control the LCD RS pin */
    GPIO_TypeDef        *CsPort;        /* port where the pin to control the LCD chip select is */
    uint32_t            CsPin;          /* pin to control the LCD chip select pin */
    GPIO_TypeDef        *BklPort;       /* port where the pin to control the LCD backlight is */
    uint32_t            BklPin;         /* pin to control the LCD backlight pin */
} LCD_HandleTypeDef;

/** 
  * @defgroup LCD instruction code
  @{ */
#define wakeup                 0x30  /*!< wakeup code  */
#define function_set           0x39  /*!< Function set code  */
#define internal_osc_frequency 0x14  /*!< Internal osc frequency code  */
#define power_control          0x56  /*!< Power control code  */
#define follower_control       0x6D  /*!< Follower control code  */
#define contrast_cmd           0x70  /*!< Contrast code  */
#define display_on             0x0C  /*!< Display on code  */
#define entry_mode             0x06  /*!< Entry mode code  */
#define clear_screen           0x01  /*!< Clear screen code  */
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

extern uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd );
void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd );
extern uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );
extern uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );
extern uint8_t HEL_LCD_String( LCD_HandleTypeDef *hlcd, char *str );
extern uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );
extern void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state );
extern uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast );
     
#endif