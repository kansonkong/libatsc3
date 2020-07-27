/*
 * atsc3_route_package_extract_test.c
 *
 *  Created on: July 7, 2020
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_mime_multipart_related_parser.h"
#include "../atsc3_sls_metadata_fragment_types_parser.h"
#include "../atsc3_route_package_utils.h"

// #define __TRACE__

#define _ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extract_unsigned_payload_test(const char* filename) {

	const char* package_extract_path_test_original = "package_extract_path_1";
	atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_file_new();
	atsc3_fdt_file->app_context_id_list = (char*)package_extract_path_test_original;

	char* package_extract_path = atsc3_route_package_generate_path_from_appContextIdList(atsc3_fdt_file);

	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("--enter: atsc3_route_package_extract_unsigned_payload_test with filename: %s, original package extract path: %s, hashed package extract path: %s", filename, package_extract_path_test_original, package_extract_path);

	atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload = atsc3_route_package_extract_unsigned_payload(filename, package_extract_path);

	if(atsc3_route_package_extracted_envelope_metadata_and_payload) {

		atsc3_route_package_extracted_envelope_metadata_and_payload_set_fdt_attributes(atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_fdt_file);
		_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_DEBUG("atsc3_route_package_extract_payload_metadata is: %p, num objects: %d", atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count);

		atsc3_route_package_extract_payload_metadata_dump(atsc3_route_package_extracted_envelope_metadata_and_payload);
	} else {
		_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_ERROR("atsc3_route_package_extracted_envelope_metadata_and_payload is NULL!");
	}


	return atsc3_route_package_extracted_envelope_metadata_and_payload;
}



int main(int argc, char* argv[] ) {

	_MIME_PARSER_INFO_ENABLED = 1;
	_MIME_PARSER_DEBUG_ENABLED = 1;
	_MIME_PARSER_TRACE_ENABLED = 1;

	_ROUTE_PACKAGE_UTILS_DEBUG_ENABLED = 1;
	_ROUTE_PACKAGE_UTILS_TRACE_ENABLED = 1;

	_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG_ENABLED = 1;

#ifdef __TRACE__
	_XML_INFO_ENABLED = 1;
	_XML_DEBUG_ENABLED = 1;
	_XML_TRACE_ENABLED = 1;
#endif
	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("---%s---", argv[0]);

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


	char* test_app_pkg_payload = "../../test_data/route-dash/2020-07-02-mpd-patching/route-5004/App.pkg";

	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("Running tests with payload: %s", test_app_pkg_payload);
	atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_app_pkg = atsc3_route_package_extract_unsigned_payload_test(test_app_pkg_payload);

	assert(atsc3_route_package_extracted_envelope_metadata_and_payload_app_pkg);
	assert(atsc3_route_package_extracted_envelope_metadata_and_payload_app_pkg->atsc3_mime_multipart_related_payload_v.count == 56);


	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("---");
	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("atsc3_route_package_extract_test test_app_pkg_payload complete, result: %p", atsc3_route_package_extracted_envelope_metadata_and_payload_app_pkg);
	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("---");



	char* test_alert_pkg_payload = "../../test_data/route-dash/2020-07-02-mpd-patching/route-5004/Alert.pkg";

	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("Running tests with payload: %s", test_alert_pkg_payload);
	atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_alert_pkg = atsc3_route_package_extract_unsigned_payload_test(test_alert_pkg_payload);

	assert(atsc3_route_package_extracted_envelope_metadata_and_payload_alert_pkg);
	assert(atsc3_route_package_extracted_envelope_metadata_and_payload_alert_pkg->atsc3_mime_multipart_related_payload_v.count == 1);
	assert(atsc3_route_package_extracted_envelope_metadata_and_payload_alert_pkg->atsc3_mime_multipart_related_payload_v.data[0]->content_type);


	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("---");
	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("atsc3_route_package_extract_test test_alert_pkg_payload complete, result: %p", atsc3_route_package_extracted_envelope_metadata_and_payload_alert_pkg);
	_ATSC3_ROUTE_PACKAGE_EXTRACT_TEST_INFO("---");

	return 0;
}

