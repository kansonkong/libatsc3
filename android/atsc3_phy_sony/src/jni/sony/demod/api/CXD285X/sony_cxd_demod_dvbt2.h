/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/05/09
  Modification ID : 9849c174faf82fcc958734f152ba29f70274fdec
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_demod_dvbt2.h

          This file provides the demodulator control interface specific to DVB-T2.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_DEMOD_DVBT2_H
#define sony_cxd_DEMOD_DVBT2_H

#include "sony_cxd_common.h"
#include "sony_cxd_demod.h"

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief DVBT2 specific tune information, stored in the tune param struct
        result.  This should be checked if a call to DVBT2 tune returns
        sony_cxd_RESULT_OK_CONFIRM.
*/
typedef enum {
    /**
     @brief Tune successful.
    */
    sony_cxd_DEMOD_DVBT2_TUNE_INFO_OK,

    /**
     @brief PLP provided in tune params is not available.  The demodulator
            will output the auto PLP in this case.
    */
    sony_cxd_DEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID
} sony_cxd_demod_dvbt2_tune_info_t;

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The tune parameters for a DVB-T2 signal
*/
typedef struct sony_cxd_dvbt2_tune_param_t{
    /**
     @brief Center frequency in kHz of the DVB-T2 channel.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Bandwidth of the DVB-T2 channel.
    */
    sony_cxd_dtv_bandwidth_t bandwidth;

    /**
     @brief The data PLP ID to select in acquisition.
    */
    uint8_t dataPlpId;

    /**
     @brief The DVB-T2 profile to select in acquisition.  Must be set to either
            sony_cxd_DVBT2_PROFILE_BASE or sony_cxd_DVBT2_PROFILE_LITE.  If the profile
            is unknown use the blind tune API with the profile set to
            sony_cxd_DVBT2_PROFILE_ANY.
    */
    sony_cxd_dvbt2_profile_t profile;

    /**
     @brief Specific tune information relating to DVB-T2 acquisition.  If result
            from Tune function is sony_cxd_RESULT_OK_CONFIRM this result code
            will provide more information on the tune process.  Refer to
            ::sony_cxd_demod_dvbt2_tune_info_t for further details on the specific
            codes.
    */
    sony_cxd_demod_dvbt2_tune_info_t tuneInfo;
}sony_cxd_dvbt2_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for DVB-T2 channels.  Called from
        the integration layer ::sony_cxd_integ_dvbt2_Tune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_Tune (sony_cxd_demod_t * pDemod,
                                     sony_cxd_dvbt2_tune_param_t * pTuneParam);

/**
 @brief Put the demodulator into ::sony_cxd_DEMOD_STATE_SLEEP state.  Can be called
        from Active, Shutdown or Sleep states.  Called from the integration layer
        ::sony_cxd_integ_Sleep API.

 @param pDemod The demodulator instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_Sleep (sony_cxd_demod_t * pDemod);

/**
 @brief Check DVB-T demodulator lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_cxd_demod_dvbt2_monitor_SyncStat instead
        to check current lock status.

 @param pDemod The demodulator instance
 @param pLock Demod lock state

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_CheckDemodLock (sony_cxd_demod_t * pDemod,
                                               sony_cxd_demod_lock_result_t * pLock);

/**
 @brief Check DVB-T TS lock status.

 @note  This API is for checking lock status in tuning stage.
        After tuning (while receiving the signal),
        please use sony_cxd_demod_dvbt2_monitor_SyncStat instead
        to check current lock status.

 @param pDemod The demodulator instance
 @param pLock TS lock state

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_CheckTSLock (sony_cxd_demod_t * pDemod,
                                            sony_cxd_demod_lock_result_t * pLock);

/**
 @brief Setup the PLP configuration of the demodulator. Selecting both the device
        PLP operation (automatic/manual PLP select) and the PLP to be selected
        in manual PLP mode.

 @param pDemod The demodulator instance.
 @param autoPLP The auto PLP setting.
        - 0x00: The data PLP ID set by plpId will be output.
                If the PLP with the ID is not found, then a PLP error is indicated
                (::sony_cxd_demod_dvbt2_monitor_DataPLPError) but the
                demod will still output the first found data PLP Id.
        - 0x01: Fully automatic. The first PLP found during acquisition will be output.
 @param plpId The PLP Id to set.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_SetPLPConfig (sony_cxd_demod_t * pDemod,
                                             uint8_t autoPLP,
                                             uint8_t plpId);

/**
 @brief Set the DVB-T2 profile and tune mode for acquisition.

        Note: This API changes DVB-T2 early unlock detection time.
        If sony_cxd_DVBT2_PROFILE_LITE or sony_cxd_DVBT2_PROFILE_ANY are specified,
        early unlock detection time will be longer.

 @param pDemod The demodulator instance.
 @param profile The DVB-T2 profile option.  sony_cxd_DVBT2_PROFILE_BASE and
        sony_cxd_DVBT2_PROFILE_LITE will set the demodulator into fixed tune mode
        without recovery. sony_cxd_DVBT2_PROFILE_ANY will use auto detection
        meaning the demodulator will acquire to the first valid frame
        received, Base or Lite.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_SetProfile (sony_cxd_demod_t * pDemod,
                                           sony_cxd_dvbt2_profile_t profile);

/**
 @brief Check DVB-T2 L1_Post valid status.

 @param pDemod The demodulator instance
 @param pL1PostValid L1 Post valid status

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_CheckL1PostValid (sony_cxd_demod_t * pDemod,
                                                 uint8_t * pL1PostValid);

/**
 @brief Clear BER, SNR history data for averaged BER, SNR, SQI monitors.

 @note  This function may be useful if the user would like to calculate
        averaged BER, SNR, SQI without using old BER, SNR data.
        Please check document and example code for recommended setting and sequence.

 @param pDemod  The demodulator instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_dvbt2_ClearBERSNRHistory (sony_cxd_demod_t * pDemod);

#endif /* sony_cxd_DEMOD_DVBT2_H */
