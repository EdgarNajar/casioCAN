/**
 * @file    app_serial.c
 * @brief   **Message processing implementation**
 *
 * Contains the functions to initialize the CAN port with the apropiate parameters,
 * the callback to the CAN interruption, also the main state machine to message processing were
 * the CAN frame will be evaluate to aprove it or not and to be assigned to the struct
 * variable by the fuction in charge of it.
 *
 * @note    None
 */
#include "app_bsp.h"
#include "app_serial.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );

/**
 * @brief  Structure type variable for user CAN initialization
 */
/* cppcheck-suppress misra-c2012-8.4 ; function declared fie app_bsp.h as extern */
FDCAN_HandleTypeDef CANHandler;

/**
 * @brief  Structure type variable for CAN transmissin initialization
 */
static FDCAN_TxHeaderTypeDef CANTxHeader;

/**
 * @brief  Structure type variable for time data
 */
APP_MsgTypeDef MSGHandler;

/**
 * @brief  Variable for new message
 */
static uint8_t NewMessage[num_8];

/**
 * @brief  Variable to indicate the interrup of message occur
 */
static uint8_t flagMessage = num_0;

/**
 * @brief  To indicate a new message arrived
 */
static uint8_t msgRecieve;

/**
 * @brief  Is used to move in state machine
 */
APP_States STATE_CONTROL = STATE_IDLE;

/**
 * @brief   **Initialize of CAN port**
 *
 * Is the function to initialize all the required to start working with the CAN port 
 * and the messages reception processing
 *
 * @param   CANHandler [out] Structure type variable to configure CAN operation mode
 * @param   CANTxHeader [out] Structure type variable to configure CAN transmicion
 *
 * @note None
 */
void Serial_Init( void )
{
    FDCAN_FilterTypeDef CANFilter;

    /* Declaration of the options to configure the FDCAN1 module to transmit to the CAN bus at 100Kbps
     and sample point of 75%, the frequency with which the CAN module is powered is
     fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler
     fCAN = 16MHz / 1 / 10 = 1.6MHz
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
    CANHandler.Init.ExtFiltersNbr        = val_ExtFiltersNbr;
    CANHandler.Init.StdFiltersNbr        = val_StdFiltersNbr;  /* inicialize filter */
    CANHandler.Init.NominalPrescaler     = val_NominalPrescaler;
    CANHandler.Init.NominalSyncJumpWidth = val_NominalSyncJumpWidth;
    CANHandler.Init.NominalTimeSeg1      = val_NominalTimeSeg1;
    CANHandler.Init.NominalTimeSeg2      = val_NominalTimeSeg2;
    /* Set configuration of CAN module*/
    HAL_FDCAN_Init( &CANHandler );

    /* Declaration of options for configur parameters of CAN transmicion */
    CANTxHeader.IdType      = FDCAN_STANDARD_ID;
    CANTxHeader.FDFormat    = FDCAN_CLASSIC_CAN;
    CANTxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CANTxHeader.Identifier  = val_Identifier;
    CANTxHeader.DataLength  = FDCAN_DLC_BYTES_8;

    /* Configure reception filter to Rx FIFO 0, it will only accept messages with ID 0x111 */
    CANFilter.IdType       = FDCAN_STANDARD_ID;
    CANFilter.FilterIndex  = val_FilterIndex;
    CANFilter.FilterType   = FDCAN_FILTER_MASK;
    CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    CANFilter.FilterID1    = val_FilterID1;
    CANFilter.FilterID2    = val_FilterID2;
    HAL_FDCAN_ConfigFilter( &CANHandler, &CANFilter );
    /* Messages without the filter will by rejected */
    HAL_FDCAN_ConfigGlobalFilter( &CANHandler, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE );
    
    /* Change FDCAN instance from initialization mode to normal mode */
    HAL_FDCAN_Start( &CANHandler );

    /* Activate interruption by reception in fifo0 to the arrive of a message */
    HAL_FDCAN_ActivateNotification( &CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, val_BufferIndexes );
}

/**
 * @brief   **Main state machine**
 *
 * Is going to implement the state machine in charge of messages processing
 * after the interruption of CAN is trigger, depending if it is a message of time, 
 * date and alarm it will be evalate if fits the conditions for every type of data,
 * then a message will be send to indicate success or error.
 *
 * @param   msgRecieve [in] To verify if there is a new message
 * @param   NewMessage [out] To storage the message form CAN
 *
 * @note None
 */
void Serial_Task( void )
{
    uint8_t OkMessage[num_2]    = {hex_1, hex_55};
    uint8_t ErrorMessage[num_2] = {hex_1, hex_AA};
    uint16_t year = ( NewMessage[num_4] << num_8 ) + NewMessage[num_5];
    
    switch ( STATE_CONTROL )
    {
        case STATE_IDLE:
            msgRecieve = CanTp_SingleFrameRx( NewMessage, NewMessage );

            if( msgRecieve == num_1 )
            {
                STATE_CONTROL = STATE_MESSAGE;
            }
            else
            {
                STATE_CONTROL = STATE_IDLE;
            }
            break;
        
        case STATE_MESSAGE:
            if( (NewMessage[num_1] == num_1) &&
                (NewMessage[num_0] == num_4) ) // check for single frame code
            {
                STATE_CONTROL = STATE_TIME;
            }
            else if( (NewMessage[num_1] == num_2) &&
                     (NewMessage[num_0] == num_5) ) // check for single frame code
            {
                STATE_CONTROL = STATE_DATE;
            }
            else if( (NewMessage[num_1] == num_3) &&
                     (NewMessage[num_0] == num_3) ) // check for single frame code
            {
                STATE_CONTROL = STATE_ALARM;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;

        case STATE_TIME:
            // Check for hours, minutes and seconds
            if( (NewMessage[num_2] >= hex_0) && (NewMessage[num_2] <= hex_23) &&
                (NewMessage[num_3] >= hex_0) && (NewMessage[num_3] <= hex_59) &&
                (NewMessage[num_4] >= hex_0) && (NewMessage[num_4] <= hex_59) ) 
            {
                STATE_CONTROL = STATE_OK;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;
        
        case STATE_DATE:
            // Check for days, months and year
            if( (NewMessage[num_2] >= hex_1) && (NewMessage[num_2] <= hex_31) &&
                (NewMessage[num_3] >= JANUARY) && (NewMessage[num_3] <= DECEMBER) &&
                (year >= hex_1901) && (year <= hex_2099) )
            {
                if( (year % num_4) == num_0 ) // Check leap year and february
                {
                    if( (NewMessage[num_3] == FEBRUARY) && (NewMessage[num_2] <= hex_29) )
                    {
                        STATE_CONTROL = STATE_OK;
                    }
                }
                else if( (NewMessage[num_3] == FEBRUARY) && (NewMessage[num_2] <= hex_28) ) // Check for february
                {
                    STATE_CONTROL = STATE_OK;
                }
                else if( ( (NewMessage[num_3] == APRIL) || 
                           (NewMessage[num_3] == JUNE) || 
                           (NewMessage[num_3] == SEPTEMBER) || 
                           (NewMessage[num_3] == NOVEMBER)) && 
                           (NewMessage[num_3] <= hex_30) ) // Check for months with 30 days
                {
                    STATE_CONTROL = STATE_OK;
                }
                else if( (NewMessage[num_3] == JANUARY) || 
                         (NewMessage[num_3] == MARCH) || 
                         (NewMessage[num_3] == MAY) || 
                         (NewMessage[num_3] == JULY) || 
                         (NewMessage[num_3] == AUGUST) ||
                         (NewMessage[num_3] == OCTOBER) || 
                         (NewMessage[num_3] == DECEMBER) ) // Otherwise, the month has 31 days
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
            // Check for hours and minutes
            if( (NewMessage[num_2] >= hex_0) && (NewMessage[num_2] <= hex_23) &&
                (NewMessage[num_3] >= hex_0) && (NewMessage[num_3] <= hex_59) )
            {
                STATE_CONTROL = STATE_OK;
            }
            else
            {
                STATE_CONTROL = STATE_ERROR;
            }
            break;

        case STATE_OK:
            CanTp_SingleFrameTx( OkMessage, num_1 );
            STATE_CONTROL = STATE_IDLE;
            break;
    
        case STATE_ERROR:
            CanTp_SingleFrameTx( ErrorMessage, num_1 );
            STATE_CONTROL = STATE_IDLE;
            break;

        default :
            break;
    }
}

/**
 * @brief   **Reception of CAN messages**
 *
 * Add the new message to the fifo queue
 *
 * @param   hfdcan [in] To handle CAN
 * @param   RxFifo0ITs [in] To storage the CAN message
 * @param   flagMessage [out] Flag to indicate a new message arrive
 * @param   NewMessage [out] Function to storage the new message
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
    FDCAN_RxHeaderTypeDef CANRxHeader;

    if(( RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE ) != num_0 )
    {
        /* Retrieve Rx messages from RX FIFO0 */
        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, NewMessage );
        
        flagMessage = num_0;
    }
}

/**
 * @brief   **To send the bytes specified**
 *
 * This function add the new message to the bufer with the help of the function
 * HAL_FDCAN_AddMessageToTxFifoQ defined in HAL library
 *
 * @param   data [in] Pointer to data
 * @param   size [in] Size of data
 * 
 * @note None
 */
static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size )
{
    UNUSED(size);
    HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
}

/**
 * @brief   **To read messages from CAN**
 *
 * The function not validate any message received, just read the bytes from CAN
 *
 * @param   data [in] Pointer to data
 * @param   size [in] Size of data
 * @param   msgRecieve [out] To verify if there is a new message
 * @param   flagMessage [in/out] Flag to indicate a new message arrive
 * @param   MSGHandler [out] Structure type variable to place all data
 *
 * @retval  The function returns 1 when a certain number of bytes were received, 
 *          otherwise, no message was received
 *
 * @note None
 */
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size )
{
    UNUSED(size);
    APP_Messages message = data[num_1];
    msgRecieve = SERIAL_MSG_NONE;

    if( flagMessage == num_1 )
    {
        MSGHandler.msg = data[num_1];

        switch ( message )
        {
            case SERIAL_MSG_TIME:
                MSGHandler.tm.tm_hour = data[num_2];
                MSGHandler.tm.tm_min  = data[num_3];
                MSGHandler.tm.tm_sec  = data[num_4];
               break;

            case SERIAL_MSG_DATE:
                MSGHandler.tm.tm_mday = data[num_2];
                MSGHandler.tm.tm_mon  = data[num_3];
                MSGHandler.tm.tm_yday = data[num_4];
                MSGHandler.tm.tm_year = data[num_5];
                break;


            case SERIAL_MSG_ALARM:
                MSGHandler.tm.tm_hour = data[num_2];
                MSGHandler.tm.tm_min  = data[num_3];
                break;
        
            default :
                break;
        }

        flagMessage = num_0;
        msgRecieve  = num_1;
    }

    return msgRecieve;
}
