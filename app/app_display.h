/**
 * @file    app_display.h
 * @brief   **File to control the LCD**
 *
 * This file will control the LCD and the way the information is displayed in the LCD
 *
 * @note    None
 */
#ifndef APP_DISPLAY_
#define APP_DISPLAY_

#include "app_bsp.h"

/** 
  * @defgroup States for display data
  @{ */
#define DISPLAY_MSG      (uint8_t)4   /*!< Display message    */
#define ALARM_ACTIVE     (uint8_t)5   /*!< Display alarm      */
#define DISPLAY_ALERT    (uint8_t)6   /*!< Display alert      */
#define DISPLAY_ALARM    (uint8_t)7   /*!< Display alarm      */
#define BUTTON_PRESSED   (uint8_t)8   /*!< Button is pressed  */
#define BUTTON_RELEASED  (uint8_t)9   /*!< Button is released */
#define BLINK            (uint8_t)10  /*!< To toggle the LCD  */
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

/** 
  * @defgroup Time to read display
  @{ */
#define HUNDRED_MS (uint32_t)100  /*!< A hundred miliseconds */
/**
  @} */

void Display_Init( void );
void Display_Task( void );

#endif