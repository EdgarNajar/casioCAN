/**
 * @file    main.c
 * @brief   **main file with the calls to all functions needed**
 *
 * This file contains the implementation of the functions needed to run a clock
 * using CAN protocol 
 *
 * @note    none
 *
 */

#include "app_bsp.h"
#include "app_serial.h"

/**
 * @brief   **implementation of the main program**
 *
 * contains the calls to all the functions nedded to run the clock
 *
 * @retval  none
 *
 * @note none
 *
 */

int main( void )
{
    HAL_Init();
    Serial_Init();

    for( ;; )
    {
        Serial_Task();
    }
}
