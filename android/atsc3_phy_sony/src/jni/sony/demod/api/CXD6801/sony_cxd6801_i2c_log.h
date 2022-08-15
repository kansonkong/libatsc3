/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
/**
 @file  sony_i2c_log.h

 @brief The logging interface for I2C I/O access via sony_i2c_t.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_I2C_LOG_H
#define SONY_CXD6801_I2C_LOG_H

#include <stdio.h>
#include "sony_cxd6801_i2c.h"

/**
 @brief The i2c logging structure. This struct instance is stored in "user" member of sony_i2c_t struct.
*/
typedef struct sony_cxd6801_i2c_log  {
    sony_cxd6801_i2c_t* pI2cReal;   /**< "Real" i2c struct instance */
    FILE* fp;               /**< FILE pointer for saving log data */
} sony_cxd6801_i2c_log_t;

typedef sony_cxd6801_i2c_log_t sony_i2c_log_t;
/**
  @brief I2c struct instance creation (for logging)

  @param pI2c         Instance of i2c control struct
  @param pI2cReal     Instance of "Real" i2c control struct
  @param pI2cLog      Instance of sony_i2c_log_t struct
  @return SONY_RESULT_OK if success
*/
sony_cxd6801_result_t sony_i2c_CreateI2cLog(sony_cxd6801_i2c_t *pI2c, sony_cxd6801_i2c_t *pI2cReal, sony_cxd6801_i2c_log_t *pI2cLog);

/**
  @brief Enable/Disable i2c logging

  @param pI2c         Instance of i2c control struct
  @param fp           File pointer for saving log (NULL->disabling logging)
  @return SONY_RESULT_OK if success
*/
sony_cxd6801_result_t sony_i2c_EnableI2cLog(sony_cxd6801_i2c_t *pI2c, FILE *fp);

#endif /* SONY_I2C_LOG_H */
