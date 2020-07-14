/*
 * atsc3_pcre2_regex_utils.c
 *
 *  Created on: Jul 6, 2020
 *      Author: jjustman
 */

#include "atsc3_pcre2_regex_utils.h"

int _PCRE2_REGEX_UTILS_INFO_ENABLED = 1;
int _PCRE2_REGEX_UTILS_DEBUG_ENABLED = 0;
int _PCRE2_REGEX_UTILS_TRACE_ENABLED = 0;

//jjustman-2020-07-13 - todo: support replacing via $1 ... $n reference capture group(s)

atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context_new(const char* pcre2_regex_pattern) {
	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = calloc(1, sizeof(atsc3_pcre2_regex_context_t));
	atsc3_pcre2_regex_context->pattern = (PCRE2_SPTR) pcre2_regex_pattern;

	//compile our regex pattern

	atsc3_pcre2_regex_context->re = pcre2_compile(atsc3_pcre2_regex_context->pattern,
													PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
													PCRE2_MULTILINE | PCRE2_DOTALL,                     /* default options */
													&atsc3_pcre2_regex_context->errornumber,          /* for error number */
													&atsc3_pcre2_regex_context->erroroffset,          /* for error offset */
													NULL);                 /* use default compile context */

	if (atsc3_pcre2_regex_context->re == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(atsc3_pcre2_regex_context->errornumber, buffer, sizeof(buffer));
		__PCRE2_REGEX_UTILS_ERROR("PCRE2 compilation failed at offset %d: %s\n", (int)atsc3_pcre2_regex_context->erroroffset, buffer);
		goto error;
	}

	//setup the match_data block for right size of capture groups
	atsc3_pcre2_regex_context->match_data = pcre2_match_data_create_from_pattern(atsc3_pcre2_regex_context->re, NULL);


	return atsc3_pcre2_regex_context;

error:

	if(atsc3_pcre2_regex_context) {
		atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);
	}

	return NULL;
}


void atsc3_pcre2_regex_context_free(atsc3_pcre2_regex_context_t** atsc3_pcre2_regex_context_p) {
	if(atsc3_pcre2_regex_context_p) {
		atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = *atsc3_pcre2_regex_context_p;
		if(atsc3_pcre2_regex_context) {
			//TODO: jjustman-2020-07-13 - clean up other members here

			free(atsc3_pcre2_regex_context);
			atsc3_pcre2_regex_context = NULL;
		}
		*atsc3_pcre2_regex_context_p = NULL;
	}
}
/*
 *
 *
	subject = (PCRE2_SPTR) block_mpd->p_buffer;
	subject_length = (PCRE2_SIZE)strlen((char *)subject);
 */
