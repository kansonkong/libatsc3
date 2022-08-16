/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_atsc.h

          This file provides the demodulator control interface specific to ATSC.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_ATSC_H
#define SONY_CXD6801_DEMOD_ATSC_H

#include "sony_cxd6801_common.h"
#include "sony_cxd6801_demod.h"

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a ATSC signal
*/
typedef struct sony_atsc_tune_param_t {
    /**
     @brief Center frequency in kHz of the ATSC channel.
    */
    uint32_t centerFreqKHz;
} sony_cxd6801_atsc_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for ATSC channels.

        Called from the integration layer ::sony_integ_atsc_Tune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_Tune(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc_tune_param_t * pTuneParam);

/**
 @brief Completes the demodulator ATSC acquisition setup.

        Must be called after system specific demod and RF tunes.
        Called from the integration layer ::sony_integ_atsc_Tune API.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_TuneEnd(sony_cxd6801_demod_t * pDemod);

/**
 @brief Soft reset the demodulator for ATSC.

        The soft reset will begin the devices acquisition process.
        Called from ::sony_demod_SoftReset API.

 @param pDemod The demod instance.

 @return SONY_RESULT_OK if successfully reset.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_SoftReset(sony_cxd6801_demod_t * pDemod);

/**
 @brief Put the demodulator into ::SONY_DEMOD_STATE_SLEEP state.

        Can be called from Active, Shutdown or Sleep states.
        Called from the integration layer ::sony_integ_Sleep API.

 @param pDemod The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_Sleep(sony_cxd6801_demod_t * pDemod);

/**
 @brief Check ATSC demodulator lock status.

 @param pDemod The demodulator instance
 @param pLock Demod lock state

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_CheckDemodLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock);

/**
 @brief Check ATSC TS lock status.

 @param pDemod The demodulator instance
 @param pLock TS lock state

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_CheckTSLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock);

/**
 @brief Write 1-byte data to the demodulator register with SLV-R I2C channel.

 @param pDemod The demodulator instance.
 @param bank   The bank number of the target demodulator registers.
 @param registerAddress  The register address of the target demodulator register.
 @param value   The value to write into demodulator register contents.
 @param bitMask The mask to apply with the value.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_SlaveRWriteRegister(sony_cxd6801_demod_t * pDemod,
                                                   uint8_t  bank,
                                                   uint8_t  registerAddress,
                                                   uint8_t  value,
                                                   uint8_t  bitMask);

/**
 @brief Read 1-byte data from the demodulator register with SLV-R I2C channel.

 @param pDemod The demodulator instance.
 @param bank The bank number of the target demodulator registers.
 @param registerAddress The register address of the target demodulator register.
 @param pValue The buffer to store the demodulator register contents into.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_SlaveRReadRegister(sony_cxd6801_demod_t * pDemod,
                                                  uint8_t bank,
                                                  uint8_t registerAddress,
                                                  uint8_t * pValue);

#endif /* SONY_CXD6801_DEMOD_ATSC_H */
