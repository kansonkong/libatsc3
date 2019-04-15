/*
 * atsc3_logging_externs.c
 *
 *  Created on: Mar 7, 2019
 *      Author: jjustman
 */
#include <stdio.h>
#include <stdbool.h>

#include "atsc3_logging_externs.h"
FILE* __DEBUG_LOG_FILE;
bool  __DEBUG_LOG_AVAILABLE = true;

#ifndef _TEST_RUN_VALGRIND_OSX_

//overload printf to write to stderr
int printf(const char *format, ...)  {

  if(__DEBUG_LOG_AVAILABLE && !__DEBUG_LOG_FILE) {
    __DEBUG_LOG_FILE = fopen("debug.log", "w");
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
