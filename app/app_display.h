/**
 * @file    app_display.c
 * @brief   **structures, variables and function prototypes**
 *
 * declaration of all structures, variables and function prototypes to initialize the SPI
 *
 * @note    None
 */
#ifndef APP_DISPLAY_
#define APP_DISPLAY_

#include "app_bsp.h"

/** 
  * @defgroup Rows of the LCD
  @{ */
#define ROW_ONE (uint8_t)0  /*!< First row  */
#define ROW_TWO (uint8_t)1  /*!< Second row */
/**
  @} */

/** 
  * @defgroup Columns of the LCD
  @{ */
#define COL_1  (uint8_t)0x1   /*!< Column 1 */
#define COL_3  (uint8_t)0x3   /*!< Column 3 */
/**
  @} */

/** 
  * @defgroup Backlight state of the LCD
  @{ */
#define LCD_ON     (uint8_t)1  /*!< Turn on backlight  */
#define LCD_OFF    (uint8_t)2  /*!< Turn off backlight */
#define LCD_TOGGLE (uint8_t)3  /*!< Toggle backlight   */
/**
  @} */

/** 
  * @defgroup States for display data
  @{ */
#define DISPLAY_IDLE (uint8_t)0  /*!< States for display time */
#define DISPLAY_TIME (uint8_t)1  /*!< States for display time */
#define DISPLAY_DATE (uint8_t)2  /*!< States for display date */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define NUM_0  (uint8_t)0   /*!< Number 0  */
#define NUM_1  (uint8_t)1   /*!< Number 1  */
#define NUM_2  (uint8_t)2   /*!< Number 2  */
#define NUM_3  (uint8_t)3   /*!< Number 3  */
#define NUM_4  (uint8_t)4   /*!< Number 4  */
#define NUM_5  (uint8_t)5   /*!< Number 5  */
#define NUM_6  (uint8_t)6   /*!< Number 6  */
#define NUM_7  (uint8_t)7   /*!< Number 7  */
#define NUM_8  (uint8_t)8   /*!< Number 8  */
#define NUM_9  (uint8_t)9   /*!< Number 9  */
#define NUM_10 (uint8_t)10  /*!< Number 10 */
#define NUM_11 (uint8_t)11  /*!< Number 11 */
#define NUM_12 (uint8_t)12  /*!< Number 12 */
#define NUM_13 (uint8_t)13  /*!< Number 13 */
#define NUM_14 (uint8_t)14  /*!< Number 14 */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define HEX_10 (uint8_t)0x10  /*!< Number 10 in hexa   */
/**
  @} */

void Display_Init( void );
void Display_Task( void );

#endif