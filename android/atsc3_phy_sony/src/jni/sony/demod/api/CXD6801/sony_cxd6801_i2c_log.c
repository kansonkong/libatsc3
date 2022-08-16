/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/11/30
  Modification ID : 253a4918e2da2cf28a9393596fa16f25024e504d
------------------------------------------------------------------------------*/

#include "sony_cxd6801_i2c_log.h"

/*------------------------------------------------------------------------------
  I2c functions for logging.
  These function calls "Real" i2c access functions and output the data.
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t LogRead(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData,
                                uint32_t size, uint8_t mode)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_i2c_log_t *pI2cLog = NULL;

    SONY_CXD6801_TRACE_I2C_ENTER("LogRead");

    if((!pI2c) || (!pI2c->user) || (!pData)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog = (sony_i2c_log_t*)(pI2c->user);

    if(!pI2cLog->pI2cReal){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Real i2c access */
    result = pI2cLog->pI2cReal->Read(pI2cLog->pI2cReal, deviceAddress, pData, size, mode);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_I2C_RETURN(result);
    }

    if(pI2cLog->fp){
        unsigned int i = 0;
        fprintf(pI2cLog->fp, "R (%02X)    ", deviceAddress);
        for(i=0; i<size; i++){
            fprintf(pI2cLog->fp, " %02X", pData[i]);
        }
        fprintf(pI2cLog->fp, "\n");
    }
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

static sony_cxd6801_result_t LogWrite(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t * pData,
                                uint32_t size, uint8_t mode)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_i2c_log_t *pI2cLog = NULL;

    SONY_CXD6801_TRACE_I2C_ENTER("LogWrite");

    if((!pI2c) || (!pI2c->user) || (!pData)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog = (sony_i2c_log_t*)(pI2c->user);

    if(!pI2cLog->pI2cReal){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Real i2c access */
    result = pI2cLog->pI2cReal->Write(pI2cLog->pI2cReal, deviceAddress, pData, size, mode);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_I2C_RETURN(result);
    }

    if(pI2cLog->fp){
        unsigned int i = 0;
        fprintf(pI2cLog->fp, "W (%02X)    ", deviceAddress);
        for(i=0; i<size; i++){
            fprintf(pI2cLog->fp, " %02X", pData[i]);
        }
        fprintf(pI2cLog->fp, "\n");
    }
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

static sony_cxd6801_result_t LogReadRegister(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress,
                                        uint8_t* pData, uint32_t size)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_i2c_log_t *pI2cLog = NULL;

    SONY_CXD6801_TRACE_I2C_ENTER("LogReadRegister");

    if((!pI2c) || (!pI2c->user) || (!pData)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog = (sony_cxd6801_i2c_log_t*)(pI2c->user);

    if(!pI2cLog->pI2cReal){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Real i2c access */
    result = pI2cLog->pI2cReal->ReadRegister(pI2cLog->pI2cReal, deviceAddress, subAddress, pData, size);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_I2C_RETURN(result);
    }

    if(pI2cLog->fp){
        unsigned int i = 0;
        fprintf(pI2cLog->fp, "RR(%02X, %02X)", deviceAddress, subAddress);
        for(i=0; i<size; i++){
            fprintf(pI2cLog->fp, " %02X", pData[i]);
        }
        fprintf(pI2cLog->fp, "\n");
    }
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

static sony_cxd6801_result_t LogWriteRegister(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress,
                                        const uint8_t* pData, uint32_t size)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_i2c_log_t *pI2cLog = NULL;

    SONY_CXD6801_TRACE_I2C_ENTER("LogWriteRegister");

    if((!pI2c) || (!pI2c->user) || (!pData)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog = (sony_cxd6801_i2c_log_t*)(pI2c->user);

    if(!pI2cLog->pI2cReal){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Real i2c access */
    result = pI2cLog->pI2cReal->WriteRegister(pI2cLog->pI2cReal, deviceAddress, subAddress, pData, size);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_I2C_RETURN(result);
    }

    if(pI2cLog->fp){
        unsigned int i = 0;
        fprintf(pI2cLog->fp, "WR(%02X, %02X)", deviceAddress, subAddress);
        for(i=0; i<size; i++){
            fprintf(pI2cLog->fp, " %02X", pData[i]);
        }
        fprintf(pI2cLog->fp, "\n");
    }
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

/*--------------------------------------------------------------------
  I2c struct instance creation (for logging)

  sony_i2c_t*         pI2c         Instance of i2c control struct
  sony_i2c_t*         pI2cReal     Instance of "Real" i2c control struct
  sony_i2c_log_t*     pI2cLog      Instance of sony_i2c_log_t struct
--------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_i2c_CreateI2cLog(sony_cxd6801_i2c_t *pI2c, sony_cxd6801_i2c_t *pI2cReal, sony_i2c_log_t *pI2cLog)
{
    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_CreateI2cLog");

    if((!pI2c) || (!pI2cReal) || (!pI2cLog)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2c->Read = LogRead;
    pI2c->Write = LogWrite;
    pI2c->ReadRegister = LogReadRegister;
    pI2c->WriteRegister = LogWriteRegister;
    pI2c->WriteOneRegister = sony_cxd6801_i2c_CommonWriteOneRegister;
    pI2c->gwAddress = pI2cReal->gwAddress;
    pI2c->gwSub = pI2cReal->gwSub;
    pI2c->user = pI2cLog;

    pI2cLog->pI2cReal = pI2cReal;
    pI2cLog->fp = NULL;

    SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_OK);
}

/*--------------------------------------------------------------------
  Enable/Disable i2c logging

  sony_i2c_t*         pI2c         Instance of i2c control struct
  FILE*               fp           File pointer for saving log (NULL->disable logging)
--------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_i2c_EnableI2cLog(sony_cxd6801_i2c_t *pI2c, FILE *fp)
{
    sony_cxd6801_i2c_log_t *pI2cLog = NULL;

    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_EnableI2cLog");

    if((!pI2c) || (!pI2c->user)){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog = (sony_cxd6801_i2c_log_t*)(pI2c->user);

    if(!pI2cLog->pI2cReal){
        SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pI2cLog->fp = fp;

    SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_OK);
}

