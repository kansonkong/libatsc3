/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "atsc3_utils.h"
#include "atsc3_mime_multipart_related_parser.h"
#include "atsc3_sls_metadata_fragment_types_parser.h"
#include "atsc3_logging_externs.h"

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
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("---");

	//test 1
	char* test1_filename = "../test_data/sba-dash/0-4653138";
	_ATSC3_MIME_MULTIPART_TEST_UTILS_INFO("Running test: %s", test1_filename);
	int ret_test1 = parse_mime_multipart(test1_filename);
	assert(ret_test1 == 5);

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

