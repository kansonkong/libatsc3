/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_mime_multipart_related_parser.h"
#include "../atsc3_sls_metadata_fragment_types_parser.h"

// #define __TRACE__

#define _ATSC3_MIME_MULTIPART_TEST_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MIME_MULTIPART_TEST_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MIME_MULTIPART_TEST_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MIME_MULTIPART_TEST_UTILS_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


int parse_mime_multipart(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");

	if(fp) {
		//validate our struct
		atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_parser(fp);
		if(atsc3_mime_multipart_related_instance) {
			atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance);

			//build out the atsc3_sls_metadata_fragment_types

			atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance);

			/*
			 * 	atsc3_mime_multipart_related_instance_t* 		atsc3_mime_multipart_related_instance;
				atsc3_mbms_metadata_envelope_t* 				atsc3_mbms_metadata_envelope;
				atsc3_route_user_service_bundle_description_t*	atsc3_route_user_service_bundle_description;
				atsc3_route_s_tsid_t*							atsc3_route_s_tsid;
				atsc3_route_mpd_t*								atsc3_route_mpd;
				atsc3_sls_held_fragment_t*						atsc3_sls_held_fragment;
			 */
			if(atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance) ret++;
			if(atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope) ret++;
			if(atsc3_sls_metadata_fragments->atsc3_route_user_service_bundle_description) ret++;
			if(atsc3_sls_metadata_fragments->atsc3_route_s_tsid) ret++;
			if(atsc3_sls_metadata_fragments->atsc3_route_mpd) ret++;
			if(atsc3_sls_metadata_fragments->atsc3_sls_held_fragment) ret++;


		} else {
			_ATSC3_MIME_MULTIPART_TEST_UTILS_ERROR("atsc3_mime_multipart_related_instance is null!");
			ret = -1;
		}
	}

	return ret;
}

int main(int argc, char* argv[] ) {

#ifdef __TRACE__
	_XML_INFO_ENABLED = 1;
	_XML_DEBUG_ENABLED = 1;
	_XML_TRACE_ENABLED = 1;
#endif
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---%s---", argv[0]);

	/**
	 * test group 1 - from santa barbara plugfest
	 *
	 * 	0-4653134
	 * 	0-4653135
	 * 	0-4653136
	 * 	0-4653137
	 * 	0-4653138
	 * 	0-4653139
	 * 	0-4653140
	 * 	0-4653141
	 * 	0-4653142
	 *
	 */


	//phx-dash-2
	char* testphxdash_filename = "../test_data/phx-dash-2/0-458758";
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("Running test: %s", testphxdash_filename);
	int ret_phx_test = parse_mime_multipart(testphxdash_filename);
	assert(ret_phx_test == 4);

	assert(0);

	#define __SBA_DASH_PATH__ ../test_data/sba-dash/

	char* test_sba_filenames[] = {	"../test_data/sba-dash/0-4653134",
									"../test_data/sba-dash/0-4653135",
									"../test_data/sba-dash/0-4653136",
									"../test_data/sba-dash/0-4653137",
									"../test_data/sba-dash/0-4653138",
									"../test_data/sba-dash/0-4653139",
									"../test_data/sba-dash/0-4653140",
									"../test_data/sba-dash/0-4653141",
									"../test_data/sba-dash/0-4653142" };


	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---starting sba test---");

	for(int i=0; i < 9; i++) {
		char* sba_test_filename = test_sba_filenames[i];
		_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("Running test: %s", sba_test_filename);
		int ret_test1 = parse_mime_multipart(sba_test_filename);

		//first test data file is a partial alc
		if(i == 0) {
			assert(ret_test1 == 3);
		} else if(i == 5 || i == 6) {
			//the 4653139 and 4653140 test data files are missing the first alc packet, and thus can't be parsed.
			assert(ret_test1 == -1);
		} else {
			assert(ret_test1 == 5);
		}
	}


	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---");

	//test 2
	char* test2_filename = "../test_data/ds-route/0-458760";
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("Running test: %s", test2_filename);
	int ret_test2 = parse_mime_multipart(test2_filename);
	assert(ret_test2 == 4);

	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---");

	//test 3
	//this one contains an embedded envelope...so only 2
	char* test3_filename = "../test_data/phx-dash/0-196655";
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("Running test: %s", test3_filename);
	int ret_test3 = parse_mime_multipart(test3_filename);
	assert(ret_test3 == 3);



	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---");
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("mime_multipart_related_parser test complete");
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---");

	return 0;
}

