/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/12/04
  Modification ID : 271367e61eabcaae9f67d930a1a4a65cb7ff5c58
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_dvbt.h

 @brief The tuner and demodulator control interface specific to DVB-T.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_DVBT_H
#define SONY_TUNERDEMOD_DVBT_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_tunerdemod.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a DVB-T signal.
*/
typedef struct sony_dvbt_tune_param_t{
    uint32_t centerFreqKHz;             /**< Center frequency in kHz of the DVB-T channel. */
    sony_dtv_bandwidth_t bandwidth;     /**< Bandwidth of the DVB-T channel. */
    sony_dvbt_profile_t profile;        /**< Indicates the HP/LP profile to be selected. */
}sony_dvbt_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the tuner/demodulator for DVB-T channels. (1st step)

        DVB-T tuner/demod setting function for tuning is separated in
        sony_tunerdemod_dvbt_Tune1 and sony_tunerdemod_dvbt_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_dvbt_Tune and ::sony_integ_dvbt_BlindTune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                          sony_dvbt_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the tuner/demodulator for DVB-T channels. (2nd step)

        DVB-T tuner/demod setting function for tuning is separated in
        sony_tunerdemod_dvbt_Tune1 and sony_tunerdemod_dvbt_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_dvbt_Tune and ::sony_integ_dvbt_BlindTune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                          sony_dvbt_tune_param_t * pTuneParam);

/**
 @brief DVB-T dependent setting for Sleep.

        Called internally as part of Active to Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod  The driver instance

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt_SleepSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Check DVB-T demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_dvbt_monitor_SyncStat
        and sony_demod_dvbt_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod The demodulator instance.
 @param pLock Demodulator lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Check DVB-T TS lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_dvbt_monitor_SyncStat
        and sony_demod_dvbt_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod  The driver instance.
 @param pLock TS lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                sony_tunerdemod_lock_result_t * pLock);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_DVBT_H */
