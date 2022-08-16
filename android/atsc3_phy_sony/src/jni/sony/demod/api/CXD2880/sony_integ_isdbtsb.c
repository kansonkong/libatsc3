/*------------------------------------------------------------------------------
  Copyright 2016 Sony Corporation

  Last Updated    : 2016/03/29
  Modification ID : c247b0fd8ce5951ee0f186647f755862889fa2e5
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbtsb.h"
#include "sony_tunerdemod_isdbt_monitor.h"
#include "sony_integ_isdbtsb.h"
#include "sony_integ_isdbt.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t isdbtsb_Tune_NormalTsb (sony_tunerdemod_t * pTunerDemod,
                                             sony_isdbtsb_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("isdbtsb_Tune_NormalTsb");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_isdbtsb_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_isdbtsb_Tune2 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for Demod lock OR TS lock */
    result = sony_integ_isdbt_WaitDemodOrTSLock (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get TMCC information to check current signal is ISDB-Tsb (1seg or 3seg) or not. */
    {
        sony_isdbt_tmcc_info_t tmccInfo;
        int segmentNum = 0;

        result = sony_tunerdemod_isdbt_monitor_TMCCInfo (pTunerDemod, &tmccInfo);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        } else if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* If number of segment is 15, the layer is unused. */
        if (tmccInfo.currentInfo.layerA.segmentsNum != 15) {
            segmentNum += tmccInfo.currentInfo.layerA.segmentsNum;
        }

        if (tmccInfo.currentInfo.layerB.segmentsNum != 15) {
            segmentNum += tmccInfo.currentInfo.layerB.segmentsNum;
        }

        if (tmccInfo.currentInfo.layerC.segmentsNum != 15) {
            segmentNum += tmccInfo.currentInfo.layerC.segmentsNum;
        }

        if ((segmentNum != 1) && (segmentNum != 3)) {
            /* This signal seems NOT to be ISDB-Tsb. Return SONY_RESULT_OK_CONFIRM. */
            SONY_TRACE_RETURN (SONY_RESULT_OK_CONFIRM);
        }
    }

    /* Signal is ISDB-Tsb, so write AGC setting. */
    result = sony_tunerdemod_isdbtsb_AGCSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t isdbtsb_Tune_ForceALayerPreset (sony_tunerdemod_t * pTunerDemod,
                                                     sony_isdbtsb_tune_param_t * pTuneParam,
                                                     sony_tunerdemod_isdbt_preset_info_t * pPresetInfo)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("isdbtsb_Tune_ForceALayerPreset");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_isdbtsb_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Preset setting for force A layer only */
    result = sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly (pTunerDemod, pPresetInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Signal must be ISDB-Tsb, so write AGC setting. */
    result = sony_tunerdemod_isdbtsb_AGCSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_isdbtsb_Tune2 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for Demod lock OR TS lock */
    result = sony_integ_isdbt_WaitDemodOrTSLock (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_isdbtsb_Tune (sony_tunerdemod_t * pTunerDemod,
                                       sony_isdbtsb_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbtsb_Tune");

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

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != SONY_DTV_BW_6_MHZ) && (pTuneParam->bandwidth != SONY_DTV_BW_7_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Clear cancellation flag. */
    sony_atomic_set (&(pTunerDemod->cancel), 0);

    result = isdbtsb_Tune_NormalTsb (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK_CONFIRM) {
        /* Success to lock ISDB-Tsb or error. */
        SONY_TRACE_RETURN (result);
    }

    /* Here, demod lock but TMCC information said not 1seg or 3seg -> Try force layer A preset setting */
    {
        sony_tunerdemod_isdbt_preset_info_t presetInfo;

        result = sony_tunerdemod_isdbt_monitor_PresetInfo (pTunerDemod, &presetInfo);
        if (result == SONY_RESULT_ERROR_HW_STATE) {
            /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
        } else if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Check partial reception flag and layer A segment number. */
        if (((presetInfo.data[0] & 0x01) != 0x01) || (((presetInfo.data[2] >> 3) & 0x0F) != 0x01)) {
            SONY_TRACE_RETURN (SONY_RESULT_OK_CONFIRM);
        }

        result = isdbtsb_Tune_ForceALayerPreset (pTunerDemod, pTuneParam, &presetInfo);
        SONY_TRACE_RETURN (result);
    }
}

sony_result_t sony_integ_isdbt_tsb_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                              uint32_t centerFreqKHz,
                                              sony_dtv_bandwidth_t bandwidth,
                                              sony_dtv_system_t system,
                                              sony_dtv_system_t * pSystemTuned)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_isdbt_preset_info_t presetInfo;

    SONY_TRACE_ENTER ("sony_integ_isdbt_tsb_BlindTune");

    if ((!pTunerDemod) || (!pSystemTuned)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Confirm the demod is in a valid state to accept this API */
    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Check bandwidth validity */
    if ((bandwidth != SONY_DTV_BW_6_MHZ) && (bandwidth != SONY_DTV_BW_7_MHZ) &&
        (bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Check for invalid system parameter */
    if ((system != SONY_DTV_SYSTEM_ISDBT) && (system != SONY_DTV_SYSTEM_ISDBTSB) && (system != SONY_DTV_SYSTEM_ANY)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (!pTunerDemod->scanMode) {
        /* Clear cancellation flag. */
        sony_atomic_set (&(pTunerDemod->cancel), 0);
    }

    /* Try ISDB-Tsb tune */
    if ((system == SONY_DTV_SYSTEM_ISDBTSB) || (system == SONY_DTV_SYSTEM_ANY)) {
        sony_isdbtsb_tune_param_t isdbtsbTuneParam;
        isdbtsbTuneParam.centerFreqKHz = centerFreqKHz;
        isdbtsbTuneParam.bandwidth = bandwidth;
        isdbtsbTuneParam.subChannel = 7; /* Assume area broadcasting */

        result = isdbtsb_Tune_NormalTsb (pTunerDemod, &isdbtsbTuneParam);
        if (result == SONY_RESULT_OK) {
            /* ISDB-Tsb signal */
            *pSystemTuned = SONY_DTV_SYSTEM_ISDBTSB;
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        } else if (result == SONY_RESULT_OK_CONFIRM) {
            /* Demod is locked but the signal seems NOT to be normal ISDB-Tsb (1seg, 3seg). */

            /* Get current preset information for 2nd, 3rd tuning. */
            result = sony_tunerdemod_isdbt_monitor_PresetInfo (pTunerDemod, &presetInfo);
            if (result == SONY_RESULT_ERROR_HW_STATE) {
                /* Demod lock is lost causing monitor to fail, return UNLOCK instead of HW STATE */
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_UNLOCK);
            } else if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            if (system == SONY_DTV_SYSTEM_ISDBTSB) {
                /* Check partial reception flag and layer A segment number. */
                if (((presetInfo.data[0] & 0x01) != 0x01) || (((presetInfo.data[2] >> 3) & 0x0F) != 0x01)) {
                    *pSystemTuned = SONY_DTV_SYSTEM_ISDBTSB;
                    SONY_TRACE_RETURN (SONY_RESULT_OK_CONFIRM);
                }

                /* Try force A layer preset tuning next. */
            } else {
                /* Enable preset for next ISDB-T tuning. */
                result = sony_tunerdemod_isdbt_SetPreset (pTunerDemod, &presetInfo);
                if (result != SONY_RESULT_OK) {
                    SONY_TRACE_RETURN (result);
                }
            }
        } else {
            /* Other error */
            SONY_TRACE_RETURN (result);
        }
    }

    /* Try ISDB-T tune */
    if ((system == SONY_DTV_SYSTEM_ISDBT) || (system == SONY_DTV_SYSTEM_ANY)) {
        sony_isdbt_tune_param_t isdbtTuneParam;
        isdbtTuneParam.centerFreqKHz = centerFreqKHz;
        isdbtTuneParam.bandwidth = bandwidth;
        /* Enable one-segment optimization setting for scanning use. */
        isdbtTuneParam.oneSegmentOptimize = 1;

        result = sony_integ_isdbt_Tune (pTunerDemod, &isdbtTuneParam);

        if (system == SONY_DTV_SYSTEM_ANY) {
            /* Restore preset setting */
            sony_tunerdemod_isdbt_SetPreset (pTunerDemod, NULL);
        }

        if (result == SONY_RESULT_OK) {
            *pSystemTuned = SONY_DTV_SYSTEM_ISDBT;
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        } else if ((result == SONY_RESULT_ERROR_UNLOCK) || (result == SONY_RESULT_ERROR_TIMEOUT)) {
            if (system == SONY_DTV_SYSTEM_ISDBT) {
                SONY_TRACE_RETURN (result); /* Failed. Finish here */
            } else {
                /* Check partial reception flag and layer A segment number. */
                if (((presetInfo.data[0] & 0x01) != 0x01) || (((presetInfo.data[2] >> 3) & 0x0F) != 0x01)) {
                    SONY_TRACE_RETURN (result);
                }

                /* Try force A layer preset tuning next. */
            }
        } else {
            /* Other error */
            SONY_TRACE_RETURN (result);
        }
    }

    /* Try ISDB-Tsb (force A layer preset) tune */
    if ((system == SONY_DTV_SYSTEM_ISDBTSB) || (system == SONY_DTV_SYSTEM_ANY)) {
        sony_isdbtsb_tune_param_t isdbtsbTuneParam;
        isdbtsbTuneParam.centerFreqKHz = centerFreqKHz;
        isdbtsbTuneParam.bandwidth = bandwidth;
        isdbtsbTuneParam.subChannel = 7; /* Assume area broadcasting */

        result = isdbtsb_Tune_ForceALayerPreset (pTunerDemod, &isdbtsbTuneParam, &presetInfo);
        if (result == SONY_RESULT_OK) {
            *pSystemTuned = SONY_DTV_SYSTEM_ISDBTSB;
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        } else {
            SONY_TRACE_RETURN (result);
        }
    }

    /* This line is never reached. */
    SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
}

sony_result_t sony_integ_isdbt_tsb_Scan (sony_tunerdemod_t * pTunerDemod,
                                         sony_integ_isdbt_tsb_scan_param_t * pScanParam,
                                         sony_integ_isdbt_tsb_scan_callback_t callBack)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_integ_isdbt_tsb_scan_result_t scanResult;

    SONY_TRACE_ENTER ("sony_integ_isdbt_tsb_Scan");

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
    if ((pScanParam->bandwidth != SONY_DTV_BW_6_MHZ) && (pScanParam->bandwidth != SONY_DTV_BW_7_MHZ) &&
        (pScanParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        sony_dtv_system_t systemFound = SONY_DTV_SYSTEM_UNKNOWN;

        scanResult.tuneResult = sony_integ_isdbt_tsb_BlindTune (pTunerDemod, scanResult.centerFreqKHz,
            pScanParam->bandwidth, pScanParam->system, &systemFound);

        switch (scanResult.tuneResult) {
        case SONY_RESULT_OK:
        case SONY_RESULT_OK_CONFIRM:
            /* Channel found, callback to application */
            scanResult.system = systemFound;

            if (systemFound == SONY_DTV_SYSTEM_ISDBT) {
                scanResult.isdbtTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                scanResult.isdbtTuneParam.bandwidth = pScanParam->bandwidth;
                scanResult.isdbtTuneParam.oneSegmentOptimize = 1;
                callBack (pTunerDemod, &scanResult, pScanParam);
            } else if (systemFound == SONY_DTV_SYSTEM_ISDBTSB) {
                scanResult.isdbtsbTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
                scanResult.isdbtsbTuneParam.bandwidth = pScanParam->bandwidth;
                scanResult.isdbtsbTuneParam.subChannel = 7;
                callBack (pTunerDemod, &scanResult, pScanParam);
            } else {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
            }

            break;

        /* Intentional fall-through. */
        case SONY_RESULT_ERROR_UNLOCK:
        case SONY_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to application for progress updates */
            scanResult.system = SONY_DTV_SYSTEM_UNKNOWN;
            callBack (pTunerDemod, &scanResult, pScanParam);
            break;

        default:
            /* Serious error or cancel occurred -> cancel operation. */
            sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0x00);
            SONY_TRACE_RETURN (scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0x00);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0x00);

    SONY_TRACE_RETURN (result);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 