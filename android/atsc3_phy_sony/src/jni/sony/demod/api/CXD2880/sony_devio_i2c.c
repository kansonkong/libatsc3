/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/01/16
  Modification ID : d4b3fe129855d90c25271de02796915983f1e6ee
------------------------------------------------------------------------------*/

#include "sony_devio_i2c.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t sony_regio_i2c_ReadRegister (sony_regio_t* pRegio, sony_regio_target_t target,
    uint8_t subAddress, uint8_t* pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_i2c_t* pI2c = NULL;

    SONY_TRACE_IO_ENTER ("sony_regio_i2c_ReadRegister");
    if ((!pRegio) || (!pRegio->pIfObject)) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pI2c = (sony_i2c_t*)(pRegio->pIfObject);

    result = pI2c->ReadRegister (pI2c, ((target == SONY_REGIO_TARGET_SYSTEM)
        ? (pRegio->i2cAddressSystem) : (pRegio->i2cAddressDemod)), subAddress, pData, size);

    SONY_TRACE_IO_RETURN (result);
}

static sony_result_t sony_regio_i2c_WriteRegister (sony_regio_t* pRegio, sony_regio_target_t target,
    uint8_t subAddress, const uint8_t *pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_i2c_t* pI2c = NULL;

    SONY_TRACE_IO_ENTER ("sony_regio_i2c_WriteRegister");

    if ((!pRegio) || (!pRegio->pIfObject)) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pI2c = (sony_i2c_t*)(pRegio->pIfObject);

    result = pI2c->WriteRegister (pI2c, ((target == SONY_REGIO_TARGET_SYSTEM)
        ? (pRegio->i2cAddressSystem) : (pRegio->i2cAddressDemod)), subAddress, pData, size);
    SONY_TRACE_IO_RETURN (result);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_regio_i2c_Create (sony_regio_t *pRegio, sony_i2c_t *pI2c, uint8_t i2cAddressDemod)
{
    SONY_TRACE_IO_ENTER ("sony_regio_i2c_Create");

    if ((!pRegio) || (!pI2c)) {
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    pRegio->ReadRegister = sony_regio_i2c_ReadRegister;
    pRegio->WriteRegister = sony_regio_i2c_WriteRegister;
    pRegio->WriteOneRegister = sony_regio_CommonWriteOneRegister;
    pRegio->pIfObject = pI2c;
    pRegio->i2cAddressSystem = i2cAddressDemod + 4;
    pRegio->i2cAddressDemod = i2cAddressDemod;
    pRegio->slaveSelect = 0;

    SONY_TRACE_IO_RETURN (SONY_RESULT_OK);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 
