/*
 * atsc3_logging_externs.c
 *
 *  Created on: Mar 7, 2019
 *      Author: jjustman
 */

#include "atsc3_logging_externs.h"


#ifndef _TEST_RUN_VALGRIND_OSX_

//overload printf to write to stderr
int printf(const char *format, ...)  {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
    va_end(argptr);

	return 0;
}

#endif
