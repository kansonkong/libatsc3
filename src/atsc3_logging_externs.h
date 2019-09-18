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

extern int _ATSC3_UTILS_INFO_ENABLED;
extern int _ATSC3_UTILS_DEBUG_ENABLED;
extern int _ATSC3_UTILS_TRACE_ENABLED;

extern int _ALP_PARSER_INFO_ENABLED;
extern int _ALP_PARSER_DEBUG_ENABLED;

extern int _XML_INFO_ENABLED;
extern int _XML_DEBUG_ENABLED;
extern int _XML_TRACE_ENABLED;

extern int _LLS_INFO_ENABLED;
extern int _LLS_DEBUG_ENABLED;
extern int _LLS_TRACE_ENABLED;

extern int _LLS_SLT_PARSER_INFO_ENABLED;
extern int _LLS_SLT_PARSER_INFO_MMT_ENABLED;
extern int _LLS_SLT_PARSER_INFO_ROUTE_ENABLED;

extern int _SLS_METADATA_FRAGMENT_PARSER_INFO_ENABLED;
extern int _SLS_METADATA_FRAGMENT_PARSER_DEBUG_ENABLED;
extern int _SLS_METADATA_FRAGMENT_PARSER_TRACE_ENABLED;

extern int _FDT_PARSER_DEBUG_ENABLED;

extern int _LLS_SLT_PARSER_DEBUG_ENABLED;
extern int _LLS_SLT_PARSER_TRACE_ENABLED;

extern int _LLS_ALC_UTILS_INFO_ENABLED;
extern int _LLS_ALC_UTILS_DEBUG_ENABLED;

extern int _LLSU_TRACE_ENABLED;
extern int _LLSU_MMT_TRACE_ENABLED;

extern int _ROUTE_MBMS_ENVELOPE_PARSER_INFO_ENABLED;
extern int _ROUTE_MBMS_ENVELOPE_PARSER_DEBUG_ENABLED;

extern int _ROUTE_MPD_PARSER_INFO_ENABLED;
extern int _ROUTE_MPD_PARSER_DEBUG_ENABLED;

extern int _ROUTE_S_TSID_PARSER_INFO_ENABLED;
extern int _ROUTE_S_TSID_PARSER_DEBUG_ENABLED;

extern int _ROUTE_USBD_PARSER_INFO_ENABLED;
extern int _ROUTE_USBD_PARSER_DEBUG_ENABLED;

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _MMTP_TRACE_ENABLED;

extern int _MMT_MPU_PARSER_DEBUG_ENABLED;
extern int _MMT_MPU_PARSER_TRACE_ENABLED;
    
extern int _MMT_MPU_DEBUG_ENABLED;
extern int _MMT_MPU_TRACE_ENABLED;

extern int _MMT_SIGNALLING_MESSAGE_ERROR_23008_1_ENABLED;
extern int _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED;
extern int _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED;

extern int _MMT_RECON_FROM_SAMPLE_SIGNAL_INFO_ENABLED;

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;
extern int _ALC_UTILS_IOTRACE_ENABLED;

extern int _ALC_RX_DEBUG_ENABLED;
extern int _ALC_RX_TRACE_ENABLED;


extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

extern int _STLTP_PARSER_DEBUG_ENABLED;

extern int _MIME_PARSER_INFO_ENABLED;
extern int _MIME_PARSER_DEBUG_ENABLED;
extern int _MIME_PARSER_TRACE_ENABLED;
    
extern int _IP_UDP_RTP_PARSER_DEBUG_ENABLED;
extern int _IP_UDP_RTP_PARSER_TRACE_ENABLED;

extern int _STLTP_TYPES_DEBUG_ENABLED;
extern int _STLTP_TYPES_TRACE_ENABLED;
extern int _STLTP_PARSER_TRACE_ENABLED;

extern int _DSTP_TYPES_DEBUG_ENABLED;
extern int _DSTP_TYPES_TRACE_ENABLED;

//c++ linkage
//extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;

//jjustman-2019-07-24 - normaolized debug logging format
    
#define __LIBATSC3_TIMESTAMP_ERROR(...)     printf("%-24.24s:%4d:ERROR:%.4f:",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_WARN(...)      printf("%-24.24s:%4d:WARN :%.4f:",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_INFO(...)      printf("%-24.24s:%4d:INFO :%.4f:",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_DEBUG(...)     printf("%-24.24s:%4d:DEBUG:%.4f:",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_TRACE(...)     printf("%-24.24s:%4d:TRACE:%.4f:",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");

#define __ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define __DEBUGF(...)  printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __DEBUGA(...) 	__PRINTF(__VA_ARGS__);
#define __DEBUGN(...)  __PRINTLN(__VA_ARGS__);
#else
#define __DEBUG(...)
#define __DEBUGF(...)
#define __DEBUGA(...)
#define __DEBUGN(...)
#endif

#ifdef _ENABLE_TRACE
#define __TRACE(...)   __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);

#else
#define __TRACE(...)
#endif

#ifdef __cplusplus
}
#endif


#endif /* ATSC3_LOGGING_EXTERNS_H_ */
