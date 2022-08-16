/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_monitor.h"
#include "sony_integ.h"
#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sony_result_t sony_integ_Initialize (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_stopwatch_t timer;
    uint32_t elapsedTime = 0;
    uint8_t cpuTaskCompleted = 0;

    SONY_TRACE_ENTER ("sony_integ_Initialize");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_Initialize1 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Start timer */
    result = sony_stopwatch_start (&timer);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Wait for finishing power on calibration. */
    while (1) {
        result = sony_stopwatch_elapsed (&timer, &elapsedTime);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_CheckInternalCPUStatus (pTunerDemod, &cpuTaskCompleted);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (cpuTaskCompleted) {
            break;
        }

        if (elapsedTime > SONY_TUNERDEMOD_WAIT_INITIALIZE_TIMEOUT) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_TIMEOUT);
        } else {
            result = sony_stopwatch_sleep (&timer, SONY_TUNERDEMOD_WAIT_INITIALIZE_INTERVAL);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    result = sony_tunerdemod_Initialize2 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_integ_Cancel (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("sony_integ_Cancel");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set the cancellation flag. */
    sony_atomic_set (&(pTunerDemod->cancel), 1);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_integ_CheckCancellation (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("sony_integ_CheckCancellation");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Check the cancellation flag. */
    if (sony_atomic_read (&(pTunerDemod->cancel)) != 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_CANCEL);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}
#undef uint32_t 
#undef int32_t 
#undef int8_t 
