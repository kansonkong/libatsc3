/*------------------------------------------------------------------------------
  Copyright 2015 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_isdbtmm.h

 @brief The tuner and demodulator control interface specific to ISDB-Tmm.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_ISDBTMM_H
#define SONY_TUNERDEMOD_ISDBTMM_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_isdbtmm.h"
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
 @brief The tune parameters for a ISDB-Tmm signal.
*/
typedef struct sony_isdbtmm_tune_param_t {
    /**
     @brief Segment index to specify ISDB-Tmm 33 segments. (0 - 32)

            For Type-A super segument, valid segment indexes are as follows:

             index | Frequency (MHz)     | Arrangement
             ------|---------------------|--------------
                 6 | 210 + 3/7 (210.429) | Pattern (A), (C)
                13 | 213 + 3/7 (213.429) | Pattern (B)
                19 | 216                 | Pattern (C)
                26 | 219                 | Pattern (A), (B)

            For Type-B super segment, valid segment indexes are as follows:

             index | Frequency (MHz)     | Arrangement
             ------|---------------------|--------------
                 0 | 207 + 6/7 (207.857) | Pattern (B)
                 1 | 208 + 2/7 (208.286) | Pattern (B)
                 2 | 208 + 5/7 (208.714) | Pattern (B)
                 3 | 209 + 1/7 (209.143) | Pattern (B)
                 4 | 209 + 4/7 (209.571) | Pattern (B)
                 5 | 210                 | Pattern (B)
                 6 | 210 + 3/7 (210.429) | Pattern (B)
                13 | 213 + 3/7 (213.429) | Pattern (A)
                14 | 213 + 6/7 (213.857) | Pattern (A)
                15 | 214 + 2/7 (214.286) | Pattern (A)
                16 | 214 + 5/7 (214.714) | Pattern (A)
                17 | 215 + 1/7 (215.143) | Pattern (A)
                18 | 215 + 4/7 (215.571) | Pattern (A)
                19 | 216                 | Pattern (A)
                26 | 219                 | Pattern (C)
                27 | 219 + 3/7 (219.429) | Pattern (C)
                28 | 219 + 6/7 (219.857) | Pattern (C)
                29 | 220 + 2/7 (220.286) | Pattern (C)
                30 | 220 + 5/7 (220.714) | Pattern (C)
                31 | 221 + 1/7 (221.143) | Pattern (C)
                32 | 221 + 4/7 (221.571) | Pattern (C)
    */
    uint8_t segmentIndex;

    /**
     @brief Super segment type. (Type-A or Type-B)
    */
    sony_isdbtmm_super_segment_t superSegmentType;

    /**
     @brief One segument optimization mode. (Only for Type-A super segment.)

            If 1 is set, the driver use special optimized setting to receive
            center one-segment in Type-A super segment.
            For full segment reception of Type-A super segment, this flag should be set 0.
    */
    uint8_t oneSegmentOptimize;
} sony_isdbtmm_tune_param_t;

/**
 @brief The "raw" tune parameters for a ISDB-Tmm Type-A segment.

        This tune parameter is used internally.
        For debug, the user can call "raw" API directly to tune nonstandardized signal.
*/
typedef struct sony_isdbtmm_A_tune_param_t {
    /**
     @brief Center frequency(kHz)
    */
    uint32_t centerFreqKHz;

    /**
     @brief One segment optimization mode.

            If 1 is set, the driver use special optimized setting to receive
            center one-segment in Type-A super segment.
            For full segment reception of Type-A super segment, this flag should be set 0.
    */
    uint8_t oneSegmentOptimize;

    /**
     @brief Frequency shift direction for one segment optimization setting.

            0 - lower
            1 - upper
    */
    uint8_t oneSegmentOptimizeShiftDirection;
} sony_isdbtmm_A_tune_param_t;

/**
 @brief The "raw" tune parameters for a ISDB-Tmm Type-B segment.

        This tune parameter is used internally.
        For debug, the user can call "raw" API directly to tune nonstandardized signal.
*/
typedef struct sony_isdbtmm_B_tune_param_t {
    /**
     @brief Tuner tuning frequency(kHz)
    */
    uint32_t tunerFrequencyKHz;

    /**
     @brief Start (lowest frequency) sub-channel number indicator to be tuned. (0 - 13)

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

            For 7 segment case, it should be 0.
    */
    uint8_t startSubChannel;

    /**
     @brief Total number of segments in tuned Type-B super segment. (1 - 13)

            Because of hardware spec, 14 cannot be set.
            If 1 is set, because of hardware spec, tuner center frequency should be equal to segment center frequency.
            (tunerCenterSubChannel == targetSubChannel is necessary.)
    */
    uint8_t totalSegmentNumber;

    /**
     @brief Tuner center sub-channel number indicator. ((-5) - 18)

            This number can be minus or bigger than biggest sub-channel number indicator
            of target Type-B super segument.
    */
    int8_t tunerCenterSubChannel;

    /**
     @brief Target sub-channel number indicator. (0 - 13)

            This number should be between startSubChannel and (startSubChannel + totalSegmentNumber - 1).
    */
    uint8_t targetSubChannel;

    /**
     @brief Frequency shift direction for one segment optimization setting.

            0 - lower
            1 - upper

            Frequency shift optimization is necessary only if tuner center frequency
            and target segment center frequency are same. (tunerCenterSubChannel == targetSubChannel)
    */
    uint8_t oneSegmentOptimizeShiftDirection;
} sony_isdbtmm_B_tune_param_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Configure ISDB-Tmm Type-A tune parameters from ISDB-Tmm common tune parameters.

        ::sony_integ_isdbtmm_Tune calls this API to set up sony_isdbtmm_A_tune_param_t.

 @param pTunerDemod The driver instance.
 @param pTuneParamA The ISDB-Tmm Type-A tune parameters.
 @param pTuneParam The ISDB-Tmm common tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_A_ConvertTuneParam (sony_tunerdemod_t * pTunerDemod,
                                                          sony_isdbtmm_A_tune_param_t * pTuneParamA,
                                                          sony_isdbtmm_tune_param_t * pTuneParam);

/**
 @brief Configure ISDB-Tmm Type-B tune parameters from ISDB-Tmm common tune parameters.

        ::sony_integ_isdbtmm_Tune calls this API to set up sony_isdbtmm_B_tune_param_t.

 @param pTunerDemod The driver instance.
 @param pTuneParamB The ISDB-Tmm Type-B tune parameters.
 @param pTuneParam The ISDB-Tmm common tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_B_ConvertTuneParam (sony_tunerdemod_t * pTunerDemod,
                                                          sony_isdbtmm_B_tune_param_t * pTuneParamB,
                                                          sony_isdbtmm_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-Tmm Type-A super segment channels. (1st step)

        ISDB-Tmm Type-A tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbtmm_A_Tune1 and sony_tunerdemod_isdbtmm_A_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtmm_A_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_A_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_A_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-Tmm Type-A super segment channels. (2nd step)

        ISDB-Tmm Type-A tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbtmm_A_Tune1 and sony_tunerdemod_isdbtmm_A_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtmm_A_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_A_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_A_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-Tmm Type-B super segment channels. (1st step)

        ISDB-Tmm Type-B tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbtmm_B_Tune1 and sony_tunerdemod_isdbtmm_B_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtmm_B_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_B_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_B_tune_param_t * pTuneParam);

/**
 @brief Enable acquisition on the demodulator for ISDB-Tmm Type-B super segment channels. (2nd step)

        ISDB-Tmm Type-B tuner/demod setting function for tuning is separated in
        sony_tunerdemod_isdbtmm_B_Tune1 and sony_tunerdemod_isdbtmm_B_Tune2
        because need long sleep (100ms) between them.
        These APIs are called from ::sony_integ_isdbtmm_B_Tune.

 @param pTunerDemod The driver instance.
 @param pTuneParam The tune parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_B_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_B_tune_param_t * pTuneParam);


/**
 @brief ISDB-Tmm Type-A dependent setting for Sleep.

        Called internally as part of Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_A_SleepSetting (sony_tunerdemod_t * pTunerDemod);

/**
 @brief ISDB-Tmm Type-B dependent setting for Sleep.

        Called internally as part of Sleep state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_Sleep API for Active to Sleep state transition.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbtmm_B_SleepSetting (sony_tunerdemod_t * pTunerDemod);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_ISDBTMM_H */
