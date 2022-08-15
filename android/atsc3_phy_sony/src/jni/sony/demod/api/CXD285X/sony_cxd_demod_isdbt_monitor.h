/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/08/01
  Modification ID : b30d76210d343216ea52b88e9b450c8fd5c0359f
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_demod_isdbt_monitor.h

          This file provides the ISDB-T demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_DEMOD_ISDBT_MONITOR_H
#define sony_cxd_DEMOD_ISDBT_MONITOR_H

#include "sony_cxd_demod.h"
#include "sony_cxd_isdbt.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the demodulator.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pDemod   The demodulator instance.
 @param pDmdLockStat Address of demodulator lock flag
                 - 0: Not lock
                 - 1: Lock
 @param pTSLockStat  Indicates the TS lock condition.
                 - 0: TS not locked.
                 - 1: TS locked.
 @param pUnlockDetected  Early unlock condition
                 - 0: No early unlock.
                 - 1: Early unlock detected.

 @return sony_cxd_RESULT_OK if successful and pSyncStat, pTSLockStat valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SyncStat(sony_cxd_demod_t * pDemod,
                                                 uint8_t * pDmdLockStat,
                                                 uint8_t * pTSLockStat,
                                                 uint8_t * pUnlockDetected);

/**
 @brief Monitor the IFAGC value.
        Actual dB gain dependent on attached tuner.

 @param pDemod The demodulator instance.
 @param pIFAGCOut The IFAGC output register value. Range 0x000 - 0xFFF. Unitless.

 @return sony_cxd_RESULT_OK if successful and pIFAGCOut valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_IFAGCOut(sony_cxd_demod_t * pDemod,
                                                 uint32_t * pIFAGCOut);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

        This function will compensate for an inverting tuner architecture if the
        demodulator has been configured accordignly using ::sony_cxd_DEMOD_CONFIG_SPECTRUM_INV
        config option for ::sony_cxd_demod_SetConfig.

 @param pDemod The demodulator instance.
 @param pOffset Carrier offset value (Hz).

 @return sony_cxd_RESULT_OK if successful and pOffset valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_CarrierOffset(sony_cxd_demod_t * pDemod,
                                                      int32_t * pOffset);

/**
 @brief Monitor the Pre-RS BER.

 @param pDemod The demodulator instance.
 @param pBERA Layer A BER value (Pre reed solomon decoder) x 1e7.
 @param pBERB Layer B BER value (Pre reed solomon decoder) x 1e7.
 @param pBERC Layer C BER value (Pre reed solomon decoder) x 1e7.

 @return sony_cxd_RESULT_OK if successful and pBER valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PreRSBER(sony_cxd_demod_t * pDemod,
                                                 uint32_t * pBERA, uint32_t * pBERB, uint32_t * pBERC);

/**
 @brief Monitor the TMCC information.

 @param pDemod The demodulator instance.
 @param pTMCCInfo The TMCC information.

 @return sony_cxd_RESULT_OK if successful and pInfo valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_TMCCInfo(sony_cxd_demod_t * pDemod,
	sony_cxd_isdbt_tmcc_info_t * pTMCCInfo);

/**
  @brief Monitor the TMCC preset information.

        This information enable demodulator fast acquisition by passing to
        ::sony_cxd_demod_isdbt_SetPreset. Tuning time will be shortened.

  @param pDemod The demodulator instance.
  @param pPresetInfo The TMCC Preset information struct instance.

  @return sony_cxd_RESULT_OK if successful and pPresetInfo is valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PresetInfo(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_isdbt_preset_info_t * pPresetInfo);

/**
 @brief Monitors the number RS (Reed Solomon) errors detected by the
        RS decoder over 1 second. Also known as the code word reject count.

 @param pDemod The demodulator instance.
 @param pPENA The number of RS errors detected over 1 second. (Layer A)
 @param pPENB The number of RS errors detected over 1 second. (Layer B)
 @param pPENC The number of RS errors detected over 1 second. (Layer C)

 @return sony_cxd_RESULT_OK if successful and pPEN valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PacketErrorNumber(sony_cxd_demod_t * pDemod,
                                                          uint32_t * pPENA, uint32_t * pPENB, uint32_t * pPENC);

/**
 @brief Monitors the channel spectrum sense.  To ensure correct polarity detection
        please use the ::sony_cxd_DEMOD_CONFIG_SPECTRUM_INV config option in
        ::sony_cxd_demod_SetConfig to select the appropriate spectrum inversion
        for the tuner output.

 @param pDemod The demodulator instance.
 @param pSense Indicates the spectrum sense.

 @return sony_cxd_RESULT_OK if successful and pSense is valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SpectrumSense(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_terr_cable_spectrum_sense_t * pSense);

/**
 @brief Monitors the estimated SNR value.

 @param pDemod The demodulator instance.
 @param pSNR The estimated SNR in dBx1000.

 @return sony_cxd_RESULT_OK if successful and pSNR is valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SNR(sony_cxd_demod_t * pDemod,
                                            int32_t * pSNR);

/**
 @brief Monitor the detected mode/guard.

 @param pDemod  The demodulator instance.
 @param pMode   Mode estimation result.
 @param pGuard  Guard interval estimation result.

 @return sony_cxd_RESULT_OK if successful and pMode, pGuard are valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_ModeGuard(sony_cxd_demod_t * pDemod,
	sony_cxd_isdbt_mode_t * pMode,
	sony_cxd_isdbt_guard_t * pGuard);

/**
 @brief Monitor the sampling frequency offset value.

 @param pDemod The demodulator instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return sony_cxd_RESULT_OK if pPPM is valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SamplingOffset(sony_cxd_demod_t * pDemod,
                                                       int32_t * pPPM);

/**
 @brief Monitor the PER (Packet Error Rate) parameters.

 @param pDemod The demod instance.
 @param pPERA The estimated layer A PER value (Post reed solomon decoder) x 1e6.
 @param pPERB The estimated layer B PER value (Post reed solomon decoder) x 1e6.
 @param pPERC The estimated layer C PER value (Post reed solomon decoder) x 1e6.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PER(sony_cxd_demod_t * pDemod,
                                            uint32_t * pPERA, uint32_t * pPERB, uint32_t * pPERC);

/**
 @brief Calculate the ISDB-T TS (Transport Stream) rate from TMCC and guard interval information.

 @param pDemod The demodulator instance.
 @param pTSRateKbpsA The calculated TS rate of layer A in kbps.
 @param pTSRateKbpsB The calculated TS rate of layer B in kbps.
 @param pTSRateKbpsC The calculated TS rate of layer C in kbps.

 @return sony_cxd_RESULT_OK if pTSRateKbps is valid.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_TSRate(sony_cxd_demod_t * pDemod,
                                               uint32_t * pTSRateKbpsA, uint32_t * pTSRateKbpsB, uint32_t * pTSRateKbpsC);

/**
  @brief Monitor the AC EEW (Earthquake Early Warning by AC signal) information

  @param pDemod     pDemod the demod instance.
  @param pIsExist   ACEEW exist flag.
                    If you get "0", ACEEW information does not exist.
                    If you get "1", ACEEW information exists, please check pACEEWInfo.
  @param pACEEWInfo ACEEW Information struct instance.

  @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_ACEEWInfo(sony_cxd_demod_t * pDemod,
                                                 uint8_t * pIsExist,
												 sony_cxd_isdbt_aceew_info_t * pACEEWInfo);

#endif /* sony_cxd_DEMOD_ISDBT_MONITOR_H */
