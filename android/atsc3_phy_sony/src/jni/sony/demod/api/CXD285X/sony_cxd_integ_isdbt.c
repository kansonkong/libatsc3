/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/

#include "sony_cxd_integ.h"
#include "sony_cxd_integ_isdbt.h"
#include "sony_cxd_demod_isdbt_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_integ_isdbt_Tune(sony_cxd_integ_t * pInteg,
	sony_cxd_isdbt_tune_param_t * pTuneParam)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_Tune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pDemod->scanMode) {
        /* Clear cancellation flag. */
		sony_cxd_atomic_set(&(pInteg->cancel), 0);
    }

    /* Check bandwidth validity */
	if ((pTuneParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) &&
		(pTuneParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) &&
		(pTuneParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Tune the demodulator */
	result = sony_cxd_demod_isdbt_Tune(pInteg->pDemod, pTuneParam);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Tune)) {
        /* Enable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x01);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        /* Tune the RF part */
		result = pInteg->pTunerTerrCable->Tune(pInteg->pTunerTerrCable, pTuneParam->centerFreqKHz, sony_cxd_DTV_SYSTEM_ISDBT, pTuneParam->bandwidth);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        /* Disable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x00);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }

    /* Reset the demod to enable acquisition */
	result = sony_cxd_demod_TuneEnd(pInteg->pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if (pInteg->pDemod->scanMode) {
        /* Wait for Demod lock */
		result = sony_cxd_integ_isdbt_WaitDemodLock(pInteg);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }
    else {
        /* Wait for TS lock */
		result = sony_cxd_integ_isdbt_WaitTSLock(pInteg);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_integ_isdbt_EWSTune(sony_cxd_integ_t * pInteg,
	sony_cxd_isdbt_tune_param_t * pTuneParam)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_EWSTune");

    if ((!pInteg) || (!pTuneParam) || (!pInteg->pDemod)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
	sony_cxd_atomic_set(&(pInteg->cancel), 0);

    /* Check bandwidth validity */
	if ((pTuneParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) &&
		(pTuneParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) &&
		(pTuneParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Tune the demodulator */
	result = sony_cxd_demod_isdbt_EWSTune(pInteg->pDemod, pTuneParam);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if ((pInteg->pTunerTerrCable) && (pInteg->pTunerTerrCable->Tune)) {
        /* Enable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x01);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        /* Tune the RF part */
		result = pInteg->pTunerTerrCable->Tune(pInteg->pTunerTerrCable, pTuneParam->centerFreqKHz, sony_cxd_DTV_SYSTEM_ISDBT, pTuneParam->bandwidth);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        /* Disable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x00);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }

    /* Reset the demod to enable acquisition */
	result = sony_cxd_demod_isdbt_EWSTuneEnd(pInteg->pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Wait for Demod lock */
	result = sony_cxd_integ_isdbt_WaitDemodLock(pInteg);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_integ_isdbt_Scan(sony_cxd_integ_t * pInteg,
	sony_cxd_integ_isdbt_scan_param_t * pScanParam,
	sony_cxd_integ_isdbt_scan_callback_t callBack)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_integ_isdbt_scan_result_t scanResult;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_Scan");

    if ((!pInteg) || (!pScanParam) || (!callBack) || (!pInteg->pDemod)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pInteg->pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancellation flag. */
	sony_cxd_atomic_set(&(pInteg->cancel), 0);

    /* Ensure the scan parameters are valid. */
    if (pScanParam->endFrequencyKHz < pScanParam->startFrequencyKHz) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    if (pScanParam->stepFrequencyKHz == 0) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Check bandwidth validity */
	if ((pScanParam->bandwidth != sony_cxd_DTV_BW_6_MHZ) && (pScanParam->bandwidth != sony_cxd_DTV_BW_7_MHZ) &&
		(pScanParam->bandwidth != sony_cxd_DTV_BW_8_MHZ)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* Set the start frequency */
    scanResult.centerFreqKHz = pScanParam->startFrequencyKHz;
    scanResult.tuneParam.bandwidth = pScanParam->bandwidth;

    /* Set scan mode enabled */
	result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_ISDBT, 0x01);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Scan routine */
    while (scanResult.centerFreqKHz <= pScanParam->endFrequencyKHz) {
        scanResult.tuneParam.centerFreqKHz = scanResult.centerFreqKHz;
		scanResult.tuneResult = sony_cxd_integ_isdbt_Tune(pInteg, &scanResult.tuneParam);
        switch (scanResult.tuneResult) {
		case sony_cxd_RESULT_OK:
            /* Channel found, callback to application */
            scanResult.centerFreqKHz = scanResult.tuneParam.centerFreqKHz;
            callBack (pInteg, &scanResult, pScanParam);
            break;
		case sony_cxd_RESULT_ERROR_UNLOCK:
		case sony_cxd_RESULT_ERROR_TIMEOUT:
            /* Channel not found, callback to applicaton for progress updates */
            callBack (pInteg, &scanResult, pScanParam);
            break;
        default:
            /* Serious error occurred -> cancel operation. */
			sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_ISDBT, 0x00);
			sony_cxd_TRACE_RETURN(scanResult.tuneResult);
        }

        scanResult.centerFreqKHz += pScanParam->stepFrequencyKHz;

        /* Check cancellation. */
		result = sony_cxd_integ_CheckCancellation(pInteg);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_ISDBT, 0x00);
			sony_cxd_TRACE_RETURN(result);
        }
    }

    /* Clear scan mode */
	result = sony_cxd_demod_terr_cable_SetScanMode(pInteg->pDemod, sony_cxd_DTV_SYSTEM_ISDBT, 0x00);

	sony_cxd_TRACE_RETURN(result);
}


sony_cxd_result_t sony_cxd_integ_isdbt_WaitTSLock(sony_cxd_integ_t * pInteg)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_demod_lock_result_t lock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;
	sony_cxd_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_WaitTSLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Wait for TS lock */
	result = sony_cxd_stopwatch_start(&timer);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    for (;;) {
		result = sony_cxd_stopwatch_elapsed(&timer, &elapsed);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

		if (elapsed >= sony_cxd_ISDBT_WAIT_TS_LOCK) {
            continueWait = 0;
        }

		result = sony_cxd_demod_isdbt_CheckTSLock(pInteg->pDemod, &lock);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        switch (lock) {
		case sony_cxd_DEMOD_LOCK_RESULT_LOCKED:
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);

		case sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED:
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
		result = sony_cxd_integ_CheckCancellation(pInteg);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        if (continueWait) {
			result = sony_cxd_stopwatch_sleep(&timer, sony_cxd_ISDBT_WAIT_LOCK_INTERVAL);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        } else {
			result = sony_cxd_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_integ_isdbt_WaitDemodLock(sony_cxd_integ_t * pInteg)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_demod_lock_result_t lock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;
    sony_cxd_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_WaitDemodLock");

    if ((!pInteg) || (!pInteg->pDemod)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Wait for demod lock */
	result = sony_cxd_stopwatch_start(&timer);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    for (;;) {
		result = sony_cxd_stopwatch_elapsed(&timer, &elapsed);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

		if (elapsed >= sony_cxd_ISDBT_WAIT_DEMOD_LOCK) {
            continueWait = 0;
        }

		result = sony_cxd_demod_isdbt_CheckDemodLock(pInteg->pDemod, &lock);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        switch (lock) {
		case sony_cxd_DEMOD_LOCK_RESULT_LOCKED:
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);

		case sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED:
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_UNLOCK);

        default:
            /* continue waiting... */
            break;
        }

        /* Check cancellation. */
		result = sony_cxd_integ_CheckCancellation(pInteg);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        if (continueWait) {
			result = sony_cxd_stopwatch_sleep(&timer, sony_cxd_ISDBT_WAIT_LOCK_INTERVAL);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        } else {
			result = sony_cxd_RESULT_ERROR_TIMEOUT;
            break;
        }
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_integ_isdbt_monitor_RFLevel(sony_cxd_integ_t * pInteg, int32_t * pRFLeveldB)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;

	sony_cxd_TRACE_ENTER("sony_cxd_integ_isdbt_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pRFLeveldB)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pInteg->pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pInteg->pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT)  {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (!pInteg->pTunerTerrCable) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    if (pInteg->pTunerTerrCable->ReadRFLevel) {
        /* Enable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x01);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        result = pInteg->pTunerTerrCable->ReadRFLevel (pInteg->pTunerTerrCable, pRFLeveldB);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        /* Disable the I2C repeater */
		result = sony_cxd_demod_I2cRepeaterEnable(pInteg->pDemod, 0x00);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    } else if (pInteg->pTunerTerrCable->CalcRFLevelFromAGC) {
        uint32_t ifAgc;

		result = sony_cxd_demod_isdbt_monitor_IFAGCOut(pInteg->pDemod, &ifAgc);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

        result = pInteg->pTunerTerrCable->CalcRFLevelFromAGC (pInteg->pTunerTerrCable, ifAgc, pRFLeveldB);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    } else {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

	sony_cxd_TRACE_RETURN(result);
}


