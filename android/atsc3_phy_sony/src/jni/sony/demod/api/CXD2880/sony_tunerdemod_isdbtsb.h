/*------------------------------------------------------------------------------
  Copyright 2016 Sony Corporation

  Last Updated    : 2016/03/29
  Modification ID : c247b0fd8ce5951ee0f186647f755862889fa2e5
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_isdbtsb.h

 @brief The tuner and demodulator control interface specific to ISDB-Tsb.

 @note  One-segment type of area broadcasting is only supported for now.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_ISDBTSB_H
#define SONY_TUNERDEMOD_ISDBTSB_H

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
 @brief The tune parameters for a ISDB-Tsb signal.
*/
typedef struct sony_isdbtsb_tune_param_t {
    /**
     @brief Center frequency(kHz)
    */
    uint32_t centerFreqKHz;

    /**
     @brief Bandwidth of the ISDB-Tsb channel.
    */
    sony_dtv_bandwidth_t bandwidth;

    /**
     @brief Center sub-channel number indicator to be tuned. (0 - 13)

            indicator | sub-channel number
            ----------|--------------------
                    0 | 41,  0,  1
                    1 |  2,  3,  4
                    2 |  5,  6,  7
                    3 |  8,  9, 10
                    4 | 11, 12, 13
                    5 | 14, 15, 16
                    6 | 17, 18, 19
                    7 | 20, 21, 22
                    8 | 23, 24, 25
                    9 | 26, 27, 28
                   10 | 29, 30, 31
                   11 | 32, 33, 34
                   12 | 35, 36, 37
                   13 | 38, 39, 40

            For one-segment type of area broadcasting reception, it should be 7.
    */
    uint8_t subChannel;
} sony_isdbtsb_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable acquisition on the demodulator for ISDB-Tsb channels. (1st step)

        ISDB-Tsb tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbsb_Tune1 and sony_tunerdemod_isdbtsb_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtsb_Tune and ::sony_integ_isdbt_tsb_BlindTune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtsb_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                             sony_isdbtsb_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-Tsb channels. (2nd step)

        ISDB-Tsb tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbsb_Tune1 and sony_tunerdemod_isdbtsb_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtsb_Tune and ::sony_integ_isdbt_tsb_BlindTune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtsb_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                             sony_isdbtsb_tune_param_t * pTuneParam);

/**
 @brief AGC optimized setting for ISDB-Tsb channels.

        This setting should be used for ISDB-Tsb signal only.
        For scanning, this setting should be used after determing that the signal is ISDB-Tsb.
        This API is called from ::sony_integ_isdbtsb_Tune and ::sony_integ_isdbt_tsb_BlindTune.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtsb_AGCSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Force preset setting for force decoding A layer only.

        This is used for one-segment type of area broadcasting to ignore TMCC information in the signal.
        (TR-B35 said TMCC information for 13 segment should be sent for one-segment type area broadcasting signal.)
        This API is called from ::sony_integ_isdbtsb_Tune and ::sony_integ_isdbt_tsb_BlindTune.

 @param pTunerDemod The driver instance.
 @param pPresetInfo Preset information will be used.
                    If this is NULL pointer, disable force preset setting.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly (sony_tunerdemod_t * pTunerDemod,
                                                                    sony_tunerdemod_isdbt_preset_info_t * pPresetInfo);

/**
 @brief ISDB-Tsb dependent setting for Sleep.

        Called internally as part of Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtsb_SleepSetting (sony_tunerdemod_t * pTunerDemod);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_ISDBTSB_H */
