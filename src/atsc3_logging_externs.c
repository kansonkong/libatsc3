/*
 * atsc3_logging_externs.c
 *
 *  Created on: Mar 7, 2019
 *      Author: jjustman
 */
#include <stdio.h>
#include <stdbool.h>

#include "atsc3_logging_externs.h"

static FILE* __DEBUG_LOG_FILE = NULL;
static bool  __DEBUG_LOG_AVAILABLE = true;


#ifdef __LIBATSC3_ANDROID__
#include <android/log.h>

char  __ANDROID_LOG_VPRINTF_BUFFER[__ANDROID_MAX_LOG_LINE_LENGTH__] = { 0 };

//overload printf to write to android logs
int printf(const char *format, ...)  {
    int ret = 0;
    va_list argptr;
    va_start(argptr, format);
    ret = __android_log_vprint(ANDROID_LOG_DEBUG, "NDK", format, argptr);
    va_end(argptr);

    return ret;
}

//jjustman-2020-09-09 - SLSDK sl_utils SL_Printf uses vprintf(), so override for android
int vprintf(const char* __fp, va_list __args) {
    int ret = 0;
    ret = __android_log_vprint(ANDROID_LOG_DEBUG, "NDK", __fp, __args);
    return ret;
}

#else
#ifndef _TEST_RUN_VALGRIND_OSX_

#ifndef __DISABLE_NCURSES__

#ifndef _WIN32

#define __DEBUG_LOG_FILE_NAME__ "debug.log"

//overload printf to write to stderr
int printf(const char *format, ...)  {

  if(__DEBUG_LOG_AVAILABLE && !__DEBUG_LOG_FILE) {
	  char launch_timestamp_string[32] = {0};
	  double launch_timestamp = gt();
	  snprintf((char*)&launch_timestamp_string, 31, "%s.%.4f", __DEBUG_LOG_FILE_NAME__, launch_timestamp);
	  fprintf(stderr, "Debug logfile at: %s\n", launch_timestamp_string);
    __DEBUG_LOG_FILE = fopen(launch_timestamp_string, "w");
    if(!__DEBUG_LOG_FILE) {
      __DEBUG_LOG_AVAILABLE = false;
      __DEBUG_LOG_FILE = stderr;
    }
  }

    va_list argptr;
	va_start(argptr, format);
	vfprintf(__DEBUG_LOG_FILE, format, argptr);
    va_end(argptr);
    fflush(__DEBUG_LOG_FILE);
	return 0;
}
#endif

#endif
#endif
#endif
