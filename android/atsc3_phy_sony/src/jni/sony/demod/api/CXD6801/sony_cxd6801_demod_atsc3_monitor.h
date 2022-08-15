/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/08/30
  Modification ID : 902dae58eb5249e028ccc5d7f60bb66cca9b43a9
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_atsc3_monitor.h

          This file provides the ATSC 3.0 demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_ATSC3_MONITOR_H
#define SONY_CXD6801_DEMOD_ATSC3_MONITOR_H

#include "sony_cxd6801_demod.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/

#define SONY_CXD6801_DEMOD_ATSC3_MONITOR_PRELDPCBER_INVALID 0xFFFFFFFF  /**< BER value if the BER value is invalid */
#define SONY_CXD6801_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID  0xFFFFFFFF  /**< BER value if the BER value is invalid */
#define SONY_CXD6801_DEMOD_ATSC3_MONITOR_POSTBCHFER_INVALID 0xFFFFFFFF  /**< FER value if the FER value is invalid */

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the ATSC 3.0 demodulator.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status basically.

 @param pDemod The demodulator instance.
 @param pSyncStat The demodulator state.
        - 0: WAIT_GO,
        - 1: WAIT_AGC,
        - 2: WAIT_BSDET,
        - 3: WAIT_L1Basic,
        - 4: WAIT_L1Detail,
        - 5: WAIT_DMD_OK,
        - 6: DMD_OK
 @param alpLockStat Pointer of 4 length array to store ALP lock status for each PLPs.
        - 0: ALP not locked.
        - 1: ALP locked.
 @param pALPLockAll Indicates all selected PLPs by ::sony_demod_atsc3_SetPLPConfig are locked or not.
        - 0: Not all PLPs are locked.
        - 1: All PLPs are locked.
 @param pUnlockDetected Indicates the early unlock condition.
        - 0: No early unlock.
        - 1: Early unlock.

 @return SONY_RESULT_OK if pSyncStat, pTSLockStat is valid, otherwise an error is returned.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SyncStat(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t * pSyncStat,
                                                 uint8_t alpLockStat[4],
                                                 uint8_t * pALPLockAll,
                                                 uint8_t * pUnlockDetected);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

        This function will compensate for an inverting tuner architecture if the
        demodulator has been configured accordingly using ::SONY_DEMOD_CONFIG_SPECTRUM_INV
        config option for ::sony_demod_SetConfig.

 @param pDemod The demodulator instance.
 @param pOffset The detected carrier offset in Hz.

 @return SONY_RESULT_OK if pOffset is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_CarrierOffset(sony_cxd6801_demod_t * pDemod,
                                                      int32_t * pOffset);

/**
 @brief Monitor the IFAGC value of the ATSC 3.0 demodulator.

        Actual dB gain dependent on attached tuner.

 @param pDemod The demodulator instance.
 @param pIFAGC The IFAGC output register value. Range 0 - 0xFFF. Unitless.

 @return SONY_RESULT_OK if pIFAGC is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_IFAGCOut(sony_cxd6801_demod_t * pDemod,
                                                 uint32_t * pIFAGC);

/**
 @brief Monitor the Bootstrap data.

 @param pDemod The demodulator instance.
 @param pBootstrap The bootstrap data monitored.

 @return SONY_RESULT_OK if successful and pBootstrap is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_Bootstrap(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_bootstrap_t * pBootstrap);

/**
 @brief Monitor the demodulator information on the active channel.

        These parameters are basically come from L1, but currently used by demodulator circuit.

 @param pDemod The demodulator instance.
 @param pOfdm Pointer to receive the tuning information.

 @return SONY_RESULT_OK if the pPresetInfo is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_OFDM(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_ofdm_t * pOfdm);

/**
 @brief Monitor the L1-Basic information.

 @param pDemod The demodulator instance.
 @param pL1Basic Pointer to receive the L1-Basic information.

 @return SONY_RESULT_OK if the pL1Basic is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Basic(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic);

/**
 @brief Monitor the L1-Basic and L1-Detail raw information.

        This API read both L1-Basic and L1-Detail at the same time
        because several L1-Detail members depend on L1-Basic information.
        Note that this API may read many data by I2C. (depend on L1-Detail)

        L1-Detail information read by this API is "raw" data.
        To format it, please use sony_demod_atsc3_monitor_L1Detail_convert.

 @param pDemod The demodulator instance.
 @param pL1Basic Pointer to receive the L1-Basic information.
 @param pL1Detail Pointer to receive the L1-Detail information raw data.

 @return SONY_RESULT_OK if the pPresetInfo is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Detail_raw(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic,
	sony_cxd6801_atsc3_l1detail_raw_t * pL1Detail);

/**
 @brief Format L1-Detail information.

        This API do not read new L1-Basic, L1-Detail information from the demod,
        simply format L1-Detail raw information to structs,
        sony_atsc3_l1detail_common_t, sony_atsc3_l1detail_subframe_t, sony_atsc3_l1detail_plp_t.

        Note that if the specified PLP mapped to multiple subframes,
        this API shows firstly found information related to the PLP.

 @param pDemod The demodulator instance.
 @param pL1Basic Pointer to receive the L1-Basic information.
 @param pL1Detail Pointer to receive the L1-Detail information raw data.
 @param plpID Target PLP ID to be read.
 @param pL1DetailCommon Formatted L1-Detail information. (common part)
 @param pL1DetailSubframe Formatted L1-Detail information. (subframe dependent part)
 @param pL1DetailPlp Formatted L1-Detail information. (PLP dependent part)

 @return SONY_RESULT_OK if the pPresetInfo is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Detail_convert(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic,
	sony_cxd6801_atsc3_l1detail_raw_t * pL1Detail,
                                                         uint8_t plpID,
														 sony_cxd6801_atsc3_l1detail_common_t * pL1DetailCommon,
														 sony_cxd6801_atsc3_l1detail_subframe_t * pL1DetailSubframe,
														 sony_cxd6801_atsc3_l1detail_plp_t * pL1DetailPlp);

/**
 @brief Monitor the PLP list in current RF channel.

 @param pDemod The demodulator instance.
 @param plpList Pointer to an array of sony_atsc3_plp_list_entry_t
        that can receive the list of PLPs carried. (max length is 64)
        This parameter can be NULL. If so, number of PLP is only returned.
 @param pNumPLPs The number of PLPs.

 @return SONY_RESULT_OK if the pPLPList and pNumPLPs are valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PLPList(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_plp_list_entry_t plpList[SONY_CXD6801_ATSC3_NUM_PLP_MAX],
                                                uint8_t * pNumPLPs);

/**
 @brief Monitor the PLP error indicator.

        A PLP error is indicated
        when one of the selected PLP was not found in the channel.

 @param pDemod The demodulator instance.
 @param pPLPError The error indicated.
        - 0: No data PLP error.
        - 1: Data PLP error detected.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PLPError(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t * pPLPError);

/**
 @brief Monitor the selected 4 PLPs are valid or not.

        The demodulator can correct PLP ID for single PLP case.
        If so, even if specified PLP ID is not correct, tune API can be success.
        This function checks such case too.

        This function is used internally in common.

 @param pDemod The demodulator instance.
 @param plpValid Pointer of 4 length array to store PLP valid information for each PLPs.
        - 0: Selected PLP is invalid.
        - 1: Selected PLP is valid.

 @return SONY_RESULT_OK if successful.
         SONY_RESULT_ERROR_HW_STATE if all 4 PLPs are invalid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SelectedPLPValid(sony_cxd6801_demod_t * pDemod,
                                                         uint8_t plpValid[4]);

/**
 @brief Monitors the channel spectrum sense.

        To ensure correct polarity detection
        please use the ::SONY_DEMOD_CONFIG_SPECTRUM_INV config option in
        ::sony_demod_SetConfig to select the appropriate spectrum inversion
        for the tuner output.

 @param pDemod The demodulator instance.
 @param pSense Indicates the spectrum sense.

 @return SONY_RESULT_OK if successful and pSense is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SpectrumSense(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_terr_cable_spectrum_sense_t * pSense);

/**
 @brief Monitor the SNR estimation made by the demodulator.

        Clipped to a maximum of 40dB.

 @param pDemod The demodulator instance.
 @param pSNR The returned SNR in dB x 1000.

 @return SONY_RESULT_OK if pSNR is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SNR(sony_cxd6801_demod_t * pDemod,
                                            int32_t * pSNR);

/**
 @brief Monitor the pre-LDPC BER.

        This provides the BER for each (max 4) PLPs.
        If plpID[N] is not specified in tune API,
        ber[N] will become SONY_DEMOD_ATSC3_MONITOR_PRELDPCBER_INVALID.

 @param pDemod The demodulator instance.
 @param ber Pointer of 4 length array to store each PLP BER. BER x 1e7 value are returned.

 @return SONY_RESULT_OK if the pBER is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PreLDPCBER(sony_cxd6801_demod_t * pDemod,
                                                   uint32_t ber[4]);

/**
 @brief Monitor the pre-BCH BER.

        This provides the BER for each (max 4) PLPs.
        If plpID[N] is not specified in tune API,
        ber[N] will become SONY_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID.

        Note that BER is supported depend on the PLP's FEC type.
        - SONY_ATSC3_PLP_FEC_BCH_LDPC_16K : Supported
        - SONY_ATSC3_PLP_FEC_BCH_LDPC_64K : Supported
        - SONY_ATSC3_PLP_FEC_CRS_LDPC_16K : NOT supported
        - SONY_ATSC3_PLP_FEC_CRS_LDPC_64K : NOT supported
        - SONY_ATSC3_PLP_FEC_LDPC_16K     : NOT supported
        - SONY_ATSC3_PLP_FEC_LDPC_64K     : NOT supported
        If "NOT supported" cases, ber[N] will become SONY_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID.

 @param pDemod The demodulator instance.
 @param ber Pointer of 4 length array to store each PLP BER. BER x 1e9 value are returned.

 @return SONY_RESULT_OK if pBER is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PreBCHBER(sony_cxd6801_demod_t * pDemod,
                                                  uint32_t ber[4]);

/**
 @brief Monitor the post BCH FER (FEC block error rate) parameters.

        This provides the FER for each (max 4) PLPs.
        If plpID[N] is not specified in tune API,
        fer[N] will become SONY_DEMOD_ATSC3_MONITOR_POSTBCHFER_INVALID.

        Regardless of PLP FEC type, this API can return estimated FER value.

 @param pDemod The demod instance.
 @param fer Pointer of 4 length array to store each PLP FER. FER x 1e6 value are returned.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PostBCHFER(sony_cxd6801_demod_t * pDemod,
                                                   uint32_t fer[4]);

/**
 @brief Monitor the Baseband Packet Error Number in 1 sec.

        This provides the Packet Error Number for each (max 4) PLPs.
        If plpID[N] is not specified in tune API,
        pen[N] will become 0.

 @param pDemod The demodulator instance.
 @param pen Pointer of 4 length array to store each PLP Packet Error Number.

 @return SONY_RESULT_OK if pPEN is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_BBPacketErrorNumber(sony_cxd6801_demod_t * pDemod,
                                                            uint32_t pen[4]);

/**
 @brief Monitor the sampling frequency offset value.

 @param pDemod The demodulator instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if pPPM is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SamplingOffset(sony_cxd6801_demod_t * pDemod,
                                                       int32_t * pPPM);

/**
 @brief Monitor the FEC type, Modulation, Code rate parameter for currently selected PLPs.

 @param pDemod The demodulator instance.
 @param fecmodcod Pointer of 4 length array to store each PLP parameters.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_FecModCod(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_plp_fecmodcod_t fecmodcod[4]);

/**
 @brief Monitor the averaged pre-BCH BER.

        This provides the BER for each (max 4) PLPs.
        If plpID[N] is not specified in tune API,
        ber[N] will become SONY_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID.

        Note that BER is supported depend on the PLP's FEC type.
        - SONY_ATSC3_PLP_FEC_BCH_LDPC_16K : Supported
        - SONY_ATSC3_PLP_FEC_BCH_LDPC_64K : Supported
        - SONY_ATSC3_PLP_FEC_CRS_LDPC_16K : NOT supported
        - SONY_ATSC3_PLP_FEC_CRS_LDPC_64K : NOT supported
        - SONY_ATSC3_PLP_FEC_LDPC_16K     : NOT supported
        - SONY_ATSC3_PLP_FEC_LDPC_64K     : NOT supported
        If "NOT supported" cases, ber[N] will become SONY_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID.

 @param pDemod The demodulator instance.
 @param ber Pointer to array for storing each PLP BER. BER x 1e9 value are returned.

 @return SONY_RESULT_OK if pBER is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_AveragedPreBCHBER(sony_cxd6801_demod_t * pDemod,
                                                          uint32_t ber[4]);

/**
 @brief Monitor the estimated averaged SNR value.

        Clipped to a maximum of 40dB.

 @param pDemod The demodulator instance.
 @param pSNR The returned SNR in dB x 1000.

 @return SONY_RESULT_OK if pSNR is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_AveragedSNR(sony_cxd6801_demod_t * pDemod,
                                                    int32_t * pSNR);

#endif /* SONY_CXD6801_DEMOD_ATSC3_MONITOR_H */
