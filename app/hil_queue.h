/**
 * @file    hil_queue.h
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
#ifndef HIL_QUEUE_
#define HIL_QUEUE_

#include "app_bsp.h"

/** 
  * @defgroup State of the queue
  @{ */
#define QUEUE_NOT_OK (uint8_t)0  /*!< Queue state not ok */
#define QUEUE_OK     (uint8_t)1  /*!< Queue state ok     */
/**
  @} */

/** 
  * @defgroup Initialization values to control queue
  @{ */
#define QUEUE_FULL      (uint8_t)1  /*!< Queue full          */
#define QUEUE_NOT_FULL  (uint8_t)0  /*!< Queue not full      */
#define QUEUE_EMPTY     (uint8_t)1  /*!< Queue empty         */
#define QUEUE_NOT_EMPTY (uint8_t)0  /*!< Queue not empty     */
#define INIT_HEAD       (uint8_t)0  /*!< First value of head */
#define INIT_TAIL       (uint8_t)0  /*!< First value of head */
#define NO_DATA         (uint8_t)0  /*!< No data receive     */
/**
  @} */

/** 
  * @defgroup Disable all interrupts
  @{ */
#define DISABLE_ALL_INTERRUPTS (uint8_t)0XFF  /*!< To disable all interrupts */
/**
  @} */

extern void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );
extern uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data );
extern uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );
extern uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );
extern void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue );
extern uint8_t HIL_QUEUE_WriteISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr );
extern uint8_t HIL_QUEUE_ReadISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr );
extern uint8_t HIL_QUEUE_IsEmptyISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr );
extern void HIL_QUEUE_FlushISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr );

#endif