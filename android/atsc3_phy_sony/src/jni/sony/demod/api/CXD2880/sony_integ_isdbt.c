/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2016/03/01
  Modification ID : 821957233ed7087a9a4711da5ab28f0c008ce6a5
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbt.h"
#include "sony_integ_isdbt.h"
#include "sony_common.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_isdbt_Tune(sony_tunerdemod_t * pTunerDemod,
                                    sony_isdbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbt_Tune");

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

    if (!pTunerDemod->scanMode) {
        /* Clear cancellation flag. */
        sony_atomic_set (&(pTunerDemod->cancel), 0);
    }

    /* Check bandwidth validity */
    if ((pTuneParam->bandwidth != SONY_DTV_BW_6_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_7_MHZ) &&
        (pTuneParam->bandwidth != SONY_DTV_BW_8_MHZ)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_isdbt_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_isdbt_Tune2 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->scanMode) {
        /* Wait for Demod lock */
        result = sony_integ_isdbt_WaitDemodLock (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else {
        /* Wait for Demod lock OR TS lock */
        result = sony_integ_isdbt_WaitDemodOrTSLock (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_integ_isdbt_Scan(sony_tunerdemod_t * pTunerDemod,
                                    sony_integ_isdbt_scan_param_t * pScanParam,
                                    sony_integ_isdbt_scan_callback_t callBack)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_integ_isdbt_scan_result_t scanResult;

    SONY_TRACE_ENTER ("sony_integ_isdbt_Scan");

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
    scanResult.tuneParam.bandwidth = pScanParam->bandwidth;

    /* Enable one-segment optimization setting for scanning */
    scanResult.tuneParam.oneSegmentOptimize = 1;

    /* Set scan mode enabled */
    result = sony_tunerdemod_SetScanMode(pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneParam.centerFreqKHz = scanResult.centerFreqKHz;
        scanResult.tuneResult = sony_integ_isdbt_Tune(pTunerDemod, &scanResult.tuneParam);
        switch (scanResult.tuneResult) {
        case SONY_RESULT_OK:
            /* Channel found, callback to application */
            callBack (pTunerDemod, &scanResult, pScanParam);
            break;
        case SONY_RESULT_ERROR_UNLOCK:
        case SONY_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to applicaton for progress updates */
            callBack (pTunerDemod, &scanResult, pScanParam);
            break;
        default:
            /* Serious error occurred -> cancel operation. */
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


sony_result_t sony_integ_isdbt_WaitTSLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_isdbt_WaitTSLock");

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

        if (elapsed >= SONY_ISDBT_WAIT_TS_LOCK) {
            continueWait = 0;
        }

        result = sony_tunerdemod_isdbt_CheckTSLock (pTunerDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_ISDBT_WAIT_LOCK_INTERVAL);
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

sony_result_t sony_integ_isdbt_WaitDemodLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_isdbt_WaitDemodLock");

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

        if (elapsed >= SONY_ISDBT_WAIT_DEMOD_LOCK) {
            continueWait = 0;
        }

        result = sony_tunerdemod_isdbt_CheckDemodLock (pTunerDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_ISDBT_WAIT_LOCK_INTERVAL);
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

sony_result_t sony_integ_isdbt_WaitDemodOrTSLock (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_tunerdemod_lock_result_t lock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
    sony_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

    SONY_TRACE_ENTER ("sony_integ_isdbt_WaitDemodOrTSLock");

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

        if (elapsed >= SONY_ISDBT_WAIT_DEMOD_LOCK) {
            continueWait = 0;
        }

        result = sony_tunerdemod_isdbt_CheckDemodOrTSLock (pTunerDemod, &lock);
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
            result = sony_stopwatch_sleep (&timer, SONY_ISDBT_WAIT_LOCK_INTERVAL);
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
