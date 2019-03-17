/*
 * atsc3_logging_externs.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#include <stdio.h>      /* printf */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#ifndef ATSC3_LOGGING_EXTERNS_H_
#define ATSC3_LOGGING_EXTERNS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int _XML_INFO_ENABLED;
extern int _XML_DEBUG_ENABLED;
extern int _XML_TRACE_ENABLED;

extern int _LLS_INFO_ENABLED;
extern int _LLS_DEBUG_ENABLED;
extern int _LLS_TRACE_ENABLED;

extern int _LLS_SLT_PARSER_INFO_ENABLED;
extern int _LLS_SLT_PARSER_INFO_MMT_ENABLED;
extern int _LLS_SLT_PARSER_INFO_ROUTE_ENABLED;

extern int _LLS_SLT_PARSER_DEBUG_ENABLED;
extern int _LLS_SLT_PARSER_TRACE_ENABLED;

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _MMTP_TRACE_ENABLED;

extern int _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED;
extern int _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED;

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

extern int _STLTP_PARSER_DEBUG_ENABLED;


//c++ linkage
//extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;


#define __ERROR(...)   printf("%s:%d:ERROR :","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __WARN(...)    printf("%s:%d:WARN: ","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __INFO(...)    printf("%s:%d: ","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __DEBUGF(...)  printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
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

#ifdef __cplusplus
}
#endif


#endif /* ATSC3_LOGGING_EXTERNS_H_ */
