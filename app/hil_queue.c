/**
 * @file    hil_queue.c
 * @brief   **Circular buffer driver**
 *
 * This is a driver to implement a circular buffer, with the help of memcpy function 
 * from the standard library and void pointers we will handle any type of element for the queue.
 * The basic principle behind it is very simple, each element of the X type is made up 
 * of N bytes so we will only have to copy the N bytes that make up each element 
 * when writing or reading in the queue.
 *
 * @note    None
 */
#include "hil_queue.h"
#include <string.h>

/**
 * @brief   **Initialization of queue values**
 *
 * This function initialize the values of Head, 
 * Tail and Full to 0, Empty is initialize to 1, 
 * to start working with the queue.
 *
 * @param   hqueue [in] Pointer to structure data
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Buffer != NULL), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Elements != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Size != NO_DATA), QUEUE_PAR_ERROR );

    hqueue->Head  = INIT_HEAD;
    hqueue->Tail  = INIT_TAIL;
    hqueue->Empty = QUEUE_EMPTY;
    hqueue->Full  = QUEUE_NOT_FULL;
}

/**
 * @brief   **Write data to the queue**
 *
 * The empty pointer data is used to copy the information to the buffer controlled by hqueue.
 * First we check if the buffer is full, if it is, we can’t write to it and a message of 
 * error will be returned, if there is room we add the data and increase Head. 
 * Finally when Head reaches Tail the buffer is full.
 * 
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Buffer != NULL), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Elements != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Size != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( data != NULL, QUEUE_PAR_ERROR );

    uint8_t ret_state = QUEUE_NOT_OK;

    if( (hqueue->Full) == QUEUE_NOT_FULL )
    {
        /* cppcheck-suppress misra-c2012-11.5 ; Needed to perform writing */
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform writing */
        (void)memcpy(((uint8_t *)(hqueue->Buffer) + (hqueue->Head)), data, hqueue->Size);
        hqueue->Head = ((hqueue->Head)++) % hqueue->Elements;
        ret_state = QUEUE_OK;
    }
    
    if( (hqueue->Empty) == QUEUE_EMPTY )
    {
        hqueue->Empty = QUEUE_NOT_EMPTY;
    }

    if( (hqueue->Head) == (hqueue->Tail) )
    {
        hqueue->Full = QUEUE_FULL;
    }

    return ret_state;
}

/**
 * @brief   **Read data from the queue**
 *
 * To read the data first we check if the buffer is empty, if it is, 
 * we can’t read anyting, so a message of error will be returned, 
 * if there is something in the buffer, we read it and increase Tail. 
 * Finally when Tail reaches Head the buffer is empty.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK  
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Buffer != NULL), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Elements != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Size != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( data != NULL, QUEUE_PAR_ERROR );

    uint8_t ret_state = QUEUE_NOT_OK;

    if( (hqueue->Empty) == QUEUE_NOT_EMPTY )
    {
        /* cppcheck-suppress misra-c2012-11.5 ; Needed to perform reading */
        /* cppcheck-suppress misra-c2012-18.4 ; Needed to perform reading */
        (void)memcpy(data, ((uint8_t *)(hqueue->Buffer) + (hqueue->Tail)), hqueue->Size);
        hqueue->Tail = ((hqueue->Tail)++) % hqueue->Elements;
        ret_state = QUEUE_OK;
    }
    
    if( (hqueue->Full) == QUEUE_FULL )
    {
        hqueue->Full = QUEUE_NOT_FULL;
    }

    if( (hqueue->Head) == (hqueue->Tail) )
    {
        hqueue->Empty = QUEUE_EMPTY;
    }

    return ret_state;
}

/**
 * @brief   **Check if the queue is empty**
 *
 * This function checks if the buffer is empty comparing Head and Tail,
 * if are equal then the buffer is empty.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Buffer != NULL), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Elements != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Size != NO_DATA), QUEUE_PAR_ERROR );

    return hqueue->Empty;
}

/**
 * @brief   **Flush the queue**
 *
 * This function returns the initial values 
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.7 ; Will be use in future applications */
void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue )
{
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Buffer != NULL), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Elements != NO_DATA), QUEUE_PAR_ERROR );
    /* cppcheck-suppress misra-c2012-11.8 ; Needed to the macro to detect erros */
    assert_error( (hqueue->Size != NO_DATA), QUEUE_PAR_ERROR );

    HIL_QUEUE_Init( hqueue );
}

/**
 * @brief   **Write to buffer disabling the interrupts**
 *
 * This function disable the interrupts before writing, 
 * calls the HIL_QUEUE_Write function and then enable the interrupts. 
 * The third parameter will indicate which interrupt vector should be disable. 
 * The value 0xFF will indicate that all interrupts should be disable.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK 
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
 * @brief   **Read to buffer disabling the interrupts**
 *
 * This function disable the interrupts before reading, 
 * calls the HIL_QUEUE_Read function and then enable the interrupts. 
 * The third parameter will indicate which interrupt vector should be disable. 
 * The value 0xFF will indicate that all interrupts should be disable.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK 
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
 * @brief   **Check if buffer is empty disabling the interrupts**
 *
 * This function disable the interrupts then checks if the buffer is empty
 * calling the HIL_QUEUE_IsEmpty function and then enable the interrupts. 
 * The third parameter will indicate which interrupt vector should be disable. 
 * The value 0xFF will indicate that all interrupts should be disable.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
 *
 * @retval  Indicates the state of the queue QUEUE_NOT_OK and QUEUE_OK  
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
 * @brief   **Flush the queue disabling the interrupts**
 *
 * This function disable the interrupts then calls the HIL_QUEUE_Flush function 
 * to restart the values of Head, Tail, Empty and Full and then enabled the interrupts. 
 * The third parameter will indicate which interrupt vector should be disabled. 
 * The value 0xFF will indicate that all interrupts should be disabled.
 *
 * @param   hqueue [in] Pointer to structure data
 * @param   data   [in] Pointer to data
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
