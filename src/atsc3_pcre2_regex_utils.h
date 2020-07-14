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

typedef struct atsc3_pcre2_regex_context {

	pcre2_code *re;
	PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
	PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */
	PCRE2_SPTR name_table;

	int crlf_is_newline;
	int errornumber;
	int find_all;
	int i;
	int rc;
	int utf8;

	uint32_t option_bits;
	uint32_t namecount;
	uint32_t name_entry_size;
	uint32_t newline;

	PCRE2_SIZE erroroffset;
	PCRE2_SIZE *ovector;
	PCRE2_SIZE subject_length;

	pcre2_match_data *match_data;

} atsc3_pcre2_regex_context_t;

atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context_new(const char* pcre2_regex_pattern);

void atsc3_pcre2_regex_context_free(atsc3_pcre2_regex_context_t** atsc3_pcre2_regex_context_p);

#define __PCRE2_REGEX_UTILS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __PCRE2_REGEX_UTILS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __PCRE2_REGEX_UTILS_INFO(...)  if(_PCRE2_REGEX_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __PCRE2_REGEX_UTILS_DEBUG(...) if(_PCRE2_REGEX_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __PCRE2_REGEX_UTILS_TRACE(...) if(_PCRE2_REGEX_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_PCRE2_REGEX_UTILS_H_ */
