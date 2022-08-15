/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2016/04/14
  Modification ID : b3a863c9449ebbf8408830fb7cfafb763ac7a67f
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_dvbt2.h"
#include "sony_tunerdemod_dvbt2_monitor.h"
#include "sony_integ_dvbt2.h"
#include "sony_common.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Waits for demodulator lock, polling ::sony_tunerdemod_dvbt2_monitor_SyncStat
        at 10ms intervals.  Called as part of the Tune process.
*/
static sony_result_t dvbt2_WaitDemodLock (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_profile_t profile);

/**
 @brief Waits for L1 Post to be valid to ensure that subsequent calls to
        L1 based monitors do not return HW State error.  Polls
        ::sony_tunerdemod_dvbt2_CheckL1PostValid at 10ms intervals.  Called as
        part of the Tune, BlindTune and Scan processes.
*/
static sony_result_t dvbt2_WaitL1PostLock (sony_tunerdemod_t * pTunerDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_dvbt2_Tune (sony_tunerdemod_t * pTunerDemod,
                                          sony_dvbt2_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_Tune");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
    sony_atomic_set (&(pTunerDemod->cancel), 0);

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != SONY_DTV_BW_1_7_MHZ) && (pTuneParam->bandwidth != SONY_DTV_BW_5_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_6_MHZ) && (pTuneParam->bandwidth != SONY_DTV_BW_7_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for valid profile selection */
    if ((pTuneParam->profile != SONY_DVBT2_PROFILE_BASE) && (pTuneParam->profile != SONY_DVBT2_PROFILE_LITE)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_dvbt2_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_dvbt2_Tune2 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for demodulator lock */
    result = dvbt2_WaitDemodLock (pTunerDemod, pTuneParam->profile);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* FEF optimized setting for diver */
    result = sony_tunerdemod_dvbt2_DiverFEFSetting (pTunerDemod);
    if (result == SONY_RESULT_ERROR_HW_STATE) {
        /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
    }
    else if (result != SONY_RESULT_OK) {
        /* Serious error, so return result */
        SONY_TRACE_RETURN (result);
    }

    /* In DVB-T2, L1 Post information may not immediately be valid after acquisition
     * (L1POST_OK bit != 1).  This wait loop handles such cases.  This issue occurs
     * only under clean signal lab conditions, and will therefore not extend acquistion
     * time under normal conditions.
     */
    result = dvbt2_WaitL1PostLock (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Confirm correct PLP selection in acquisition */
    {
        uint8_t plpNotFound;

        result = sony_tunerdemod_dvbt2_monitor_DataPLPError (pTunerDemod, &plpNotFound);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        }
        else if (result != SONY_RESULT_OK) {
            /* Serious error, so return result */
            SONY_TRACE_RETURN (result);
        }

        if (plpNotFound) {
            result = SONY_RESULT_OK_CONFIRM;
            pTuneParam->tuneInfo = SONY_TUNERDEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID;
        }
        else {
            pTuneParam->tuneInfo = SONY_TUNERDEMOD_DVBT2_TUNE_INFO_OK;
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt2_Scan (sony_tunerdemod_t * pTunerDemod,
                                          sony_integ_dvbt2_scan_param_t * pScanParam,
                                          sony_integ_dvbt2_scan_callback_t callBack)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_integ_dvbt2_scan_result_t scanResult;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_Scan");

    if ((!pTunerDemod) || (!pScanParam) || (!callBack)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
    sony_atomic_set (&(pTunerDemod->cancel), 0);

    /* Ensure the scan parameters are valid. */
    if (pScanParam->endFrequencyKHz < pScanParam->startFrequencyKHz) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pScanParam->stepFrequencyKHz == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Check bandwidth validity */
    if ((pScanParam->bandwidth != SONY_DTV_BW_1_7_MHZ) && (pScanParam->bandwidth != SONY_DTV_BW_5_MHZ) &&
        (pScanParam->bandwidth != SONY_DTV_BW_6_MHZ) && (pScanParam->bandwidth != SONY_DTV_BW_7_MHZ) &&
        (pScanParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        sony_dvbt2_profile_t profileFound = SONY_DVBT2_PROFILE_ANY;
        uint8_t channelComplete = 0;
        sony_dvbt2_profile_t blindTuneProfile = pScanParam->t2Profile;

        while (!channelComplete) {
            scanResult.tuneResult = sony_integ_dvbt2_BlindTune(pTunerDemod, scanResult.centerFreqKHz, pScanParam->bandwidth, blindTuneProfile, &profileFound);
            switch (scanResult.tuneResult) {
            case SONY_RESULT_OK:
                {
                    uint8_t numPLPs = 0;
                    uint8_t plpIds[255];
                    uint8_t mixed;
                    uint8_t i;

                    scanResult.dvbt2TuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                    scanResult.dvbt2TuneParam.bandwidth = pScanParam->bandwidth;
                    scanResult.dvbt2TuneParam.tuneInfo = SONY_TUNERDEMOD_DVBT2_TUNE_INFO_OK;
                    scanResult.dvbt2TuneParam.profile = profileFound;

                    result = sony_integ_dvbt2_Scan_PrepareDataPLPLoop(pTunerDemod, plpIds, &numPLPs, &mixed);
                    if (result == SONY_RESULT_ERROR_HW_STATE) {
                        /* Callback to application for progress updates */
                        scanResult.tuneResult = SONY_RESULT_ERROR_UNLOCK;
                        callBack (pTunerDemod, &scanResult, pScanParam);

                        /* Error in monitored data, ignore current channel */
                        channelComplete = 1;
                    }
                    else if (result != SONY_RESULT_OK) {
                        /* Serious error occurred -> cancel operation. */
                        sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);
                        SONY_TRACE_RETURN (result);
                    }
                    else {
                        /* Set PLP ID in tune parameter structure and provide callback for first PLP */
                        scanResult.dvbt2TuneParam.dataPLPID = plpIds[0];
                        callBack (pTunerDemod, &scanResult, pScanParam);

                        /* Callback for each subsequent PLP in current profile */
                        for (i = 1; i < numPLPs; i++) {
                            result = sony_integ_dvbt2_Scan_SwitchDataPLP(pTunerDemod, mixed, plpIds[i], profileFound);
                            if (result != SONY_RESULT_OK) {
                                sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);
                                SONY_TRACE_RETURN (result);
                            }
                            /* Set PLP ID in tune parameter structure and provide callback */
                            scanResult.dvbt2TuneParam.dataPLPID = plpIds[i];
                            callBack (pTunerDemod, &scanResult, pScanParam);
                        }

                        /* If profile is ANY, check for mixed profile channels */
                        if (blindTuneProfile == SONY_DVBT2_PROFILE_ANY) {
                            /* Check for mixed profiles available */
                            if (mixed) {
                                /* Set Blind Tune parameters to DVB-T2 only and the other profile
                                 * compared to that already located */
                                if (profileFound == SONY_DVBT2_PROFILE_BASE) {
                                    blindTuneProfile = SONY_DVBT2_PROFILE_LITE;
                                }
                                else if (profileFound == SONY_DVBT2_PROFILE_LITE) {
                                    blindTuneProfile = SONY_DVBT2_PROFILE_BASE;
                                }
                                else {
                                    /* Serious error occurred -> cancel operation. */
                                    sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);
                                    SONY_TRACE_RETURN (result);
                                }
                            }
                            else {
                                /* Channel is not mixed profile so continue to next frequency */
                                channelComplete = 1;
                            }
                        }
                        else {
                            /* Profile is fixed therefore this frequency is complete */
                            channelComplete = 1;
                        }
                    }
                }
                break;

            /* Intentional fall-through. */
            case SONY_RESULT_ERROR_UNLOCK:
            case SONY_RESULT_ERROR_TIMEOUT:
                /* Channel not found, callback to application for progress updates */
                callBack (pTunerDemod, &scanResult, pScanParam);

                /* Go to next frequency */
                channelComplete = 1;
                break;

            default:
                {
                    /* Serious error occurred -> cancel operation. */
                    sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);
                    SONY_TRACE_RETURN (scanResult.tuneResult);
                }
            }
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT2, 0x00);

    SONY_TRACE_RETURN (result);
}


sony_result_t sony_integ_dvbt2_Scan_PrepareDataPLPLoop (sony_tunerdemod_t * pTunerDemod, uint8_t pPLPIds[], uint8_t *pNumPLPs, uint8_t *pMixed)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_Scan_PrepareDataPLPLoop");

    if ((!pTunerDemod) || (!pPLPIds) || (!pNumPLPs) || (!pMixed)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Obtain the T2 PLP list from Data PLP monitor */
    result = sony_tunerdemod_dvbt2_monitor_DataPLPs (pTunerDemod, pPLPIds, pNumPLPs);
    if (result == SONY_RESULT_OK) {
        /* Check for mixed profile channels */
        sony_dvbt2_ofdm_t ofdm;

        result = sony_tunerdemod_dvbt2_monitor_OFDM (pTunerDemod, &ofdm);
        if (result == SONY_RESULT_OK) {
           *pMixed = ofdm.mixed;
        }
    }

    SONY_TRACE_RETURN (result);
}


sony_result_t sony_integ_dvbt2_Scan_SwitchDataPLP (sony_tunerdemod_t * pTunerDemod, uint8_t mixed, uint8_t plpId, sony_dvbt2_profile_t profile)
{
    sony_result_t result = SONY_RESULT_OK;
    uint16_t plpAcquisitionTime = 0;
    sony_stopwatch_t timer;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_Scan_SwitchDataPLP");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = sony_tunerdemod_dvbt2_SetPLPConfig (pTunerDemod, 0x00, plpId);
    if (result != SONY_RESULT_OK) {
        /* Serious error occurred -> cancel operation. */
        SONY_TRACE_RETURN (result);
    }

    if ((profile == SONY_DVBT2_PROFILE_BASE) && (mixed)) {
        plpAcquisitionTime = 510;
    }
    else if ((profile == SONY_DVBT2_PROFILE_LITE) && (mixed)) {
        plpAcquisitionTime = 1260;
    }
    else {
        plpAcquisitionTime = 260;
    }

    /* Start stopwatch to measure PLP acquisition time */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    for (;;) {
        uint32_t elapsed;

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= plpAcquisitionTime) {
            break; /* finish waiting */
        } else {
            result = sony_stopwatch_sleep (&timer, SONY_DVBT2_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    SONY_TRACE_RETURN (result);
}


sony_result_t sony_integ_dvbt2_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                          uint32_t centerFreqKHz,
                                          sony_dtv_bandwidth_t bandwidth,
                                          sony_dvbt2_profile_t profile,
                                          sony_dvbt2_profile_t * pProfileTuned)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_dvbt2_tune_param_t tuneParam;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_BlindTune");

    if ((!pTunerDemod) || (!pTunerDemod) || (!pProfileTuned)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (!pTunerDemod->scanMode) {
        /* Clear cancellation flag. */
        sony_atomic_set (&(pTunerDemod->cancel), 0);
    }

    /* Confirm the demod is in a valid state to accept this API */
    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Check bandwidth validity */
    if ((bandwidth != SONY_DTV_BW_1_7_MHZ) && (bandwidth != SONY_DTV_BW_5_MHZ) &&
        (bandwidth != SONY_DTV_BW_6_MHZ) && (bandwidth != SONY_DTV_BW_7_MHZ) &&
        (bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if ((pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) && (profile == SONY_DVBT2_PROFILE_ANY)) {
        /* In diver, T2-base/T2-Lite auto setting is unavailable.
         * So, this code emulates profile == SONY_DVBT2_PROFILE_ANY behavior by software.
         * (It simply tries T2-base and if failed, tries T2-Lite.)
         */
        result = sony_integ_dvbt2_BlindTune (pTunerDemod, centerFreqKHz, bandwidth, SONY_DVBT2_PROFILE_BASE, pProfileTuned);
        if ((result == SONY_RESULT_ERROR_TIMEOUT) || (result == SONY_RESULT_ERROR_UNLOCK)) {
            result = sony_integ_dvbt2_BlindTune (pTunerDemod, centerFreqKHz, bandwidth, SONY_DVBT2_PROFILE_LITE, pProfileTuned);
        }

        SONY_TRACE_RETURN (result);
    }

    /* Attempt DVB-T2 acquisition */
    tuneParam.bandwidth = bandwidth;
    tuneParam.centerFreqKHz = centerFreqKHz;
    tuneParam.dataPLPID = SONY_DVBT2_TUNE_PARAM_PLPID_AUTO;
    tuneParam.profile = profile;

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_dvbt2_Tune1 (pTunerDemod, &tuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_dvbt2_Tune2 (pTunerDemod, &tuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for demodulator lock */
    result = dvbt2_WaitDemodLock (pTunerDemod, profile);
    switch (result) {
    case SONY_RESULT_OK:
        /* FEF optimized setting for diver */
        result = sony_tunerdemod_dvbt2_DiverFEFSetting (pTunerDemod);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        }
        else if (result != SONY_RESULT_OK) {
            /* Serious error, so return result */
            SONY_TRACE_RETURN (result);
        }

        /* In DVB-T2, L1 Post information may not immediately be valid after acquisition
         * (L1POST_OK bit != 1).  This wait loop handles such cases.  This issue occurs
         * only under clean signal lab conditions, and will therefore not extend acquistion
         * time under normal conditions.
         */
        result = dvbt2_WaitL1PostLock (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (profile == SONY_DVBT2_PROFILE_ANY) {
            /* Obtain the current profile if detection was automatic. */
            result = sony_tunerdemod_dvbt2_monitor_Profile (pTunerDemod, pProfileTuned);
            if (result == SONY_RESULT_ERROR_HW_STATE) {
                /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
            }
            else if (result != SONY_RESULT_OK) {
                /* Serious error, so return result */
                SONY_TRACE_RETURN (result);
            }
        }
        else {
            /* Else, set the tuned profile to the input parameter */
            *pProfileTuned = profile;
        }

        break;

    /* Intentional fall-through */
    case SONY_RESULT_ERROR_TIMEOUT:
    case SONY_RESULT_ERROR_UNLOCK:
        break;

    default:
        SONY_TRACE_RETURN(result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt2_WaitTSLock (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_profile_t profile)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    uint16_t timeout = 0;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_dvbt2_WaitTSLock");

    if ((!pTunerDemod) || (!pTunerDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TS lock */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (profile == SONY_DVBT2_PROFILE_BASE) {
        timeout = SONY_DVBT2_BASE_WAIT_TS_LOCK;
    }
    else if (profile == SONY_DVBT2_PROFILE_LITE) {
        timeout = SONY_DVBT2_LITE_WAIT_TS_LOCK;
    }
    else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= timeout) {
            continueWait = 0;
        }

        result = sony_tunerdemod_dvbt2_CheckTSLock (pTunerDemod, &lock);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        switch (lock) {
        case SONY_TUNERDEMOD_LOCK_RESULT_LOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_OK);

        case SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_stopwatch_sleep (&timer, SONY_DVBT2_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        } else {
            result = SONY_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
Local Functions
------------------------------------------------------------------------------*/
static sony_result_t dvbt2_WaitDemodLock (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_profile_t profile)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    uint16_t timeout = 0;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("dvbt2_WaitDemodLock");

    if ((!pTunerDemod) || (!pTunerDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Wait for demod lock */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (profile == SONY_DVBT2_PROFILE_BASE) {
        timeout = SONY_DVBT2_BASE_WAIT_DEMOD_LOCK;
    }
    else if ((profile == SONY_DVBT2_PROFILE_LITE) || (profile == SONY_DVBT2_PROFILE_ANY)) {
        timeout = SONY_DVBT2_LITE_WAIT_DEMOD_LOCK;
    }
    else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= timeout) {
            continueWait = 0;
        }

        result = sony_tunerdemod_dvbt2_CheckDemodLock (pTunerDemod, &lock);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        switch (lock) {
        case SONY_TUNERDEMOD_LOCK_RESULT_LOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_OK);

        case SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_stopwatch_sleep (&timer, SONY_DVBT2_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        } else {
            result = SONY_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t dvbt2_WaitL1PostLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;
    uint8_t l1PostValid;

    SONY_TRACE_ENTER ("dvbt2_WaitL1PostLock");

    if ((!pTunerDemod) || (!pTunerDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Check for timeout condition */
        if (elapsed >= SONY_DVBT2_L1POST_TIMEOUT) {
            continueWait = 0;
        }

        result = sony_tunerdemod_dvbt2_CheckL1PostValid (pTunerDemod, &l1PostValid);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* If L1 Post is valid, return from loop, else continue waiting */
        if (l1PostValid) {
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        }

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_stopwatch_sleep (&timer, SONY_DVBT2_WAIT_LOCK_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
        else {
            result = SONY_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_TRACE_RETURN (result);
}
#undef uint32_t 
#undef int32_t 
#undef int8_t 
