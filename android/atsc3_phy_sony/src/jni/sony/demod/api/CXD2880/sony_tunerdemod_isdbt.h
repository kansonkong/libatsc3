/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/12/04
  Modification ID : 271367e61eabcaae9f67d930a1a4a65cb7ff5c58
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_isdbt.h

 @brief The tuner and demodulator control interface specific to ISDB-T.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_ISDBT_H
#define SONY_TUNERDEMOD_ISDBT_H

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
 @brief The tune parameters for a ISDB-T signal.
*/
typedef struct sony_isdbt_tune_param_t {
    uint32_t centerFreqKHz;             /**< Center frequency(kHz) of the ISDB-T channel. */
    sony_dtv_bandwidth_t bandwidth;     /**< Bandwidth of the ISDB-T channel. */

    /**
     @brief One segument optimization mode.

            If 1 is set, the driver use special optimized setting to receive
            center one-segment.
            For full segment reception, this flag should be set 0.
    */
    uint8_t oneSegmentOptimize;
} sony_isdbt_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for ISDB-T channels. (1st step)

        ISDB-T tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbt_Tune1 and sony_tunerdemod_isdbt_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbt_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                           sony_isdbt_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-T channels. (2nd step)

        ISDB-T tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbt_Tune1 and sony_tunerdemod_isdbt_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbt_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                           sony_isdbt_tune_param_t * pTuneParam);

/**
 @brief ISDB-T dependent setting for Sleep.

        Called internally as part of Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_SleepSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Check ISDB-T demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_isdbt_monitor_SyncStat
        and sony_demod_isdbt_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod The driver instance.
 @param pLock Demod lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Check ISDB-T TS lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_isdbt_monitor_SyncStat
        and sony_demod_isdbt_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod  The driver instance.
 @param pLock TS lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                 sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Check ISDB-T demod lock and TS lock status.

        In ISDB-T, TS lock condition can be 1 earlier than demod lock condition
        if preset tuning is enabled. (by sony_tunerdemod_isdbt_SetPreset)
        So, this function returns SONY_TUNERDEMOD_LOCK_RESULT_LOCKED
        if demod lock OR TS lock status is 1.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_isdbt_monitor_SyncStat
        and sony_demod_isdbt_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod The driver instance.
 @param pLock Lock state. (demod lock OR TS lock)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_CheckDemodOrTSLock (sony_tunerdemod_t * pTunerDemod,
                                                        sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Set preset information to enable fast acquisition mode.

        Preset information can be obtained from ::sony_tunerdemod_isdbt_monitor_PresetInfo API.
        New setting will be used in next ISDB-T/Tmm tuning.

 @param pTunerDemod The driver instance.
 @param pPresetInfo Preset information to enable fast acquisition mode.
                    If this is NULL pointer, disable fast acquisition mode.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_SetPreset (sony_tunerdemod_t * pTunerDemod,
                                               sony_tunerdemod_isdbt_preset_info_t * pPresetInfo);


/**
 @brief ISDB-T demod setting.

        This function is public because it is shared with ISDB-Tmm.
        This is an internal function and should NOT be called from user application.

 @param pTunerDemod The driver instance.
 @param bandwidth Bandwidth to be tuned.
 @param clockMode Tuner and Demodulator clock mode.
 @param demodClkEnable If 0, avoid enabling demod clock. (For ISDB-Tmm Type-B)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_X_tune_DemodSetting (sony_tunerdemod_t * pTunerDemod, sony_dtv_bandwidth_t bandwidth,
                                                         sony_tunerdemod_clockmode_t clockMode, uint8_t demodClkEnable);

/**
 @brief Write ISDB-T preset setting.

        This function is public because it is shared with ISDB-Tmm.
        This is an internal function and should NOT be called from user application.

 @param pTunerDemod The driver instance.
 @param pPresetInfo Preset information to enable fast acquisition mode.
                    If this is NULL pointer, disable fast acquisition mode.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_PresetSetting (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_isdbt_preset_info_t * pPresetInfo);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_ISDBT_H */
