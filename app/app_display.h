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

/** 
  * @defgroup Rows of the LCD
  @{ */
#define ROW_1 (uint8_t)0  /*!< First row  */
#define ROW_2 (uint8_t)1  /*!< Second row */
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
 * @brief  Struct type variable to clock messages
 */
extern APP_MsgTypeDef ClockMsg;

/** 
  * @defgroup States for display data
  @{ */
#define DISPLAY_TIME (uint8_t)3  /*!< States for display time */
#define DISPLAY_DATE (uint8_t)4  /*!< States for display date */
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
#define NUM_16 (uint8_t)16  /*!< Number 16 */
#define NUM_48 (uint8_t)48  /*!< Number 48 */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define HEX_1  (uint8_t)0x1   /*!< Number 1 in hexa    */
#define HEX_3  (uint8_t)0x3   /*!< Number 3 in hexa    */
#define HEX_10 (uint8_t)0x10  /*!< Number 10 in hexa   */
/**
  @} */

void Display_Init( void );
void Display_Task( void );

static void Display_TimeString( APP_MsgTypeDef *tm );
static void Display_DateString( APP_MsgTypeDef *tm );
// /
//  * @brief   Asssemble the time string**
//  *
//  * Make the string to display in the lower row of the LCD compose of the time with the
//  * following format "hh:mm:ss"
//  *
//  * @param   buffer array where the string will be stored
//  * @param   tm structure with the time to display
//  /
// static void Display_TimeString( charbuffer, APP_TmTypeDef *tm )
// /
//  * @brief   Asssemble the date string**

// /
//  * @brief   Asssemble the date string**
//  *
//  * Make the string to display in the upper row of the LCD compose of the date and the day of the week
//  * in the followin format "mmm,dd yyyy dw"
//  *
//  * @param   buffer array where the string will be stored
//  * @param   tm structure with the date to display
//  /
// static void Display_DateString( charbuffer, APP_TmTypeDef *tm )

#endif