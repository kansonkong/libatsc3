/*
 * atsc3_route_dash_utils.h
 *
 *  Created on: Jul 13, 2020
 *      Author: jjustman
 */



#ifndef ATSC3_ROUTE_DASH_UTILS_H
#define ATSC3_ROUTE_DASH_UTILS_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_pcre2_regex_utils.h"

#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_PATTERN "(<Representation.*?id=\"(.*?)\".*?>.*?<SegmentTemplate.*?startNumber=\"(.*?)\".*?<\\/Representation>)"
#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_FLAGS "msg"


//atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector






#define __ROUTE_DASH_UTILS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ROUTE_DASH_UTILS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ROUTE_DASH_UTILS_INFO(...)  if(_ROUTE_DASH_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __ROUTE_DASH_UTILS_DEBUG(...) if(_ROUTE_DASH_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ROUTE_DASH_UTILS_TRACE(...) if(_ROUTE_DASH_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_PCRE2_REGEX_UTILS_H_ */
