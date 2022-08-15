/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_regio_log.h

 @brief The logging interface for register I/O access via sony_regio_t.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONY_REGIO_LOG_H
#define SONY_REGIO_LOG_H

#include <stdio.h>
#include "sony_regio.h"

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/**
 @brief Register I/O interface type.
*/
typedef enum {
    SONY_REGIO_LOG_IF_I2C,  /**< I2C */
    SONY_REGIO_LOG_IF_SPI,  /**< SPI */
    SONY_REGIO_LOG_IF_SDIO  /**< SDIO */
} sony_regio_log_if_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief The register I/O logging structure. This struct instance is stored in "user" member of sony_regio_t struct.
*/
typedef struct sony_regio_log_t {
    sony_regio_t* pRegioReal;      /**< "Real" register I/O struct instance */
    FILE* fp;                      /**< FILE pointer for saving log data */
    sony_regio_log_if_t regioIf;   /**< Register I/O interface type */
} sony_regio_log_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/**
  @brief register I/O struct instance creation (for logging)

  @param pRegio         Instance of register I/O control struct
  @param pRegioReal     Instance of "Real" register I/O control struct
  @param pRegioLog      Instance of sony_regio_log_t struct
  @param regioIf        Interface type
  @return SONY_RESULT_OK if success
*/
sony_result_t sony_regio_CreateRegioLog (sony_regio_t *pRegio, sony_regio_t *pRegioReal,
                                         sony_regio_log_t *pRegioLog, sony_regio_log_if_t regioIf);

/**
  @brief Enable/Disable register I/O logging

  @param pRegio         Instance of register I/O control struct
  @param fp             File pointer for saving log (NULL->disable logging)
  @return SONY_RESULT_OK if success
*/
sony_result_t sony_regio_EnableRegioLog (sony_regio_t *pRegio, FILE *fp);

#endif /* SONY_REGIO_LOG_H */
