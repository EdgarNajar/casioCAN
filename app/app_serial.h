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

#include "app_bsp.h"

/** 
  * @defgroup Months, months of the year in hexa
  @{ */
#define JANUARY   (uint8_t)0x01  /*!< January in hexa   */
#define FEBRUARY  (uint8_t)0x02  /*!< February in hexa  */
#define MARCH     (uint8_t)0x03  /*!< March in hexa     */
#define APRIL     (uint8_t)0x04  /*!< April in hexa     */
#define MAY       (uint8_t)0x05  /*!< May in hexa       */
#define JUNE      (uint8_t)0x06  /*!< June in hexa      */
#define JULY      (uint8_t)0x07  /*!< July in hexa      */
#define AUGUST    (uint8_t)0x08  /*!< August in hexa    */
#define SEPTEMBER (uint8_t)0x09  /*!< September in hexa */
#define OCTOBER   (uint8_t)0x10  /*!< October in hexa   */
#define NOVEMBER  (uint8_t)0x11  /*!< November in hexa  */
#define DECEMBER  (uint8_t)0x12  /*!< December in hexa  */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define num_0 (uint8_t)0  /*!< Number 0 */
#define num_1 (uint8_t)1  /*!< Number 1 */
#define num_2 (uint8_t)2  /*!< Number 2 */
#define num_3 (uint8_t)3  /*!< Number 3 */
#define num_4 (uint8_t)4  /*!< Number 4 */
#define num_5 (uint8_t)5  /*!< Number 5 */
#define num_8 (uint8_t)8  /*!< Number 8 */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define hex_0     (uint8_t)0x00     /*!< Number 0 in hexa    */
#define hex_1     (uint8_t)0x01     /*!< Number 1 in hexa    */
#define hex_23    (uint8_t)0x23     /*!< Number 23 in hexa   */
#define hex_28    (uint8_t)0x28     /*!< Number 28 in hexa   */
#define hex_29    (uint8_t)0x29     /*!< Number 29 in hexa   */
#define hex_30    (uint8_t)0x30     /*!< Number 30 in hexa   */
#define hex_31    (uint8_t)0x31     /*!< Number 31 in hexa   */
#define hex_55    (uint8_t)0x55     /*!< Number 55 in hexa   */
#define hex_59    (uint8_t)0x59     /*!< Number 59 in hexa   */
#define hex_AA    (uint8_t)0xAA     /*!< Number AA in hexa   */
#define hex_0F    (uint8_t)0x0F     /*!< Number 0F in hexa   */
#define hex_1901  (uint32_t)0x1901  /*!< Number 1901 in hexa */
#define hex_2099  (uint32_t)0x2099  /*!< Number 2099 in hexa */
/**
  @} */

/** 
  * @defgroup Defines for CAN configuration
  @{ */
#define val_ExtFiltersNbr        0      /*!< Value for ExtFiltersNbr        */
#define val_StdFiltersNbr        1      /*!< Value for StdFiltersNbr        */
#define val_NominalPrescaler     10     /*!< Value for NominalPrescaler     */
#define val_NominalSyncJumpWidth 1      /*!< Value for NominalSyncJumpWidth */
#define val_NominalTimeSeg1      11     /*!< Value for NominalTimeSeg1      */
#define val_NominalTimeSeg2      4      /*!< Value for NominalTimeSeg2      */
#define val_Identifier           0x122  /*!< Value for Identifier           */
#define val_FilterIndex          0      /*!< Value for FilterIndex          */
#define val_FilterID1            0x111  /*!< Value for FilterID1            */
#define val_FilterID2            0x7FF  /*!< Value for BufferIndexes        */
#define val_BufferIndexes        0      /*!< Value for BufferIndexes        */
/**
  @} */

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif