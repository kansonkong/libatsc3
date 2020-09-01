/*
 * atsc3_logging_externs.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#include <stdio.h>      /* printf */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "atsc3_utils.h"

#ifndef ATSC3_LOGGING_EXTERNS_H_
#define ATSC3_LOGGING_EXTERNS_H_

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef __LIBATSC3_ANDROID__
//#include <android/log.h>
//#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "TAG", __VA_ARGS__);
//#endif

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

extern int _ATSC3_UTILS_INFO_ENABLED;
extern int _ATSC3_UTILS_DEBUG_ENABLED;
extern int _ATSC3_UTILS_TRACE_ENABLED;

extern int _ATSC3_UDP_INFO_ENABLED;
extern int _ATSC3_UDP_DEBUG_ENABLED;
extern int _ATSC3_UDP_TRACE_ENABLED;

extern int _ALP_PARSER_INFO_ENABLED;
extern int _ALP_PARSER_DEBUG_ENABLED;

extern int _XML_INFO_ENABLED;
extern int _XML_DEBUG_ENABLED;
extern int _XML_TRACE_ENABLED;

extern int _LLS_INFO_ENABLED;
extern int _LLS_DEBUG_ENABLED;
extern int _LLS_TRACE_ENABLED;

extern int _LLS_TYPES_INFO_ENABLED;
extern int _LLS_TYPES_DEBUG_ENABLED;
extern int _LLS_TYPES_TRACE_ENABLED;

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
extern int _LLS_ALC_UTILS_TRACE_ENABLED;

extern int _LLS_MMT_UTILS_INFO_ENABLED;
extern int _LLS_MMT_UTILS_DEBUG_ENABLED;
extern int _LLS_MMT_UTILS_TRACE_ENABLED;

extern int _ROUTE_SLS_PROCESSOR_INFO_ENABLED;
extern int _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED;
extern int _ROUTE_SLS_PROCESSOR_TRACE_ENABLED;

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

extern int _MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED;
extern int _MMT_RECON_FROM_SAMPLE_TRACE_ENABLED;

extern int _MMT_RECON_FROM_SAMPLE_SIGNAL_INFO_ENABLED;

//mmt context callback logging
extern int _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED;
extern int _MMT_CONTEXT_MPU_DEBUG_ENABLED;
extern int _MMT_CONTEXT_MPU_TRACE_ENABLED;

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;
extern int _ALC_UTILS_IOTRACE_ENABLED;

extern int _ALC_RX_DEBUG_ENABLED;
extern int _ALC_RX_TRACE_ENABLED;
extern int _ALC_RX_TRACE_TAB_ENABLED;

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

extern int _STLTP_PARSER_INFO_ENABLED;
extern int _STLTP_PARSER_DUMP_ENABLED;
extern int _STLTP_PARSER_DEBUG_ENABLED;
extern int _STLTP_PARSER_TRACE_ENABLED;

extern int _MIME_PARSER_INFO_ENABLED;
extern int _MIME_PARSER_DEBUG_ENABLED;
extern int _MIME_PARSER_TRACE_ENABLED;
    
extern int _IP_UDP_RTP_PARSER_INFO_ENABLED;
extern int _IP_UDP_RTP_PARSER_DEBUG_ENABLED;
extern int _IP_UDP_RTP_PARSER_TRACE_ENABLED;

extern int _STLTP_TYPES_DEBUG_ENABLED;
extern int _STLTP_TYPES_TRACE_ENABLED;

extern int _DSTP_TYPES_DEBUG_ENABLED;
extern int _DSTP_TYPES_TRACE_ENABLED;

extern int _AEAT_TYPES_INFO_ENABLED;
extern int _AEAT_TYPES_DEBUG_ENABLED;
extern int _AEAT_TYPES_TRACE_ENABLED;

extern int _A344_RECEIVER_QUERY_INFO_ENABLED;
extern int _A344_RECEIVER_QUERY_DEBUG_ENABLED;
extern int _A344_RECEIVER_QUERY_TRACE_ENABLED;

extern int _AEAT_PARSER_INFO_ENABLED;
extern int _AEAT_PARSER_DEBUG_ENABLED;
extern int _AEAT_PARSER_TRACE_ENABLED;

extern int _ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED;
extern int _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED;
extern int _ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED;

extern int _ATSC3_PCAP_TYPE_DEBUG_ENABLED;
extern int _ATSC3_PCAP_TYPE_TRACE_ENABLED;

extern int _SL_TLV_DEMOD_DEBUG_ENABLED;
extern int _SL_TLV_DEMOD_TRACE_ENABLED;

extern int _PCRE2_REGEX_UTILS_INFO_ENABLED;
extern int _PCRE2_REGEX_UTILS_DEBUG_ENABLED;
extern int _PCRE2_REGEX_UTILS_TRACE_ENABLED;

extern int _ROUTE_PACKAGE_UTILS_DEBUG_ENABLED;
extern int _ROUTE_PACKAGE_UTILS_TRACE_ENABLED;

extern int _ROUTE_DASH_UTILS_INFO_ENABLED;
extern int _ROUTE_DASH_UTILS_DEBUG_ENABLED;
extern int _ROUTE_DASH_UTILS_TRACE_ENABLED;

extern int _ROUTE_OBJECT_INFO_ENABLED;
extern int _ROUTE_OBJECT_DEBUG_ENABLED;
extern int _ROUTE_OBJECT_TRACE_ENABLED;

extern int _SLS_ALC_FLOW_INFO_ENABLED;
extern int _SLS_ALC_FLOW_DEBUG_ENABLED;
extern int _SLS_ALC_FLOW_TRACE_ENABLED;

extern int _HELD_PARSER_INFO_ENABLED;
extern int _HELD_PARSER_DEBUG_ENABLED;
extern int _HELD_PARSER_TRACE_ENABLED;

extern int _ATSC3_STLTP_DEPACKETIZER_INFO_ENABLED;
extern int _ATSC3_STLTP_DEPACKETIZER_DEBUG_ENABLED;
extern int _ATSC3_STLTP_DEPACKETIZER_TRACE_ENABLED;

extern int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_INFO_ENABLED;
extern int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_DEBUG_ENABLED;
extern int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_TRACE_ENABLED;

extern int _ATSC3_ALP_TYPES_INFO_ENABLED;
extern int _ATSC3_ALP_TYPES_DUMP_ENABLED;
extern int _ATSC3_ALP_TYPES_DEBUG_ENABLED;
extern int _ATSC3_ALP_TYPES_TRACE_ENABLED;



//c++ linkage
//extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;

//jjustman-2019-07-24 - normaolized debug logging format

#ifdef __ANDROID__
#define __ANDROID_MAX_LOG_LINE_LENGTH__ 1025

extern char  __ANDROID_LOG_VPRINTF_BUFFER[];
//     vsnprintf(char * restrict str, size_t size, const char * restrict format, va_list ap);
//va_list argptr; va_start(argptr, format);
//__VA_OPT__(,) __VA_ARGS__
#define __LIBATSC3_TIMESTAMP_ERROR(format, ...)         {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:ERROR:%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_WARN(format, ...)          {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:WARN :%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_INFO(format, ...)          {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:INFO :%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_DUMP(format, ...)          {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:DUMP :%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_DEBUG(format, ...)         {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:DEBUG:%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_TAB_DEBUG(format, ...)     {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:DEBUG:%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_TRACE(format, ...)         {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:TRACE:%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }
#define __LIBATSC3_TIMESTAMP_TRACE_TAB(format, ...)     {snprintf(__ANDROID_LOG_VPRINTF_BUFFER,__ANDROID_MAX_LOG_LINE_LENGTH__-1, format, ##__VA_ARGS__ );	printf("%-32.32s:%4d:TTRAC:%.4f:%s",__FILENAME__,__LINE__,  gt(), __ANDROID_LOG_VPRINTF_BUFFER); }

#define __ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#else

#define __LIBATSC3_TIMESTAMP_ERROR(...)     	printf("%-24.24s:%4d:ERROR:%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_WARN(...)     		printf("%-24.24s:%4d:WARN :%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_INFO(...)      	printf("%-24.24s:%4d:INFO :%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_DUMP(...)      	printf("%-24.24s:%4d:DUMP :%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_DEBUG(...)    		printf("%-24.24s:%4d:DEBUG:%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_TRACE(...)    		printf("%-24.24s:%4d:TRACE:%.4f:",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");
#define __LIBATSC3_TIMESTAMP_TRACE_TAB(...)     printf("%-24.24s\t%4d\tTRACE\t%.4f\t",__FILENAME__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n");

#define __ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif

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
