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
#include "atsc3_logging_externs.h"

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


		} else {
			_ATSC3_MIME_MULTIPART_TEST_UTILS_ERROR("atsc3_mime_multipart_related_instance is null!");
			ret = -1;
		}
	}

	return ret;
}

int main(int argc, char* argv[] ) {


	 parse_mime_multipart("../test_data/sba-dash/0-4653138"); //application/mbms-envelope+xml
	 return 0;
}

