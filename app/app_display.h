/**
 * @file    app_display.c
 * @brief   **structures, variables and function prototypes**
 *
 * declaration of all structures, variables and function prototypes to initialize the SPI
 *
 * @note    none
 */

#ifndef APP_DISPLAY_
#define APP_DISPLAY_

/**
 * @brief  struct type variable to clock messages
 */

extern APP_MsgTypeDef ClockMsg;

/**
  * @brief   enum for display states control
  */

typedef enum{
    DISPLAY_TIME = 3,
    DISPLAY_DATE
}Display_States;

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