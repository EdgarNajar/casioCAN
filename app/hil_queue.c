/**
 * @file    hil_queue.c
 * @brief   **Circular buffer driver**
 *
 *  
 *
 * @note    None
 */
#include "hil_queue.h"

/**
 * @brief  Structure type variable to handle the queue
 */
QUEUE_HandleTypeDef hqueue;

/** 
  * @defgroup LCD instruction code
  @{ */
#define QUEUE_NOT_OK (uint8_t)0
#define QUEUE_OK     (uint8_t)1
/**
  @} */

/** 
  * @defgroup LCD instruction code
  @{ */
#define QUEUE_FULL      (uint8_t)1
#define QUEUE_NOT_FULL  (uint8_t)0
#define QUEUE_EMPTY     (uint8_t)1
#define QUEUE_NOT_EMPTY (uint8_t)0
#define INIT_HEAD       (uint8_t)0
#define INIT_TAIL       (uint8_t)0
/**
  @} */

/** 
  * @defgroup LCD instruction code
  @{ */
#define DISABLE_ALL_INTERRUPTS (uint8_t)0XFF
/**
  @} */

void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head  = INIT_HEAD;
    hqueue->Tail  = INIT_TAIL;
    hqueue->Empty = QUEUE_EMPTY;
    hqueue->Full  = QUEUE_NOT_FULL;
}

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

uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue )
{
    if( hqueue->Head == hqueue->Tail )
    {
        hqueue->Empty = QUEUE_EMPTY;
    }

    return hqueue->Empty;
}

void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head  = INIT_HEAD;
    hqueue->Tail  = INIT_TAIL;
    hqueue->Empty = QUEUE_EMPTY;
    hqueue->Full  = QUEUE_NOT_FULL;
}

uint8_t HIL_QUEUE_WriteISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{

}

uint8_t HIL_QUEUE_ReadISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{

}

uint8_t HIL_QUEUE_IsEmptyISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{

}

uint8_t HIL_QUEUE_FlushISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{

}