/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/08/30
  Modification ID : 902dae58eb5249e028ccc5d7f60bb66cca9b43a9
------------------------------------------------------------------------------*/

#include "sony_cxd6801_integ_atsc.h"
#include "sony_cxd6801_demod_atsc_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Waits for demodulator lock, polling ::sony_demod_atsc_monitor_SyncStat
        at 10ms intervals.  Called as part of the Tune process.
*/
static sony_cxd6801_result_t cxd6801_atsc_WaitVQLock (sony_cxd6801_integ_t * pInteg);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc_Tune (sony_cxd6801_integ_t * pInteg,
                                    sony_cxd6801_atsc_tune_param_t * pTuneParam)
{
    sony_cxd6801_result_t result;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_integ_atsc_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Tune the demodulator */
    result = sony_cxd6801_demod_atsc_Tune (pInteg->pDemod, pTuneParam);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->TerrCableTune)) {
        /* Enable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        /* Tune the RF part */
		result = pInteg->pTuner->TerrCableTune(pInteg->pTuner, pTuneParam->centerFreqKHz, SONY_CXD6801_DTV_SYSTEM_ATSC, SONY_CXD6801_DTV_BW_6_MHZ);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
    }

    result = sony_cxd6801_demod_atsc_TuneEnd(pInteg->pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }

    if (!pInteg->pDemod->scanMode) {
        result = sony_cxd6801_integ_atsc_WaitTSLock(pInteg);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
    } else {
		result = cxd6801_atsc_WaitVQLock(pInteg);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc_Scan (sony_cxd6801_integ_t * pInteg,
                                    sony_cxd6801_integ_atsc_scan_param_t * pScanParam,
                                    sony_cxd6801_integ_atsc_scan_callback_t callBack)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_integ_atsc_scan_result_t scanResult;

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_integ_atsc_Scan");

    if ((!pInteg) || (!pScanParam) || (!callBack) || (!pInteg->pDemod)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Ensure the scan parameters are valid. */
    if (pScanParam->endFrequencyKHz < pScanParam->startFrequencyKHz) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pScanParam->stepFrequencyKHz == 0) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;
	scanResult.tuneResult = SONY_CXD6801_RESULT_OK;
    scanResult.tuneParam.centerFreqKHz = pScanParam->startFrequencyKHz;

    /* Set scan mode enabled */
	result = sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC, 0x01);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneParam.centerFreqKHz = scanResult.centerFreqKHz;
        scanResult.tuneResult = sony_cxd6801_integ_atsc_Tune(pInteg, &scanResult.tuneParam);
        switch (scanResult.tuneResult) {
        /* Channel found, callback to application */
		case SONY_CXD6801_RESULT_OK:
            callBack (pInteg, &scanResult, pScanParam);
            break;

		case SONY_CXD6801_RESULT_ERROR_UNLOCK:
		case SONY_CXD6801_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to application for progress updates */
            callBack (pInteg, &scanResult, pScanParam);
            break;

        default:
            /* Serious error occurred -> cancel operation. */
			sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC, 0x00);
			SONY_CXD6801_TRACE_RETURN(scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
		if (result != SONY_CXD6801_RESULT_OK) {
			sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC, 0x00);
			SONY_CXD6801_TRACE_RETURN(result);
        }
    }

    /* Clear scan mode */
	result = sony_cxd6801_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC, 0x00);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }
	SONY_CXD6801_TRACE_RETURN(result);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc_WaitTSLock (sony_cxd6801_integ_t * pInteg)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
	sony_cxd6801_demod_lock_result_t lock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_cxd6801_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;
    /*uint32_t waitTimeOut = 0;*/

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_integ_atsc_WaitTSLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TS lock */
    result = sony_cxd6801_stopwatch_start (&timer);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }

    for (;;) {
        result = sony_cxd6801_stopwatch_elapsed(&timer, &elapsed);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

		if (elapsed >= SONY_CXD6801_ATSC_WAIT_TS_LOCK) {
            continueWait = 0;
        }

        result = sony_cxd6801_demod_atsc_CheckTSLock (pInteg->pDemod, &lock);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        switch (lock) {
		case SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);

		case SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        if (continueWait) {
			result = sony_cxd6801_stopwatch_sleep(&timer, SONY_CXD6801_ATSC_WAIT_LOCK_INTERVAL);
			if (result != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(result);
            }
        } else {
			result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

	SONY_CXD6801_TRACE_RETURN(result);
}

sony_cxd6801_result_t sony_cxd6801_integ_atsc_monitor_RFLevel (sony_cxd6801_integ_t * pInteg,
                                               int32_t * pRFLeveldB)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_integ_atsc_WaitTSLock");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pInteg->pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC)  {
        /* Not ATSC */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pTuner) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

	if (pInteg->pTuner->rfLevelFuncTerr == SONY_CXD6801_TUNER_RFLEVEL_FUNC_READ && pInteg->pTuner->ReadRFLevel) {
        /* Enable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        result = pInteg->pTuner->ReadRFLevel (pInteg->pTuner, pRFLeveldB);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        /* Disable the I2C repeater */
        result = sony_cxd6801_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
	}
	else if (pInteg->pTuner->rfLevelFuncTerr == SONY_CXD6801_TUNER_RFLEVEL_FUNC_CALCFROMAGC
        && pInteg->pTuner->CalcRFLevelFromAGC) {
        uint32_t ifAgc;

        result = sony_cxd6801_demod_atsc_monitor_IFAGCOut (pInteg->pDemod, &ifAgc);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        result = pInteg->pTuner->CalcRFLevelFromAGC (pInteg->pTuner, ifAgc, pRFLeveldB);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
    } else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    /* dBm * 100 -> dBm * 1000 */
    *pRFLeveldB *= 10;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t cxd6801_atsc_WaitVQLock(sony_cxd6801_integ_t * pInteg)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
	sony_cxd6801_demod_lock_result_t lock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_cxd6801_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;
    uint32_t waitTimeOut = 0;
	SONY_CXD6801_TRACE_ENTER("cxd6801_atsc_WaitVQLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	waitTimeOut = SONY_CXD6801_ATSC_WAIT_DEMOD_LOCK;

    /* Wait for demod lock */
    result = sony_cxd6801_stopwatch_start (&timer);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(result);
    }

    for (;;) {
        result = sony_cxd6801_stopwatch_elapsed(&timer, &elapsed);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        if (elapsed >= waitTimeOut) {
            continueWait = 0;
        }

        result = sony_cxd6801_demod_atsc_CheckDemodLock (pInteg->pDemod, &lock);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        switch (lock) {
		case SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);

		case SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
        result = sony_cxd6801_integ_CheckCancellation (pInteg);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }

        if (continueWait) {
			result = sony_cxd6801_stopwatch_sleep(&timer, SONY_CXD6801_ATSC_WAIT_LOCK_INTERVAL);
			if (result != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        else {
			result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

	SONY_CXD6801_TRACE_RETURN(result);
}
