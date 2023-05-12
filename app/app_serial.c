/**
 * @file    app_serial.c
 * @brief   **message processing implementation**
 *
 * contains the functions to initialize the CAN port and the main state machine 
 *
 * @note    none
 */

#include "app_bsp.h"
#include "app_serial.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );

/**
 * @brief  structure type variable for user CAN initialization
 */

/* cppcheck-suppress misra-c2012-8.4 ; function declared fie app_bsp.h as extern */
FDCAN_HandleTypeDef CANHandler;

/**
 * @brief  structure type variable for CAN transmissin initialization
 */

static FDCAN_TxHeaderTypeDef CANTxHeader;

/**
 * @brief  structure type variable for time data
 */

APP_MsgTypeDef MSGHandler;

/**
 * @brief  variable for new message
 */

static uint8_t NewMessage[8];

/**
 * @brief  variable to indicate the interrup of message occur
 */

static uint8_t flagMessage = 0;

/**
 * @brief  to indicate a new message arrived
 */
  
static uint8_t msgRecieve;

/**
 * @brief  is used to move in state machine
 */

/* cppcheck-suppress misra-c2012-8.9 ; declared as global to access state machine */
/* cppcheck-suppress misra-c2012-8.4 ; declared as global to access state machine */
APP_States STATE_CONTROL = STATE_IDLE;

/**
 * @brief   **initialize of CAN port**
 *
 * is the function to initialize all the required to start working with the CAN port 
 * and the messages reception processing
 *
 * @param   CANHandler [out] structure type variable to configure CAN operation mode
 * @param   CANTxHeader [out] structure type variable to configure CAN transmicion
 *
 * @retval  none
 *
 * @note none
 */

void Serial_Init( void )
{
    FDCAN_FilterTypeDef CANFilter;

    /* Declaration of the options to configure the FDCAN1 module to transmit to the CAN bus at 100Kbps
     and sample point of 75%, the frequency with which the CAN module is powered is
     fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler
     fCAN = 32MHz / 1 / 20 = 1.6MHz
     The number of time quantas required is
     Ntq = fCAN / CANbaudrate
     Ntq = 1.6MHz / 100Kbps = 16 
     The sample point percentage is
     Sp = ( CANHandler.Init.NominalTimeSeg1 +  1 / Ntq ) * 100
     Sp = ( ( 11 + 1 ) / 16 ) * 100 = 75% */

    CANHandler.Instance                  = FDCAN1;
    CANHandler.Init.Mode                 = FDCAN_MODE_NORMAL;
    CANHandler.Init.FrameFormat          = FDCAN_FRAME_CLASSIC;
    CANHandler.Init.ClockDivider         = FDCAN_CLOCK_DIV1;
    CANHandler.Init.TxFifoQueueMode      = FDCAN_TX_FIFO_OPERATION;
    CANHandler.Init.AutoRetransmission   = DISABLE;
    CANHandler.Init.TransmitPause        = DISABLE;
    CANHandler.Init.ProtocolException    = DISABLE;
    CANHandler.Init.ExtFiltersNbr        = 0;
    CANHandler.Init.StdFiltersNbr        = 1;  /* inicialize filter */
    CANHandler.Init.NominalPrescaler     = 20;
    CANHandler.Init.NominalSyncJumpWidth = 1;
    CANHandler.Init.NominalTimeSeg1      = 11;
    CANHandler.Init.NominalTimeSeg2      = 4;
    /* set configuration of CAN module*/
    HAL_FDCAN_Init( &CANHandler );

    /* Declaration of options for configur parameters of CAN transmicion */
    CANTxHeader.IdType      = FDCAN_STANDARD_ID;
    CANTxHeader.FDFormat    = FDCAN_CLASSIC_CAN;
    CANTxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CANTxHeader.Identifier  = 0x122;
    CANTxHeader.DataLength  = FDCAN_DLC_BYTES_8;

    /* Configure reception filter to Rx FIFO 0, it will only accept messages with ID 0x111 */
    CANFilter.IdType       = FDCAN_STANDARD_ID;
    CANFilter.FilterIndex  = 0;
    CANFilter.FilterType   = FDCAN_FILTER_MASK;
    CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    CANFilter.FilterID1    = 0x111;
    CANFilter.FilterID2    = 0x7FF;
    HAL_FDCAN_ConfigFilter( &CANHandler, &CANFilter );
    /* messages without the filter will by rejected*/
    HAL_FDCAN_ConfigGlobalFilter( &CANHandler, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE );
    
    /* Change FDCAN instance from initialization mode to normal mode */
    HAL_FDCAN_Start( &CANHandler );

    /* activate interruption by reception in fifo0 to the arrive of a message */
    HAL_FDCAN_ActivateNotification( &CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0 );
}

/**
 * @brief   **main state machine**
 *
 * is going to implement the state machine in charge of messages processing
 *
 * @param   msgRecieve [in] to verify if there is a new message
 * @param   NewMessage [out] to storage the message form CAN
 *
 * @retval  none
 *
 * @note none
 */

void Serial_Task( void )
{
    uint8_t OkMessage[2]    = {0x01, 0x55};
    uint8_t ErrorMessage[2] = {0x01, 0xAA};
    uint16_t year = ( NewMessage[4] << 8 ) + NewMessage[5];
    
    switch ( STATE_CONTROL )
    {
        case STATE_IDLE:
            msgRecieve = CanTp_SingleFrameRx( NewMessage, NewMessage );

            if( msgRecieve == 1u )
            {
                STATE_CONTROL = STATE_MESSAGE;
            }
            else
            {
                STATE_CONTROL = STATE_IDLE;
            }
            break;
        
        case STATE_MESSAGE:
            if( NewMessage[1] == 0x01u )
            {
                STATE_CONTROL = STATE_TIME;
            }
            else if( NewMessage[1] == 0x02u )
            {
                STATE_CONTROL = STATE_DATE;
            }
            else if( NewMessage[1] == 0x03u )
            {
                STATE_CONTROL = STATE_ALARM;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;

        case STATE_TIME:
            // check for hours, minutes and seconds
            if( (NewMessage[2] >= 0x00u) && (NewMessage[2] <= 0x23u) &&
                (NewMessage[3] >= 0x00u) && (NewMessage[3] <= 0x59u) &&
                (NewMessage[4] >= 0x00u) && (NewMessage[4] <= 0x59u) ) 
            {
                STATE_CONTROL = STATE_OK;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;
        
        case STATE_DATE:
            // check for days, months and year
            if( (NewMessage[2] >= 0x01u) && (NewMessage[2] <= 0x31u) &&
                (NewMessage[3] >= JANUARY) && (NewMessage[3] <= DECEMBER) &&
                (year >= 0x1901u) && (year <= 0x2099u) )
            {
                if( (year % 4u) == 0u ) // check leap year, february and coeficient for day of the week
                {
                    if( (NewMessage[3] == FEBRUARY) && (NewMessage[2] <= 0x29u) )
                    {
                        STATE_CONTROL = STATE_OK;
                    }
                }
                else if( (NewMessage[3] == FEBRUARY) && (NewMessage[2] <= 0x28u) ) // check for february
                {
                    STATE_CONTROL = STATE_OK;
                }
                else if( ( (NewMessage[3] == APRIL) || 
                           (NewMessage[3] == JUNE) || 
                           (NewMessage[3] == SEPTEMBER) || 
                           (NewMessage[3] == NOVEMBER)) && 
                           (NewMessage[3] <= 0x30u) ) // check for months with 30 days
                {
                    STATE_CONTROL = STATE_OK;
                }
                else if( (NewMessage[3] == JANUARY) || 
                         (NewMessage[3] == MARCH) || 
                         (NewMessage[3] == MAY) || 
                         (NewMessage[3] == JULY) || 
                         (NewMessage[3] == AUGUST) ||
                         (NewMessage[3] == OCTOBER) || 
                         (NewMessage[3] == DECEMBER) ) // otherwise, the month has 31 days
                {
                    STATE_CONTROL = STATE_OK;
                }
                else{
                }
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;

        case STATE_ALARM:
            // check for hours and minutes
            if( (NewMessage[2] >= 0x00u) && (NewMessage[2] <= 0x23u) &&
                (NewMessage[3] >= 0x00u) && (NewMessage[3] <= 0x59u) )
            {
                STATE_CONTROL = STATE_OK;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;

        case STATE_OK:
            CanTp_SingleFrameTx( OkMessage, 2 );
            STATE_CONTROL = STATE_IDLE;
            break;
    
        case STATE_ERROR:
            CanTp_SingleFrameTx( ErrorMessage, 2 );
            STATE_CONTROL = STATE_IDLE;
            break;

        default :
            break;
    }
}

/**
 * @brief   **reception of CAN messages**
 *
 * add the new message to the fifo queue
 *
 * @param   NewMessage [in] to storage the message form CAN
 * @param   flagMessage [out] flag to indicate a new message arrive
 *
 * @retval  none
 *
 * @note none
 */

/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
    FDCAN_RxHeaderTypeDef CANRxHeader;

    if(( RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE ) != 0 )
    {
        /* Retrieve Rx messages from RX FIFO0 */
        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, NewMessage );
        
        flagMessage = 1u;
    }
}

/**
 * @brief   **to send the bytes specified**
 *
 * this function add the new message to the bufer with the help of the function
 * HAL_FDCAN_AddMessageToTxFifoQ defined in HAL library
 *
 * @retval  none
 *
 * @note none
 */

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size )
{
    UNUSED(size);
    HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
}

/**
 * @brief   **to read messages from CAN**
 *
 *  the function not validate any message received, just read the bytes from CAN
 *
 * @param   msgRecieve [out] to verify if there is a new message
 * @param   flagMessage [in/out] flag to indicate a new message arrive
 * @param   MSGHandler [out] structure type variable to place all data
 *
 * @retval  the function returns 1 when a certain number of bytes were received, 
 *          otherwise, no message was received
 *
 * @note none
 */

static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size )
{
    UNUSED(size);
    APP_Messages message = data[1];
    msgRecieve = SERIAL_MSG_NONE;

    if( flagMessage == 1u )
    {
        MSGHandler.msg = data[1];

        switch ( message )
        {
            case SERIAL_MSG_TIME:
                MSGHandler.tm.tm_hour = data[2];
                MSGHandler.tm.tm_min  = data[3];
                MSGHandler.tm.tm_sec  = data[4];
               break;

            case SERIAL_MSG_DATE:
                MSGHandler.tm.tm_mday = data[2];
                MSGHandler.tm.tm_mon  = data[3];
                MSGHandler.tm.tm_yday = data[4];
                MSGHandler.tm.tm_year = data[5];
                break;


            case SERIAL_MSG_ALARM:
                MSGHandler.tm.tm_hour = data[2];
                MSGHandler.tm.tm_min  = data[3];
                break;
        
            default :
                break;
        }

        flagMessage = 0;
        msgRecieve = 1u;
    }

    return msgRecieve;
}
