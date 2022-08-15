/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd6801_demod_atsc_monitor.h

          This file provides the ATSC demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_ATSC_MONITOR_H
#define SONY_CXD6801_DEMOD_ATSC_MONITOR_H

#include "sony_cxd6801_demod.h"

/**
 @brief Sync state monitor.

 @param pDemod The demodulator instance.
 @param pVQLockStat The demodulator lock condition.
        - 0: DMD not locked.
        - 1: DMD locked.
 @param pAGCLockStat Indicates the AGC lock condition.
        - 0: AGC not locked.
        - 1: AGC locked.
 @param pTSLockStat Indicates the TS lock condition.
        - 0: TS not locked.
        - 1: TS locked.
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_CXD6801_RESULT_OK if successful and pVQLockStat, pAGCLockStat,
         pTSLockStat, pUnlockDetected are valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SyncStat (sony_cxd6801_demod_t * pDemod,
                                                uint8_t * pVQLockStat,
                                                uint8_t * pAGCLockStat,
                                                uint8_t * pTSLockStat,
                                                uint8_t * pUnlockDetected);

/**
 @brief Monitor the IFAGC value.
        Actual dB gain dependent on attached tuner.

 @param pDemod The demodulator instance.
 @param pIFAGCOut The IFAGC output register value. Range 0x000 - 0x3FFF. Unitless.

 @return SONY_CXD6801_RESULT_OK if successful and pIFAGCOut valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_IFAGCOut (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pIFAGCOut);


/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

        This function will compensate for an inverting tuner architecture if the
        demodulator has been configured accordingly using ::SONY_CXD6801_DEMOD_CONFIG_SPECTRUM_INV
        config option for ::sony_cxd6801_demod_SetConfig.

 @param pDemod The demodulator instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_CXD6801_RESULT_OK if successful and pOffset valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_CarrierOffset (sony_cxd6801_demod_t * pDemod,
                                                     int32_t * pOffset);

/**
 @brief Monitor the Pre-RS BER.

 @param pDemod The demodulator instance.
 @param pBER Bit Error Rate value (Pre Reed-Solomon decoder) x 1e7.

 @return SONY_CXD6801_RESULT_OK if successful and pBER valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PreRSBER (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pBER);

/**
 @brief Monitor the Pre-RS SER (Symbol Error Rate).

 @param pDemod The demodulator instance.
 @param pSER Symbol Error Rate value (Pre Reed-Solomon decoder) x 1e7.

 @return SONY_CXD6801_RESULT_OK if successful and pSER valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PreRSSER (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pSER);

/**
 @brief Monitor the Post-RS WER (Word Error Rate).

 @param pDemod The demodulator instance.
 @param pWER Word Error Rate value (Post Reed-Solomon decoder) x 1e6.

 @return SONY_CXD6801_RESULT_OK if successful and pWER valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PostRSWER (sony_cxd6801_demod_t * pDemod,
                                                 uint32_t * pWER);


/**
 @brief Monitor the Packet Error Detection.

        This value will freeze when the packet error detected.
        Another read transaction will refresh the value.

 @param pDemod The demodulator instance.
 @param pPacketErrorDetected The Packet Error Presence.
        - 0: Packet Error absence.
        - 1: Packet Error presence.
 @return SONY_CXD6801_RESULT_OK if successful and pPacketErrorDetected valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PacketError (sony_cxd6801_demod_t * pDemod,
                                                   uint8_t * pPacketErrorDetected);

/**
 @brief Monitors the estimated SNR value.

 @param pDemod The demodulator instance.
 @param pSNR The estimated SNR in dBx1000.

 @return SONY_CXD6801_RESULT_OK if successful and pSNR is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SNR (sony_cxd6801_demod_t * pDemod,
                                           int32_t * pSNR);

/**
 @brief Monitor the sampling frequency offset value.

 @param pDemod The demodulator instance.
 @param pPPM The sampling frequency offset in ppm x 100.

 @return SONY_CXD6801_RESULT_OK if successful and pPPM is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SamplingOffset (sony_cxd6801_demod_t * pDemod,
                                                      int32_t * pPPM);

/**
 @brief Monitor the signal level data to be used for ::SONY_CXD6801_DEMOD_CONFIG_ATSC_NO_SIGNAL_THRESH and
        SONY_CXD6801_DEMOD_CONFIG_ATSC_SIGNAL_THRESH config option for ::sony_cxd6801_demod_SetConfig.

        This function should not be used in the normal sequence,
        but in the optimization sequence for early unlock detection.

 @param pDemod The demodulator instance.
 @param pSignalLevelData Indicates the Signal Level data for unlock optimization. Unitless.

 @return SONY_CXD6801_RESULT_OK if successful and pSignalLevelData is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SignalLevelData_ForUnlockOptimization (sony_cxd6801_demod_t * pDemod,
                                                                             uint32_t * pSignalLevelData);

/**
 @brief Monitor the value of the Internal Digital AGC output.

 @param pDemod The demodulator instance.
 @param pDigitalAGCOut The value of the Internal Digital AGC output. (0x00000 - 0x7FFFF)

 @return SONY_CXD6801_RESULT_OK if pDigitalAGCOut is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_InternalDigitalAGCOut (sony_cxd6801_demod_t * pDemod,
                                                             uint32_t * pDigitalAGCOut);

#endif /* SONY_CXD6801_DEMOD_ATSC_MONITOR_H */
