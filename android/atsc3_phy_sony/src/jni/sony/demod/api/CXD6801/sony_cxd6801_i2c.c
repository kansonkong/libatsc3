/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/11/30
  Modification ID : 253a4918e2da2cf28a9393596fa16f25024e504d
------------------------------------------------------------------------------*/

#include "sony_cxd6801_i2c.h"

#include "sony_cxd6801_stdlib.h" /* for memcpy */

#define BURST_WRITE_MAX 128 /* Max length of burst write */

sony_cxd6801_result_t sony_cxd6801_i2c_CommonReadRegister(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_CommonReadRegister");

    if(!pI2c || (!pData)){
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = pI2c->Write(pI2c, deviceAddress, &subAddress, 1, SONY_CXD6801_I2C_START_EN);
	if (result == SONY_CXD6801_RESULT_OK){
		result = pI2c->Read(pI2c, deviceAddress, pData, size, SONY_CXD6801_I2C_START_EN | SONY_CXD6801_I2C_STOP_EN);
    }

    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

sony_cxd6801_result_t sony_cxd6801_i2c_CommonWriteRegister(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t buffer[BURST_WRITE_MAX + 1];

    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_CommonWriteRegister");

    if(!pI2c || (!pData)){
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
    if(size > BURST_WRITE_MAX){
        /* Buffer is too small... */
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    buffer[0] = subAddress;
    sony_cxd6801_memcpy(&(buffer[1]), pData, size);

    /* send the new buffer */
	result = pI2c->Write(pI2c, deviceAddress, buffer, size + 1, SONY_CXD6801_I2C_START_EN | SONY_CXD6801_I2C_STOP_EN);
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

sony_cxd6801_result_t sony_cxd6801_i2c_CommonWriteOneRegister(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_CommonWriteOneRegister");

    if(!pI2c){
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
    result = pI2c->WriteRegister(pI2c, deviceAddress, subAddress, &data, 1);
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

/* For Read-Modify-Write */
sony_cxd6801_result_t sony_cxd6801_i2c_SetRegisterBits(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_I2C_ENTER("sony_cxd6801_i2c_SetRegisterBits");

    if(!pI2c){
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
    if(mask == 0x00){
        /* Nothing to do */
		SONY_CXD6801_TRACE_I2C_RETURN(SONY_CXD6801_RESULT_OK);
    }

    if(mask != 0xFF){
        uint8_t rdata = 0x00;
        result = pI2c->ReadRegister(pI2c, deviceAddress, subAddress, &rdata, 1);
		if (result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_I2C_RETURN(result); }
        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pI2c->WriteOneRegister(pI2c, deviceAddress, subAddress, data);
    SONY_CXD6801_TRACE_I2C_RETURN(result);
}

