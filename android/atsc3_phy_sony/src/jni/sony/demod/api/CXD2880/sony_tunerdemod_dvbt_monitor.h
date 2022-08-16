/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_dvbt_monitor.h

 @brief The DVB-T tuner and demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_DVBT_MONITOR_H
#define SONY_TUNERDEMOD_DVBT_MONITOR_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_dvbt.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the DVB-T demodulator.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_dvbt_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pSyncStat The demodulator state.
        - 0: WAIT_AGC,
        - 1: WAIT_MGD,
        - 2: WAIT_FFCR,
        - 3: WAIT_ARGD,
        - 4: WAIT_TITP,
        - 5: WAIT_TPS,
        - 6: TPS_LOCK
 @param pTSLockStat Indicates the TS lock condition.
        - 0: TS not locked.
        - 1: TS locked.
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_RESULT_OK if successful and pSyncStat, pTSLockStat and pUnlockDetected valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod,
                                                     uint8_t * pSyncStat,
                                                     uint8_t * pTSLockStat,
                                                     uint8_t * pUnlockDetected);

/**
 @brief Monitors the synchronisation state of the DVB-T demodulator. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_dvbt_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pSyncStat The demodulator state.
        - 0: WAIT_AGC,
        - 1: WAIT_MGD,
        - 2: WAIT_FFCR,
        - 3: WAIT_ARGD,
        - 4: WAIT_TITP,
        - 5: WAIT_TPS,
        - 6: TPS_LOCK
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_RESULT_OK if successful and pSyncStat and pUnlockDetected valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                         uint8_t * pSyncStat,
                                                         uint8_t * pUnlockDetected);

/**
 @brief Monitor the detected mode/guard (not TPS mode/guard).

 @param pTunerDemod The driver instance.
 @param pMode Mode estimation result.
 @param pGuard Guard interval estimation result.

 @return SONY_RESULT_OK if successful and pMode, pGuard valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_ModeGuard (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt_mode_t * pMode,
                                                      sony_dvbt_guard_t * pGuard);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_dvbt_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod,
                                                          int32_t * pOffset);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel. (Sub IC)

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_dvbt_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                              int32_t * pOffset);

/**
 @brief Monitor the Pre-Viterbi BER.

 @param pTunerDemod The driver instance.
 @param pBER BER value (Pre viterbi decoder) x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_PreViterbiBER (sony_tunerdemod_t * pTunerDemod,
                                                          uint32_t * pBER);

/**
 @brief Monitor the Pre-RS BER.

 @param pTunerDemod The driver instance.
 @param pBER BER value (Pre reed solomon decoder) x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_PreRSBER (sony_tunerdemod_t * pTunerDemod,
                                                     uint32_t * pBER);

/**
 @brief Monitor the TPS information.

 @param pTunerDemod The driver instance.
 @param pInfo The TPS information.

 @return SONY_RESULT_OK if successful and pInfo valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_TPSInfo (sony_tunerdemod_t * pTunerDemod,
                                                    sony_dvbt_tpsinfo_t * pInfo);

/**
 @brief Monitors the number RS (Reed Solomon) errors detected by the
        RS decoder over 1 second.

        Also known as the code word reject count.

 @param pTunerDemod The driver instance.
 @param pPEN The number of RS errors detected over 1 second.

 @return SONY_RESULT_OK if successful and pPEN valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod,
                                                              uint32_t * pPEN);

/**
 @brief Monitors the channel spectrum sense.

        To ensure correct polarity detection please use
        the sony_tunerdemod_config_id_t::SONY_TUNERDEMOD_CONFIG_SPECTRUM_INV
        config option in ::sony_tunerdemod_SetConfig
        to select the appropriate spectrum inversion for the tuner output.

 @param pTunerDemod The driver instance.
 @param pSense Indicates the spectrum sense.

 @return SONY_RESULT_OK if successful and pSense valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod,
                                                          sony_tunerdemod_spectrum_sense_t * pSense);

/**
 @brief Monitors the estimated SNR value, clipped to a maximum of 40dB.

        If SNR value cannot be get because demodulator is not locked,
        -1000dB is returned.

 @note  If the driver is configured for diver, this API returns the total SNR value.
        To get SNR value of main/sub ICs, use sony_tunerdemod_dvbt_monitor_SNR_diver.

 @param pTunerDemod The driver instance.
 @param pSNR The estimated SNR in dBx1000.

 @return SONY_RESULT_OK if successful and pSNR valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
                                                int32_t * pSNR);

/**
 @brief Monitors the estimated SNR value, clipped to a maximum of 40dB. (For diver system)

        If SNR value cannot be get because demodulator is not locked,
        -1000dB is returned.

 @param pTunerDemod The driver instance.
 @param pSNR The estimated total SNR in dBx1000.
 @param pSNRMain The estimated SNR in dBx1000 for main IC only in diver system.
 @param pSNRSub The estimated SNR in dBx1000 for sub IC only in diver system.

 @return SONY_RESULT_OK if successful and pSNR valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                      int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub);

/**
 @brief Monitor the sampling frequency offset value.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_dvbt_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod,
                                                           int32_t * pPPM);

/**
 @brief Monitor the sampling frequency offset value. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_dvbt_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pPPM);

/**
 @brief Monitor the DVB-T quality metric.

        Based on Nordig SQI equation.

 @param pTunerDemod The driver instance.
 @param pQuality The quality as a percentage (0-100).

 @return SONY_RESULT_OK if successful and pQuality valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_Quality (sony_tunerdemod_t * pTunerDemod,
                                                    uint8_t * pQuality);


/**
 @brief Monitor the DVB-T PER (Packet Error Rate) parameters.

 @param pTunerDemod The driver instance.
 @param pPER The estimated PER x 1e6.

 @return SONY_RESULT_OK if successful and pPER valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_PER (sony_tunerdemod_t * pTunerDemod,
                                                uint32_t * pPER);

/**
 @brief DVB-T monitor for SSI (Signal Strength Indicator), based on the RF Level monitor value
        ::sony_tunerdemod_monitor_RFLevel.

        The RF Level monitor function should be optimised for your HW configuration before using
        this monitor.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SSI returns
        main IC value, and sony_tunerdemod_dvbt_monitor_SSI_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pSSI The Signal Strength Indicator value in %

 @return SONY_RESULT_OK if successful and pSSI valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SSI (sony_tunerdemod_t * pTunerDemod,
                                                uint8_t * pSSI);

/**
 @brief DVB-T monitor for SSI (Signal Strength Indicator), based on the RF Level monitor value
        ::sony_tunerdemod_monitor_RFLevel. (Sub IC)

        The RF Level monitor function should be optimised for your HW configuration before using
        this monitor.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_SSI returns
        main IC value, and sony_tunerdemod_dvbt_monitor_SSI_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pSSI The Signal Strength Indicator value in %

 @return SONY_RESULT_OK if successful and pSSI valid.
*/
sony_result_t sony_tunerdemod_dvbt_monitor_SSI_sub (sony_tunerdemod_t * pTunerDemod,
                                                    uint8_t * pSSI);
#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_DVBT_MONITOR_H */
