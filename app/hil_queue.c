/**
 * @file    hil_queue.c
 * @brief   **Circular buffer driver**
 *
 * This is a driver to implement a circular buffer
 *
 * @note    None
 */
#include "hil_queue.h"

/**
 * @brief   **Initialization of queue values**
 *
 * Thi function initialize the values of Head to 0, Tail to 0, Empty to 1
 * and Full to 0, to start working with the queue 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head  = INIT_HEAD;
    hqueue->Tail  = INIT_TAIL;
    hqueue->Empty = QUEUE_EMPTY;
    hqueue->Full  = QUEUE_NOT_FULL;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t ret_state = QUEUE_NOT_OK;

    if( hqueue->Full == QUEUE_NOT_FULL )
    {
        (hqueue->Head)++;
        hqueue->Buffer = &data;
        hqueue->Empty = QUEUE_NOT_EMPTY;
        ret_state = QUEUE_OK;
    }
    else if( hqueue->Head == hqueue->Tail )
    {
        hqueue->Full = QUEUE_FULL;
        ret_state = QUEUE_NOT_OK;
    }

    return ret_state;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t ret_state = QUEUE_NOT_OK;

    if( hqueue->Empty == QUEUE_NOT_EMPTY )
    {
        (hqueue->Tail)++;
        hqueue->Buffer = &data;
        hqueue->Full = QUEUE_NOT_FULL;
        ret_state = QUEUE_OK;
    }
    else if( hqueue->Head == hqueue->Tail )
    {
        hqueue->Empty = QUEUE_EMPTY;
        ret_state = QUEUE_NOT_OK;
    }

    return ret_state;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue )
{
    if( hqueue->Head == hqueue->Tail )
    {
        hqueue->Empty = QUEUE_EMPTY;
    }

    return hqueue->Empty;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head  = INIT_HEAD;
    hqueue->Tail  = INIT_TAIL;
    hqueue->Empty = QUEUE_EMPTY;
    hqueue->Full  = QUEUE_NOT_FULL;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_WriteISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{
    uint8_t ret_status;

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __disable_irq();
    }
    else
    {
        HAL_NVIC_DisableIRQ( isr );
    }

    ret_status = HIL_QUEUE_Write( hqueue, data );

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __enable_irq();
    }
    else
    {
        HAL_NVIC_EnableIRQ( isr );
    }

    return ret_status;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_ReadISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{
    uint8_t ret_status;

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __disable_irq();
    }
    else
    {
        HAL_NVIC_DisableIRQ( isr );
    }

    ret_status = HIL_QUEUE_Read( hqueue, data );

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __enable_irq();
    }
    else
    {
        HAL_NVIC_EnableIRQ( isr );
    }

    return ret_status;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
uint8_t HIL_QUEUE_IsEmptyISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{
    uint8_t ret_status;

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __disable_irq();
    }
    else
    {
        HAL_NVIC_DisableIRQ( isr );
    }

    ret_status = HIL_QUEUE_IsEmpty( hqueue );

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __enable_irq();
    }
    else
    {
        HAL_NVIC_EnableIRQ( isr );
    }

    return ret_status;
}

/**
 * @brief   ** **
 *
 * 
 *
 * @param   data        [in]     Pointer to data
 *
 * @retval  
 *
 * @note None
 */
void HIL_QUEUE_FlushISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{
    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __disable_irq();
    }
    else
    {
        HAL_NVIC_DisableIRQ( isr );
    }

    HIL_QUEUE_Flush( hqueue );

    if( isr == DISABLE_ALL_INTERRUPTS )
    { 
        __enable_irq();
    }
    else
    {
        HAL_NVIC_EnableIRQ( isr );
    }
}