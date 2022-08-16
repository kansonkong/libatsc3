/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/

#include "sony_cxd6801_integ.h"
#include "sony_cxd6801_integ_atsc3.h"
#include "sony_cxd6801_demod.h"
#include "sony_cxd6801_demod_atsc3_monitor.h"

static sony_cxd6801_result_t atsc3_WaitDemodLock (sony_cxd6801_integ_t * pInteg);
static sony_cxd6801_result_t atsc3_WaitCWTracking (sony_cxd6801_integ_t * pInteg);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_Tune (sony_cxd6801_integ_t * pInteg,
                                     sony_cxd6801_atsc3_tune_param_t * pTuneParam)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_6_MHZ) && (pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_7_MHZ) &&
        (pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_8_MHZ)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    result = sony_cxd6801_demod_atsc3_AutoDetectSeq_Init (pInteg->pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Configure for manual PLP selection. */
#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
    if ((pInteg->pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)
        || (pInteg->pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        /* PLP configuration for channel bonding case */
        result = sony_cxd6801_demod_atsc3_chbond_SetPLPConfig (pInteg->pDemod, pTuneParam->plpIDNum, pTuneParam->plpID, pTuneParam->plpBond);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else
#endif /* SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND */
    {
        result = sony_cxd6801_demod_atsc3_SetPLPConfig (pInteg->pDemod, pTuneParam->plpIDNum, pTuneParam->plpID);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Tune the demodulator */
    result = sony_cxd6801_demod_atsc3_Tune (pInteg->pDemod, pTuneParam);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->TerrCableTune)) {
        /* Enable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Tune the RF part */
        result = pInteg->pTuner->TerrCableTune (pInteg->pTuner, pTuneParam->centerFreqKHz, SONY_CXD6801_DTV_SYSTEM_ATSC3, pTuneParam->bandwidth);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Reset the demod to enable acquisition */
    result = sony_cxd6801_demod_TuneEnd (pInteg->pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = atsc3_WaitDemodLock (pInteg);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = atsc3_WaitCWTracking (pInteg);
    if ((result != SONY_CXD6801_RESULT_OK) && (result != SONY_CXD6801_RESULT_ERROR_TIMEOUT)) {
        /* Even if timeout error occurred and failed to configure CW tracking,
           should continue without error. Demodulator may lock target ALPs after that. */
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (!pInteg->pDemod->scanMode) {
        result = sony_cxd6801_integ_atsc3_WaitALPLock (pInteg);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc3_EASTune (sony_cxd6801_integ_t * pInteg,
                                        sony_cxd6801_atsc3_tune_param_t * pTuneParam)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_EASTune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_6_MHZ) && (pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_7_MHZ) &&
        (pTuneParam->bandwidth != SONY_CXD6801_DTV_BW_8_MHZ)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    result = sony_cxd6801_demod_atsc3_AutoDetectSeq_Init (pInteg->pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Tune the demodulator */
    result = sony_cxd6801_demod_atsc3_EASTune (pInteg->pDemod, pTuneParam);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->TerrCableTune)) {
        /* Enable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Tune the RF part */
        result = pInteg->pTuner->TerrCableTune (pInteg->pTuner, pTuneParam->centerFreqKHz, SONY_CXD6801_DTV_SYSTEM_ATSC3, pTuneParam->bandwidth);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Reset the demod to enable acquisition */
    result = sony_cxd6801_demod_atsc3_EASTuneEnd (pInteg->pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = atsc3_WaitDemodLock (pInteg);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = atsc3_WaitCWTracking (pInteg);
    if ((result != SONY_CXD6801_RESULT_OK) && (result != SONY_CXD6801_RESULT_ERROR_TIMEOUT)) {
        /* Even if timeout error occurred and failed to configure CW tracking,
           should continue without error. Demodulator may lock target ALPs after that. */
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc3_Scan (sony_cxd6801_integ_t * pInteg,
                                     sony_cxd6801_integ_atsc3_scan_param_t * pScanParam,
                                     sony_cxd6801_integ_atsc3_scan_callback_t callBack)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_integ_atsc3_scan_result_t scanResult;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_Scan");

    if ((!pInteg) || (!pScanParam) || (!callBack) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Ensure the scan parameters are valid. */
    if (pScanParam->endFrequencyKHz < pScanParam->startFrequencyKHz) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pScanParam->stepFrequencyKHz == 0) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Check bandwidth validity */
    if ((pScanParam->bandwidth != SONY_CXD6801_DTV_BW_6_MHZ) && (pScanParam->bandwidth != SONY_CXD6801_DTV_BW_7_MHZ) &&
        (pScanParam->bandwidth != SONY_CXD6801_DTV_BW_8_MHZ)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency and other parameters. */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;
    scanResult.tuneResult = SONY_CXD6801_RESULT_OK;
    scanResult.tuneParam.centerFreqKHz = pScanParam->startFrequencyKHz;
    scanResult.tuneParam.bandwidth = pScanParam->bandwidth;
    scanResult.tuneParam.plpIDNum = 1;
    scanResult.tuneParam.plpID[0] = 0;
    scanResult.tuneParam.plpID[1] = 0;
    scanResult.tuneParam.plpID[2] = 0;
    scanResult.tuneParam.plpID[3] = 0;
#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
    scanResult.tuneParam.plpBond[0] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE;
    scanResult.tuneParam.plpBond[1] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE;
    scanResult.tuneParam.plpBond[2] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE;
    scanResult.tuneParam.plpBond[3] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE;
#endif

    /* Set scan mode enabled */
    result = sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3, 0x01);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneParam.centerFreqKHz = scanResult.centerFreqKHz;
        scanResult.tuneResult = sony_cxd6801_integ_atsc3_Tune(pInteg, &scanResult.tuneParam);
        switch (scanResult.tuneResult) {
        case SONY_CXD6801_RESULT_OK:
            /* Channel found, callback to application */
            callBack (pInteg, &scanResult, pScanParam);
            break;
        case SONY_CXD6801_RESULT_ERROR_UNLOCK:
        case SONY_CXD6801_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to application for progress updates */
            callBack (pInteg, &scanResult, pScanParam);
            break;
        default:
            /* Serious error occurred -> cancel operation. */
            sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3, 0x00);
            SONY_CXD6801_TRACE_RETURN (scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
        if (result != SONY_CXD6801_RESULT_OK) {
            sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3, 0x00);
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3, 0x00);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc3_SwitchPLP (sony_cxd6801_integ_t * pInteg,
                                          uint8_t plpIDNum,
                                          uint8_t plpID[4])
{
    sony_cxd6801_result_t result;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_SwitchPLP");

    if ((!pInteg) || (!pInteg->pDemod) || (plpIDNum > 4)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->atsc3EasState == SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS) {
        /* In EAS mode, ALP is not output */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
    if ((pInteg->pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)
        || (pInteg->pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        /* Unavailable for channel bonding */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }
#endif /* SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND */

    result = sony_cxd6801_demod_atsc3_SetPLPConfig (pInteg->pDemod, plpIDNum, plpID);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = sony_cxd6801_integ_atsc3_WaitALPLock (pInteg);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc3_WaitALPLock (sony_cxd6801_integ_t * pInteg)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;
    sony_cxd6801_demod_lock_result_t lock;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_WaitALPLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = sony_cxd6801_stopwatch_start (&timer);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_cxd6801_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Check for timeout condition */
        if (elapsed >= SONY_CXD6801_ATSC3_WAIT_ALP_LOCK) {
            continueWait = 0;
        }

        result = sony_cxd6801_demod_atsc3_CheckALPLock (pInteg->pDemod, &lock);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        switch (lock) {
        case SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED:
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);

        case SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED:
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_cxd6801_stopwatch_sleep (&timer, SONY_CXD6801_ATSC3_WAIT_LOCK_INTERVAL);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        else {
            result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            break;
        }
    }
    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc3_monitor_RFLevel (sony_cxd6801_integ_t * pInteg,
                                                int32_t * pRFLeveldB)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc3_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3)  {
        /* Not ATSC 3.0 */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    if (pInteg->pTuner->rfLevelFuncTerr == SONY_CXD6801_TUNER_RFLEVEL_FUNC_READ && pInteg->pTuner->ReadRFLevel) {
        /* Enable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->ReadRFLevel (pInteg->pTuner, pRFLeveldB);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else if (pInteg->pTuner->rfLevelFuncTerr == SONY_CXD6801_TUNER_RFLEVEL_FUNC_CALCFROMAGC
        && pInteg->pTuner->CalcRFLevelFromAGC) {
        uint32_t ifAgc;

        result = sony_cxd6801_demod_atsc3_monitor_IFAGCOut (pInteg->pDemod, &ifAgc);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = pInteg->pTuner->CalcRFLevelFromAGC (pInteg->pTuner, ifAgc, pRFLeveldB);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    /* dBm * 100 -> dBm * 1000 */
    *pRFLeveldB *= 10;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t atsc3_WaitDemodLock (sony_cxd6801_integ_t * pInteg)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_demod_lock_result_t lock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;
    uint16_t timeout = 0;
    sony_cxd6801_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_CXD6801_TRACE_ENTER ("atsc3_WaitDemodLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Wait for demod lock */
    result = sony_cxd6801_stopwatch_start (&timer);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    timeout = SONY_CXD6801_ATSC3_WAIT_DEMOD_LOCK;

    for (;;) {
        result = sony_cxd6801_stopwatch_elapsed (&timer, &elapsed);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (elapsed >= timeout) {
            continueWait = 0;
        }

        result = sony_cxd6801_demod_atsc3_CheckDemodLock (pInteg->pDemod, &lock);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        switch (lock) {
        case SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED:
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);

        case SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED:
            result = sony_cxd6801_demod_atsc3_AutoDetectSeq_UnlockCase (pInteg->pDemod, &continueWait);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
            if (!continueWait) {
                SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_UNLOCK);
            }
            /* continue waiting... */
            break;
        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_cxd6801_stopwatch_sleep (&timer, SONY_CXD6801_ATSC3_WAIT_LOCK_INTERVAL);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t atsc3_WaitCWTracking (sony_cxd6801_integ_t * pInteg)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint16_t timeout = 0;
    sony_cxd6801_stopwatch_t timer;
    uint8_t completed = 1;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_CXD6801_TRACE_ENTER ("atsc3_WaitCWTracking");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Wait for CW Tracking Completion */
    result = sony_cxd6801_stopwatch_start (&timer);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    timeout = SONY_CXD6801_ATSC3_WAIT_CW_TRACKING_TIMEOUT;

    for (;;) {
        result = sony_cxd6801_stopwatch_elapsed (&timer, &elapsed);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (elapsed >= timeout) {
            continueWait = 0;
        }

        result = sony_cxd6801_demod_atsc3_AutoDetectSeq_SetCWTracking (pInteg->pDemod, &completed);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        if (completed) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_cxd6801_stopwatch_sleep (&timer, SONY_CXD6801_ATSC3_WAIT_LOCK_INTERVAL);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    SONY_CXD6801_TRACE_RETURN (result);
}
