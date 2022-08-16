/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2014/11/27
  Modification ID : c385cfc07211cb751200038a6e4232fe8b2f8cb5
------------------------------------------------------------------------------*/
/**
 @file  sony_devio_i2c.h

 @brief The I/O interface via I2C.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DEVIO_I2C_H
#define SONY_DEVIO_I2C_H

#include "sony_common.h"
#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
#include "sony_regio.h"
#include "sony_i2c.h"

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/**
 @brief Set up the Register I/O struct instance for I2C.

 @param pRegio           Register I/O struct instance.
 @param pI2c             The I2C APIs that the driver will use.
 @param i2cAddressDemod  I2C slave address of DEMOD side in 8bit form. (0xD8/0xDA/0xC8/0xCA)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_regio_i2c_Create(sony_regio_t *pRegio, sony_i2c_t *pI2c, uint8_t i2cAddressDemod);


#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_DEVIO_I2C_H */
