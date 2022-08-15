/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/01/16
  Modification ID : d4b3fe129855d90c25271de02796915983f1e6ee
------------------------------------------------------------------------------*/
#include "sony_cxd_common.h"
#include <linux/time.h>
#include <linux/module.h>
#include "sony_common.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char


#ifndef _WINDOWS
sony_result_t sony_stopwatch_start (sony_stopwatch_t * pStopwatch)
{
//#error sony_stopwatch_start is not implemented
	struct timeval begin;
	
	sony_cxd_TRACE_ENTER("sony_cxd_stopwatch_start");

    if (!pStopwatch) {
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
	
	do_gettimeofday(&begin);
	pStopwatch->startTime = begin.tv_sec*1000 + begin.tv_usec/1000;
	
	sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_result_t sony_stopwatch_sleep (sony_stopwatch_t * pStopwatch, uint32_t ms)
{
//#error sony_stopwatch_sleep is not implemented
	sony_cxd_TRACE_ENTER("sony_cxd_stopwatch_sleep");
    if (!pStopwatch) {
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
    sony_cxd_ARG_UNUSED(*pStopwatch);
    sony_cxd_SLEEP (ms);

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_result_t sony_stopwatch_elapsed (sony_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
//#error sony_stopwatch_elapsed is not implemented
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

sony_result_t sony_stopwatch_start (sony_stopwatch_t * pStopwatch)
{
    SONY_TRACE_ENTER("sony_stopwatch_start");

    if (!pStopwatch) {
		SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = timeGetTime ();

	SONY_TRACE_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_stopwatch_sleep (sony_stopwatch_t * pStopwatch, uint32_t ms)
{
	SONY_TRACE_ENTER("sony_stopwatch_sleep");
    if (!pStopwatch) {
		SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
	SONY_ARG_UNUSED(*pStopwatch);
	SONY_SLEEP(ms);

	SONY_TRACE_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_stopwatch_elapsed (sony_stopwatch_t * pStopwatch, uint32_t* pElapsed)
{
	SONY_TRACE_ENTER("sony_stopwatch_elapsed");

    if (!pStopwatch || !pElapsed) {
		SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    *pElapsed = timeGetTime () - pStopwatch->startTime;

	SONY_TRACE_RETURN(SONY_RESULT_OK);
}
#endif

#undef uint32_t 
#undef int32_t 
#undef int8_t 
