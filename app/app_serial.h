/**
 * @file    app_serial.h
 * @brief   **Message processing implementation**
 *
 * Contains the functions to initialize the CAN port with the apropiate parameters,
 * the callback to the CAN interruption, also the main state machine to message processing were
 * the CAN frame will be evaluate to aprove it or not and to be assigned to the struct
 * variable by the fuction in charge of it.
 * 
 * @note    None
 */
#ifndef APP_SERIAL_H__
#define APP_SERIAL_H__

/** 
  * @defgroup Months, months of the year in hexa
  @{ */
#define JANUARY   (uint8_t)0x01  /*!< january in hexa   */
#define FEBRUARY  (uint8_t)0x02  /*!< february in hexa  */
#define MARCH     (uint8_t)0x03  /*!< march in hexa     */
#define APRIL     (uint8_t)0x04  /*!< april in hexa     */
#define MAY       (uint8_t)0x05  /*!< may in hexa       */
#define JUNE      (uint8_t)0x06  /*!< june in hexa      */
#define JULY      (uint8_t)0x07  /*!< july in hexa      */
#define AUGUST    (uint8_t)0x08  /*!< august in hexa    */
#define SEPTEMBER (uint8_t)0x09  /*!< september in hexa */
#define OCTOBER   (uint8_t)0x10  /*!< october in hexa   */
#define NOVEMBER  (uint8_t)0x11  /*!< november in hexa  */
#define DECEMBER  (uint8_t)0x12  /*!< december in hexa  */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define num_0 (uint8_t)0  /*!< number 0 */
#define num_1 (uint8_t)1  /*!< number 1 */
#define num_2 (uint8_t)2  /*!< number 2 */
#define num_3 (uint8_t)3  /*!< number 3 */
#define num_4 (uint8_t)4  /*!< number 4 */
#define num_5 (uint8_t)5  /*!< number 5 */
#define num_8 (uint8_t)8  /*!< number 8 */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define hex_0     (uint8_t)0x00     /*!< number 0 in hexa    */
#define hex_1     (uint8_t)0x01     /*!< number 1 in hexa    */
#define hex_23    (uint8_t)0x23     /*!< number 23 in hexa   */
#define hex_28    (uint8_t)0x28     /*!< number 28 in hexa   */
#define hex_29    (uint8_t)0x29     /*!< number 29 in hexa   */
#define hex_30    (uint8_t)0x30     /*!< number 30 in hexa   */
#define hex_31    (uint8_t)0x31     /*!< number 31 in hexa   */
#define hex_55    (uint8_t)0x55     /*!< number 55 in hexa   */
#define hex_59    (uint8_t)0x59     /*!< number 59 in hexa   */
#define hex_AA    (uint8_t)0xAA     /*!< number AA in hexa   */
#define hex_1901  (uint32_t)0x1901  /*!< number 1901 in hexa */
#define hex_2099  (uint32_t)0x2099  /*!< number 2099 in hexa */
/**
  @} */

/** 
  * @defgroup Defines for CAN configuration
  @{ */
#define val_ExtFiltersNbr        0      /*!< value for ExtFiltersNbr        */
#define val_StdFiltersNbr        1      /*!< value for StdFiltersNbr        */
#define val_NominalPrescaler     10     /*!< value for NominalPrescaler     */
#define val_NominalSyncJumpWidth 1      /*!< value for NominalSyncJumpWidth */
#define val_NominalTimeSeg1      11     /*!< value for NominalTimeSeg1      */
#define val_NominalTimeSeg2      4      /*!< value for NominalTimeSeg2      */
#define val_Identifier           0x122  /*!< value for Identifier           */
#define val_FilterIndex          0      /*!< value for FilterIndex          */
#define val_FilterID1            0x111  /*!< value for FilterID1            */
#define val_FilterID2            0x7FF  /*!< value for BufferIndexes        */
#define val_BufferIndexes        0      /*!< value for BufferIndexes        */
/**
  @} */

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif