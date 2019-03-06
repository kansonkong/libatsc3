/*
 * atsc3_logging_externs.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_LOGGING_EXTERNS_H_
#define ATSC3_LOGGING_EXTERNS_H_

extern int _LLS_DEBUG_ENABLED;
extern int _LLS_TRACE_ENABLED;

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _MMTP_TRACE_ENABLED;

extern int _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED;
extern int _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED;

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;


#define __ERROR(...)   printf("%s:%d:ERROR :","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __WARN(...)    printf("%s:%d:WARN: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __INFO(...)    printf("%s:%d: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __DEBUGF(...)  printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __DEBUGA(...) 	__PRINTF(__VA_ARGS__);
#define __DEBUGN(...)  __PRINTLN(__VA_ARGS__);
#else
#define __DEBUGF(...)
#define __DEBUGA(...)
#define __DEBUGN(...)
#endif

#ifdef _ENABLE_TRACE
#define __TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

#else
#define __TRACE(...)
#endif


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





#endif /* ATSC3_LOGGING_EXTERNS_H_ */
