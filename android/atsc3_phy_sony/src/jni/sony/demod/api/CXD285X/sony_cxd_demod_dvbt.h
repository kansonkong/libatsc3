/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/05/09
  Modification ID : 9849c174faf82fcc958734f152ba29f70274fdec
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_demod_dvbt.h

          This file provides the demodulator control interface specific to DVB-T.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_DEMOD_DVBT_H
#define sony_cxd_DEMOD_DVBT_H

#include "sony_cxd_common.h"
#include "sony_cxd_demod.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a DVB-T signal
*/
typedef struct sony_cxd_dvbt_tune_param_t{
    uint32_t centerFreqKHz;             /**< Center frequency in kHz of the DVB-T channel */
    sony_cxd_dtv_bandwidth_t bandwidth;   /**< Bandwidth of the DVB-T channel */
    sony_cxd_dvbt_profile_t profile;        /**< Indicates the HP/LP profile to be selected. */
}sony_cxd_dvbt_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Configure the DVBT profile for acquisition, Low Priority or High Priority.

 @param pDemod The demodulator instance
 @param profile The profile to use for DVB-T acquisition.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_SetProfile (sony_cxd_demod_t * pDemod,
                                          sony_cxd_dvbt_profile_t profile);

/**
 @brief Enable acquisition on the demodulator for DVB-T channels.  Called from
        the integration layer ::sony_cxd_integ_dvbt_Tune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_Tune (sony_cxd_demod_t * pDemod,
                                    sony_cxd_dvbt_tune_param_t * pTuneParam);

/**
 @brief Put the demodulator into ::sony_cxd_DEMOD_STATE_SLEEP state.  Can be called
        from Active, Shutdown or Sleep states.  Called from the integration layer
        ::sony_cxd_integ_Sleep API.

 @param pDemod  The demodulator instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_Sleep (sony_cxd_demod_t * pDemod);

/**
 @brief Check DVB-T demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_cxd_demod_dvbt_monitor_SyncStat instead
        to check current lock status.

 @param pDemod  The demodulator instance
 @param pLock Demod lock state

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_CheckDemodLock (sony_cxd_demod_t * pDemod,
                                              sony_cxd_demod_lock_result_t * pLock);

/**
 @brief Check DVB-T TS lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_cxd_demod_dvbt_monitor_SyncStat instead
        to check current lock status.

 @param pDemod  The demodulator instance
 @param pLock TS lock state

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_CheckTSLock (sony_cxd_demod_t * pDemod,
                                           sony_cxd_demod_lock_result_t * pLock);

/**
 @brief Check DVB-T Echo Optomisation requirement.

 @param pDemod  The demodulator instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_EchoOptimization (sony_cxd_demod_t * pDemod);

/**
 @brief Clear BER, SNR history data for averaged BER, SNR, SQI monitors.

 @note  This function may be useful if the user would like to calculate
        averaged BER, SNR, SQI without using old BER, SNR data.
        Please check document and example code for recommended setting and sequence.

 @param pDemod  The demodulator instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt_ClearBERSNRHistory (sony_cxd_demod_t * pDemod);

#endif /* sony_cxd_DEMOD_DVBT_H */
