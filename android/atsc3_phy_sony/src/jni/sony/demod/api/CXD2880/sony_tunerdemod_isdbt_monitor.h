/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_isdbt_monitor.h

 @brief The ISDB-T tuner and demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_ISDBT_MONITOR_H
#define SONY_TUNERDEMOD_ISDBT_MONITOR_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_isdbt.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Target layer for monitoring BER, PER and so on.
*/
typedef enum sony_tunerdemod_isdbt_monitor_target_t{
    SONY_TUNERDEMOD_ISDBT_MONITOR_TARGET_LAYER_A = 0,    /**< Layer A */
    SONY_TUNERDEMOD_ISDBT_MONITOR_TARGET_LAYER_B,        /**< Layer B */
    SONY_TUNERDEMOD_ISDBT_MONITOR_TARGET_LAYER_C,        /**< Layer C */
    SONY_TUNERDEMOD_ISDBT_MONITOR_TARGET_LAYER_UNKNOWN   /**< Unkown Layer */
} sony_tunerdemod_isdbt_monitor_target_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the ISDB-T demodulator.

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_isdbt_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pDmdLockStat Address of demodulator lock flag
                 - 0: Not lock
                 - 1: Lock
 @param pTSLockStat  Indicates the TS lock condition.
                 - 0: TS not locked.
                 - 1: TS locked.
 @param pUnlockDetected  Early unlock condition
                 - 0: No early unlock.
                 - 1: Early unlock detected.

 @return SONY_RESULT_OK if successful and pSyncStat, pTSLockStat and pUnlockDetected valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pDmdLockStat,
                                                      uint8_t * pTSLockStat,
                                                      uint8_t * pUnlockDetected);

/**
 @brief Monitors the synchronisation state of the ISDB-T demodulator. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_isdbt_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pDmdLockStat Address of demodulator lock flag
                 - 0: Not lock
                 - 1: Lock
 @param pUnlockDetected  Early unlock condition
                 - 0: No early unlock.
                 - 1: Early unlock detected.

 @return SONY_RESULT_OK if successful and pSyncStat and pUnlockDetected valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                          uint8_t * pDmdLockStat,
                                                          uint8_t * pUnlockDetected);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_isdbt_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod,
                                                           int32_t * pOffset);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel. (Sub IC)

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_isdbt_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pOffset);

/**
 @brief Monitor the Pre-RS BER.

        BER value will be 0 for unused layers.

 @param pTunerDemod The driver instance.
 @param pBERA Layer A BER value (Pre reed solomon decoder) x 1e7.
 @param pBERB Layer B BER value (Pre reed solomon decoder) x 1e7.
 @param pBERC Layer C BER value (Pre reed solomon decoder) x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_PreRSBER (sony_tunerdemod_t * pTunerDemod,
                                                      uint32_t * pBERA, uint32_t * pBERB, uint32_t * pBERC);

/**
 @brief Monitor the TMCC information.

 @param pTunerDemod The driver instance.
 @param pTMCCInfo The TMCC information.

 @return SONY_RESULT_OK if successful and pTMCCInfo valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_TMCCInfo (sony_tunerdemod_t * pTunerDemod,
                                                      sony_isdbt_tmcc_info_t * pTMCCInfo);

/**
  @brief Monitor the preset information.

        This information enable demodulator fast acquisition by passing to
        ::sony_tunerdemod_isdbt_SetPreset. Tuning time will be shortened.

  @param pTunerDemod The driver instance.
  @param pPresetInfo The preset information.

  @return SONY_RESULT_OK if successful and pPresetInfo valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_PresetInfo (sony_tunerdemod_t * pTunerDemod,
                                                        sony_tunerdemod_isdbt_preset_info_t * pPresetInfo);

/**
 @brief Monitors the number RS (Reed Solomon) errors detected by the
        RS decoder over 1 second.

        Also known as the code word reject count.
        Error value will be 0 for unused layers.

 @param pTunerDemod The driver instance.
 @param pPENA The number of RS errors detected over 1 second. (Layer A)
 @param pPENB The number of RS errors detected over 1 second. (Layer B)
 @param pPENC The number of RS errors detected over 1 second. (Layer C)

 @return SONY_RESULT_OK if successful and pPEN valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod,
                                                               uint32_t * pPENA, uint32_t * pPENB, uint32_t * pPENC);

/**
 @brief Monitors the channel spectrum sense.

        To ensure correct polarity detection please use the
        sony_tunerdemod_config_id_t::SONY_TUNERDEMOD_CONFIG_SPECTRUM_INV
        config option in ::sony_tunerdemod_SetConfig to select the appropriate spectrum
        inversion for the tuner output.

 @param pTunerDemod The driver instance.
 @param pSense Indicates the spectrum sense.

 @return SONY_RESULT_OK if successful and pSense valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod,
                                                           sony_tunerdemod_spectrum_sense_t * pSense);

/**
 @brief Monitors the estimated SNR value.

        If SNR value cannot be get because demodulator is not locked,
        -1000dB is returned.

 @note  If the driver is configured for diver, this API returns the total SNR value.
        To get SNR value of main/sub ICs, use sony_tunerdemod_isdbt_monitor_SNR_diver.

 @param pTunerDemod The driver instance.
 @param pSNR The estimated SNR in dBx1000.

 @return SONY_RESULT_OK if successful and pSNR valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
                                                 int32_t * pSNR);

/**
 @brief Monitors the estimated SNR value. (For diver system)

        If SNR value cannot be get because demodulator is not locked,
        -1000dB is returned.

 @param pTunerDemod The driver instance.
 @param pSNR The estimated total SNR in dBx1000.
 @param pSNRMain The estimated SNR in dBx1000 for main IC only in diver system.
 @param pSNRSub The estimated SNR in dBx1000 for sub IC only in diver system.

 @return SONY_RESULT_OK if successful and pSNR valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                       int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub);

/**
 @brief Monitor the detected mode and guard interval.

 @param pTunerDemod  The driver instance.
 @param pMode Mode estimation result.
 @param pGuard Guard interval estimation result.

 @return SONY_RESULT_OK if successful and pMode, pGuard valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_ModeGuard (sony_tunerdemod_t * pTunerDemod,
                                                       sony_isdbt_mode_t * pMode,
                                                       sony_isdbt_guard_t * pGuard);

/**
 @brief Monitor the sampling frequency offset value.

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_isdbt_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod,
                                                            int32_t * pPPM);

/**
 @brief Monitor the sampling frequency offset value. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_isdbt_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_isdbt_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                                int32_t * pPPM);

/**
 @brief Monitor the PER (Packet Error Rate) parameters.

        PER value will be 0 for unused layers.

 @param pTunerDemod The driver instance.
 @param pPERA The estimated layer A PER value (Post reed solomon decoder) x 1e6.
 @param pPERB The estimated layer B PER value (Post reed solomon decoder) x 1e6.
 @param pPERC The estimated layer C PER value (Post reed solomon decoder) x 1e6.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_PER (sony_tunerdemod_t * pTunerDemod,
                                                 uint32_t * pPERA, uint32_t * pPERB, uint32_t * pPERC);

/**
 @brief Calculate the ISDB-T TS (Transport Stream) rate from TMCC and guard interval information.

        TS rate value will be 0 for unused layers.

 @param pTunerDemod The driver instance.
 @param pTSRateKbpsA The calculated TS rate of layer A in kbps.
 @param pTSRateKbpsB The calculated TS rate of layer B in kbps.
 @param pTSRateKbpsC The calculated TS rate of layer C in kbps.

 @return SONY_RESULT_OK if pTsRateKbps is valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_TSRate (sony_tunerdemod_t * pTunerDemod,
                                                    uint32_t * pTSRateKbpsA, uint32_t * pTSRateKbpsB, uint32_t * pTSRateKbpsC);

/**
  @brief Monitor the AC EEW (Earthquake Early Warning by AC signal) informations.

  @param pTunerDemod The driver instance.
  @param pIsExist ACEEW exist flag.
                  - 0 : ACEEW information does not exist.
                  - 1 : ACEEW information exists, please check pACEEWInfo.
  @param pACEEWInfo ACEEW Information.

  @return SONY_RESULT_OK if successful and pIsExist, pACEEW valid.
*/
sony_result_t sony_tunerdemod_isdbt_monitor_ACEEWInfo (sony_tunerdemod_t * pTunerDemod,
                                                       uint8_t * pIsExist,
													   sony_isdbt_aceew_info_t * pACEEWInfo);
#undef uint32_t 
#undef int32_t 
#undef int8_t 

#endif /* SONY_TUNERDEMOD_ISDBT_MONITOR_H */
