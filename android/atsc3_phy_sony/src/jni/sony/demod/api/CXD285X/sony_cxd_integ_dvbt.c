/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
#include "sony_cxd_integ.h"
#include "sony_cxd_integ_dvbt.h"
#include "sony_cxd_demod.h"
#include "sony_cxd_demod_dvbt_monitor.h"

/*------------------------------------------------------------------------------
Local Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Waits for demodulator lock, polling ::sony_cxd_demod_dvbt_monitor_SyncStat
        at 10ms intervals.  Called as part of the Tune process.
*/
static sony_cxd_result_t dvbt_WaitDemodLock (sony_cxd_integ_t * pInteg);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_integ_dvbt_Tune(sony_cxd_integ_t * pInteg,
                                   sony_cxd_dvbt_tune_param_t * pTuneParam)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
    sony_cxd_atomic_set (&(pInteg->cancel), 0);

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != sony_cxd_DTV_BW_5_MHZ) && (pTuneParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) &&
        (pTuneParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) && (pTuneParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Set DVB-T profile for acquisition */
    result = sony_cxd_demod_dvbt_SetProfile(pInteg->pDemod, pTuneParam->profile);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Tune the demodulator */
    result = sony_cxd_demod_dvbt_Tune (pInteg->pDemod, pTuneParam);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Tune)) {
        /* Enable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Tune the RF part */
        result = pInteg->pTunerTerrCable->Tune (pInteg->pTunerTerrCable, pTuneParam->centerFreqKHz, sony_cxd_DTV_SYSTEM_DVBT, pTuneParam->bandwidth);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }

    /* Reset the demod to enable acquisition */
    result = sony_cxd_demod_TuneEnd (pInteg->pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Wait for TS lock */
    result = sony_cxd_integ_dvbt_WaitTSLock (pInteg);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_Scan(sony_cxd_integ_t * pInteg,
                                   sony_cxd_integ_dvbt_scan_param_t * pScanParam,
                                   sony_cxd_integ_dvbt_scan_callback_t callBack)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_integ_dvbt_scan_result_t scanResult;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_Scan");

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
    if ((pScanParam->bandwidth != sony_cxd_DTV_BW_5_MHZ) && (pScanParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) &&
        (pScanParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) && (pScanParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_DVBT, 0x01);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneResult = sony_cxd_integ_dvbt_BlindTune(pInteg, scanResult.centerFreqKHz, pScanParam->bandwidth);
        switch (scanResult.tuneResult) {
        /* Channel found, callback to application */
        case sony_cxd_RESULT_OK:
            scanResult.dvbtTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
            scanResult.dvbtTuneParam.bandwidth = pScanParam->bandwidth;
            scanResult.dvbtTuneParam.profile = sony_cxd_DVBT_PROFILE_HP;
            callBack (pInteg, &scanResult, pScanParam);
            break;

        /* Intentional fall-through. */
        case sony_cxd_RESULT_ERROR_UNLOCK:
        case sony_cxd_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to application for progress updates */
            callBack (pInteg, &scanResult, pScanParam);
            break;

        default:
            /* Serious error occurred -> cancel operation. */
            sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_DVBT, 0x00);
            sony_cxd_TRACE_RETURN (scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_cxd_integ_CheckCancellation (pInteg);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_DVBT, 0x00);
            sony_cxd_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_DVBT, 0x00);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_BlindTune(sony_cxd_integ_t * pInteg,
                                        uint32_t centerFreqKHz,
                                        sony_cxd_dtv_bandwidth_t bandwidth)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_dvbt_tune_param_t tuneParam;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_BlindTune");

    if ((!pInteg) || (!pInteg->pDemod)) {
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
    if ((bandwidth != sony_cxd_DTV_BW_5_MHZ) && (bandwidth != sony_cxd_DTV_BW_6_MHZ) &&
        (bandwidth != sony_cxd_DTV_BW_7_MHZ) && (bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Attempt DVB-T acquisition */
    tuneParam.bandwidth = bandwidth;
    tuneParam.centerFreqKHz = centerFreqKHz;
    tuneParam.profile = sony_cxd_DVBT_PROFILE_HP;

    /* Set DVB-T profile to HP to allow detection of hierachical and non-hierachical modes */
    result = sony_cxd_demod_dvbt_SetProfile(pInteg->pDemod, sony_cxd_DVBT_PROFILE_HP);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Tune the demodulator */
    result = sony_cxd_demod_dvbt_Tune (pInteg->pDemod, &tuneParam);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }


    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Tune)) {
        /* Enable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Tune the RF part */
        result = pInteg->pTunerTerrCable->Tune (pInteg->pTunerTerrCable, tuneParam.centerFreqKHz, sony_cxd_DTV_SYSTEM_DVBT, tuneParam.bandwidth);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }


    /* Reset the demod to enable acquisition */
    result = sony_cxd_demod_TuneEnd (pInteg->pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Wait for demodulator lock */
    result = dvbt_WaitDemodLock (pInteg);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_WaitTSLock (sony_cxd_integ_t * pInteg)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_demod_lock_result_t lock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_cxd_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_WaitTSLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TS lock */
    result = sony_cxd_stopwatch_start (&timer);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_cxd_stopwatch_elapsed(&timer, &elapsed);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        if (elapsed >= sony_cxd_DVBT_WAIT_TS_LOCK) {
            continueWait = 0;
        }

        result = sony_cxd_demod_dvbt_CheckTSLock (pInteg->pDemod, &lock);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Check Echo Optomization */
        result = sony_cxd_demod_dvbt_EchoOptimization(pInteg->pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        switch (lock) {
        case sony_cxd_DEMOD_LOCK_RESULT_LOCKED:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);

        case sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd_integ_CheckCancellation (pInteg);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_cxd_stopwatch_sleep (&timer, sony_cxd_DVBT_WAIT_LOCK_INTERVAL);
            if (result != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (result);
            }
        } else {
            result = sony_cxd_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_monitor_RFLevel (sony_cxd_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != sony_cxd_DTV_SYSTEM_DVBT)  {
        /* Not DVB-T */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pTunerTerrCable) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    if (pInteg->pTunerTerrCable->ReadRFLevel) {
        /* Enable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        result = pInteg->pTunerTerrCable->ReadRFLevel (pInteg->pTunerTerrCable, pRFLeveldB);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    } else if (pInteg->pTunerTerrCable->CalcRFLevelFromAGC) {
        uint32_t ifAgc;

        result = sony_cxd_demod_dvbt_monitor_IFAGCOut(pInteg->pDemod, &ifAgc);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        result = pInteg->pTunerTerrCable->CalcRFLevelFromAGC (pInteg->pTunerTerrCable, ifAgc, pRFLeveldB);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    } else {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_integ_dvbt_monitor_SSI (sony_cxd_integ_t * pInteg, uint8_t * pSSI)
{
    int32_t rfLevel;
    sony_cxd_dvbt_tpsinfo_t tps;
    int32_t prel;
    int32_t tempSSI = 0;
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    static const int32_t pRefdBm1000[3][5] = {
    /*    1/2,    2/3,    3/4,    5/6,    7/8,               */
        {-93000, -91000, -90000, -89000, -88000}, /* QPSK    */
        {-87000, -85000, -84000, -83000, -82000}, /* 16-QAM  */
        {-82000, -80000, -78000, -77000, -76000}, /* 64-QAM  */
    };

    sony_cxd_TRACE_ENTER ("sony_cxd_integ_dvbt_monitor_SSI");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSSI)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->system != sony_cxd_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level */
    result = sony_cxd_integ_dvbt_monitor_RFLevel (pInteg, &rfLevel);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Monitor TPS for Modulation / Code Rate */
    result = sony_cxd_demod_dvbt_monitor_TPSInfo (pInteg->pDemod, &tps);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Ensure correct TPS values. */
    if ((tps.constellation >= sony_cxd_DVBT_CONSTELLATION_RESERVED_3) || (tps.rateHP >= sony_cxd_DVBT_CODERATE_RESERVED_5)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_OTHER);
    }

    /* prel = prec - pref */
    prel = rfLevel - pRefdBm1000[tps.constellation][tps.rateHP];

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)             SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = (4 * prel) + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    sony_cxd_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd_result_t dvbt_WaitDemodLock (sony_cxd_integ_t * pInteg)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_demod_lock_result_t lock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_cxd_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    sony_cxd_TRACE_ENTER ("dvbt_WaitDemodLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Wait for demod lock */
    result = sony_cxd_stopwatch_start (&timer);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    for (;;) {
        result = sony_cxd_stopwatch_elapsed(&timer, &elapsed);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        if (elapsed >= sony_cxd_DVBT_WAIT_DEMOD_LOCK) {
            continueWait = 0;
        }

        result = sony_cxd_demod_dvbt_CheckDemodLock (pInteg->pDemod, &lock);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        /* Check Echo Optomization */
        result = sony_cxd_demod_dvbt_EchoOptimization(pInteg->pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        switch (lock) {
        case sony_cxd_DEMOD_LOCK_RESULT_LOCKED:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);

        case sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd_integ_CheckCancellation (pInteg);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        if (continueWait) {
            result = sony_cxd_stopwatch_sleep (&timer, sony_cxd_DVBT_WAIT_LOCK_INTERVAL);
            if (result != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (result);
            }
        } else {
            result = sony_cxd_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

    sony_cxd_TRACE_RETURN (result);
}
