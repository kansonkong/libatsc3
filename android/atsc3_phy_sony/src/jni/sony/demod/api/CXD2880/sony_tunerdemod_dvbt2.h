/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2016/04/14
  Modification ID : b3a863c9449ebbf8408830fb7cfafb763ac7a67f
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_dvbt2.h

 @brief The tuner and demodulator control interface specific to DVB-T2.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_DVBT2_H
#define SONY_TUNERDEMOD_DVBT2_H

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
/**
 @brief DVB-T2 specific tune information, stored in the tune param struct result.

        This should be checked if a call to DVB-T2 tune returns SONY_RESULT_OK_CONFIRM.
*/
typedef enum {
    /**
     @brief Tune successful.
    */
    SONY_TUNERDEMOD_DVBT2_TUNE_INFO_OK,

    /**
     @brief PLP provided in tune params is not available.

            The demodulator will output the auto PLP in this case.
    */
    SONY_TUNERDEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID
} sony_tunerdemod_dvbt2_tune_info_t;

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a DVB-T2 signal
*/
typedef struct sony_dvbt2_tune_param_t {
    /**
     @brief Center frequency in kHz of the DVB-T2 channel.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Bandwidth of the DVB-T2 channel.
    */
    sony_dtv_bandwidth_t bandwidth;

    /**
     @brief The data PLP ID to select in acquisition.

            If SONY_DVBT2_TUNE_PARAM_PLPID_AUTO is set, first data PLP listed
            in L1-Post information will be automatically selected.
    */
    uint16_t dataPLPID;

    /**
     @brief The DVB-T2 profile to select in acquisition.

            Must be set to either SONY_DVBT2_PROFILE_BASE or SONY_DVBT2_PROFILE_LITE.
            If the profile is unknown use the blind tune API with the profile set to
            SONY_DVBT2_PROFILE_ANY.
    */
    sony_dvbt2_profile_t profile;

    /**
     @brief Specific tune information relating to DVB-T2 acquisition.

            If result from Tune function is SONY_RESULT_OK_CONFIRM this result code
            will provide more information on the tune process.
            Refer to ::sony_tunerdemod_dvbt2_tune_info_t for further details on the specific codes.
    */
    sony_tunerdemod_dvbt2_tune_info_t tuneInfo;
} sony_dvbt2_tune_param_t;

#define SONY_DVBT2_TUNE_PARAM_PLPID_AUTO  0xFFFF /**< Special PLP ID for sony_dvbt2_tune_param::dataPLPID */

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for DVB-T2 channels. (1st step)

        DVB-T2 tuner/demod setting function for tuning is separated in
        sony_tunerdemod_dvbt2_Tune1 and sony_tunerdemod_dvbt2_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_dvbt2_Tune and ::sony_integ_dvbt2_BlindTune.

 @note  If the driver is configured for diver and pTuneParam->profile == SONY_DVBT2_PROFILE_ANY,
        this function returns error.
        It's because in diver system, if SONY_DVBT2_PROFILE_ANY register setting is used,
        main IC and sub IC may lock different T2 profile.
        This situation is invalid and should be avoided.
        So, diver system should try both T2-base and T2-Lite manually instead of using SONY_DVBT2_PROFILE_ANY setting.
        Please check ::sony_integ_dvbt2_BlindTune too.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                           sony_dvbt2_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for DVB-T2 channels. (2nd step)

        DVB-T2 tuner/demod setting function for tuning is separated in
        sony_tunerdemod_dvbt2_Tune1 and sony_tunerdemod_dvbt2_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_dvbt2_Tune and ::sony_integ_dvbt2_BlindTune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                           sony_dvbt2_tune_param_t * pTuneParam);

/**
 @brief DVB-T2 dependent setting for Sleep.

        Called internally as part of Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_SleepSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Check DVB-T demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_dvbt2_monitor_SyncStat
        and sony_demod_dvbt2_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod The driver instance.
 @param pLock Demod lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Check DVB-T TS lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_demod_dvbt2_monitor_SyncStat
        and sony_demod_dvbt2_monitor_SyncStat_sub instead
        to check current lock status.

 @param pTunerDemod The driver instance.
 @param pLock TS lock state.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                 sony_tunerdemod_lock_result_t * pLock);

/**
 @brief Setup the PLP configuration of the demodulator.

        Selecting both the device PLP operation (automatic/manual PLP select) and
        the PLP to be selected in manual PLP mode.

        This is an internal function and should NOT be called from user application.

 @param pTunerDemod The driver instance.
 @param autoPLP The auto PLP setting.
                  - 0x00: The data PLP ID set by plpId will be output.
                          If the PLP with the ID is not found, then a PLP error is indicated
                          (::sony_tunerdemod_dvbt2_monitor_DataPLPError) but the
                          demod will still output the first found data PLP Id.
                  - 0x01: Fully automatic. The first PLP found during acquisition will be output.
 @param plpId The PLP Id to set.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_SetPLPConfig (sony_tunerdemod_t * pTunerDemod,
                                                  uint8_t autoPLP,
                                                  uint8_t plpId);

/**
 @brief Set special setting if diver and FEF case.

 @note  This API is called from sony_integ_dvbt2_Tune
        and sony_integ_dvbt2_BlindTune APIs to write optimum setting
        for diver cofigured and FEF included signal reception case.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_DiverFEFSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Check DVB-T2 L1_Post valid status.

 @param pTunerDemod The driver instance.
 @param pL1PostValid L1 Post valid status.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_dvbt2_CheckL1PostValid (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pL1PostValid);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_DVBT2_H */
