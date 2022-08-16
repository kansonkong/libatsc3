/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/

#include "sony_cxd_integ.h"
#include "sony_cxd_integ_dvbt_t2.h"
#include "sony_cxd_demod.h"
#include "sony_cxd_demod_dvbt_monitor.h"
#include "sony_cxd_demod_dvbt2_monitor.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_integ_dvbt_t2_Scan(sony_cxd_integ_t * pInteg,
                                      sony_cxd_integ_dvbt_t2_scan_param_t * pScanParam,
                                      sony_cxd_integ_dvbt_t2_scan_callback_t callBack)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_integ_dvbt_t2_scan_result_t scanResult;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_t2_Scan");

    if ((!pInteg) || (!pScanParam) || (!callBack) || (!pInteg->pDemod)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
    sony_cxd_atomic_set (&(pInteg->cancel), 0);

    /* Ensure the scan parameters are valid. */
    if (pScanParam->endFrequencyKHz < pScanParam->startFrequencyKHz) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pScanParam->stepFrequencyKHz == 0) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Check bandwidth validity */
    if ((pScanParam->bandwidth != sony_cxd_DTV_BW_1_7_MHZ) && (pScanParam->bandwidth != sony_cxd_DTV_BW_5_MHZ) &&
        (pScanParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) && (pScanParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) &&
        (pScanParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for invalid combination of 1.7MHz scanning with DVB-T/ANY. */
    if ((pScanParam->system != sony_cxd_DTV_SYSTEM_DVBT2) && (pScanParam->bandwidth == sony_cxd_DTV_BW_1_7_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x01);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        sony_cxd_dtv_system_t systemFound = sony_cxd_DTV_SYSTEM_UNKNOWN;
        sony_cxd_dvbt2_profile_t profileFound = sony_cxd_DVBT2_PROFILE_ANY;
        uint8_t channelComplete = 0;
        sony_cxd_dtv_system_t blindTuneSystem = pScanParam->system;
        sony_cxd_dvbt2_profile_t blindTuneProfile = pScanParam->t2Profile;

        while (!channelComplete) {
            scanResult.tuneResult = sony_cxd_integ_dvbt_t2_BlindTune(pInteg, scanResult.centerFreqKHz, pScanParam->bandwidth, blindTuneSystem, blindTuneProfile, &systemFound, &profileFound);
            switch (scanResult.tuneResult) {
            case sony_cxd_RESULT_OK:
                scanResult.system = systemFound;

                /* Channel found, callback to application */
                if (systemFound == sony_cxd_DTV_SYSTEM_DVBT){
                    scanResult.dvbtTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                    scanResult.dvbtTuneParam.bandwidth = pScanParam->bandwidth;
                    scanResult.dvbtTuneParam.profile = sony_cxd_DVBT_PROFILE_HP;
                    callBack (pInteg, &scanResult, pScanParam);

                    /* DVB-T detected, set channel complete to move to next frequency */
                    channelComplete = 1;
                }
                else if (systemFound == sony_cxd_DTV_SYSTEM_DVBT2) {
                    uint8_t numPLPs = 0;
                    uint8_t plpIds[255];
                    uint8_t mixed;
                    uint8_t i;

                    scanResult.dvbt2TuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                    scanResult.dvbt2TuneParam.bandwidth = pScanParam->bandwidth;
                    scanResult.dvbt2TuneParam.tuneInfo = sony_cxd_DEMOD_DVBT2_TUNE_INFO_OK;
                    scanResult.dvbt2TuneParam.profile = profileFound;

                    result = sony_cxd_integ_dvbt2_Scan_PrepareDataPLPLoop(pInteg, plpIds, &numPLPs, &mixed);
                    if (result == sony_cxd_RESULT_ERROR_HW_STATE) {
                        /* Callback to application for progress updates */
                        scanResult.system = sony_cxd_DTV_SYSTEM_UNKNOWN;
                        scanResult.tuneResult = sony_cxd_RESULT_ERROR_UNLOCK;
                        callBack (pInteg, &scanResult, pScanParam);

                        /* Error in monitored data, ignore current channel */
                        channelComplete = 1;
                    }
                    else if (result != sony_cxd_RESULT_OK) {
                        /* Serious error occurred -> cancel operation. */
                        sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);
                        sony_cxd_TRACE_RETURN (result);
                    }
                    else {
                        /* Set PLP ID in tune parameter structure and provide callback for first PLP */
                        scanResult.dvbt2TuneParam.dataPlpId = plpIds[0];
                        callBack (pInteg, &scanResult, pScanParam);

                        /* Callback for each subsequent PLP in current profile */
                        for (i = 1; i < numPLPs; i++) {
                            result = sony_cxd_integ_dvbt2_Scan_SwitchDataPLP(pInteg, mixed, plpIds[i], profileFound);
                            if (result != sony_cxd_RESULT_OK) {
                                sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);
                                sony_cxd_TRACE_RETURN (result);
                            }
                            /* Set PLP ID in tune parameter structure and provide callback */
                            scanResult.dvbt2TuneParam.dataPlpId = plpIds[i];
                            callBack (pInteg, &scanResult, pScanParam);
                        }

                        /* If profile is ANY, check for mixed profile channels */
                        if (blindTuneProfile == sony_cxd_DVBT2_PROFILE_ANY) {
                            /* Check for mixed profiles available */
                            if (mixed) {
                                /* Set Blind Tune parameters to DVB-T2 only and the other profile
                                    * compared to that already located */
                                blindTuneSystem = sony_cxd_DTV_SYSTEM_DVBT2;

                                if (profileFound == sony_cxd_DVBT2_PROFILE_BASE) {
                                    blindTuneProfile = sony_cxd_DVBT2_PROFILE_LITE;
                                }
                                else if (profileFound == sony_cxd_DVBT2_PROFILE_LITE) {
                                    blindTuneProfile = sony_cxd_DVBT2_PROFILE_BASE;
                                }
                                else {
                                    /* Serious error occurred -> cancel operation. */
                                    sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);
                                    sony_cxd_TRACE_RETURN (result);
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
            case sony_cxd_RESULT_ERROR_UNLOCK:
            case sony_cxd_RESULT_ERROR_TIMEOUT:
                /* Channel not found, callback to application for progress updates */
                scanResult.system = sony_cxd_DTV_SYSTEM_UNKNOWN;
                callBack (pInteg, &scanResult, pScanParam);

                /* Go to next frequency */
                channelComplete = 1;
                break;

            default:
                {
                    /* Serious error occurred -> cancel operation. */
                    sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);
                    sony_cxd_TRACE_RETURN (scanResult.tuneResult);
                }
            }
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_cxd_integ_CheckCancellation (pInteg);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);
            sony_cxd_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, pScanParam->system, 0x00);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_t2_BlindTune(sony_cxd_integ_t * pInteg,
                                           uint32_t centerFreqKHz,
                                           sony_cxd_dtv_bandwidth_t bandwidth,
                                           sony_cxd_dtv_system_t system,
                                           sony_cxd_dvbt2_profile_t profile,
                                           sony_cxd_dtv_system_t * pSystemTuned,
                                           sony_cxd_dvbt2_profile_t * pProfileTuned)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t channelFound = 0;
    sony_cxd_dtv_system_t tuneSystems[2] = {sony_cxd_DTV_SYSTEM_UNKNOWN , sony_cxd_DTV_SYSTEM_UNKNOWN};
    uint8_t tuneIteration;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_t2_BlindTune");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSystemTuned) || (!pProfileTuned)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (!pInteg->pDemod->scanMode) {
        /* Clear cancellation flag. */
        sony_cxd_atomic_set (&(pInteg->cancel), 0);
    }

    /* Confirm the demod is in a valid state to accept this API */
    if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Check bandwidth validity */
    if ((bandwidth != sony_cxd_DTV_BW_1_7_MHZ) && (bandwidth != sony_cxd_DTV_BW_5_MHZ) &&
        (bandwidth != sony_cxd_DTV_BW_6_MHZ) && (bandwidth != sony_cxd_DTV_BW_7_MHZ) &&
        (bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for invalid system parameter */
    if ((system != sony_cxd_DTV_SYSTEM_DVBT) && (system != sony_cxd_DTV_SYSTEM_DVBT2) && (system != sony_cxd_DTV_SYSTEM_ANY)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for invalid combination of 1.7MHz tune */
    if ((system != sony_cxd_DTV_SYSTEM_DVBT2) && (bandwidth == sony_cxd_DTV_BW_1_7_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    if (system == sony_cxd_DTV_SYSTEM_ANY) {
        tuneSystems[0] = pInteg->pDemod->blindTuneDvbt2First? sony_cxd_DTV_SYSTEM_DVBT2 : sony_cxd_DTV_SYSTEM_DVBT;
        tuneSystems[1] = pInteg->pDemod->blindTuneDvbt2First? sony_cxd_DTV_SYSTEM_DVBT : sony_cxd_DTV_SYSTEM_DVBT2;
    }
    else {
        tuneSystems[0] = system;
    }

    for (tuneIteration = 0; tuneIteration <=1; tuneIteration++) {
#ifdef SAME_TUNER_CONFIG_DVBT_T2
       /* If SAME_TUNER_CONFIG_DVBT_T2 is defined the driver assumes
          that the tuner configuration used for DVBT tuning is identical to
          DVBT2, therefore the tuner does not need to be reconfigured.  This will
          help to reduce blindtune and therefore scan duration. This is only
          valid if sony_cxd_DTV_SYSTEM_ANY is used. */

        /* Function pointer variable to store the pInteg->pTunerTerrCable->Tune pointer temporary */
        sony_cxd_result_t (*SavedTuneFunc) (struct sony_cxd_tuner_terr_cable_t *, uint32_t,
                                        sony_cxd_dtv_system_t, sony_cxd_dtv_bandwidth_t) = NULL;

        if (tuneIteration == 1)
        {
            SavedTuneFunc = pInteg->pTunerTerrCable->Tune;
            /* Set to NULL to signal sony_cxd_integ_dvbt_BlindTune or sony_cxd_integ_dvbt2_BlindTune
               to use the same tuner configuration. */
            pInteg->pTunerTerrCable->Tune = NULL;
        }

#endif
        /* Attempt DVB-T acquisition */
        if ((tuneSystems[tuneIteration] == sony_cxd_DTV_SYSTEM_DVBT) && (!channelFound)) {
            result = sony_cxd_integ_dvbt_BlindTune(pInteg, centerFreqKHz, bandwidth);
            if (result == sony_cxd_RESULT_OK) {
                *pSystemTuned = sony_cxd_DTV_SYSTEM_DVBT;
                channelFound = 1;
            }
        }
        /* Attempt DVB-T2 acquisition */
        if ((tuneSystems[tuneIteration] == sony_cxd_DTV_SYSTEM_DVBT2) && (!channelFound)) {
            result = sony_cxd_integ_dvbt2_BlindTune(pInteg, centerFreqKHz, bandwidth, profile, pProfileTuned);
            if (result == sony_cxd_RESULT_OK) {
                *pSystemTuned = sony_cxd_DTV_SYSTEM_DVBT2;
                channelFound = 1;
            }
        }

#ifdef SAME_TUNER_CONFIG_DVBT_T2
        /* If SAME_TUNER_CONFIG_DVBT_T2 is defined
           Restore Tune function pointer, if conditions are true.
        */
        if ((tuneIteration == 1)  && (SavedTuneFunc))
        {
            pInteg->pTunerTerrCable->Tune = SavedTuneFunc;
        }
#endif

    }
    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_t2_monitor_RFLevel (sony_cxd_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_t2_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system == sony_cxd_DTV_SYSTEM_DVBT) {
        result = sony_cxd_integ_dvbt_monitor_RFLevel (pInteg, pRFLeveldB);
    }
    else if (pInteg->pDemod->system == sony_cxd_DTV_SYSTEM_DVBT2) {
        result = sony_cxd_integ_dvbt2_monitor_RFLevel (pInteg, pRFLeveldB);
    }
    else {
        /* Not DVB-T or DVB-T2*/
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    sony_cxd_TRACE_RETURN (result);
}
