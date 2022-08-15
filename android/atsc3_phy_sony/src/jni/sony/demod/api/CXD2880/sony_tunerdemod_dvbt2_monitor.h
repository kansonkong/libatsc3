/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_dvbt2_monitor.h

 @brief The DVB-T2 tuner and demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_DVBT2_MONITOR_H
#define SONY_TUNERDEMOD_DVBT2_MONITOR_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_dvbt2.h"
#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the T2 demodulator.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_dvbt2_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pSyncStat The demodulator state.
        - 0: WAIT_GO,
        - 1: WAIT_AGC,
        - 2: WAIT_P1DET,
        - 3: WAIT_L1PRE,
        - 4: WAIT_L1POST,
        - 5: WAIT_DMD_OK,
        - 6: DMD_OK
 @param pTSLockStat Indicates the TS lock condition.
        - 0: TS not locked.
        - 1: TS locked.
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_RESULT_OK if successful and pSyncStat, pTSLockStat and pUnlockDetected valid.
*/

sony_result_t sony_tunerdemod_dvbt2_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pSyncStat,
                                                      uint8_t * pTSLockStat,
                                                      uint8_t * pUnlockDetected);

/**
 @brief Monitors the synchronisation state of the T2 demodulator. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SyncStat returns
        main IC status, and sony_tunerdemod_dvbt2_monitor_SyncStat_sub returns sub IC status.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status.

 @param pTunerDemod The driver instance.
 @param pSyncStat The demodulator state.
        - 0: WAIT_GO,
        - 1: WAIT_AGC,
        - 2: WAIT_P1DET,
        - 3: WAIT_L1PRE,
        - 4: WAIT_L1POST,
        - 5: WAIT_DMD_OK,
        - 6: DMD_OK
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_RESULT_OK if successful and pSyncStat and pUnlockDetected valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                          uint8_t * pSyncStat,
                                                          uint8_t * pUnlockDetected);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel, using
        the continual pilot (CP) estimation from the demodulator.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset The detected carrier offset in Hz.

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod,
                                                           int32_t * pOffset);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel, using
        the continual pilot (CP) estimation from the demodulator. (Sub IC)

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_CarrierOffset returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_CarrierOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pOffset The detected carrier offset in Hz.

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pOffset);

/**
 @brief Monitor the L1-pre signalling information.

        L1-pre signalling information is available when TS is locked,
        and earlier in the acquisition sequence, after the P1 symbols
        have been decoded.

 @param pTunerDemod The driver instance.
 @param pL1Pre Pointer to receive the L1-pre signalling information.

 @return SONY_RESULT_OK if successful and pL1Pre valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_L1Pre (sony_tunerdemod_t * pTunerDemod,
                                                   sony_dvbt2_l1pre_t * pL1Pre);

/**
 @brief Monitor the T2 version in use.

        The version is available from the L1-pre signalling information.

 @param pTunerDemod The driver instance.
 @param pVersion Pointer to receive the version information.

 @return SONY_RESULT_OK if successful and pVersion valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_Version (sony_tunerdemod_t * pTunerDemod,
                                                     sony_dvbt2_version_t * pVersion);

/**
 @brief Monitor the tuning information on the active channel.

 @param pTunerDemod The driver instance.
 @param pOFDM Pointer to receive the tuning information.

 @return SONY_RESULT_OK if successful and pOFDM valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_OFDM (sony_tunerdemod_t * pTunerDemod,
                                                  sony_dvbt2_ofdm_t * pOFDM);

/**
 @brief Monitor the data PLPs that the demodulator has detected.

        If a single PLP service is in use, then numberPlps = 1
        and the plpIds[0] shall contain the signalled PLP Id.

 @param pTunerDemod The driver instance.
 @param pPLPIds Pointer to an array of at least 255 bytes in length
                that can receive the list of data PLPs carried.
                This parameter can be NULL. If so, number of data PLP is only returned.
 @param pNumPLPs The number of data PLPs detected (signalled in L1-post).

 @return SONY_RESULT_OK if successful and pPLPIds and pNumPLPs valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_DataPLPs (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pPLPIds,
                                                      uint8_t * pNumPLPs);

/**
 @brief Monitor the active PLP information.

        For multiple PLP systems, then able to monitor both the data and common PLP.
        For single PLP systems, only able to monitor the data PLP.

 @param pTunerDemod The driver instance.
 @param type The type of the PLP to monitor.
 @param pPLPInfo The PLP structure to receive the PLP information into.

 @return SONY_RESULT_OK if successful and pPLPInfo valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_ActivePLP (sony_tunerdemod_t * pTunerDemod,
                                                       sony_dvbt2_plp_btype_t type,
                                                       sony_dvbt2_plp_t * pPLPInfo);

/**
 @brief Monitor the data PLP error indicator.

        A data PLP error is indicated when the selected PLP was not found in the channel.

 @param pTunerDemod The driver instance.
 @param pPLPError The error indicated.
        - 0: No data PLP error.
        - 1: Data PLP error detected.

 @return SONY_RESULT_OK if successful and pPLPError valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_DataPLPError (sony_tunerdemod_t * pTunerDemod,
                                                          uint8_t * pPLPError);

/**
 @brief Monitor the L1 change indicator.

 @param pTunerDemod The driver instance.
 @param pL1Change Indicator of L1 change.
        - 0: L1 has not changed.
        - 1: L1 changed.

 @return SONY_RESULT_OK if successful and pL1Change valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_L1Change (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pL1Change);

/**
 @brief Monitors the basic L1-post data.

        The device must have L1-post lock before calling,
        if L1 post lock is not detected and error is returned.

 @param pTunerDemod The driver instance.
 @param pL1Post The L1 post data monitored.

 @return SONY_RESULT_OK if successful and pL1Post valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_L1Post (sony_tunerdemod_t * pTunerDemod,
                                                    sony_dvbt2_l1post_t * pL1Post);

/**
 @brief Monitors the BBHEADER data.

        The device must have TS lock before calling,
        if TS lock is not detected and error is returned.

 @param pTunerDemod The demodulator instance.
 @param type The type of the PLP to monitor.
 @param pBBHeader The BBHEADER data monitored.

 @return SONY_RESULT_OK if successful and pBBHeader is valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_BBHeader (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt2_plp_btype_t type,
                                                      sony_dvbt2_bbheader_t * pBBHeader);

/**
 @brief Monitor the in-band type B signalling TS (Transport Stream) rate.

        Available V1.2.1 or later.

 @param pTunerDemod The demodulator instance.
 @param type The target PLP type.
 @param pTSRateBps The TS rate of the PLP in bits/s.

 @return SONY_RESULT_OK if pTSRateBps is valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_InBandBTSRate (sony_tunerdemod_t * pTunerDemod,
                                                           sony_dvbt2_plp_btype_t type,
                                                           uint32_t * pTSRateBps);

/**
 @brief Monitors the channel spectrum sense.

        To ensure correct polarity detection please use the
        sony_tunerdemod_config_id_t::SONY_TUNERDEMOD_CONFIG_SPECTRUM_INV
        config option in ::sony_tunerdemod_SetConfig
        to select the appropriate spectrum inversion for the tuner output.

 @param pTunerDemod The driver instance.
 @param pSense Indicates the spectrum sense.

 @return SONY_RESULT_OK if successful and pSense valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod,
                                                           sony_tunerdemod_spectrum_sense_t * pSense);

/**
 @brief Monitors the estimated SNR value, clipped to a maximum of 40dB.

        If SNR value cannot be get because demodulator is not locked,
        -1000dB is returned.

 @note  If the driver is configured for diver, this API returns the total SNR value.
        To get SNR value of main/sub ICs, use sony_tunerdemod_dvbt2_monitor_SNR_diver.

 @param pTunerDemod The driver instance.
 @param pSNR The estimated SNR in dBx1000.

 @return SONY_RESULT_OK if successful and pSNR valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
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
sony_result_t sony_tunerdemod_dvbt2_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                       int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub);

/**
 @brief Monitor the pre-LDPC BER.

        This provides the data PLP BER in multiple PLP case.

 @param pTunerDemod The driver instance.
 @param pBER The returned BER x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_PreLDPCBER (sony_tunerdemod_t * pTunerDemod,
                                                        uint32_t * pBER);

/**
 @brief Monitor the post BCH FER (FEC block error rate) parameters.

 @param pTunerDemod The driver instance.
 @param pFER The estimated FER x 1e6.

 @return SONY_RESULT_OK if successful and pFER valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_PostBCHFER (sony_tunerdemod_t * pTunerDemod,
                                                        uint32_t * pFER);

/**
 @brief Monitor the pre-BCH BER.

 @param pTunerDemod The driver instance.
 @param pBER  The returned BER x 1e9.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_PreBCHBER (sony_tunerdemod_t * pTunerDemod,
                                                       uint32_t * pBER);

/**
 @brief Monitor the Packet Error Number.

        Also known as the code word reject count.

 @param pTunerDemod The driver instance.
 @param pPEN The returned Packet Error Number.

 @return SONY_RESULT_OK if successful and pPEN valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod,
                                                               uint32_t * pPEN);

/**
 @brief Monitor the sampling frequency offset value.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod,
                                                            int32_t * pPPM);

/**
 @brief Monitor the sampling frequency offset value. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SamplingOffset returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_SamplingOffset_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if successful and pPPM valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                                int32_t * pPPM);

/**
 @brief Monitor the demodulator *estimated* DVB-T2 TS (Transport Stream) rate.

 @param pTunerDemod The driver instance.
 @param pTSRateKbps The estimated TS rate in kbps.

 @return SONY_RESULT_OK if successful and pTSRateKbps valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_TSRate (sony_tunerdemod_t * pTunerDemod,
                                                    uint32_t * pTSRateKbps);

/**
 @brief Monitor the DVB-T2 quality metric.

 @param pTunerDemod The driver instance.
 @param pQuality The quality as a percentage (0-100).

 @return SONY_RESULT_OK if successful and pQuality valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_Quality (sony_tunerdemod_t * pTunerDemod,
                                                     uint8_t * pQuality);

/**
 @brief Monitor the DVB-T2 PER (Packet Error Rate).

 @param pTunerDemod The driver instance.
 @param pPER The estimated PER x 1e6.

 @return SONY_RESULT_OK if successful and pPER valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_PER (sony_tunerdemod_t * pTunerDemod,
                                                 uint32_t * pPER);

/**
 @brief Monitor the active PLP constellation.

        The common PLP in the current group can also be monitored if one exists.

 @param pTunerDemod The driver instance.
 @param type The type of the PLP to monitor (data or common).
 @param pQAM The PLP constellation.

 @return SONY_RESULT_OK if successful and pQAM valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_QAM (sony_tunerdemod_t * pTunerDemod,
                                                 sony_dvbt2_plp_btype_t type,
                                                 sony_dvbt2_plp_constell_t * pQAM);

/**
 @brief Monitor the active PLP code rate.

        The common PLP in the current group can also be monitored if one exists.

 @param pTunerDemod The driver instance.
 @param type The type of the PLP to monitor (data or common).
 @param pCodeRate The PLP code rate.

 @return SONY_RESULT_OK if successful and pCodeRate valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_CodeRate (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt2_plp_btype_t type,
                                                      sony_dvbt2_plp_code_rate_t * pCodeRate);

/**
 @brief Monitor the currently acquired DVB-T2 profile.

        This will return SONY_RESULT_ERROR_HW_STATE if the register value is not valid.

 @param pTunerDemod The driver instance.
 @param pProfile The profile of the DVB-T2 output from the demodulator.

 @return SONY_RESULT_OK if successful and pProfile valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_Profile (sony_tunerdemod_t * pTunerDemod,
                                                     sony_dvbt2_profile_t * pProfile);

/**
 @brief DVB-T2 monitor for SSI (Signal Strength Indicator), based on the RF Level monitor value
        ::sony_tunerdemod_monitor_RFLevel.

        The RF Level monitor function should be optimised for your HW configuration before using
        this monitor.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SSI returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_SSI_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pSSI The Signal Strength Indicator value in %

 @return SONY_RESULT_OK if successful and pSSI valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SSI (sony_tunerdemod_t * pTunerDemod,
                                                 uint8_t * pSSI);

/**
 @brief DVB-T2 monitor for SSI (Signal Strength Indicator), based on the RF Level monitor value
        ::sony_tunerdemod_monitor_RFLevel. (Sub IC)

        The RF Level monitor function should be optimised for your HW configuration before using
        this monitor.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt2_monitor_SSI returns
        main IC value, and sony_tunerdemod_dvbt2_monitor_SSI_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pSSI The Signal Strength Indicator value in %

 @return SONY_RESULT_OK if successful and pSSI valid.
*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SSI_sub (sony_tunerdemod_t * pTunerDemod,
                                                     uint8_t * pSSI);
#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_DVBT2_MONITOR_H */
