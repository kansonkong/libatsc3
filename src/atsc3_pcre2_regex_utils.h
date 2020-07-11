/*
 * atsc3_pcre2_regex_utils.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 */

#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pcre2.h>

#ifndef ATSC3_PCRE2_REGEX_UTILS_H_
#define ATSC3_PCRE2_REGEX_UTILS_H_

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"




#define __PCRE2_REGEX_UTILS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __PCRE2_REGEX_UTILS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __PCRE2_REGEX_UTILS_INFO(...)  if(_PCRE2_REGEX_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __PCRE2_REGEX_UTILS_DEBUG(...) if(_PCRE2_REGEX_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __PCRE2_REGEX_UTILS_TRACE(...) if(_PCRE2_REGEX_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_PCRE2_REGEX_UTILS_H_ */
