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
#include "hil_queue.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );
static uint8_t Valid_Time( uint8_t *data );
static uint8_t Valid_Alarm( uint8_t *data );
static uint8_t Valid_Date( uint8_t *data );
static void WeekDay( uint8_t *data );
static void Serial_StMachine( void );
static void State_Ok( void );
static void State_Error( void );

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
static uint8_t NewMessage[NUM_8];

/**
 * @brief  To indicate a new message arrived
 */
static uint8_t msgRecieve;

/**
 * @brief  To storage the messages from CAN
 */
static QUEUE_HandleTypeDef CANqueue;

/**
 * @brief  To storage messages from serial
 */
QUEUE_HandleTypeDef SerialQueue;

/**
 * @brief  Struct type variable to handle te queue
 */
QUEUE_HandleTypeDef queueHandle;

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
 *          fCAN = 32MHz / 1 / 20 = 1.6MHz
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
    CANHandler.Init.ExtFiltersNbr        = VAL_EXTFILTERSNBR;
    CANHandler.Init.StdFiltersNbr        = VAL_STDFILTERSNDR;  /* inicialize filter */
    CANHandler.Init.NominalPrescaler     = VAL_NOMINALPRESCALER;
    CANHandler.Init.NominalSyncJumpWidth = VAL_NOMINALSYNCJUMPWIDTH;
    CANHandler.Init.NominalTimeSeg1      = VAL_NOMINALTIMESEG1;
    CANHandler.Init.NominalTimeSeg2      = VAL_NOMINALTIMESEG2;
    /* Set configuration of CAN module*/
    Status = HAL_FDCAN_Init( &CANHandler );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_INIT_RET_ERROR );

    /* Declaration of options for configur parameters of CAN transmicion */
    CANTxHeader.IdType      = FDCAN_STANDARD_ID;
    CANTxHeader.FDFormat    = FDCAN_CLASSIC_CAN;
    CANTxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CANTxHeader.Identifier  = VAL_IDENTIFIER;
    CANTxHeader.DataLength  = FDCAN_DLC_BYTES_8;

    /* Configure reception filter to Rx FIFO 0, it will only accept messages with ID 0x111 */
    CANFilter.IdType       = FDCAN_STANDARD_ID;
    CANFilter.FilterIndex  = VAL_FILTERINDEX;
    CANFilter.FilterType   = FDCAN_FILTER_MASK;
    CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    CANFilter.FilterID1    = VAL_FILTERID1;
    CANFilter.FilterID2    = VAL_FILTERID2;
    Status = HAL_FDCAN_ConfigFilter( &CANHandler, &CANFilter );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_CONFIL_RET_ERROR );
    /* Messages without the filter will by rejected */
    Status = HAL_FDCAN_ConfigGlobalFilter( &CANHandler, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_CONFIGLOB_RET_ERROR );

    /* Change FDCAN instance from initialization mode to normal mode */
    Status = HAL_FDCAN_Start( &CANHandler );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_START_RET_ERROR );

    /* Activate interruption by reception in fifo0 to the arrive of a message */
    Status = HAL_FDCAN_ActivateNotification( &CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, VAL_BUFFERINDEXES );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_ACTNOT_RET_ERROR );

    static uint64_t CAN_buffer[MSG_10];
    CANqueue.Buffer   = CAN_buffer;
    CANqueue.Elements = MSG_10;
    CANqueue.Size     = sizeof( uint64_t );
    HIL_QUEUE_Init( &CANqueue );

    static APP_MsgTypeDef Serial_buffer[MSG_10];
    SerialQueue.Buffer   = Serial_buffer;
    SerialQueue.Elements = MSG_10;
    SerialQueue.Size     = sizeof( APP_MsgTypeDef );
    HIL_QUEUE_Init( &SerialQueue );
}

/**
 * @brief   **Call of the serial state machine**
 *
 * This function calls the serial state machine every 10ms
 * with the help of queues, therefore it won't be execute all the time
 *
 * @note None
 */
void Serial_Task( void )
{
    while( HIL_QUEUE_IsEmptyISR( &CANqueue, TIM16_FDCAN_IT0_IRQn ) == NUM_0 )
    {
        /*Read the first message*/
        (void)HIL_QUEUE_ReadISR( &CANqueue, NewMessage, TIM16_FDCAN_IT0_IRQn );
        MSGHandler.msg = NewMessage[NUM_1];
        Serial_StMachine();
    }
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
 * @param   NewMessage     [out] To storage the message form CAN
 *
 * @note None
 */
void Serial_StMachine( void )
{
    switch ( MSGHandler.msg )
    {
        case STATE_TIME:
            if( CanTp_SingleFrameRx( &NewMessage[NUM_1], &NewMessage[NUM_0] ) == NUM_1 )
            {
                MSGHandler.tm.tm_hour = NewMessage[NUM_1];
                MSGHandler.tm.tm_min  = NewMessage[NUM_2];
                MSGHandler.tm.tm_sec  = NewMessage[NUM_3];

                if( Valid_Time( &NewMessage[NUM_0] ) == NUM_1 )
                {
                    MSGHandler.msg = SERIAL_MSG_TIME;
                    State_Ok();
                }
                else
                {
                    MSGHandler.msg = SERIAL_MSG_NONE;
                    State_Error();
                }
            }
            break;
        
        case STATE_DATE:
            if( CanTp_SingleFrameRx( &NewMessage[NUM_1], &NewMessage[NUM_0] ) == NUM_1 )
            {
                MSGHandler.tm.tm_mday = NewMessage[NUM_1];
                MSGHandler.tm.tm_mon  = NewMessage[NUM_2];
                MSGHandler.tm.tm_yday = NewMessage[NUM_3];
                MSGHandler.tm.tm_year = NewMessage[NUM_4];
                WeekDay( &NewMessage[NUM_0] );

                if( Valid_Date( &NewMessage[NUM_0] ) == NUM_1 )
                {
                    MSGHandler.msg = SERIAL_MSG_DATE;
                    State_Ok();
                }
                else
                {
                    MSGHandler.msg = SERIAL_MSG_NONE;
                    State_Error();
                }
            }
            break;

        case STATE_ALARM:
            if( CanTp_SingleFrameRx( &NewMessage[NUM_1], &NewMessage[NUM_0] ) == NUM_1 )
            {
                MSGHandler.tm.tm_hour = NewMessage[NUM_1];
                MSGHandler.tm.tm_min  = NewMessage[NUM_2];

                if( Valid_Alarm( &NewMessage[NUM_0] ) == NUM_1 )
                {
                    MSGHandler.msg = SERIAL_MSG_ALARM;
                    State_Ok();
                }
                else
                {
                    MSGHandler.msg = SERIAL_MSG_NONE;
                    State_Error();
                }
            }
            break;

            default :
                break;
    }
}

/**
 * @brief   **Message ok**
 *
 * This function write a message in case the CAN message is correct
 * and sends it with the help of the funcion CanTp_SingleFrameTx
 *
 * @param   NewMessage [in] New message from CAN
 *
 * @note None
 */
void State_Ok( void )
{
    uint8_t i = NUM_0;
    uint8_t msn_ok[NUM_1]    = {HEX_55};

    if( NewMessage[NUM_0] == NUM_1 )
    {
        i = (NUM_4 << NUM_4) + (HEX_55 & HEX_0F);
    }
    else if( NewMessage[NUM_0] == NUM_2 )
    {
        i = (NUM_5 << NUM_4) + (HEX_55 & HEX_0F);
    }
    else if( NewMessage[NUM_0] == NUM_3 )
    {
        i = (NUM_3 << NUM_4) + (HEX_55 & HEX_0F);
    }
    else
    {}

    (void)HIL_QUEUE_WriteISR( &SerialQueue, &MSGHandler, TIM16_FDCAN_IT0_IRQn );
    CanTp_SingleFrameTx( &msn_ok[NUM_0], i );
}

/**
 * @brief   **Message error**
 *
 * This function write a message in case the CAN message is  not correct
 * and sends it with the help of the funcion CanTp_SingleFrameTx
 *
 * @param   NewMessage [in] New message from CAN
 *
 * @note None
 */
void State_Error( void )
{
    uint8_t i = NUM_0;
    uint8_t msn_error[NUM_1] = {HEX_AA};

    if( NewMessage[NUM_0] == NUM_1 )
    {
        i = (NUM_4 << NUM_4) + (HEX_AA & HEX_0F);
    }
    else if( NewMessage[NUM_0] == NUM_2 )
    {
        i = (NUM_5 << NUM_4) + (HEX_AA & HEX_0F);
    }
    else if( NewMessage[NUM_0] == NUM_3 )
    {
        i = (NUM_3 << NUM_4) + (HEX_AA & HEX_0F);
    }
    else
    {}

    CanTp_SingleFrameTx( &msn_error[NUM_0], i );
}

/**
 * @brief   **Reception of CAN messages**
 *
 * Callback to the CAN interrup, it is activated when a message 
 * is recieve then is store in the variable NewMessage
 *
 * @param   hfdcan      [in]  To handle CAN
 * @param   RxFifo0ITs  [in]  void Valid_Time( uint8_t *data );
 * @param   NewMessage  [out] Function to storage the new message
 *
 * @note None
 */
/* cppcheck-suppress misra-c2012-8.4 ; function defined in HAL library */
void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
    FDCAN_RxHeaderTypeDef CANRxHeader;

    if(( RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE ) != NUM_0 )
    {
        /* Retrieve Rx messages from RX FIFO0 */
        Status = HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, NewMessage );
        /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
        assert_error( Status == HAL_OK, CAN_GETMSG_RET_ERROR );

        (void)HIL_QUEUE_Write( &CANqueue, NewMessage );
    }
}

/**
 * @brief   **Validate time**
 *
 * This function validates if the time message is in range,
 * from 0 to 23 for hours, from 0 to 59 for minutes and
 * from 0 to 59 for seconds and returns the result.
 * Then store the data in the structure MSGHandler.
 *
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 * @param   state_control [out] Variable to control state machine
 *
 * @retval  The function returns 1 if time is correct and 0 if not 
 * 
 * @note None
 */
static uint8_t Valid_Time( uint8_t *data )
{
    uint8_t ret_val = NUM_0;

    if( ((data[NUM_1] >= HEX_0) && (data[NUM_1] <= HEX_23)) &&
        ((data[NUM_2] >= HEX_0) && (data[NUM_2] <= HEX_59)) &&
        ((data[NUM_3] >= HEX_0) && (data[NUM_3] <= HEX_59)) ) 
    {
        ret_val = NUM_1;
    }

    return ret_val;
}

/**
 * @brief   **Validate alarm**
 *
 * This function validates if the Alarm message is in range,
 * from 0 to 23 for hours and from 0 to 59 to minutes, 
 * no seconds nedded, and then returns the result.
 * Then store the data in the structure MSGHandler.
 *
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 * @param   state_control [out] Variable to control state machine
 *
 * @retval  The function returns 1 if the alarm is correct and 0 if not 
 * 
 * @note None
 */
static uint8_t Valid_Alarm( uint8_t *data )
{
    uint8_t ret_val = NUM_0;

    if( ((data[NUM_1] >= HEX_0) && (data[NUM_1] <= HEX_23)) &&
        ((data[NUM_2] >= HEX_0) && (data[NUM_2] <= HEX_59)) ) 
    {
        ret_val = NUM_1;
    }

    return ret_val;
}

/**
 * @brief   **Validate date**
 *
 * This function validates if the date message is in range,
 * from 1 to 31 for days, from 1 to 12 for months and
 * from 1901 to 2099 for years and returns the result.
 * In addition it also vaidates if the date is in a leap-year
 * and adjust the month of February to manage 29 days.
 * Then store the data in the structure MSGHandler.
 * 
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 * @param   state_control [out] Variable to control state machine
 *
 * @retval  The function returns 1 if the date is correct and 0 if not 
 *
 * @note None
 */
static uint8_t Valid_Date( uint8_t *data )
{
    uint8_t ret_val;
    uint16_t year;
    year = data[NUM_3];
    year = (year << NUM_8)  + data[NUM_4];

    if( ((data[NUM_1] >= HEX_1)   && (data[NUM_1] <= HEX_31)) &&
        ((data[NUM_2] >= JANUARY) && (data[NUM_2] <= DECEMBER)) &&
        ((year >= HEX_1901) && (year <= HEX_2099)) )
    {
        if( (year % NUM_4) == NUM_0 ) // Check leap year and february
        {
            if( (data[NUM_2] == FEBRUARY) && (data[NUM_1] <= HEX_29) )
            {
                ret_val = NUM_1;
            }
            else
            {
                ret_val = NUM_0;
            }
        }
        else if( (data[NUM_2] == FEBRUARY) && (data[NUM_1] <= HEX_28) ) // Check for february
        {
            ret_val = NUM_1;
        }
        else if( ( (data[NUM_2] == APRIL) || 
                   (data[NUM_2] == JUNE) || 
                   (data[NUM_2] == SEPTEMBER) || 
                   (data[NUM_2] == NOVEMBER)) && 
                   (data[NUM_2] <= HEX_30) ) // Check for months with 30 days
        {
            ret_val = NUM_1;
        }
        else if( (data[NUM_2] == JANUARY) || 
                 (data[NUM_2] == MARCH) || 
                 (data[NUM_2] == MAY) || 
                 (data[NUM_2] == JULY) || 
                 (data[NUM_2] == AUGUST) ||
                 (data[NUM_2] == OCTOBER) || 
                 (data[NUM_2] == DECEMBER) ) // Otherwise, the month has 31 days
        {
            ret_val = NUM_1;
        }
        else
        {
            ret_val = NUM_0;
        }
    }
    else
    {
        ret_val = NUM_0;
    }

    return ret_val;
}

/**
 * @brief   **Validate the day of the week**
 *
 * This function calculates the day of the week according to the date,
 * to do this, we will use Zeller's congruence. Taking the day, month and year
 * to calculate the year of the century and the century. In this algorithm 
 * January and February are counted as months 13 and 14 of the previous year.
 * With this data we use the formula for the Gregorian calendar.
 * 
 * @param   data          [in]  Pointer to data
 * @param   MSGHandler    [out] Structure type variable to place all data
 *
 * @note None
 */
void WeekDay( uint8_t *data )
{
    uint32_t dayofweek;
    uint32_t days;
    uint8_t month;
    uint8_t MSyear;
    uint8_t LSyear;
    uint8_t correctdays[NUM_7] = {HEX_5, HEX_6, HEX_0, HEX_1, HEX_2, HEX_3, HEX_4};
    uint16_t year;
    uint16_t century;
    uint16_t yearcentury;

    days   = (data[NUM_1] >> NUM_4) & HEX_0F;
    days   = (days*NUM_10) + (data[NUM_1] & HEX_0F);
    month  = (data[NUM_2] >> NUM_4) & HEX_0F;
    month  = (month*NUM_10) + (data[NUM_2] & HEX_0F);
    MSyear = (data[NUM_3] >> NUM_4) & HEX_0F;
    MSyear = (MSyear*NUM_10) + (data[NUM_3] & HEX_0F);
    LSyear = (data[NUM_4] >> NUM_4) & HEX_0F;
    LSyear = (LSyear*NUM_10) + (data[NUM_4] & HEX_0F);

    year = ((uint16_t)MSyear*NUM_100) + (uint16_t)LSyear;

    if( month < NUM_3 )
    {
        month += NUM_12;
        year--;
    }

    century = year / NUM_100;
    yearcentury = year % NUM_100;

    dayofweek = ((uint32_t)days + ((NUM_13 * ((uint32_t)month + NUM_1))/NUM_5) + yearcentury + ((uint32_t)yearcentury/NUM_4) + ((uint32_t)century/NUM_4) + (NUM_5*(uint32_t)century)) % NUM_7;

    dayofweek = correctdays[dayofweek];
    
    MSGHandler.tm.tm_wday = dayofweek + NUM_1;
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

    data_length = size >> NUM_4;

    for( uint8_t i = NUM_0; i < data_length; i++)
    {
        data[i + NUM_1] = data[NUM_0];
    }

    data[NUM_0] = data_length;

    Status = HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
    /* cppcheck-suppress misra-c2012-11.8 ; Nedded to the macro to detect erros */
    assert_error( Status == HAL_OK, CAN_ADDMSG_RET_ERROR );
}

/**
 * @brief   **CAN-TP single frame format**
 *
 * The function validate if the message received complies with CAN-TP single frame format
 * then eliminates the first byte and pack the rest of the data to be processed
 *
 * @param   data        [in]     Pointer to data
 * @param   size        [in]     Size of data
 * @param   msgRecieve  [out]    To verify if there is a new message
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

    frame_type  = size[NUM_0] >> NUM_4;
    data_length = size[NUM_0] & HEX_0F;

    if( ((frame_type == NUM_0) && (data_length == NUM_3)) ||
        ((frame_type == NUM_0) && (data_length == NUM_4)) ||
        ((frame_type == NUM_0) && (data_length == NUM_5)) )
    {
        for(uint8_t i = NUM_0; i < data_length; i++)
        {
            NewMessage[i] = data[i];
        }

        msgRecieve = NUM_1;
    }
    else
    {
        msgRecieve = NUM_0;
    }

    return msgRecieve;
}
