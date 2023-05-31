/**
 * @file    main.c
 * @brief   **Main file with the calls to all functions needed**
 *
 * This file contains the implementation of the functions needed to run a clock
 * using CAN protocol 
 *
 * @note    None
 *
 */
#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**
 * @brief   **Implementation of the main program**
 *
 * Contains the calls to all the functions nedded to run the clock
 *
 * @note None
 *
 */
int main( void )
{
    HAL_Init();
    Serial_Init();
    Clock_Init();

    for( ;; )
    {
        Serial_Task();
        Clock_Task();
    }
}
