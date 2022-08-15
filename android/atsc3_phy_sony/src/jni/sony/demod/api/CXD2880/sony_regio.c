/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2014/10/30
  Modification ID : 48a4df278e00b086435eed262de0e70d27d0ee29
------------------------------------------------------------------------------*/

#include "sony_regio.h"

sony_result_t sony_regio_CommonWriteOneRegister (sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t data)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_IO_ENTER ("sony_regio_CommonWriteOneRegister");

    if (!pRegio) {
        SONY_TRACE_IO_RETURN(SONY_RESULT_ERROR_ARG);
    }

    result = pRegio->WriteRegister (pRegio, target, subAddress, &data, 1);

    SONY_TRACE_IO_RETURN (result);
}

sony_result_t sony_regio_SetRegisterBits (sony_regio_t * pRegio, sony_regio_target_t target, uint8_t subAddress, uint8_t data, uint8_t mask)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_IO_ENTER ("sony_regio_SetRegisterBits");

    if (!pRegio) {
        SONY_TRACE_IO_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (mask == 0x00) {
        /* Nothing to do */
        SONY_TRACE_IO_RETURN (SONY_RESULT_OK);
    }

    if (mask != 0xFF) {
        uint8_t rdata = 0x00;

        result = pRegio->ReadRegister (pRegio, target, subAddress, &rdata, 1);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_IO_RETURN (result);
        }

        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pRegio->WriteOneRegister (pRegio, target, subAddress, data);

    SONY_TRACE_IO_RETURN(result);
}
