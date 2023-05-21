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

#define WAIT_MESSAGE (uint8_t)0
#define CHANGE_TIME  (uint8_t)1
#define CHANGE_DATE  (uint8_t)2
#define CHANGE_ALARM (uint8_t)3
#define DISPLAY      (uint8_t)4

#define val_AsynchPrediv 127
#define val_SynchPrediv  255

#define def_Hours      0x23
#define def_Minutes    0x59
#define def_Seconds    0x50
#define def_SubSeconds 0x00

#define def_Date    0x31
#define def_YearLSB 0x23
#define def_YearMSB 0x20

#define one_second 1000

#define num_0 (uint8_t)0
#define num_1 (uint8_t)1

void Clock_Init( void );
void Clock_Task( void );

#endif