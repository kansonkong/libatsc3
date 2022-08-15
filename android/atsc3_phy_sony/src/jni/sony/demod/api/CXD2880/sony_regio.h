/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_regio.h

 @brief The abstract register I/O interface definition.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_REGIO_H
#define SONY_REGIO_H

#include "sony_common.h"
#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/**
 @brief Register I/O target definition.
*/
typedef enum {
    SONY_REGIO_TARGET_SYSTEM,  /**< System part (SLV-X) */
    SONY_REGIO_TARGET_DEMOD    /**< Demod part (SLV-T) */
} sony_regio_target_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief The register I/O API defintion.

        TunerDemod part functions call these functions to read/write registers.
        This APIs are exist to abstract concrete interface between device IC and main SoC.
*/
typedef struct sony_regio_t {
    /**
     @brief Read a sub-addressed register from the device.
            Multiple byte reads are stored into pData in ascending order.
            e.g pData[0] = Value of subAddr
                pData[1] = Value of subAddr+1

     @param pRegio Register I/O struct instance.
     @param target Access target.
     @param subAddress The sub address.
     @param pData The buffer to store the register contents into.
     @param size The number of bytes to read from the device.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*ReadRegister)(struct sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t * pData, uint32_t size);
    /**
     @brief Write a sub-addressed register of the device.
            Multiple byte writes are stored into the device register
            in ascending order.
            e.g pData[0] = Value to set in subAddr
                pData[1] = Value to set in subAddr+1

     @param pRegio Register I/O struct instance.
     @param target Access target.
     @param subAddress The sub address.
     @param pData The buffer to write into register contents.
     @param size The number of bytes to write to the  device.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*WriteRegister)(struct sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, const uint8_t * pData, uint32_t size);
    /**
     @brief Write a single sub-addressed register of the device.

     @param pRegio Register I/O struct instance.
     @param target Access target.
     @param subAddress The sub address.
     @param data The byte to write into register contents.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*WriteOneRegister)(struct sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t data);

    void *  pIfObject;        /**< Interface object (I2C/SPI/SDIO) */

    uint8_t i2cAddressSystem; /**< I2C slave address of SYSTEM part. (Used for I2C) */
    uint8_t i2cAddressDemod;  /**< I2C slave address of DEMOD part. (Used for I2C) */

    uint8_t slaveSelect;      /**< Slave select ID (Used for SPI/SDIO) */

    void *  user;             /**< User defined data. */
} sony_regio_t;

/*------------------------------------------------------------------------------
  Register I/O helper functions
------------------------------------------------------------------------------*/
/**
 @brief Provides a ::sony_regio_t::WriteOneRegister implementation.

 @param pRegio Register I/O struct instance.
 @param target Access target.
 @param subAddress The sub address.
 @param data The byte to write into register contents.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_regio_CommonWriteOneRegister(sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t data);

/**
 @brief The driver uses this function to perform a read-modify-write
        cycle on a single register contents.

 @param pRegio Register I/O struct instance.
 @param target Access target.
 @param subAddress The sub address.
 @param data The byte to OR with the register contents.
 @param mask The mask to apply with the data.

 @return SONY_RESULT_OK if successful.

*/
sony_result_t sony_regio_SetRegisterBits(sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t data, uint8_t mask);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_REGIO_H */
