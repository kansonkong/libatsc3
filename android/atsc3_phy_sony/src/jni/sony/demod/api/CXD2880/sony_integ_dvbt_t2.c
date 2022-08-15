/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_dvbt.h"
#include "sony_tunerdemod_dvbt_monitor.h"
#include "sony_tunerdemod_dvbt2.h"
#include "sony_tunerdemod_dvbt2_monitor.h"
#include "sony_integ.h"
#include "sony_integ_dvbt.h"
#include "sony_integ_dvbt2.h"
#include "sony_integ_dvbt_t2.h"


#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_dvbt_t2_Scan(sony_tunerdemod_t * pTunerDemod,
                                      sony_integ_dvbt_t2_scan_param_t * pScanParam,
                                      sony_integ_dvbt_t2_scan_callback_t callBack)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_integ_dvbt_t2_scan_result_t scanResult;

    SONY_TRACE_ENTER ("sony_integ_dvbt_t2_Scan");

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

    /* Check for invalid combination of 1.7MHz scanning with DVB-T/ANY. */
    if ((pScanParam->system != SONY_DTV_SYSTEM_DVBT2) && (pScanParam->bandwidth == SONY_DTV_BW_1_7_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        sony_dtv_system_t systemFound = SONY_DTV_SYSTEM_UNKNOWN;
        sony_dvbt2_profile_t profileFound = SONY_DVBT2_PROFILE_ANY;
        uint8_t channelComplete = 0;
        sony_dtv_system_t blindTuneSystem = pScanParam->system;
        sony_dvbt2_profile_t blindTuneProfile = pScanParam->t2Profile;

        while (!channelComplete) {
            scanResult.tuneResult = sony_integ_dvbt_t2_BlindTune(pTunerDemod, scanResult.centerFreqKHz, pScanParam->bandwidth, blindTuneSystem, blindTuneProfile, &systemFound, &profileFound);
            switch (scanResult.tuneResult) {
            case SONY_RESULT_OK:
                scanResult.system = systemFound;

                /* Channel found, callback to application */
                if (systemFound == SONY_DTV_SYSTEM_DVBT){
                    scanResult.dvbtTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                    scanResult.dvbtTuneParam.bandwidth = pScanParam->bandwidth;
                    scanResult.dvbtTuneParam.profile = SONY_DVBT_PROFILE_HP;
                    callBack (pTunerDemod, &scanResult, pScanParam);

                    /* DVB-T detected, set channel complete to move to next frequency */
                    channelComplete = 1;
                }
                else if (systemFound == SONY_DTV_SYSTEM_DVBT2) {
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
                        scanResult.system = SONY_DTV_SYSTEM_UNKNOWN;
                        scanResult.tuneResult = SONY_RESULT_ERROR_UNLOCK;
                        callBack (pTunerDemod, &scanResult, pScanParam);

                        /* Error in monitored data, ignore current channel */
                        channelComplete = 1;
                    }
                    else if (result != SONY_RESULT_OK) {
                        /* Serious error occurred -> cancel operation. */
                        sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);
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
                                sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);
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
                                blindTuneSystem = SONY_DTV_SYSTEM_DVBT2;

                                if (profileFound == SONY_DVBT2_PROFILE_BASE) {
                                    blindTuneProfile = SONY_DVBT2_PROFILE_LITE;
                                }
                                else if (profileFound == SONY_DVBT2_PROFILE_LITE) {
                                    blindTuneProfile = SONY_DVBT2_PROFILE_BASE;
                                }
                                else {
                                    /* Serious error occurred -> cancel operation. */
                                    sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);
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
                scanResult.system = SONY_DTV_SYSTEM_UNKNOWN;
                callBack (pTunerDemod, &scanResult, pScanParam);

                /* Go to next frequency */
                channelComplete = 1;
                break;

            default:
                {
                    /* Serious error occurred -> cancel operation. */
                    sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);
                    SONY_TRACE_RETURN (scanResult.tuneResult);
                }
            }
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, pScanParam->system, 0x00);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt_t2_BlindTune(sony_tunerdemod_t * pTunerDemod,
                                           uint32_t centerFreqKHz,
                                           sony_dtv_bandwidth_t bandwidth,
                                           sony_dtv_system_t system,
                                           sony_dvbt2_profile_t profile,
                                           sony_dtv_system_t * pSystemTuned,
                                           sony_dvbt2_profile_t * pProfileTuned)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t channelFound = 0;
    sony_dtv_system_t tuneSystems[2] = {SONY_DTV_SYSTEM_UNKNOWN , SONY_DTV_SYSTEM_UNKNOWN};
    uint8_t tuneIteration;

    SONY_TRACE_ENTER ("sony_integ_dvbt_t2_BlindTune");

    if ((!pTunerDemod) || (!pSystemTuned) || (!pProfileTuned)) {
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

    /* Check for invalid system parameter */
    if ((system != SONY_DTV_SYSTEM_DVBT) && (system != SONY_DTV_SYSTEM_DVBT2) && (system != SONY_DTV_SYSTEM_ANY)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for invalid combination of 1.7MHz tune */
    if ((system != SONY_DTV_SYSTEM_DVBT2) && (bandwidth == SONY_DTV_BW_1_7_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (system == SONY_DTV_SYSTEM_ANY) {
        tuneSystems[0] = pTunerDemod->blindTuneDvbt2First? SONY_DTV_SYSTEM_DVBT2 : SONY_DTV_SYSTEM_DVBT;
        tuneSystems[1] = pTunerDemod->blindTuneDvbt2First? SONY_DTV_SYSTEM_DVBT : SONY_DTV_SYSTEM_DVBT2;
    }
    else {
        tuneSystems[0] = system;
    }

    for (tuneIteration = 0; tuneIteration <=1; tuneIteration++) {
        /* Attempt DVB-T acquisition */
        if ((tuneSystems[tuneIteration] == SONY_DTV_SYSTEM_DVBT) && (!channelFound)) {
            result = sony_integ_dvbt_BlindTune(pTunerDemod, centerFreqKHz, bandwidth);
            if (result == SONY_RESULT_OK) {
                *pSystemTuned = SONY_DTV_SYSTEM_DVBT;
                channelFound = 1;
            }
        }
        /* Attempt DVB-T2 acquisition */
        if ((tuneSystems[tuneIteration] == SONY_DTV_SYSTEM_DVBT2) && (!channelFound)) {
            result = sony_integ_dvbt2_BlindTune(pTunerDemod, centerFreqKHz, bandwidth, profile, pProfileTuned);
            if (result == SONY_RESULT_OK) {
                *pSystemTuned = SONY_DTV_SYSTEM_DVBT2;
                channelFound = 1;
            }
        }
    }
    SONY_TRACE_RETURN (result);
}
#undef uint32_t 
#undef int32_t 
#undef int8_t 
