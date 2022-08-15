/*------------------------------------------------------------------------------
  Copyright 2015 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbtmm.h"
#include "sony_integ_isdbtmm.h"
#include "sony_integ_isdbt.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_isdbtmm_Tune (sony_tunerdemod_t * pTunerDemod,
                                       sony_isdbtmm_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbtmm_Tune");

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

    switch (pTuneParam->superSegmentType) {
    case SONY_ISDBTMM_SUPER_SEGMENT_A:
        {
            sony_isdbtmm_A_tune_param_t tuneParamA;
            
            result = sony_tunerdemod_isdbtmm_A_ConvertTuneParam (pTunerDemod, &tuneParamA, pTuneParam);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            result = sony_integ_isdbtmm_A_Tune (pTunerDemod, &tuneParamA);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
        break;

    case SONY_ISDBTMM_SUPER_SEGMENT_B:
        {
            sony_isdbtmm_B_tune_param_t tuneParamB;
            
            result = sony_tunerdemod_isdbtmm_B_ConvertTuneParam (pTunerDemod, &tuneParamB, pTuneParam);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            result = sony_integ_isdbtmm_B_Tune (pTunerDemod, &tuneParamB);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_integ_isdbtmm_A_Tune (sony_tunerdemod_t * pTunerDemod,
                                         sony_isdbtmm_A_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbtmm_A_Tune");

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

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_isdbtmm_A_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_isdbtmm_A_Tune2 (pTunerDemod, pTuneParam);
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

sony_result_t sony_integ_isdbtmm_B_Tune (sony_tunerdemod_t * pTunerDemod,
                                         sony_isdbtmm_B_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_integ_isdbtmm_B_Tune");

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

    /* Tune the tuner/demodulator (1st step) */
    result = sony_tunerdemod_isdbtmm_B_Tune1 (pTunerDemod, pTuneParam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (SONY_TUNERDEMOD_WAIT_AGC_STABLE);

    /* Tune the tuner/demodulator (2nd step) */
    result = sony_tunerdemod_isdbtmm_B_Tune2 (pTunerDemod, pTuneParam);
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
