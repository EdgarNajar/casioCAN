/**
 * @file    app_clock.h
 * @brief   **variable, enum and function prototypes**
 *
 * declaration of all needed to make changes in data time 
 *
 * @note    none
 */
 
#ifndef APP_CLOCK_H__
#define APP_CLOCK_H__

/**
 * @brief  to determine the type of change to do
 */

extern uint8_t changes;

/**
  * @brief   enum for change option
  */

typedef enum{
    WAIT_MESSAGE,
    CHANGE_TIME,
    CHANGE_DATE,
    CHANGE_ALARM,
    DISPLAY
}change_msn;

void Clock_Init( void );
void Clock_Task( void );

#endif