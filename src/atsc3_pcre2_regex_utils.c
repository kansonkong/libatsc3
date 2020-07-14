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

//vector types impl
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_preg2_regex_match_capture_group, atsc3_preg2_regex_match_capture);
//impl to clear block_t reference: ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_preg2_regex_match_capture);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_pcre2_regex_match_capture_vector, atsc3_preg2_regex_match_capture_group);
//impl to clear children ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_preg2_regex_match_capture_group);

/*
 *
 	 jjustman-2020-07-13 - todo: support replacing via $1 ... $n reference capture group(s) directly with pcre2_substitute and pcre2_set_substitute_callout
		Substitution callouts

		int pcre2_set_substitute_callout(pcre2_match_context *mcontext,
		int (*callout_function)(pcre2_substitute_callout_block *, void *),
		void *callout_data);
*/

atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context_new(const char* pcre2_regex_pattern) {
	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = calloc(1, sizeof(atsc3_pcre2_regex_context_t));
	atsc3_pcre2_regex_context->find_all = 1; //set /g by default
	atsc3_pcre2_regex_context->pattern = (PCRE2_SPTR) pcre2_regex_pattern;

	//compile our regex pattern

	atsc3_pcre2_regex_context->re = pcre2_compile(atsc3_pcre2_regex_context->pattern,
													PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
													PCRE2_MULTILINE | PCRE2_DOTALL,                     /* default options */
													&atsc3_pcre2_regex_context->errornumber,          /* for error number */
													&atsc3_pcre2_regex_context->erroroffset,          /* for error offset */
													NULL);                 /* use default compile context */

	if (atsc3_pcre2_regex_context->re== NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(atsc3_pcre2_regex_context->errornumber, buffer, sizeof(buffer));
		__PCRE2_REGEX_UTILS_ERROR("PCRE2 compilation failed at offset %d: %s", (int)atsc3_pcre2_regex_context->erroroffset, buffer);
		goto error;
	}

	//setup the match_data block for right size of capture groups
	atsc3_pcre2_regex_context->match_data = pcre2_match_data_create_from_pattern(atsc3_pcre2_regex_context->re, NULL);

	return atsc3_pcre2_regex_context;

error:

	if(atsc3_pcre2_regex_context) {
		atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);
	}

	if(atsc3_pcre2_regex_context->re) {
		pcre2_code_free(atsc3_pcre2_regex_context->re);
		atsc3_pcre2_regex_context->re = NULL;
	}

	return NULL;
}

atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match(atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context, block_t* subject_block) {
	if(!atsc3_pcre2_regex_context || !atsc3_pcre2_regex_context->match_data) {
		__PCRE2_REGEX_UTILS_ERROR("atsc3_pcre2_regex_context (or match_data) is null!");
		return NULL;
	}

	if(!subject_block->p_size) {
		__PCRE2_REGEX_UTILS_ERROR("subject_block->p_size is 0!");
		return NULL;
	}

	int capture_group_id = 0;
	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match_capture_vector_new(subject_block);

	//placeholder handles
	atsc3_preg2_regex_match_capture_group_t* atsc3_preg2_regex_match_capture_group = NULL;
	atsc3_preg2_regex_match_capture_t* atsc3_preg2_regex_match_capture = NULL;

	int rc;

	PCRE2_SPTR subject = NULL;  /* the appropriate width (in this case, 8 bits). */
	PCRE2_SIZE subject_length;

	subject = (PCRE2_SPTR) subject_block->p_buffer;
	subject_length = (PCRE2_SIZE)strlen((char *)subject); //jjustman-2020-07-13 - todo, change to subject_block->p_size?

	rc = pcre2_match(atsc3_pcre2_regex_context->re, subject, subject_length,
					  0,                  					  /* start at offset 0 in the subject */
					  0,                 					  /* default options */
					  atsc3_pcre2_regex_context->match_data,  /* block for storing the result */
					  NULL);               					  /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0) {
	  switch(rc) {
	    case PCRE2_ERROR_NOMATCH:
	    	__PCRE2_REGEX_UTILS_TRACE("no match for pattern: %s, with data: %s", atsc3_pcre2_regex_context->pattern, subject_block->p_buffer);
	    	break;

			/*
			Handle other special cases if you like
			*/
	    default:
	    		__PCRE2_REGEX_UTILS_INFO("Matching error %d", rc);
	    	break;
	 }


	  goto error;

	}

	/* Match succeded. Get a pointer to the output vector, where string offsets are	stored. */

	atsc3_pcre2_regex_context->ovector = pcre2_get_ovector_pointer(atsc3_pcre2_regex_context->match_data);
	__PCRE2_REGEX_UTILS_TRACE("Match succeeded at offset %d ", (int)atsc3_pcre2_regex_context->ovector[0]);


	/*************************************************************************
	* We have found the first match within the subject string. If the output *
	* vector wasn't big enough, say so. Then output any substrings that were *
	* captured.                                                              *
	*************************************************************************/

	/* The output vector wasn't big enough. This should not happen, because we used	pcre2_match_data_create_from_pattern() above. */

	if (rc == 0) {
		__PCRE2_REGEX_UTILS_ERROR("ovector was not big enough for all the captured substrings");
		//jjustman-2020-07-13 todo? goto error;
		goto error;
	}
	/* We must guard against patterns such as /(?=.\K)/ that use \K in an assertion
	to set the start of a match later than its end. In this demonstration program,
	we just detect this case and give up. */

	if (atsc3_pcre2_regex_context->ovector[0] > atsc3_pcre2_regex_context->ovector[1]) {
		__PCRE2_REGEX_UTILS_ERROR("\\K was used in an assertion to set the match start after its end."
					"From end to start the match was: %.*s", (int)(atsc3_pcre2_regex_context->ovector[0] - atsc3_pcre2_regex_context->ovector[1]),
						(char *)(subject + atsc3_pcre2_regex_context->ovector[1]));
		goto error;
	}

	/* Show substrings stored in the output vector by number. Obviously, in a real
	application you might want to do things other than print them. */

	//jjustman-2020-07-14 - TODO: fix copy/paste below for Nth capture group

	atsc3_preg2_regex_match_capture_group = atsc3_preg2_regex_match_capture_group_new();
	atsc3_preg2_regex_match_capture_group->capture_group_id = capture_group_id++;
	atsc3_pcre2_regex_match_capture_vector_add_atsc3_preg2_regex_match_capture_group(atsc3_pcre2_regex_match_capture_vector, atsc3_preg2_regex_match_capture_group);

	__PCRE2_REGEX_UTILS_TRACE("first match capture group dump, rc: %d", rc);

	for (int i = 0; i < rc; i++) {
		atsc3_preg2_regex_match_capture = atsc3_preg2_regex_match_capture_new();
		atsc3_preg2_regex_match_capture->capture_reference_id = i;
		atsc3_preg2_regex_match_capture->match_start = atsc3_pcre2_regex_context->ovector[2*i];
		atsc3_preg2_regex_match_capture->match_end = atsc3_pcre2_regex_context->ovector[2*i+1];

		PCRE2_SPTR substring_start = subject + atsc3_pcre2_regex_context->ovector[2*i];
		PCRE2_SIZE substring_length = atsc3_pcre2_regex_context->ovector[2*i+1] - atsc3_pcre2_regex_context->ovector[2*i];

		//use our local subject block_t ref
		atsc3_preg2_regex_match_capture->pcre_substring_start = block_Get(atsc3_pcre2_regex_match_capture_vector->subject_block) + atsc3_pcre2_regex_context->ovector[2*i];
		atsc3_preg2_regex_match_capture->pcre_substring_length = substring_length;

		//todo - jjustman-2020-07-14 - refactor this out to common pattern for promoting pcre non-null terminated substrings to block_t null terminated strings
		atsc3_preg2_regex_match_capture->substring = block_Alloc(substring_length + 1);
		block_Write(atsc3_preg2_regex_match_capture->substring, atsc3_preg2_regex_match_capture->pcre_substring_start, substring_length);
		block_Rewind(atsc3_preg2_regex_match_capture->substring);


		atsc3_preg2_regex_match_capture_group_add_atsc3_preg2_regex_match_capture(atsc3_preg2_regex_match_capture_group, atsc3_preg2_regex_match_capture);

		__PCRE2_REGEX_UTILS_TRACE("%2d (s: %d, e: %d): %.*s", i,
													  atsc3_pcre2_regex_context->ovector[2*i],
													  atsc3_pcre2_regex_context->ovector[2*i+1],
													  (int)substring_length, (char *)substring_start);
	}

	(void)pcre2_pattern_info(atsc3_pcre2_regex_context->re, PCRE2_INFO_NAMECOUNT, &atsc3_pcre2_regex_context->namecount);          /* where to put the answer */

	if (atsc3_pcre2_regex_context->namecount == 0) {
		__PCRE2_REGEX_UTILS_TRACE("atsc3_pcre2_regex_context: %p - No named substrings", atsc3_pcre2_regex_context);
	}
		//jjustman-2020-07-13: TODO IMPL
//   else {
//
//	  PCRE2_SPTR tabptr;
//	  printf("Named substrings");
//
//	  /* Before we can access the substrings, we must extract the table for
//	  translating names to numbers, and the size of each entry in the table. */
//
//	  (void)pcre2_pattern_info(
//	    re,                       /* the compiled pattern */
//	    PCRE2_INFO_NAMETABLE,     /* address of the table */
//	    &name_table);             /* where to put the answer */
//
//	  (void)pcre2_pattern_info(
//	    re,                       /* the compiled pattern */
//	    PCRE2_INFO_NAMEENTRYSIZE, /* size of each entry in the table */
//	    &name_entry_size);        /* where to put the answer */
//
//	  /* Now we can scan the table and, for each entry, print the number, the name,
//	  and the substring itself. In the 8-bit library the number is held in two
//	  bytes, most significant first. */
//
//	  tabptr = name_table;
//	  for (i = 0; i < namecount; i++)
//	    {
//	    int n = (tabptr[0] << 8) | tabptr[1];
//	    printf("(%d) %*s: %.*s", n, name_entry_size - 3, tabptr + 2,
//	      (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
//	    tabptr += name_entry_size;
//	    }
//	  }

	// /g is set by default, otherwise return just this single match capture group
	if (!atsc3_pcre2_regex_context->find_all) {
	  pcre2_match_data_free(atsc3_pcre2_regex_context->match_data);  /* Release the memory that was used */
	  pcre2_code_free(atsc3_pcre2_regex_context->re);                /* for the match data and the pattern. */

	  return atsc3_pcre2_regex_match_capture_vector;
	}

	/* Before running the loop, check for UTF-8 and whether CRLF is a valid newline
	sequence. First, find the options with which the regex was compiled and extract
	the UTF state. */

	(void)pcre2_pattern_info(atsc3_pcre2_regex_context->re, PCRE2_INFO_ALLOPTIONS, &atsc3_pcre2_regex_context->option_bits);
	atsc3_pcre2_regex_context->utf8 = (atsc3_pcre2_regex_context->option_bits & PCRE2_UTF) != 0;

	/* Now find the newline convention and see whether CRLF is a valid newline	sequence. */

	(void)pcre2_pattern_info(atsc3_pcre2_regex_context->re, PCRE2_INFO_NEWLINE, &atsc3_pcre2_regex_context->newline);
	atsc3_pcre2_regex_context->crlf_is_newline = 	atsc3_pcre2_regex_context->newline == PCRE2_NEWLINE_ANY ||
													atsc3_pcre2_regex_context->newline == PCRE2_NEWLINE_CRLF ||
													atsc3_pcre2_regex_context->newline == PCRE2_NEWLINE_ANYCRLF;

	/* Loop for second and subsequent matches */

	for (;;) {
		uint32_t options = 0;                   /* Normally no options */
		PCRE2_SIZE start_offset = atsc3_pcre2_regex_context->ovector[1];   /* Start at end of previous match */

		/* If the previous match was for an empty string, we are finished if we are
		at the end of the subject. Otherwise, arrange to run another match at the
		same point to see if a non-empty match can be found. */

		if (atsc3_pcre2_regex_context->ovector[0] == atsc3_pcre2_regex_context->ovector[1]) {
		  if (atsc3_pcre2_regex_context->ovector[0] == subject_length) {
			  break;
		  }

		  options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
		} else {
		  /* If the previous match was not an empty string, there is one tricky case to
			  consider. If a pattern contains \K within a lookbehind assertion at the
			  start, the end of the matched string can be at the offset where the match
			  started. Without special action, this leads to a loop that keeps on matching
			  the same substring. We must detect this case and arrange to move the start on
			  by one character. The pcre2_get_startchar() function returns the starting
			  offset that was passed to pcre2_match(). */

		  PCRE2_SIZE startchar = pcre2_get_startchar(atsc3_pcre2_regex_context->match_data);
		  if (start_offset <= startchar) {
			  if (startchar >= subject_length) {
				  break;   /* Reached end of subject.   */
			  }

			  start_offset = startchar + 1;             /* Advance by one character. */
			  if (atsc3_pcre2_regex_context->utf8) {	/*  if UTF-8, it may be more than one code unit.     */
				  for (; start_offset < subject_length; start_offset++) {
					  if ((subject[start_offset] & 0xc0) != 0x80) {
						  break;
					  }
				  }
			  }
		  }
		}

		/* Run the next matching operation */

		rc = pcre2_match(atsc3_pcre2_regex_context->re, subject, subject_length,       /* the length of the subject */
						start_offset,       			    	/* starting offset in the subject */
						options,            					/* options */
						atsc3_pcre2_regex_context->match_data,  /* block for storing the result */
						NULL);               					/* use default match context */

		/* This time, a result of NOMATCH isn't an error. If the value in "options"
		is zero, it just means we have found all possible matches, so the loop ends.
		Otherwise, it means we have failed to find a non-empty-string match at a
		point where there was a previous empty-string match. In this case, we do what
		Perl does: advance the matching position by one character, and continue. We
		do this by setting the "end of previous match" offset, because that is picked
		up at the top of the loop as the point at which to start again.

		There are two complications: (a) When CRLF is a valid newline sequence, and
		the current position is just before it, advance by an extra byte. (b)
		Otherwise we must ensure that we skip an entire UTF character if we are in
		UTF mode. */

		if (rc == PCRE2_ERROR_NOMATCH) {
		  if (options == 0) {
			  break;                    /* All matches found */
		  }
		  atsc3_pcre2_regex_context->ovector[1] = start_offset + 1;              /* Advance one code unit */
		  if (atsc3_pcre2_regex_context->crlf_is_newline &&                      /* If CRLF is a newline & */
					start_offset < subject_length - 1 &&    /* we are at CRLF, */
					subject[start_offset] == '\r' &&
					subject[start_offset + 1] == '\n') {
			  /* Advance by one more. */
			  atsc3_pcre2_regex_context->ovector[1] += 1;
		  } else if (atsc3_pcre2_regex_context->utf8) {        						 /*  Otherwise, ensure we advance a whole UTF-8 */
			  while (atsc3_pcre2_regex_context->ovector[1] < subject_length) {      	 /* character. */
				  if ((subject[atsc3_pcre2_regex_context->ovector[1]] & 0xc0) != 0x80) {
					  break;
				  }
				  atsc3_pcre2_regex_context->ovector[1] += 1;
			  }
		  }
		  continue;    /* Go round the loop again */
		}

		/* Other matching errors are not recoverable. */

		if (rc < 0) {
			__PCRE2_REGEX_UTILS_ERROR("Matching error %d", rc);
			goto error;
		}

		/* Match succeded */

		__PCRE2_REGEX_UTILS_TRACE("atsc3_pcre2_regex_context: %p, Match succeeded again at offset: %d", atsc3_pcre2_regex_context, (int)atsc3_pcre2_regex_context->ovector[0]);

		/* The match succeeded, but the output vector wasn't big enough. This
		should not happen. */

		if (rc == 0) {
			__PCRE2_REGEX_UTILS_ERROR("atsc3_pcre2_regex_context: %p, ovector was not big enough for all the captured substrings, rc: %d", atsc3_pcre2_regex_context, rc);
		}

		/* We must guard against patterns such as /(?=.\K)/ that use \K in an
		assertion to set the start of a match later than its end. In this
		demonstration program, we just detect this case and give up. */

		if (atsc3_pcre2_regex_context->ovector[0] > atsc3_pcre2_regex_context->ovector[1]) {
			__PCRE2_REGEX_UTILS_ERROR("\\K was used in an assertion to set the match start after its end."
				  "From end to start the match was: %.*s", (int)(atsc3_pcre2_regex_context->ovector[0] - atsc3_pcre2_regex_context->ovector[1]),
				  (char *)(subject + atsc3_pcre2_regex_context->ovector[1]));
			goto error;
		}

		/* As before, show substrings stored in the output vector by number, and then
		also any named substrings. */

		//jjustman-2020-07-14 - TODO: fix copy/paste below for Nth capture group
		atsc3_preg2_regex_match_capture_group = atsc3_preg2_regex_match_capture_group_new();
		atsc3_preg2_regex_match_capture_group->capture_group_id = capture_group_id++;
		atsc3_pcre2_regex_match_capture_vector_add_atsc3_preg2_regex_match_capture_group(atsc3_pcre2_regex_match_capture_vector, atsc3_preg2_regex_match_capture_group);

		for (int i = 0; i < rc; i++) {
			atsc3_preg2_regex_match_capture = atsc3_preg2_regex_match_capture_new();
			atsc3_preg2_regex_match_capture->capture_reference_id = i;
			atsc3_preg2_regex_match_capture->match_start = atsc3_pcre2_regex_context->ovector[2*i];
			atsc3_preg2_regex_match_capture->match_end = atsc3_pcre2_regex_context->ovector[2*i+1];

			PCRE2_SPTR substring_start = subject + atsc3_pcre2_regex_context->ovector[2*i];
			size_t substring_length = atsc3_pcre2_regex_context->ovector[2*i+1] - atsc3_pcre2_regex_context->ovector[2*i];

			//use our local subject block_t ref
			atsc3_preg2_regex_match_capture->pcre_substring_start = block_Get(atsc3_pcre2_regex_match_capture_vector->subject_block) + atsc3_pcre2_regex_context->ovector[2*i];
			atsc3_preg2_regex_match_capture->pcre_substring_length = substring_length;

			//todo - jjustman-2020-07-14 - refactor this out to common pattern for promoting pcre non-null terminated substrings to block_t null terminated strings
			atsc3_preg2_regex_match_capture->substring = block_Alloc(substring_length + 1);
			block_Write(atsc3_preg2_regex_match_capture->substring, atsc3_preg2_regex_match_capture->pcre_substring_start, substring_length);
			block_Rewind(atsc3_preg2_regex_match_capture->substring);

			atsc3_preg2_regex_match_capture_group_add_atsc3_preg2_regex_match_capture(atsc3_preg2_regex_match_capture_group, atsc3_preg2_regex_match_capture);

			__PCRE2_REGEX_UTILS_TRACE("%2d (s: %d, e: %d): %.*s", i,
															  atsc3_pcre2_regex_context->ovector[2*i],
															  atsc3_pcre2_regex_context->ovector[2*i+1],
															  (int)substring_length, (char *)substring_start);
		}

		if (atsc3_pcre2_regex_context->namecount == 0) {
		  __PCRE2_REGEX_UTILS_TRACE("No named substrings");
		} else {
			PCRE2_SPTR tabptr = atsc3_pcre2_regex_context->name_table;
			__PCRE2_REGEX_UTILS_TRACE("Named substrings");
			for (int i = 0; i < atsc3_pcre2_regex_context->namecount; i++) {
				int n = (tabptr[0] << 8) | tabptr[1];
				__PCRE2_REGEX_UTILS_TRACE("(%d) %*s: %.*s", n, atsc3_pcre2_regex_context->name_entry_size - 3, tabptr + 2,
				(int)(atsc3_pcre2_regex_context->ovector[2*n+1] - atsc3_pcre2_regex_context->ovector[2*n]), subject + atsc3_pcre2_regex_context->ovector[2*n]);
				tabptr += atsc3_pcre2_regex_context->name_entry_size;
			}
		}
	}
	/* End of loop to find second and subsequent matches */

complete:

	__PCRE2_REGEX_UTILS_DEBUG("returning match capture vector: %p, capture groups: %d", atsc3_pcre2_regex_match_capture_vector, atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count);

	//atsc3_pcre2_regex_match_capture_vector
	return atsc3_pcre2_regex_match_capture_vector;

error:

	if(atsc3_pcre2_regex_context->match_data) {
		pcre2_match_data_free(atsc3_pcre2_regex_context->match_data);   /* Release memory used for the match */
		atsc3_pcre2_regex_context->match_data = NULL;

	}

	if(atsc3_pcre2_regex_context->re) {
		pcre2_code_free(atsc3_pcre2_regex_context->re);                 /*   data and the compiled pattern. */
		atsc3_pcre2_regex_context->re = NULL;
	}

	if(atsc3_pcre2_regex_match_capture_vector) {
		atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);
	}

	return NULL;
}

//jjustman-2020-07-14 - todo: macro _new and _free for ATSC3_ALLOC() with chained VECTOR_BUILDER destructors..

atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector_new(block_t* subject_block_to_copy) {
	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_captures = calloc(1, sizeof(atsc3_pcre2_regex_match_capture_vector_t));
	atsc3_pcre2_regex_match_captures->subject_block = block_Duplicate(subject_block_to_copy);
	block_Rewind(atsc3_pcre2_regex_match_captures->subject_block);
	return atsc3_pcre2_regex_match_captures;
}


void atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector) {
	__PCRE2_REGEX_UTILS_DEBUG("atsc3_pcre2_regex_match_capture_vector: %p", atsc3_pcre2_regex_match_capture_vector);
	__PCRE2_REGEX_UTILS_DEBUG(" subject_block (size: %d):\n%s", atsc3_pcre2_regex_match_capture_vector->subject_block->p_size, atsc3_pcre2_regex_match_capture_vector->subject_block->p_buffer);
	__PCRE2_REGEX_UTILS_DEBUG(" ---");
	__PCRE2_REGEX_UTILS_DEBUG(" capture group count: %d",  atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count);
	__PCRE2_REGEX_UTILS_DEBUG(" ---");
	for(int i = 0; i < atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count; i++) {
		atsc3_preg2_regex_match_capture_group_t* atsc3_preg2_regex_match_capture_group = atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.data[i];

		__PCRE2_REGEX_UTILS_DEBUG(" capture group: %d, contains %d references", atsc3_preg2_regex_match_capture_group->capture_group_id,
																				atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.count);

		for(int j = 0; j < atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.count; j++) {
			atsc3_preg2_regex_match_capture_t* atsc3_preg2_regex_match_capture = atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.data[j];

			__PCRE2_REGEX_UTILS_DEBUG("   ref: %d (start: %d, end: %d, length: %d), value:\n%s",  atsc3_preg2_regex_match_capture->capture_reference_id,
																									atsc3_preg2_regex_match_capture->match_start,
																									atsc3_preg2_regex_match_capture->match_end,
																									atsc3_preg2_regex_match_capture->substring->p_size,
																									block_Get(atsc3_preg2_regex_match_capture->substring));

		}
	}
}

void atsc3_preg2_regex_match_capture_free(atsc3_preg2_regex_match_capture_t** atsc3_preg2_regex_match_capture_p) {
	if(atsc3_preg2_regex_match_capture_p) {
		atsc3_preg2_regex_match_capture_t* atsc3_preg2_regex_match_capture = *atsc3_preg2_regex_match_capture_p;
		if(atsc3_preg2_regex_match_capture) {
			if(atsc3_preg2_regex_match_capture->substring) {
				block_Destroy(&atsc3_preg2_regex_match_capture->substring);
			}

			free(atsc3_preg2_regex_match_capture);
			atsc3_preg2_regex_match_capture = NULL;
		}
		*atsc3_preg2_regex_match_capture_p = NULL;
	}
}

void atsc3_preg2_regex_match_capture_group_free(atsc3_preg2_regex_match_capture_group_t** atsc3_preg2_regex_match_capture_group_p) {
	if(atsc3_preg2_regex_match_capture_group_p) {
		atsc3_preg2_regex_match_capture_group_t* atsc3_preg2_regex_match_capture_group = *atsc3_preg2_regex_match_capture_group_p;
		if(atsc3_preg2_regex_match_capture_group) {
			atsc3_preg2_regex_match_capture_group_free_atsc3_preg2_regex_match_capture(atsc3_preg2_regex_match_capture_group);

			free(atsc3_preg2_regex_match_capture_group);
			atsc3_preg2_regex_match_capture_group = NULL;
		}
		*atsc3_preg2_regex_match_capture_group_p = NULL;
	}
}

void atsc3_pcre2_regex_match_capture_vector_free(atsc3_pcre2_regex_match_capture_vector_t** atsc3_pcre2_regex_match_captures_p) {
	if(atsc3_pcre2_regex_match_captures_p) {
		atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_captures = *atsc3_pcre2_regex_match_captures_p;
		if(atsc3_pcre2_regex_match_captures) {
			if(atsc3_pcre2_regex_match_captures->subject_block) {
				block_Destroy(&atsc3_pcre2_regex_match_captures->subject_block);
			}
			atsc3_pcre2_regex_match_capture_vector_free_atsc3_preg2_regex_match_capture_group(atsc3_pcre2_regex_match_captures);

			free(atsc3_pcre2_regex_match_captures);
			atsc3_pcre2_regex_match_captures = NULL;
		}
		*atsc3_pcre2_regex_match_captures_p = NULL;
	}
}

void atsc3_pcre2_regex_context_free(atsc3_pcre2_regex_context_t** atsc3_pcre2_regex_context_p) {
	if(atsc3_pcre2_regex_context_p) {
		atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = *atsc3_pcre2_regex_context_p;
		if(atsc3_pcre2_regex_context) {
			//TODO: jjustman-2020-07-13 - clean up other members here

			//boilerplate
			if(atsc3_pcre2_regex_context->match_data) {
				pcre2_match_data_free(atsc3_pcre2_regex_context->match_data);   /* Release memory used for the match */
				atsc3_pcre2_regex_context->match_data = NULL;
			}

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
