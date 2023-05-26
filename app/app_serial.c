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
#include "app_serial.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );
static void Valid_Time( uint8_t *data );
static void Valid_Date( uint8_t *data );

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
APP_States state_control = STATE_IDLE;

/**
 * @brief   **Initialize of CAN port**
 *
 * Is the function to initialize all the required to start working with the CAN port 
 * and the messages reception processing
 *
 * @param   CANHandler  [out] Structure type variable to configure CAN operation mode
 * @param   CANTxHeader [out] Structure type variable to configure CAN transmicion
 *
 * @note    Declaration of the options to configure the FDCAN1 module to transmit to the CAN bus at 100Kbps
 *          and sample point of 75%, the frequency with which the CAN module is powered is
 *          fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler
 *          fCAN = 16MHz / 1 / 10 = 1.6MHz
 *          The number of time quantas required is
 *          Ntq = fCAN / CANbaudrate
 *          Ntq = 1.6MHz / 100Kbps = 16
 *          The sample point percentage is
 *          Sp = ( CANHandler.Init.NominalTimeSeg1 +  1 / Ntq ) * 100
 *          Sp = ( ( 11 + 1 ) / 16 ) * 100 = 75%
 */
void Serial_Init( void )
{
    FDCAN_FilterTypeDef CANFilter;

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
 * @param   msgRecieve     [in]  To verify if there is a new message
 * @param   state_control  [in]  Is used to move in state machine
 * @param   NewMessage     [out] To storage the message form CAN
 * @param   flagMessage    [out] To indicate a new message arrived
 *
 * @note None
 */
void Serial_Task( void )
{
    uint8_t i = num_0;
    uint8_t msn_error[1] = {hex_AA};
    uint8_t msn_ok[1]    = {hex_55};
    
    switch ( state_control )
    {
        case STATE_IDLE:
            if( flagMessage == num_1 )
            {
                state_control = STATE_MESSAGE;
            }
            else
            {
                state_control = STATE_IDLE;
            }

            flagMessage = num_0;
            break;
        
        case STATE_MESSAGE:
            msgRecieve = CanTp_SingleFrameRx( &NewMessage[num_1], &NewMessage[num_0] );
            
            if( msgRecieve == num_1 )
            {
                if( NewMessage[num_0] == (uint8_t)SERIAL_MSG_TIME )
                {
                    state_control = STATE_TIME;
                }
                else if( NewMessage[num_0] == (uint8_t)SERIAL_MSG_DATE )
                {
                    state_control = STATE_DATE;
                }
                else if( NewMessage[num_0] == (uint8_t)SERIAL_MSG_ALARM ) 
                {
                    state_control = STATE_ALARM;
                }
                else
                {}
            }
            else if( msgRecieve == (uint8_t)SERIAL_MSG_NONE )
            {
                state_control = STATE_ERROR;
            }
            else
            {}
            break;

        case STATE_TIME:
            // Check for hours, minutes and seconds
            Valid_Time( &NewMessage[num_0] );
            break;
        
        case STATE_DATE:
            // Check for days, months and year
            Valid_Date( &NewMessage[num_0] );
            break;

        case STATE_ALARM:
            // Check for hours and minutes
            Valid_Time( &NewMessage[num_0] );
            break;

        case STATE_OK:
            if( NewMessage[num_0] == num_1 )
            {
                i = (num_4 << 4) + (hex_55 & hex_0F);
            }
            else if( NewMessage[num_0] == num_2 )
            {
                i = (num_5 << 4) + (hex_55 & hex_0F);
            }
            else if( NewMessage[num_0] == num_3 )
            {
                i = (num_3 << 4) + (hex_55 & hex_0F);
            }
            else
            {}

            CanTp_SingleFrameTx( &msn_ok[num_0], i );
            
            state_control = STATE_IDLE;
            break;
    
        case STATE_ERROR:
            if( NewMessage[num_0] == num_1 )
            {
                i = (num_4 << 4) + (hex_AA & hex_0F);
            }
            else if( NewMessage[num_0] == num_2 )
            {
                i = (num_5 << 4) + (hex_AA & hex_0F);

            }
            else if( NewMessage[num_0] == num_3 )
            {
                i = (num_3 << 4) + (hex_AA & hex_0F);
            }
            else
            {}

            CanTp_SingleFrameTx( &msn_error[num_0], i );

            state_control = STATE_IDLE;
            break;

        default :
            break;
    }
}

/**
 * @brief   **Reception of CAN messages**
 *
 * Callback to the CAN interrup, it is activated when a message is recieve
 * then is store in the variable NewMessage and puts flagMessage to one 
 *
 * @param   hfdcan      [in]  To handle CAN
 * @param   RxFifo0ITs  [in]  void Valid_Time( uint8_t *data );
 * @param   flagMessage [out] Flag to indicate a new message arrive
 * @param   NewMessage  [out] Function to storage the new message
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
        
        flagMessage = num_1;
    }
}

/**
 * @brief   **Validate time**
 *
 * This function validates if the time message is in range,
 * from 0 to 23 for hours, from 0 to 59 for minutes and
 * from 0 to 59 for seconds and returns the result.
 * Its also use to vaidate if the Alarm message is in range,
 * from 0 to 23 for hours and from 0 to 59 to minutes, 
 * no seconds nedded, and then returns the result.
 * The function changes the value of variable state_control 
 * if the values for hour, minute and second were valid or not.
 * Then store the data in the structure MSGHandler.
 *
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 * @param   state_control [out] Variable to control state machine
 *
 * @note None
 */
static void Valid_Time( uint8_t *data )
{
    if( data[num_0] == num_1 ) // Time message
    {
        if( ((data[num_1] >= hex_0) && (data[num_1] <= hex_23)) &&
            ((data[num_2] >= hex_0) && (data[num_2] <= hex_59)) &&
            ((data[num_3] >= hex_0) && (data[num_3] <= hex_59)) ) 
        {
            state_control = STATE_OK;

            MSGHandler.msg        = data[num_0];
            MSGHandler.tm.tm_hour = data[num_1];
            MSGHandler.tm.tm_min  = data[num_2];
            MSGHandler.tm.tm_sec  = data[num_3];
        }
        else
        {
            state_control = STATE_ERROR;
        }
    }
    else // Alarm message
    {
        if( ((data[num_1] >= hex_0) && (data[num_1] <= hex_23)) &&
            ((data[num_2] >= hex_0) && (data[num_2] <= hex_59)) ) 
        {
            state_control = STATE_OK;

            MSGHandler.msg        = data[num_0];
            MSGHandler.tm.tm_hour = data[num_1];
            MSGHandler.tm.tm_min  = data[num_2];
        }
        else
        {
            state_control = STATE_ERROR;
        }
    }  
}

/**
 * @brief   **Validate date**
 *
 * This function validates if the date message is in range,
 * from 1 to 31 for days, from 1 to 12 for months and
 * from 1901 to 2099 for years and returns the result.
 * In addition it also vaidates if the date is in a leap-year
 * and adjust the month of February to manage 29 days.
 * The function changes the value of variable state_control 
 * if the values for day, months and year were valid or not.
 * Then store the data in the structure MSGHandler.
 * 
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 * @param   state_control [out] Variable to control state machine
 *
 * @retval  The function returns STATE_OK if the data for day, month and year was correct
 *          and STATE_ERROR if the data was an invalid value
 *
 * @note None
 */
static void Valid_Date( uint8_t *data )
{ 
    uint16_t year;
    year = data[num_3];
    year = (year << num_8)  + data[num_4];

    if( ((data[num_1] >= hex_1)   && (data[num_1] <= hex_31)) &&
        ((data[num_2] >= JANUARY) && (data[num_2] <= DECEMBER)) &&
        ((year >= hex_1901) && (year <= hex_2099)) )
    {
        if( (year % num_4) == num_0 ) // Check leap year and february
        {
            if( (data[num_2] == FEBRUARY) && (data[num_1] <= hex_29) )
            {
                state_control = STATE_OK;

                MSGHandler.msg        = data[num_0];
                MSGHandler.tm.tm_mday = data[num_1];
                MSGHandler.tm.tm_mon  = data[num_2];
                MSGHandler.tm.tm_yday = data[num_3];
                MSGHandler.tm.tm_year = data[num_4];
            }
            else
            {
                state_control = STATE_ERROR;
            }
        }
        else if( (data[num_2] == FEBRUARY) && (data[num_1] <= hex_28) ) // Check for february
        {
            state_control = STATE_OK;

            MSGHandler.msg        = data[num_0];
            MSGHandler.tm.tm_mday = data[num_1];
            MSGHandler.tm.tm_mon  = data[num_2];
            MSGHandler.tm.tm_yday = data[num_3];
            MSGHandler.tm.tm_year = data[num_4];
        }
        else if( ( (data[num_2] == APRIL) || 
                   (data[num_2] == JUNE) || 
                   (data[num_2] == SEPTEMBER) || 
                   (data[num_2] == NOVEMBER)) && 
                   (data[num_2] <= hex_30) ) // Check for months with 30 days
        {
            state_control = STATE_OK;

            MSGHandler.msg        = data[num_0];
            MSGHandler.tm.tm_mday = data[num_1];
            MSGHandler.tm.tm_mon  = data[num_2];
            MSGHandler.tm.tm_yday = data[num_3];
            MSGHandler.tm.tm_year = data[num_4];
        }
        else if( (data[num_2] == JANUARY) || 
                 (data[num_2] == MARCH) || 
                 (data[num_2] == MAY) || 
                 (data[num_2] == JULY) || 
                 (data[num_2] == AUGUST) ||
                 (data[num_2] == OCTOBER) || 
                 (data[num_2] == DECEMBER) ) // Otherwise, the month has 31 days
        {
            state_control = STATE_OK;

            MSGHandler.msg        = data[num_0];
            MSGHandler.tm.tm_mday = data[num_1];
            MSGHandler.tm.tm_mon  = data[num_2];
            MSGHandler.tm.tm_yday = data[num_3];
            MSGHandler.tm.tm_year = data[num_4];
        }
        else
        {
            state_control = STATE_ERROR;
        }
    }
    else
    {
        state_control = STATE_ERROR;
    }
}

/**
 * @brief   **Transmition in CAN-TP single frame format**
 *
 * This function transmit the ok or error messages through the terminal
 * when the data is already procesed, will be send 0x55 if the data was in range
 * and the 0xAA if the data was not in range with CAN-ID of 0x122
 * 
 *
 * @param   data [in] Pointer to data
 * @param   size [in] Size of data
 * 
 * @note None
 */
static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size )
{
    uint8_t data_length;

    data_length = size >> 4;

    for( uint8_t i = num_0; i < data_length; i++)
    {
        data[i + num_1] = data[num_0];
    }

    data[num_0] = data_length;

    HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
}

/**
 * @brief   **CAN-TP single frame format**
 *
 * The function validate if the message received complies with CAN-TP single frame format
 * then eliminates the firs byte and pack the ret of the data to be processed
 *
 * @param   data        [in]     Pointer to data
 * @param   size        [in]     Size of data
 * @param   msgRecieve  [out]    To verify if there is a new message
 * @param   flagMessage [in/out] Flag to indicate a new message arrive
 * @param   MSGHandler  [out]    Structure type variable to place all data
 * @param   NewMessage  [out]    Variable to storage CAN message
 *
 * @retval  The function returns 1 when a certain number of bytes were received, 
 *          otherwise, no message was received
 *
 * @note None
 */
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size )
{
    uint8_t frame_type;
    uint8_t data_length;

    frame_type  = size[num_0] >> num_4;
    data_length = size[num_0] & hex_0F;

    if( ((frame_type == num_0) && (data_length == num_3)) ||
        ((frame_type == num_0) && (data_length == num_4)) ||
        ((frame_type == num_0) && (data_length == num_5)) )
    {
        for(uint8_t i = num_0; i < data_length; i++)
        {
            NewMessage[i] = data[i];
        }

        msgRecieve = num_1;
    }
    else
    {
        msgRecieve = num_0;
    }

    return msgRecieve;
}
