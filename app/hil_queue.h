#ifndef HIL_QUEUE_
#define HIL_QUEUE_

typedef struct
{
    void        *Buffer;  //puntero al espacio de memoria usado como buffer por la cola
    uint32_t    Elements; //numero de elementos a almacenar (tama;o de la cola)
    uint8_t     Size;     //tamaño del tipo de elementos a almacenar
    uint32_t    Head;     //puntero que indica el siguiente espacio a escribir
    uint32_t    Tail;     //puntero que indica el siguiente espacio a leer
    uint8_t     Empty;    //bandera que indica si no hay elementos a leer
    uint8_t     Full;     //bandera que indica si no se puede seguir escribiendo mas elementos
    //agregar más elementos si se requieren
} QUEUE_HandleTypeDef;

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