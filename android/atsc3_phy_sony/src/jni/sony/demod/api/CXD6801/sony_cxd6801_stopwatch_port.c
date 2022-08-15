/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/

#include "sony_cxd6801_common.h"
#include "sony_cxd_common.h"
#include <linux/time.h>
#include <linux/module.h>
#include "sony_common.h"

//#define _WINDOWS

#ifndef _WINDOWS
sony_cxd6801_result_t sony_cxd6801_stopwatch_start (sony_cxd6801_stopwatch_t * pStopwatch)
{
//#error sony_cxd6801_stopwatch_start is not implemented
	struct timeval begin;
	
	sony_cxd_TRACE_ENTER("sony_cxd_stopwatch_start");

    if (!pStopwatch) {
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
	
	do_gettimeofday(&begin);
	pStopwatch->startTime = begin.tv_sec*1000 + begin.tv_usec/1000;
	
	sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_stopwatch_sleep (sony_cxd6801_stopwatch_t * pStopwatch, uint32_t ms)
{
//#error sony_cxd6801_stopwatch_sleep is not implemented
	sony_cxd_TRACE_ENTER("sony_cxd_stopwatch_sleep");
    if (!pStopwatch) {
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
    sony_cxd_ARG_UNUSED(*pStopwatch);
    sony_cxd_SLEEP (ms);

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_stopwatch_elapsed (sony_cxd6801_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
//#error sony_cxd6801_stopwatch_elapsed is not implemented
 //   return 0;
	struct timeval now;
	
	sony_cxd_TRACE_ENTER("sony_cxd_stopwatch_elapsed");

    if (!pStopwatch || !pElapsed) {
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
    
    do_gettimeofday(&now);
    *pElapsed = (now.tv_sec*1000 + now.tv_usec/1000)- pStopwatch->startTime;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}
#else

#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

sony_cxd6801_result_t sony_cxd6801_stopwatch_start (sony_cxd6801_stopwatch_t * pStopwatch)
{
    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_stopwatch_start");

    if (!pStopwatch) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = timeGetTime ();

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_stopwatch_sleep (sony_cxd6801_stopwatch_t * pStopwatch, uint32_t ms)
{
	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_stopwatch_sleep");
    if (!pStopwatch) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
	SONY_CXD6801_ARG_UNUSED(*pStopwatch);
	SONY_CXD6801_SLEEP(ms);

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_stopwatch_elapsed (sony_cxd6801_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_stopwatch_elapsed");

    if (!pStopwatch || !pElapsed) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }
    *pElapsed = timeGetTime () - pStopwatch->startTime;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
#endif
