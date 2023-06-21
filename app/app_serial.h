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
#define NUM_0   (uint8_t)0   /*!< Number 0   */
#define NUM_1   (uint8_t)1   /*!< Number 1   */
#define NUM_2   (uint8_t)2   /*!< Number 2   */
#define NUM_3   (uint8_t)3   /*!< Number 3   */
#define NUM_4   (uint8_t)4   /*!< Number 4   */
#define NUM_5   (uint8_t)5   /*!< Number 5   */
#define NUM_7   (uint8_t)7   /*!< Number 7   */
#define NUM_8   (uint8_t)8   /*!< Number 8   */
#define NUM_10  (uint8_t)10  /*!< Number 10  */
#define NUM_12  (uint8_t)12  /*!< Number 12  */
#define NUM_13  (uint8_t)13  /*!< Number 13  */
#define NUM_100 (uint8_t)100 /*!< Number 100 */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define HEX_0     (uint8_t)0x00     /*!< Number 0 in hexa    */
#define HEX_1     (uint8_t)0x01     /*!< Number 1 in hexa    */
#define HEX_2     (uint8_t)0x02     /*!< Number 2 in hexa    */
#define HEX_3     (uint8_t)0x03     /*!< Number 3 in hexa    */
#define HEX_4     (uint8_t)0x04     /*!< Number 4 in hexa    */
#define HEX_5     (uint8_t)0x05     /*!< Number 5 in hexa    */
#define HEX_6     (uint8_t)0x06     /*!< Number 6 in hexa    */
#define HEX_0F    (uint8_t)0x0F     /*!< Number F in hexa    */
#define HEX_23    (uint8_t)0x23     /*!< Number 23 in hexa   */
#define HEX_28    (uint8_t)0x28     /*!< Number 28 in hexa   */
#define HEX_29    (uint8_t)0x29     /*!< Number 29 in hexa   */
#define HEX_30    (uint8_t)0x30     /*!< Number 30 in hexa   */
#define HEX_31    (uint8_t)0x31     /*!< Number 31 in hexa   */
#define HEX_55    (uint8_t)0x55     /*!< Number 55 in hexa   */
#define HEX_59    (uint8_t)0x59     /*!< Number 59 in hexa   */
#define HEX_AA    (uint8_t)0xAA     /*!< Number AA in hexa   */
#define HEX_0F    (uint8_t)0x0F     /*!< Number 0F in hexa   */
#define HEX_1901  (uint32_t)0x1901  /*!< Number 1901 in hexa */
#define HEX_2099  (uint32_t)0x2099  /*!< Number 2099 in hexa */
/**
  @} */

/** 
  * @defgroup Defines for CAN configuration
  @{ */
#define VAL_EXTFILTERSNBR        (uint32_t)0      /*!< Value for ExtFiltersNbr        */
#define VAL_STDFILTERSNDR        (uint32_t)1      /*!< Value for StdFiltersNbr        */
#define VAL_NOMINALPRESCALER     (uint32_t)20     /*!< Value for NominalPrescaler     */
#define VAL_NOMINALSYNCJUMPWIDTH (uint32_t)1      /*!< Value for NominalSyncJumpWidth */
#define VAL_NOMINALTIMESEG1      (uint32_t)11     /*!< Value for NominalTimeSeg1      */
#define VAL_NOMINALTIMESEG2      (uint32_t)4      /*!< Value for NominalTimeSeg2      */
#define VAL_IDENTIFIER           (uint32_t)0x122  /*!< Value for Identifier           */
#define VAL_FILTERINDEX          (uint32_t)0      /*!< Value for FilterIndex          */
#define VAL_FILTERID1            (uint32_t)0x111  /*!< Value for FilterID1            */
#define VAL_FILTERID2            (uint32_t)0x7FF  /*!< Value for FilterID2            */
#define VAL_BUFFERINDEXES        (uint32_t)0      /*!< Value for BufferIndexes        */
/**
  @} */

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif