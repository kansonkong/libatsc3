/*
 * atsc3_route_dash_mpd_patch_test.c
 *
 *  Created on: July 6, 2020
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../atsc3_utils.h"
#include "../atsc3_logging_externs.h"
#include <regex.h>
#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "../atsc3_pcre2_regex_utils.h"
#include "../atsc3_route_dash_utils.h"

#define _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

#define LEAK_CHECK_RUN_COUNT 1000

#define TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS "../../test_data/route-dash/2020-07-02-mpd-patching/route-5004/mpd.mpd"
#define TEST_MPD_ADAPTATION_SET_CONTAINS_BOTH_REPRESENTATION_AND_SEGMENT_TEMPLATE_NODES_NO_CHILDREN_TEGNA_HARMONIC "../../test_data/route-dash/2020-12-23-mpd-patching/tegna/2020-12-23-KING-mpd.xml"
#define TEST_MPD_2021_04_14_AUS_8_MPD8_XML "../../test_data/2021-04-14-aus/8/mpd8.xml"

//#define TEST_MPD_REGEX_PATTERN "(<Representation.*?id=\"(.*?)\".*?>.*?<SegmentTemplate.*?startNumber=\"(.*?)\".*?<\\/Representation>)"

#define TEST_MPD_REGEX_PATTERN ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_PATTERN

#define TEST_MPD_REGEX_PATTERN_FLAGS "msg"




int test_simple_regex_extended_compare() {
	regex_t regex;
	int reti;
	char msgbuf[100];

	//reti = regcomp(&regex, "^a[[:alnum:]]", REG_EXTENDED);
	reti = regcomp(&regex, "^a", REG_EXTENDED);
	if (reti) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_ERROR("could not compile regex\n");
	    return -1;
	}

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("compiled regex is: %p", &regex);

	/* Execute regular expression */
	reti = regexec(&regex, "abc", 0, NULL, 0);
	if (!reti) {
	    puts("Match");
	}
	else if (reti == REG_NOMATCH) {
	    puts("No match");
	}
	else {
	    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	    exit(1);
	}

	/* Free memory allocated to the pattern buffer by regcomp() */
	regfree(&regex);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("--- end of test: test_simple_regex_extended_compare ---")

	return 0;
}

int test_pcre2_simple_match() {

	int rc;

	pcre2_code *re;

	PCRE2_SPTR pattern = (PCRE2_SPTR8)"^(a\\w+)\\s(\\w)*";
	PCRE2_SPTR subject = (PCRE2_SPTR8)"abc zz  asdfasdf";
	PCRE2_SIZE subject_length = strlen((char *)subject);

	int errornumber;
	PCRE2_SIZE erroroffset;
	PCRE2_SIZE *ovector;
	pcre2_match_data *match_data;



	re = pcre2_compile(
	  pattern,               /* the pattern */
	  PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
	  0,                     /* default options */
	  &errornumber,          /* for error number */
	  &erroroffset,          /* for error offset */
	  NULL);                 /* use default compile context */

	/* Compilation failed: print the error message and exit. */

	if (re == NULL) {
	  PCRE2_UCHAR buffer[256];
	  pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
	  _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_ERROR("PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
	  return 1;
	 }


	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	rc = pcre2_match(
	  re,                   /* the compiled pattern */
	  subject,              /* the subject string */
	  subject_length,       /* the length of the subject */
	  0,                    /* start at offset 0 in the subject */
	  0,                    /* default options */
	  match_data,           /* block for storing the result */
	  NULL);                /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0)  {
	  switch(rc)  {
	    case PCRE2_ERROR_NOMATCH:
	    	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_ERROR("No match");
	    	break;
	    /*
	    Handle other special cases if you like
	    */
	    default:
	    	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_ERROR("Matching error %d", rc);
	    	break;
	    }

	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /* data and the compiled pattern. */
	  return -1;
	}

	/* Match succeded. Get a pointer to the output vector, where string offsets are
	stored. */

	ovector = pcre2_get_ovector_pointer(match_data);
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("Match succeeded at offset %d", (int)ovector[0]);


	for (int i = 0; i < rc; i++) {
	  PCRE2_SPTR substring_start = subject + ovector[2*i];
	  PCRE2_SIZE substring_length = ovector[2*i+1] - ovector[2*i];
	  _ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("%2d: %.*s", i, (int)substring_length, (char *)substring_start);
	}


	pcre2_match_data_free(match_data);
	pcre2_code_free(re);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("--- end of test: test_pcre2_simple_match ---")
	return 0;
}


/*
 *
 * (Representation .* id="(.*?)".*)
 *
Match 1
Full match	839-974	Representation bandwidth="5900000" codecs="hev1.2.4.L123.90" frameRate="60000/1001" height="1080" id...
Group 1.	839-974	Representation bandwidth="5900000" codecs="hev1.2.4.L123.90" frameRate="60000/1001" height="1080" id...
Group 2.	941-949	Video1_1

Match 2
Full match	1373-1466	Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
Group 1.	1373-1466	Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
Group 2.	1459-1464	a02_2

Match 3
Full match	2027-2120	Representation audioSamplingRate="48000" bandwidth="32000" codecs="ac-4.02.00.00" id="a13_3">
Group 1.	2027-2120	Representation audioSamplingRate="48000" bandwidth="32000" codecs="ac-4.02.00.00" id="a13_3">
Group 2.	2113-2118	a13_3

Match 4
Full match	2820-2888	Representation bandwidth="140000" codecs="stpp.ttml.im1i" id="d4_4">
Group 1.	2820-2888	Representation bandwidth="140000" codecs="stpp.ttml.im1i" id="d4_4">
Group 2.	2882-2886	d4_4


(<Representation.*?id="(.*?)".*?>.*?<SegmentTemplate.*?startNumber="(.*?)".*?<\/Representation>)  /msg

Match 1
Full match	838-1148	<Representation bandwidth="5900000" codecs="hev1.2.4.L123.90" frameRate="60000/1001" height="1080" i...
Group 1.	838-1148	<Representation bandwidth="5900000" codecs="hev1.2.4.L123.90" frameRate="60000/1001" height="1080" i...
Group 2.	941-949	Video1_1
Group 3.	1097-1098	0
Match 2
Full match	1372-1797	<Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
     ...
Group 1.	1372-1797	<Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
     ...
Group 2.	1459-1464	a02_2
Group 3.	1746-1747	0
Match 3
Full match	2026-2451	<Representation audioSamplingRate="48000" bandwidth="32000" codecs="ac-4.02.00.00" id="a13_3">
     ...
Group 1.	2026-2451	<Representation audioSamplingRate="48000" bandwidth="32000" codecs="ac-4.02.00.00" id="a13_3">
     ...
Group 2.	2113-2118	a13_3
Group 3.	2400-2401	0
Match 4
Full match	2819-3086	<Representation bandwidth="140000" codecs="stpp.ttml.im1i" id="d4_4">
            <SegmentTemplate d...
Group 1.	2819-3086	<Representation bandwidth="140000" codecs="stpp.ttml.im1i" id="d4_4">
            <SegmentTemplate d...
Group 2.	2882-2886	d4_4
Group 3.	3035-3036	0

 *
 */

int test_parse_mpd_with_multiple_audio_adaption_sets() {

	block_t* block_mpd = block_Read_from_filename(TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("Regex pattern: %s", TEST_MPD_REGEX_PATTERN);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("MPD filename: %s", TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("MPD payload:\n%s", block_mpd->p_buffer);


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


	/**************************************************************************
	* First, sort out the command line. There is only one possible option at  *
	* the moment, "-g" to request repeated matching to find all occurrences,  *
	* like Perl's /g option. We set the variable find_all to a non-zero value *
	* if the -g option is present.                                            *
	**************************************************************************/

	find_all = 1;


	/* After the options, we require exactly two arguments, which are the pattern,
	and the subject string. */



	/* Pattern and subject are char arguments, so they can be straightforwardly
	cast to PCRE2_SPTR because we are working in 8-bit code units. The subject
	length is cast to PCRE2_SIZE for completeness, though PCRE2_SIZE is in fact
	defined to be size_t. */
	const char* my_pattern = TEST_MPD_REGEX_PATTERN;

	pattern = (PCRE2_SPTR) my_pattern;
	subject = (PCRE2_SPTR) block_mpd->p_buffer;
	subject_length = (PCRE2_SIZE)strlen((char *)subject);


	/*************************************************************************
	* Now we are going to compile the regular expression pattern, and handle *
	* any errors that are detected.                                          *
	*************************************************************************/

	re = pcre2_compile(
	  pattern,               /* the pattern */
	  PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
	  PCRE2_MULTILINE | PCRE2_DOTALL,                     /* default options */
	  &errornumber,          /* for error number */
	  &erroroffset,          /* for error offset */
	  NULL);                 /* use default compile context */

	/* Compilation failed: print the error message and exit. */

	if (re == NULL)
	  {
	  PCRE2_UCHAR buffer[256];
	  pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
	  printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
	    buffer);
	  return 1;
	  }


	/*************************************************************************
	* If the compilation succeeded, we call PCRE2 again, in order to do a    *
	* pattern match against the subject string. This does just ONE match. If *
	* further matching is needed, it will be done below. Before running the  *
	* match we must set up a match_data block for holding the result. Using  *
	* pcre2_match_data_create_from_pattern() ensures that the block is       *
	* exactly the right size for the number of capturing parentheses in the  *
	* pattern. If you need to know the actual size of a match_data block as  *
	* a number of bytes, you can find it like this:                          *
	*                                                                        *
	* PCRE2_SIZE match_data_size = pcre2_get_match_data_size(match_data);    *
	*************************************************************************/

	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	/* Now run the match. */

	rc = pcre2_match(
	  re,                   /* the compiled pattern */
	  subject,              /* the subject string */
	  subject_length,       /* the length of the subject */
	  0,                    /* start at offset 0 in the subject */
	  0,                    /* default options */
	  match_data,           /* block for storing the result */
	  NULL);                /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0)
	  {
	  switch(rc)
	    {
	    case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
	    /*
	    Handle other special cases if you like
	    */
	    default: printf("Matching error %d\n", rc); break;
	    }
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /*   data and the compiled pattern. */
	  return 1;
	  }

	/* Match succeded. Get a pointer to the output vector, where string offsets are
	stored. */

	ovector = pcre2_get_ovector_pointer(match_data);
	printf("Match succeeded at offset %d\n", (int)ovector[0]);


	/*************************************************************************
	* We have found the first match within the subject string. If the output *
	* vector wasn't big enough, say so. Then output any substrings that were *
	* captured.                                                              *
	*************************************************************************/

	/* The output vector wasn't big enough. This should not happen, because we used
	pcre2_match_data_create_from_pattern() above. */

	if (rc == 0)
	  printf("ovector was not big enough for all the captured substrings\n");

	/* We must guard against patterns such as /(?=.\K)/ that use \K in an assertion
	to set the start of a match later than its end. In this demonstration program,
	we just detect this case and give up. */

	if (ovector[0] > ovector[1])
	  {
	  printf("\\K was used in an assertion to set the match start after its end.\n"
	    "From end to start the match was: %.*s\n", (int)(ovector[0] - ovector[1]),
	      (char *)(subject + ovector[1]));
	  printf("Run abandoned\n");
	  pcre2_match_data_free(match_data);
	  pcre2_code_free(re);
	  return 1;
	  }

	/* Show substrings stored in the output vector by number. Obviously, in a real
	application you might want to do things other than print them. */

	for (i = 0; i < rc; i++)
	  {
	  PCRE2_SPTR substring_start = subject + ovector[2*i];
	  PCRE2_SIZE substring_length = ovector[2*i+1] - ovector[2*i];
	  printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	  }


	/**************************************************************************
	* That concludes the basic part of this demonstration program. We have    *
	* compiled a pattern, and performed a single match. The code that follows *
	* shows first how to access named substrings, and then how to code for    *
	* repeated matches on the same subject.                                   *
	**************************************************************************/

	/* See if there are any named substrings, and if so, show them by name. First
	we have to extract the count of named parentheses from the pattern. */

	(void)pcre2_pattern_info(
	  re,                   /* the compiled pattern */
	  PCRE2_INFO_NAMECOUNT, /* get the number of named substrings */
	  &namecount);          /* where to put the answer */

	if (namecount == 0) printf("No named substrings\n"); else
	  {
	  PCRE2_SPTR tabptr;
	  printf("Named substrings\n");

	  /* Before we can access the substrings, we must extract the table for
	  translating names to numbers, and the size of each entry in the table. */

	  (void)pcre2_pattern_info(
	    re,                       /* the compiled pattern */
	    PCRE2_INFO_NAMETABLE,     /* address of the table */
	    &name_table);             /* where to put the answer */

	  (void)pcre2_pattern_info(
	    re,                       /* the compiled pattern */
	    PCRE2_INFO_NAMEENTRYSIZE, /* size of each entry in the table */
	    &name_entry_size);        /* where to put the answer */

	  /* Now we can scan the table and, for each entry, print the number, the name,
	  and the substring itself. In the 8-bit library the number is held in two
	  bytes, most significant first. */

	  tabptr = name_table;
	  for (i = 0; i < namecount; i++)
	    {
	    int n = (tabptr[0] << 8) | tabptr[1];
	    printf("(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
	      (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
	    tabptr += name_entry_size;
	    }
	  }


	/*************************************************************************
	* If the "-g" option was given on the command line, we want to continue  *
	* to search for additional matches in the subject string, in a similar   *
	* way to the /g option in Perl. This turns out to be trickier than you   *
	* might think because of the possibility of matching an empty string.    *
	* What happens is as follows:                                            *
	*                                                                        *
	* If the previous match was NOT for an empty string, we can just start   *
	* the next match at the end of the previous one.                         *
	*                                                                        *
	* If the previous match WAS for an empty string, we can't do that, as it *
	* would lead to an infinite loop. Instead, a call of pcre2_match() is    *
	* made with the PCRE2_NOTEMPTY_ATSTART and PCRE2_ANCHORED flags set. The *
	* first of these tells PCRE2 that an empty string at the start of the    *
	* subject is not a valid match; other possibilities must be tried. The   *
	* second flag restricts PCRE2 to one match attempt at the initial string *
	* position. If this match succeeds, an alternative to the empty string   *
	* match has been found, and we can print it and proceed round the loop,  *
	* advancing by the length of whatever was found. If this match does not  *
	* succeed, we still stay in the loop, advancing by just one character.   *
	* In UTF-8 mode, which can be set by (*UTF) in the pattern, this may be  *
	* more than one byte.                                                    *
	*                                                                        *
	* However, there is a complication concerned with newlines. When the     *
	* newline convention is such that CRLF is a valid newline, we must       *
	* advance by two characters rather than one. The newline convention can  *
	* be set in the regex by (*CR), etc.; if not, we must find the default.  *
	*************************************************************************/

	if (!find_all)     /* Check for -g */
	  {
	  pcre2_match_data_free(match_data);  /* Release the memory that was used */
	  pcre2_code_free(re);                /* for the match data and the pattern. */
	  return 0;                           /* Exit the program. */
	  }

	/* Before running the loop, check for UTF-8 and whether CRLF is a valid newline
	sequence. First, find the options with which the regex was compiled and extract
	the UTF state. */

	(void)pcre2_pattern_info(re, PCRE2_INFO_ALLOPTIONS, &option_bits);
	utf8 = (option_bits & PCRE2_UTF) != 0;

	/* Now find the newline convention and see whether CRLF is a valid newline
	sequence. */

	(void)pcre2_pattern_info(re, PCRE2_INFO_NEWLINE, &newline);
	crlf_is_newline = newline == PCRE2_NEWLINE_ANY ||
	                  newline == PCRE2_NEWLINE_CRLF ||
	                  newline == PCRE2_NEWLINE_ANYCRLF;

	/* Loop for second and subsequent matches */

	for (;;)
	  {
	  uint32_t options = 0;                   /* Normally no options */
	  PCRE2_SIZE start_offset = ovector[1];   /* Start at end of previous match */

	  /* If the previous match was for an empty string, we are finished if we are
	  at the end of the subject. Otherwise, arrange to run another match at the
	  same point to see if a non-empty match can be found. */

	  if (ovector[0] == ovector[1])
	    {
	    if (ovector[0] == subject_length) break;
	    options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
	    }

	  /* If the previous match was not an empty string, there is one tricky case to
	  consider. If a pattern contains \K within a lookbehind assertion at the
	  start, the end of the matched string can be at the offset where the match
	  started. Without special action, this leads to a loop that keeps on matching
	  the same substring. We must detect this case and arrange to move the start on
	  by one character. The pcre2_get_startchar() function returns the starting
	  offset that was passed to pcre2_match(). */

	  else
	    {
	    PCRE2_SIZE startchar = pcre2_get_startchar(match_data);
	    if (start_offset <= startchar)
	      {
	      if (startchar >= subject_length) break;   /* Reached end of subject.   */
	      start_offset = startchar + 1;             /* Advance by one character. */
	      if (utf8)                                 /* If UTF-8, it may be more  */
	        {                                       /*   than one code unit.     */
	        for (; start_offset < subject_length; start_offset++)
	          if ((subject[start_offset] & 0xc0) != 0x80) break;
	        }
	      }
	    }

	  /* Run the next matching operation */

	  rc = pcre2_match(
	    re,                   /* the compiled pattern */
	    subject,              /* the subject string */
	    subject_length,       /* the length of the subject */
	    start_offset,         /* starting offset in the subject */
	    options,              /* options */
	    match_data,           /* block for storing the result */
	    NULL);                /* use default match context */

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

	  if (rc == PCRE2_ERROR_NOMATCH)
	    {
	    if (options == 0) break;                    /* All matches found */
	    ovector[1] = start_offset + 1;              /* Advance one code unit */
	    if (crlf_is_newline &&                      /* If CRLF is a newline & */
	        start_offset < subject_length - 1 &&    /* we are at CRLF, */
	        subject[start_offset] == '\r' &&
	        subject[start_offset + 1] == '\n')
	      ovector[1] += 1;                          /* Advance by one more. */
	    else if (utf8)                              /* Otherwise, ensure we */
	      {                                         /* advance a whole UTF-8 */
	      while (ovector[1] < subject_length)       /* character. */
	        {
	        if ((subject[ovector[1]] & 0xc0) != 0x80) break;
	        ovector[1] += 1;
	        }
	      }
	    continue;    /* Go round the loop again */
	    }

	  /* Other matching errors are not recoverable. */

	  if (rc < 0)
	    {
	    printf("Matching error %d\n", rc);
	    pcre2_match_data_free(match_data);
	    pcre2_code_free(re);
	    return 1;
	    }

	  /* Match succeded */

	  printf("\nMatch succeeded again at offset %d\n", (int)ovector[0]);

	  /* The match succeeded, but the output vector wasn't big enough. This
	  should not happen. */

	  if (rc == 0)
	    printf("ovector was not big enough for all the captured substrings\n");

	  /* We must guard against patterns such as /(?=.\K)/ that use \K in an
	  assertion to set the start of a match later than its end. In this
	  demonstration program, we just detect this case and give up. */

	  if (ovector[0] > ovector[1])
	    {
	    printf("\\K was used in an assertion to set the match start after its end.\n"
	      "From end to start the match was: %.*s\n", (int)(ovector[0] - ovector[1]),
	        (char *)(subject + ovector[1]));
	    printf("Run abandoned\n");
	    pcre2_match_data_free(match_data);
	    pcre2_code_free(re);
	    return 1;
	    }

	  /* As before, show substrings stored in the output vector by number, and then
	  also any named substrings. */

	  for (i = 0; i < rc; i++)
	    {
	    PCRE2_SPTR substring_start = subject + ovector[2*i];
	    size_t substring_length = ovector[2*i+1] - ovector[2*i];
	    printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	    }

	  if (namecount == 0) printf("No named substrings\n"); else
	    {
	    PCRE2_SPTR tabptr = name_table;
	    printf("Named substrings\n");
	    for (i = 0; i < namecount; i++)
	      {
	      int n = (tabptr[0] << 8) | tabptr[1];
	      printf("(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
	        (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
	      tabptr += name_entry_size;
	      }
	    }
	  }      /* End of loop to find second and subsequent matches */

	printf("\n");
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return 0;


}


// jjustman-2020-07-13 - todo - add support for pcre2_substring_get_byname
#define TEST_MPD_REGEX_NAMED_CAPTURE_GROUP_PATTERN "(<Representation.*?id=\"(?<representation_id>.*?)\".*?>.*?<SegmentTemplate.*?startNumber=\"(?<start_number>.*?)\".*?<\\/Representation>)"



int test_parse_mpd_with_multiple_audio_adaption_sets_named_capture_group() {

	block_t* block_mpd = block_Read_from_filename(TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("Regex pattern: %s", TEST_MPD_REGEX_NAMED_CAPTURE_GROUP_PATTERN);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("MPD filename: %s", TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("MPD payload:\n%s", block_mpd->p_buffer);


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


	/**************************************************************************
	* First, sort out the command line. There is only one possible option at  *
	* the moment, "-g" to request repeated matching to find all occurrences,  *
	* like Perl's /g option. We set the variable find_all to a non-zero value *
	* if the -g option is present.                                            *
	**************************************************************************/

	find_all = 1;


	/* After the options, we require exactly two arguments, which are the pattern,
	and the subject string. */



	/* Pattern and subject are char arguments, so they can be straightforwardly
	cast to PCRE2_SPTR because we are working in 8-bit code units. The subject
	length is cast to PCRE2_SIZE for completeness, though PCRE2_SIZE is in fact
	defined to be size_t. */
	const char* my_pattern = TEST_MPD_REGEX_PATTERN;

	pattern = (PCRE2_SPTR) my_pattern;
	subject = (PCRE2_SPTR) block_mpd->p_buffer;
	subject_length = (PCRE2_SIZE)strlen((char *)subject);


	/*************************************************************************
	* Now we are going to compile the regular expression pattern, and handle *
	* any errors that are detected.                                          *
	*************************************************************************/

	re = pcre2_compile(
	  pattern,               /* the pattern */
	  PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
	  PCRE2_MULTILINE | PCRE2_DOTALL,                     /* default options */
	  &errornumber,          /* for error number */
	  &erroroffset,          /* for error offset */
	  NULL);                 /* use default compile context */

	/* Compilation failed: print the error message and exit. */

	if (re == NULL)
	  {
	  PCRE2_UCHAR buffer[256];
	  pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
	  printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
	    buffer);
	  return 1;
	  }


	/*************************************************************************
	* If the compilation succeeded, we call PCRE2 again, in order to do a    *
	* pattern match against the subject string. This does just ONE match. If *
	* further matching is needed, it will be done below. Before running the  *
	* match we must set up a match_data block for holding the result. Using  *
	* pcre2_match_data_create_from_pattern() ensures that the block is       *
	* exactly the right size for the number of capturing parentheses in the  *
	* pattern. If you need to know the actual size of a match_data block as  *
	* a number of bytes, you can find it like this:                          *
	*                                                                        *
	* PCRE2_SIZE match_data_size = pcre2_get_match_data_size(match_data);    *
	*************************************************************************/

	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	/* Now run the match. */

	rc = pcre2_match(
	  re,                   /* the compiled pattern */
	  subject,              /* the subject string */
	  subject_length,       /* the length of the subject */
	  0,                    /* start at offset 0 in the subject */
	  0,                    /* default options */
	  match_data,           /* block for storing the result */
	  NULL);                /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0)
	  {
	  switch(rc)
	    {
	    case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
	    /*
	    Handle other special cases if you like
	    */
	    default: printf("Matching error %d\n", rc); break;
	    }
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /*   data and the compiled pattern. */
	  return 1;
	  }

	/* Match succeded. Get a pointer to the output vector, where string offsets are
	stored. */

	ovector = pcre2_get_ovector_pointer(match_data);
	printf("Match succeeded at offset %d\n", (int)ovector[0]);


	/*************************************************************************
	* We have found the first match within the subject string. If the output *
	* vector wasn't big enough, say so. Then output any substrings that were *
	* captured.                                                              *
	*************************************************************************/

	/* The output vector wasn't big enough. This should not happen, because we used
	pcre2_match_data_create_from_pattern() above. */

	if (rc == 0)
	  printf("ovector was not big enough for all the captured substrings\n");

	/* We must guard against patterns such as /(?=.\K)/ that use \K in an assertion
	to set the start of a match later than its end. In this demonstration program,
	we just detect this case and give up. */

	if (ovector[0] > ovector[1])
	  {
	  printf("\\K was used in an assertion to set the match start after its end.\n"
	    "From end to start the match was: %.*s\n", (int)(ovector[0] - ovector[1]),
	      (char *)(subject + ovector[1]));
	  printf("Run abandoned\n");
	  pcre2_match_data_free(match_data);
	  pcre2_code_free(re);
	  return 1;
	  }

	/* Show substrings stored in the output vector by number. Obviously, in a real
	application you might want to do things other than print them. */

	for (i = 0; i < rc; i++)
	  {
	  PCRE2_SPTR substring_start = subject + ovector[2*i];
	  PCRE2_SIZE substring_length = ovector[2*i+1] - ovector[2*i];
	  printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	  }


	/**************************************************************************
	* That concludes the basic part of this demonstration program. We have    *
	* compiled a pattern, and performed a single match. The code that follows *
	* shows first how to access named substrings, and then how to code for    *
	* repeated matches on the same subject.                                   *
	**************************************************************************/

	/* See if there are any named substrings, and if so, show them by name. First
	we have to extract the count of named parentheses from the pattern. */

	(void)pcre2_pattern_info(
	  re,                   /* the compiled pattern */
	  PCRE2_INFO_NAMECOUNT, /* get the number of named substrings */
	  &namecount);          /* where to put the answer */

	if (namecount == 0) printf("No named substrings\n"); else
	  {
	  PCRE2_SPTR tabptr;
	  printf("Named substrings\n");

	  /* Before we can access the substrings, we must extract the table for
	  translating names to numbers, and the size of each entry in the table. */

	  (void)pcre2_pattern_info(
	    re,                       /* the compiled pattern */
	    PCRE2_INFO_NAMETABLE,     /* address of the table */
	    &name_table);             /* where to put the answer */

	  (void)pcre2_pattern_info(
	    re,                       /* the compiled pattern */
	    PCRE2_INFO_NAMEENTRYSIZE, /* size of each entry in the table */
	    &name_entry_size);        /* where to put the answer */

	  /* Now we can scan the table and, for each entry, print the number, the name,
	  and the substring itself. In the 8-bit library the number is held in two
	  bytes, most significant first. */

	  tabptr = name_table;
	  for (i = 0; i < namecount; i++)
	    {
	    int n = (tabptr[0] << 8) | tabptr[1];
	    printf("(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
	      (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
	    tabptr += name_entry_size;
	    }
	  }


	/*************************************************************************
	* If the "-g" option was given on the command line, we want to continue  *
	* to search for additional matches in the subject string, in a similar   *
	* way to the /g option in Perl. This turns out to be trickier than you   *
	* might think because of the possibility of matching an empty string.    *
	* What happens is as follows:                                            *
	*                                                                        *
	* If the previous match was NOT for an empty string, we can just start   *
	* the next match at the end of the previous one.                         *
	*                                                                        *
	* If the previous match WAS for an empty string, we can't do that, as it *
	* would lead to an infinite loop. Instead, a call of pcre2_match() is    *
	* made with the PCRE2_NOTEMPTY_ATSTART and PCRE2_ANCHORED flags set. The *
	* first of these tells PCRE2 that an empty string at the start of the    *
	* subject is not a valid match; other possibilities must be tried. The   *
	* second flag restricts PCRE2 to one match attempt at the initial string *
	* position. If this match succeeds, an alternative to the empty string   *
	* match has been found, and we can print it and proceed round the loop,  *
	* advancing by the length of whatever was found. If this match does not  *
	* succeed, we still stay in the loop, advancing by just one character.   *
	* In UTF-8 mode, which can be set by (*UTF) in the pattern, this may be  *
	* more than one byte.                                                    *
	*                                                                        *
	* However, there is a complication concerned with newlines. When the     *
	* newline convention is such that CRLF is a valid newline, we must       *
	* advance by two characters rather than one. The newline convention can  *
	* be set in the regex by (*CR), etc.; if not, we must find the default.  *
	*************************************************************************/

	if (!find_all)     /* Check for -g */
	  {
	  pcre2_match_data_free(match_data);  /* Release the memory that was used */
	  pcre2_code_free(re);                /* for the match data and the pattern. */
	  return 0;                           /* Exit the program. */
	  }

	/* Before running the loop, check for UTF-8 and whether CRLF is a valid newline
	sequence. First, find the options with which the regex was compiled and extract
	the UTF state. */

	(void)pcre2_pattern_info(re, PCRE2_INFO_ALLOPTIONS, &option_bits);
	utf8 = (option_bits & PCRE2_UTF) != 0;

	/* Now find the newline convention and see whether CRLF is a valid newline
	sequence. */

	(void)pcre2_pattern_info(re, PCRE2_INFO_NEWLINE, &newline);
	crlf_is_newline = newline == PCRE2_NEWLINE_ANY ||
	                  newline == PCRE2_NEWLINE_CRLF ||
	                  newline == PCRE2_NEWLINE_ANYCRLF;

	/* Loop for second and subsequent matches */

	for (;;)
	  {
	  uint32_t options = 0;                   /* Normally no options */
	  PCRE2_SIZE start_offset = ovector[1];   /* Start at end of previous match */

	  /* If the previous match was for an empty string, we are finished if we are
	  at the end of the subject. Otherwise, arrange to run another match at the
	  same point to see if a non-empty match can be found. */

	  if (ovector[0] == ovector[1])
	    {
	    if (ovector[0] == subject_length) break;
	    options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
	    }

	  /* If the previous match was not an empty string, there is one tricky case to
	  consider. If a pattern contains \K within a lookbehind assertion at the
	  start, the end of the matched string can be at the offset where the match
	  started. Without special action, this leads to a loop that keeps on matching
	  the same substring. We must detect this case and arrange to move the start on
	  by one character. The pcre2_get_startchar() function returns the starting
	  offset that was passed to pcre2_match(). */

	  else
	    {
	    PCRE2_SIZE startchar = pcre2_get_startchar(match_data);
	    if (start_offset <= startchar)
	      {
	      if (startchar >= subject_length) break;   /* Reached end of subject.   */
	      start_offset = startchar + 1;             /* Advance by one character. */
	      if (utf8)                                 /* If UTF-8, it may be more  */
	        {                                       /*   than one code unit.     */
	        for (; start_offset < subject_length; start_offset++)
	          if ((subject[start_offset] & 0xc0) != 0x80) break;
	        }
	      }
	    }

	  /* Run the next matching operation */

	  rc = pcre2_match(
	    re,                   /* the compiled pattern */
	    subject,              /* the subject string */
	    subject_length,       /* the length of the subject */
	    start_offset,         /* starting offset in the subject */
	    options,              /* options */
	    match_data,           /* block for storing the result */
	    NULL);                /* use default match context */

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

	  if (rc == PCRE2_ERROR_NOMATCH)
	    {
	    if (options == 0) break;                    /* All matches found */
	    ovector[1] = start_offset + 1;              /* Advance one code unit */
	    if (crlf_is_newline &&                      /* If CRLF is a newline & */
	        start_offset < subject_length - 1 &&    /* we are at CRLF, */
	        subject[start_offset] == '\r' &&
	        subject[start_offset + 1] == '\n')
	      ovector[1] += 1;                          /* Advance by one more. */
	    else if (utf8)                              /* Otherwise, ensure we */
	      {                                         /* advance a whole UTF-8 */
	      while (ovector[1] < subject_length)       /* character. */
	        {
	        if ((subject[ovector[1]] & 0xc0) != 0x80) break;
	        ovector[1] += 1;
	        }
	      }
	    continue;    /* Go round the loop again */
	    }

	  /* Other matching errors are not recoverable. */

	  if (rc < 0)
	    {
	    printf("Matching error %d\n", rc);
	    pcre2_match_data_free(match_data);
	    pcre2_code_free(re);
	    return 1;
	    }

	  /* Match succeded */

	  printf("\nMatch succeeded again at offset %d\n", (int)ovector[0]);

	  /* The match succeeded, but the output vector wasn't big enough. This
	  should not happen. */

	  if (rc == 0)
	    printf("ovector was not big enough for all the captured substrings\n");

	  /* We must guard against patterns such as /(?=.\K)/ that use \K in an
	  assertion to set the start of a match later than its end. In this
	  demonstration program, we just detect this case and give up. */

	  if (ovector[0] > ovector[1])
	    {
	    printf("\\K was used in an assertion to set the match start after its end.\n"
	      "From end to start the match was: %.*s\n", (int)(ovector[0] - ovector[1]),
	        (char *)(subject + ovector[1]));
	    printf("Run abandoned\n");
	    pcre2_match_data_free(match_data);
	    pcre2_code_free(re);
	    return 1;
	    }

	  /* As before, show substrings stored in the output vector by number, and then
	  also any named substrings. */

	  for (i = 0; i < rc; i++)
	    {
	    PCRE2_SPTR substring_start = subject + ovector[2*i];
	    size_t substring_length = ovector[2*i+1] - ovector[2*i];
	    printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	    }

	  if (namecount == 0) printf("No named substrings\n"); else
	    {
	    PCRE2_SPTR tabptr = name_table;
	    printf("Named substrings\n");
	    for (i = 0; i < namecount; i++)
	      {
	      int n = (tabptr[0] << 8) | tabptr[1];
	      printf("(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
	        (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
	      tabptr += name_entry_size;
	      }
	    }
	  }      /* End of loop to find second and subsequent matches */

	printf("\n");
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	return 0;


}

int test_parse_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils() {

	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(TEST_MPD_REGEX_PATTERN);

	block_t* block_mpd = block_Read_from_filename(TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);

	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);

	atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

	atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

	return 0;
}


int test_replace_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils() {

	uint32_t tsi = 0;
	atsc3_sls_alc_flow_t* atsc3_sls_alc_all_mediainfo_flow = NULL;

	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_new();

	lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_sls_alc_monitor_new();

	/*
	 * create a dummy map of representations with id's:
	 *
		TSI=100, repId=Video1_1
		TSI=200, repId=a02_2
		TSI=201, repId=a13_3
		TSI=300, repId=d4_4

		LS.Srcflow.ContentInfo.MediaInfo repId values

		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t
	*/

	char* repId_Video1_1 = "Video1_1";
	char* contentType_video = "video";

	char* repId_a02_2 = "a02_2";
	char* repId_a13_3 = "a13_3";
	char* contentType_audio = "audio";

	char* repId_d4_4 = "d4_4";
	char* contentType_subtitles = "subtitles";

	tsi=100;

	media_info->content_type = contentType_video;
	media_info->rep_id = repId_Video1_1;

	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 500;

	tsi=200;

	media_info->content_type = contentType_audio;
	media_info->rep_id = repId_a02_2;

	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 1000;

	tsi=201;

	media_info->content_type = contentType_audio;
	media_info->rep_id = repId_a13_3;

	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 31337;

	tsi=300;

	media_info->content_type = contentType_subtitles;
	media_info->rep_id = repId_d4_4;

	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 168502;
	/*

	 and startNumber values:

	  	500
	  	1000
	  	31337
	  	168502

	 *
	 */

	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(TEST_MPD_REGEX_PATTERN);

	block_t* block_mpd = block_Read_from_filename(TEST_MPD_MULTIPLE_AUDIO_ADAPTATION_SETS);

	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);

	atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector = atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector, &lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v);
	assert(match_vector);
	
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("got back %d match tuples <capture, media_info, alc_flow>", match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

	for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
		atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("s-tsid repId: %s, contentType: %s, startNumber replace start: %zu, end: %zu, toi value: %d",
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);

	}

	block_t* patched_mpd = atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(match_vector , block_mpd);
	block_Rewind(patched_mpd);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("patched MPD is now:\n%s", block_Get(patched_mpd));

	block_Destroy(&patched_mpd);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&match_vector);

	atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

	block_Destroy(&block_mpd);


	return 0;
}


int test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils() {

	uint32_t tsi = 0;
	atsc3_sls_alc_flow_t* atsc3_sls_alc_all_mediainfo_flow = NULL;

	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_new();

	lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_sls_alc_monitor_new();

	/*
	 * create a dummy map of representations with id's, expected lastClosedTOI:
	 
			Video: 		TSI=100, repId=1608656841598item-01item, lastClosedTOI: 500
			Audio: 		TSI=200, repId=1608656841598item-03item, lastClosedTOI: 1000
			Audio: 		TSI=201, repId=1608656841598item-02item, lastClosedTOI: 31337
			Subtitles: 	TSI=300, repId=1608656841598item-04item, lastClosedTOI: 168502

		LS.Srcflow.ContentInfo.MediaInfo repId values

		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t
	*/

	char* contentType_video = "video";
	char* TSI100_video = "1608656841598item-01item";

	char* contentType_audio = "audio";
	char* TSI200_audio = "1608656841598item-03item";
	char* TSI201_audio = "1608656841598item-02item";

	char* contentType_subtitles = "subtitles";
	char* TSI300_subtitles = "1608656841598item-04item";

	tsi=100;
	media_info->content_type = contentType_video;
	media_info->rep_id = TSI100_video;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 500;

	tsi=200;
	media_info->content_type = contentType_audio;
	media_info->rep_id = TSI200_audio;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 1000;

	tsi=201;
	media_info->content_type = contentType_audio;
	media_info->rep_id = TSI201_audio;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 31337;

	tsi=300;
	media_info->content_type = contentType_subtitles;
	media_info->rep_id = TSI300_subtitles;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 168502;
	
	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(TEST_MPD_REGEX_PATTERN);
	block_t* block_mpd = block_Read_from_filename(TEST_MPD_ADAPTATION_SET_CONTAINS_BOTH_REPRESENTATION_AND_SEGMENT_TEMPLATE_NODES_NO_CHILDREN_TEGNA_HARMONIC);

	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);
	atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector = atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector, &lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v);
	assert(match_vector);
	
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils: got back %d match tuples <capture, media_info, alc_flow>", match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

	for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
		atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils: s-tsid repId: %s, contentType: %s, startNumber replace start: %zu, end: %zu, toi value: %d",
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);

	}

	block_t* patched_mpd = atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(match_vector , block_mpd);
	block_Rewind(patched_mpd);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils: patched MPD is now:\n%s", block_Get(patched_mpd));

	block_Destroy(&patched_mpd);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&match_vector);

	atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

	block_Destroy(&block_mpd);

	return 0;
}

int test_replace_mpd_2021_04_14_aus_8_mpd8_xml_pcre2_regex_utils() {

	uint32_t tsi = 0;
	atsc3_sls_alc_flow_t* atsc3_sls_alc_all_mediainfo_flow = NULL;

	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_new();

	lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_sls_alc_monitor_new();

	/*
	 * create a dummy map of representations with id's, expected lastClosedTOI:
	 
			Video: 		TSI=100, repId=1608656841598item-01item, lastClosedTOI: 500
			Audio: 		TSI=200, repId=1608656841598item-03item, lastClosedTOI: 1000
			Audio: 		TSI=201, repId=1608656841598item-02item, lastClosedTOI: 31337
			Subtitles: 	TSI=300, repId=1608656841598item-04item, lastClosedTOI: 168502

		LS.Srcflow.ContentInfo.MediaInfo repId values

		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t
	*/

	char* contentType_video = "video";
	char* TSI100_video = "1615852633346item-01item";

	char* contentType_audio = "audio";
	//SPA - 1615852633346item-03item
	char* TSI200_audio = "1615852633346item-03item";
	
	//ENG - audio
	char* TSI201_audio = "1615852633346item-02item";

	//ENG - subtitles
	char* contentType_subtitles = "subtitles";
	char* TSI300_subtitles = "1615852633346item-04item";

	tsi=100;
	media_info->content_type = contentType_video;
	media_info->rep_id = TSI100_video;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 500;

	tsi=200;
	media_info->content_type = contentType_audio;
	media_info->rep_id = TSI200_audio;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 1000;

	tsi=201;
	media_info->content_type = contentType_audio;
	media_info->rep_id = TSI201_audio;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 31337;

	tsi=300;
	media_info->content_type = contentType_subtitles;
	media_info->rep_id = TSI300_subtitles;
	atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, tsi, media_info);
	atsc3_sls_alc_all_mediainfo_flow->last_closed_toi = 168502;
	
	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(TEST_MPD_REGEX_PATTERN);
	block_t* block_mpd = block_Read_from_filename(TEST_MPD_2021_04_14_AUS_8_MPD8_XML);

	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);
	atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector = atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector, &lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v);
	assert(match_vector);
	
	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_2021_04_14_aus_8_mpd8_xml_pcre2_regex_utils: got back %d match tuples <capture, media_info, alc_flow>", match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

	for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
		atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_2021_04_14_aus_8_mpd8_xml_pcre2_regex_utils: s-tsid repId: %s, contentType: %s, startNumber replace start: %zu, end: %zu, toi value: %d",
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);

	}

	block_t* patched_mpd = atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(match_vector , block_mpd);
	block_Rewind(patched_mpd);

	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_2021_04_14_aus_8_mpd8_xml_pcre2_regex_utils: patched MPD is now:\n%s", block_Get(patched_mpd));

	block_Destroy(&patched_mpd);

	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&match_vector);

	atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

	block_Destroy(&block_mpd);

	return 0;
}



int main(int argc, char* argv[] ) {

	_PCRE2_REGEX_UTILS_INFO_ENABLED = 1;
	_PCRE2_REGEX_UTILS_DEBUG_ENABLED = 1;
	_PCRE2_REGEX_UTILS_TRACE_ENABLED = 1;

	_ROUTE_DASH_UTILS_DEBUG_ENABLED = 1;
	_ROUTE_DASH_UTILS_TRACE_ENABLED = 1;


	_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("---starting unit for atsc3_route_dash_mpd_patch_test.c---");
	if(false) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_simple_regex_compare");
		test_simple_regex_extended_compare();
	}


	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_pcre2_simple_match");
		test_pcre2_simple_match();
	}

//	if(false) {
//		__MMSM_INFO("test_parse_atsc3_mmt_message_no_factory");
//		test_parse_atsc3_mmt_message_no_factoy();
//	}
//
//	if(false) {
//		__MMSM_INFO("test_parse_mp_table_no_factory");
//		test_parse_mp_table_no_factory();
//	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_parse_mpd_with_multiple_audio_adaption_sets");
		test_parse_mpd_with_multiple_audio_adaption_sets();
	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_parse_mpd_with_multiple_audio_adaption_sets_named_capture_group");
		test_parse_mpd_with_multiple_audio_adaption_sets_named_capture_group();
	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_parse_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils");
		test_parse_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils();
	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils");
		test_replace_mpd_with_multiple_audio_adaption_sets_pcre2_regex_utils();
	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils");
		test_replace_mpd_with_multiple_audio_adaption_sets_contains_both_representation_and_segment_template_nodes_no_children_tegna_harmonic_pcre2_regex_utils();
	}

	if(true) {
		_ATSC3_ROUTE_DASH_MPD_PATCH_TEST_INFO("test_replace_mpd_2021-04-14-aus-8-mpd8_xml_pcre2_regex_utils");
		test_replace_mpd_2021_04_14_aus_8_mpd8_xml_pcre2_regex_utils();
	}

	return 0;
}
