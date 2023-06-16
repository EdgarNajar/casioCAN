#ifndef HIL_QUEUE_
#define HIL_QUEUE_

#include "app_bsp.h"



void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data );
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );
void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue );
uint8_t HIL_QUEUE_WriteISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr );
uint8_t HIL_QUEUE_ReadISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr );
uint8_t HIL_QUEUE_IsEmptyISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr );
uint8_t HIL_QUEUE_FlushISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr );

#endif