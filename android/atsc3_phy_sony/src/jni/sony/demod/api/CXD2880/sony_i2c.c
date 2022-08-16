/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/

#include "sony_i2c.h"

#include "sony_stdlib.h" /* for memcpy */

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
#define BURST_WRITE_MAX 128 /* Max length of burst write */

sony_result_t sony_i2c_CommonReadRegister(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_IO_ENTER("sony_i2c_CommonReadRegister");

    if(!pI2c){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    result = pI2c->Write(pI2c, deviceAddress, &subAddress, 1, SONY_I2C_START_EN);
    if(result == SONY_RESULT_OK){
        result = pI2c->Read(pI2c, deviceAddress, pData, size, SONY_I2C_START_EN | SONY_I2C_STOP_EN);
    }

    SONY_TRACE_IO_RETURN(result);
}

sony_result_t sony_i2c_CommonWriteRegister(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t buffer[BURST_WRITE_MAX + 1];

    SONY_TRACE_IO_ENTER("sony_i2c_CommonWriteRegister");

    if(!pI2c){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }
    if(size > BURST_WRITE_MAX){
        /* Buffer is too small... */
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_OVERFLOW);
    }

    buffer[0] = subAddress;
    sony_memcpy(&(buffer[1]), pData, size);

    /* send the new buffer */
    result = pI2c->Write(pI2c, deviceAddress, buffer, size+1, SONY_I2C_START_EN | SONY_I2C_STOP_EN);
    SONY_TRACE_IO_RETURN(result);
}

sony_result_t sony_i2c_CommonWriteOneRegister(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_IO_ENTER("sony_i2c_CommonWriteOneRegister");

    if(!pI2c){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }
    result = pI2c->WriteRegister(pI2c, deviceAddress, subAddress, &data, 1);
    SONY_TRACE_IO_RETURN(result);
}

/* For Read-Modify-Write */
sony_result_t sony_i2c_SetRegisterBits(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_IO_ENTER("sony_i2c_SetRegisterBits");

    if(!pI2c){
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }
    if(mask == 0x00){
        /* Nothing to do */
        SONY_TRACE_IO_RETURN(SONY_RESULT_OK);
    }

    if(mask != 0xFF){
        uint8_t rdata = 0x00;
        result = pI2c->ReadRegister(pI2c, deviceAddress, subAddress, &rdata, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_IO_RETURN(result); }
        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pI2c->WriteOneRegister(pI2c, deviceAddress, subAddress, data);
    SONY_TRACE_IO_RETURN(result);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 
