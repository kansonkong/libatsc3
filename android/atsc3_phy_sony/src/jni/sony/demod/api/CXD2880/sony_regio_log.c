/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/01/16
  Modification ID : d4b3fe129855d90c25271de02796915983f1e6ee
------------------------------------------------------------------------------*/

#include "sony_regio_log.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
  Register read/write functions for logging.
  These function calls "Real" register I/O access functions and output the data.
------------------------------------------------------------------------------*/
static sony_result_t LogReadRegister (sony_regio_t* pRegio, sony_regio_target_t target,
                                      uint8_t subAddress, uint8_t * pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_regio_log_t *pRegioLog = NULL;

    SONY_TRACE_IO_ENTER ("LogReadRegister");

    if ((!pRegio) || (!pRegio->user)) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pRegioLog = (sony_regio_log_t*)(pRegio->user);

    if (!pRegioLog->pRegioReal) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Real register access */
    result = pRegioLog->pRegioReal->ReadRegister (pRegioLog->pRegioReal, target, subAddress, pData, size);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_IO_RETURN (result);
    }

    if (pRegioLog->fp) {
        unsigned int i = 0;

        if (pRegioLog->regioIf == SONY_REGIO_LOG_IF_I2C) {
            if (target == SONY_REGIO_TARGET_SYSTEM) {
                fprintf (pRegioLog->fp, "RR(SLV-X(%02X), %02X)", pRegio->i2cAddressSystem, subAddress);
            } else {
                fprintf (pRegioLog->fp, "RR(SLV-T(%02X), %02X)", pRegio->i2cAddressDemod, subAddress);
            }
        } else {
            if (target == SONY_REGIO_TARGET_SYSTEM) {
                fprintf (pRegioLog->fp, "RR(SLV-X(%d), %02X)", pRegio->slaveSelect, subAddress);
            } else {
                fprintf (pRegioLog->fp, "RR(SLV-T(%d), %02X)", pRegio->slaveSelect, subAddress);
            }
        }

        for (i = 0; i < size; i++) {
            fprintf (pRegioLog->fp, " %02X", pData[i]);
        }
        fprintf (pRegioLog->fp, "\n");
    }
    SONY_TRACE_IO_RETURN (result);
}

static sony_result_t LogWriteRegister (sony_regio_t* pRegio, sony_regio_target_t target,
                                       uint8_t subAddress, const uint8_t * pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_regio_log_t *pRegioLog = NULL;

    SONY_TRACE_IO_ENTER ("LogWriteRegister");

    if ((!pRegio) || (!pRegio->user)) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pRegioLog = (sony_regio_log_t*)(pRegio->user);

    if (!pRegioLog->pRegioReal) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Real register access */
    result = pRegioLog->pRegioReal->WriteRegister (pRegioLog->pRegioReal, target, subAddress, pData, size);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_IO_RETURN (result);
    }

    if (pRegioLog->fp) {
        unsigned int i = 0;

        if (pRegioLog->regioIf == SONY_REGIO_LOG_IF_I2C) {
            if (target == SONY_REGIO_TARGET_SYSTEM) {
                fprintf (pRegioLog->fp, "WR(SLV-X(%02X), %02X)", pRegio->i2cAddressSystem, subAddress);
            } else {
                fprintf (pRegioLog->fp, "WR(SLV-T(%02X), %02X)", pRegio->i2cAddressDemod, subAddress);
            }
        } else {
            if (target == SONY_REGIO_TARGET_SYSTEM) {
                fprintf (pRegioLog->fp, "WR(SLV-X(%d), %02X)", pRegio->slaveSelect, subAddress);
            } else {
                fprintf (pRegioLog->fp, "WR(SLV-T(%d), %02X)", pRegio->slaveSelect, subAddress);
            }
        }

        for (i = 0; i < size; i++) {
            fprintf (pRegioLog->fp, " %02X", pData[i]);
        }
        fprintf (pRegioLog->fp, "\n");
    }
    SONY_TRACE_IO_RETURN (result);
}

sony_result_t sony_regio_CreateRegioLog (sony_regio_t *pRegio, sony_regio_t *pRegioReal,
                                         sony_regio_log_t *pRegioLog, sony_regio_log_if_t regioIf)
{
    SONY_TRACE_IO_ENTER("sony_regio_CreateRegioLog");

    if((!pRegio) || (!pRegioReal) || (!pRegioLog)){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    pRegio->ReadRegister = LogReadRegister;
    pRegio->WriteRegister = LogWriteRegister;
    pRegio->WriteOneRegister = sony_regio_CommonWriteOneRegister;
    pRegio->pIfObject = pRegioReal->pIfObject;
    pRegio->i2cAddressSystem = pRegioReal->i2cAddressSystem;
    pRegio->i2cAddressDemod = pRegioReal->i2cAddressDemod;
    pRegio->slaveSelect = pRegioReal->slaveSelect;
    pRegio->user = pRegioLog;

    pRegioLog->pRegioReal = pRegioReal;
    pRegioLog->fp = NULL;
    pRegioLog->regioIf = regioIf;

    SONY_TRACE_IO_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_regio_EnableRegioLog (sony_regio_t *pRegio, FILE *fp)
{
    sony_regio_log_t *pRegioLog = NULL;

    SONY_TRACE_IO_ENTER("sony_regio_EnableRegioLog");

    if((!pRegio) || (!pRegio->user)){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    pRegioLog = (sony_regio_log_t*)(pRegio->user);

    if(!pRegioLog->pRegioReal){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    pRegioLog->fp = fp;

    SONY_TRACE_IO_RETURN(SONY_RESULT_OK);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 