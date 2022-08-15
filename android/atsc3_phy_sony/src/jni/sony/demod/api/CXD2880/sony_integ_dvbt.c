/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_dvbt.h"
#include "sony_integ_dvbt.h"
#include "sony_common.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Waits for demodulator lock, polling ::sony_tunerdemod_dvbt_monitor_SyncStat
        at 10ms intervals.  Called as part of the Tune process.
*/
static sony_result_t dvbt_WaitDemodLock (sony_tunerdemod_t * pTunerDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_dvbt_Tune(sony_tunerdemod_t * pTunerDemod,
                                   sony_dvbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_dvbt_Tune");

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
    if ((pTuneParam->bandwidth != SONY_DTV_BW_5_MHZ) && (pTuneParam->bandwidth != SONY_DTV_BW_6_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_7_MHZ) && (pTuneParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_dvbt_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_dvbt_Tune2 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for demodulator lock */
    result = dvbt_WaitDemodLock (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt_Scan(sony_tunerdemod_t * pTunerDemod,
                                   sony_integ_dvbt_scan_param_t * pScanParam,
                                   sony_integ_dvbt_scan_callback_t callBack)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_integ_dvbt_scan_result_t scanResult;

    SONY_TRACE_ENTER ("sony_integ_dvbt_Scan");

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
    if ((pScanParam->bandwidth != SONY_DTV_BW_5_MHZ) && (pScanParam->bandwidth != SONY_DTV_BW_6_MHZ) &&
        (pScanParam->bandwidth != SONY_DTV_BW_7_MHZ) && (pScanParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneResult = sony_integ_dvbt_BlindTune(pTunerDemod, scanResult.centerFreqKHz, pScanParam->bandwidth);
        switch (scanResult.tuneResult) {
        /* Channel found, callback to application */
        case SONY_RESULT_OK:
            scanResult.dvbtTuneParam.centerFreqKHz = scanResult.centerFreqKHz;
            scanResult.dvbtTuneParam.bandwidth = pScanParam->bandwidth;
            scanResult.dvbtTuneParam.profile = SONY_DVBT_PROFILE_HP;
            callBack (pTunerDemod, &scanResult, pScanParam);
            break;

        /* Intentional fall-through. */
        case SONY_RESULT_ERROR_UNLOCK:
        case SONY_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to application for progress updates */
            callBack (pTunerDemod, &scanResult, pScanParam);
            break;

        default:
            /* Serious error occurred -> cancel operation. */
            sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT, 0x00);
            SONY_TRACE_RETURN (scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_integ_CheckCancellation (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT, 0x00);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Clear scan mode */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_DVBT, 0x00);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt_BlindTune(sony_tunerdemod_t * pTunerDemod,
                                        uint32_t centerFreqKHz,
                                        sony_dtv_bandwidth_t bandwidth)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_dvbt_tune_param_t tuneParam;

    SONY_TRACE_ENTER ("sony_integ_dvbt_BlindTune");

    if (!pTunerDemod) {
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
    if ((bandwidth != SONY_DTV_BW_5_MHZ) && (bandwidth != SONY_DTV_BW_6_MHZ) &&
        (bandwidth != SONY_DTV_BW_7_MHZ) && (bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Attempt DVB-T acquisition */
    tuneParam.bandwidth = bandwidth;
    tuneParam.centerFreqKHz = centerFreqKHz;
    /* Set DVB-T profile to HP to allow detection of hierachical and non-hierachical modes */
    tuneParam.profile = SONY_DVBT_PROFILE_HP;

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_dvbt_Tune1 (pTunerDemod, &tuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_dvbt_Tune2 (pTunerDemod, &tuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for demodulator lock */
    result = dvbt_WaitDemodLock (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_dvbt_WaitTSLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_dvbt_WaitTSLock");

    if (!pTunerDemod) {
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

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= SONY_DVBT_WAIT_TS_LOCK) {
            continueWait = 0;
        }

        result = sony_tunerdemod_dvbt_CheckTSLock (pTunerDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_DVBT_WAIT_LOCK_INTERVAL);
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
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t dvbt_WaitDemodLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("dvbt_WaitDemodLock");

    if (!pTunerDemod) {
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

    for (;;) {
        result = sony_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (elapsed >= SONY_DVBT_WAIT_DEMOD_LOCK) {
            continueWait = 0;
        }

        result = sony_tunerdemod_dvbt_CheckDemodLock (pTunerDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_DVBT_WAIT_LOCK_INTERVAL);
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
#undef uint32_t 
#undef int32_t 
#undef int8_t 
