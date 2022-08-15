/*------------------------------------------------------------------------------
  Copyright 2018-2019 Sony Semiconductor Solutions Corporation

  Last Updated    : 2019/04/23
  Modification ID : 9999462af7876667bd516e7137bc6cd1ac791eee
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_atsc3.h

          This file provides the demodulator control interface specific to ATSC 3.0.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_ATSC3_H
#define SONY_CXD6801_DEMOD_ATSC3_H

#include "sony_cxd6801_common.h"
#include "sony_cxd6801_demod.h"

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
#include "sony_demod_atsc3_chbond.h"
#endif

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a ATSC 3.0 signal
*/
typedef struct sony_atsc3_tune_param_t {
    /**
     @brief Center frequency in kHz of the ATSC 3.0 channel.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Bandwidth of the ATSC 3.0 channel.
    */
	sony_cxd6801_dtv_bandwidth_t bandwidth;

    /**
     @brief Number of valid PLP ID in plpID[].
    */
    uint8_t plpIDNum;

    /**
     @brief PLP IDs to select in acquisition.
    */
    uint8_t plpID[4];

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
    /**
     @brief PLP bonding option for each PLPs.

            Used only then the demod is configured as ATSC 3.0 channel bonding main/sub IC.
    */
    sony_demod_atsc3_chbond_plp_bond_t plpBond[4];
#endif

} sony_cxd6801_atsc3_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for ATSC 3.0 channels.  Called from
        the integration layer ::sony_integ_atsc3_Tune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_Tune(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ATSC 3.0 channels in EAS mode.

        Called from the integration layer ::sony_integ_atsc3_EASTune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_EASTune(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_tune_param_t * pTuneParam);

/**
 @brief Put the demodulator into ::SONY_DEMOD_STATE_SLEEP state.  Can be called
        from Active, Shutdown or Sleep states.  Called from the integration layer
        ::sony_integ_Sleep API.

 @param pDemod The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_Sleep(sony_cxd6801_demod_t * pDemod);

/**
 @brief Completes the demodulator acquisition setup only the case for ATSC 3.0 EAS tune.

        Must be called after system specific demod and RF tunes.
        Called from the integration layer ::sony_integ_atsc3_EASTune API.

 @param pDemod The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_EASTuneEnd(sony_cxd6801_demod_t * pDemod);

/**
 @brief Check ATSC 3.0 demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_atsc3_monitor_SyncStat instead
        to check current lock status.

 @param pDemod The demodulator instance
 @param pLock Demod lock state

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_CheckDemodLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock);

/**
 @brief Check ATSC 3.0 ALP lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_atsc3_monitor_SyncStat instead
        to check current lock status.

 @param pDemod The demodulator instance
 @param pLock TS lock state

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_CheckALPLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock);

/**
 @brief Setup the PLP configuration of the demodulator.

 @param pDemod The demodulator instance.
 @param plpIDNum Number of valid PLP ID in plpID[]. (1 - 4)
 @param plpID The pointer of PLP ID 4 length array to set.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_SetPLPConfig(sony_cxd6801_demod_t * pDemod,
                                             uint8_t plpIDNum,
                                             uint8_t plpID[4]);

/**
 @brief Clear BER, SNR history data for averaged BER, SNR, SQI monitors.

 @note  This function may be useful if the user would like to calculate
        averaged BER, SNR, SQI without using old BER, SNR data.
        Please check document and example code for recommended setting and sequence.

 @param pDemod  The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_ClearBERSNRHistory(sony_cxd6801_demod_t * pDemod);

/**
 @brief Clear GPIO EAS latch data for GPIO EAS latch mode.

        Clear GPIO EAS latch data.
        Please check ::SONY_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_STATE
		and ::SONY_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_TRANS too.
        To output EAS state from GPIO, please check ::sony_demod_GPIOSetConfig.

 @param pDemod  The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_ClearGPIOEASLatch(sony_cxd6801_demod_t * pDemod);

/**
 @brief Spectrum inversion and continuous wave auto detection.

 @note  Auto spectrum inversion and CW detection are enabled/disabled by ::sony_demod_SetConfig.
        (::SONY_DEMOD_CONFIG_ATSC3_AUTO_SPECTRUM_INV, ::SONY_DEMOD_CONFIG_ATSC3_CW_DETECTION)

 @param pDemod  The demodulator instance
 @param pContinueWait  Lock wait flag (When this flag = 1, continue waiting Lock)

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_UnlockCase(sony_cxd6801_demod_t * pDemod, uint8_t * pContinueWait);

/**
 @brief Setup continuous wave tracking.

 @note  This API has effect only when CW detection is enabled by ::sony_demod_SetConfig.
        (::SONY_DEMOD_CONFIG_ATSC3_CW_DETECTION)

 @param pDemod      The demodulator instance
 @param pCompleted  CW Tracking setting completed flag (When this flag = 1, CW tracking setting is completed)

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_SetCWTracking(sony_cxd6801_demod_t * pDemod, uint8_t * pCompleted);

/**
 @brief Initialize spectrum inversion and continuous wave auto detection related registers.

 @param pDemod  The demodulator instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_Init(sony_cxd6801_demod_t * pDemod);

#endif /* SONY_CXD6801_DEMOD_ATSC3_H */
