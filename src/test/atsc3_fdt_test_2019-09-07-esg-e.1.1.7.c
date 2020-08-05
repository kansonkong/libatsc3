/*
 * atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c
 *
 *  Created on: Sept 7th, 2019
 *      Author: jjustman
 *
 *      baseline FDT parsing for ESG mbms envelope and s-tsid payload
 *
 *      TODO: fix leak(s) in:
 *
==83703== 30,000 bytes in 1,000 blocks are definitely lost in loss record 49 of 60
==83703==    at 0x10015C6EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==83703==    by 0x10000C40A: kvp_collection_get (atsc3_utils.c:84)
==83703==    by 0x10002D83E: atsc3_fdt_file_parse_from_xml_fdt_instance (atsc3_fdt_parser.c:244)
==83703==    by 0x10002D20A: atsc3_efdt_instance_parse_from_xml_node (atsc3_fdt_parser.c:54)
==83703==    by 0x10002D981: atsc3_fdt_instance_parse_from_xml_document (atsc3_fdt_parser.c:89)
==83703==    by 0x100000CAE: parse_fdt (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:41)
==83703==    by 0x1000011D7: main (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:132)
==83703==
==83703== 112,000 bytes in 1,000 blocks are definitely lost in loss record 58 of 60
==83703==    at 0x10015C6EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==83703==    by 0x10002D071: atsc3_efdt_instance_parse_from_xml_node (atsc3_fdt_parser.c:32)
==83703==    by 0x10002D981: atsc3_fdt_instance_parse_from_xml_document (atsc3_fdt_parser.c:89)
==83703==    by 0x100000CAE: parse_fdt (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:41)
==83703==    by 0x1000011D7: main (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:132)
==83703==
==83703== 207,000 (16,000 direct, 191,000 indirect) bytes in 1,000 blocks are definitely lost in loss record 59 of 60
==83703==    at 0x10015C6EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==83703==    by 0x10000C5B5: kvp_collection_parse (atsc3_utils.c:99)
==83703==    by 0x10002D754: atsc3_fdt_file_parse_from_xml_fdt_instance (atsc3_fdt_parser.c:220)
==83703==    by 0x10002D20A: atsc3_efdt_instance_parse_from_xml_node (atsc3_fdt_parser.c:54)
==83703==    by 0x10002D981: atsc3_fdt_instance_parse_from_xml_document (atsc3_fdt_parser.c:89)
==83703==    by 0x100000CAE: parse_fdt (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:41)
==83703==    by 0x1000011D7: main (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:132)
==83703==
==83703== 252,000 (16,000 direct, 236,000 indirect) bytes in 1,000 blocks are definitely lost in loss record 60 of 60
==83703==    at 0x10015C6EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==83703==    by 0x10000C5B5: kvp_collection_parse (atsc3_utils.c:99)
==83703==    by 0x10002D3BD: atsc3_fdt_parse_from_xml_fdt_instance (atsc3_fdt_parser.c:99)
==83703==    by 0x10002D1AD: atsc3_efdt_instance_parse_from_xml_node (atsc3_fdt_parser.c:49)
==83703==    by 0x10002D981: atsc3_fdt_instance_parse_from_xml_document (atsc3_fdt_parser.c:89)
==83703==    by 0x100000CAE: parse_fdt (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:41)
==83703==    by 0x1000011D7: main (atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c:132)
 */

#define __VALGRIND_LOOP_COUNT__ 1000
#define __TEST_SPECIMEN_FDT__ "testdata/2019-09-07-esg-e.1.1.7/0-0"
/*
 *
 * <FDT-Instance xmlns="urn:ietf:params:xml:ns:fdt"
              xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/"
              Expires="4294967295"
              afdt:efdtVersion="1">
   <File Content-Location="sls"
         TOI="196660"
         Content-Length="3560"
         Content-Type="application/mbms-envelope+xml"/>
</FDT-Instance>
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_fdt_parser.h"

#define _ATSC3_FDT_TEST_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

int parse_fdt(const char* filename) {
	int ret = 0;
	atsc3_fdt_instance_t* atsc3_fdt_instance = NULL;

	FILE *fp = fopen(filename, "r");
	if(!fp) {
		_ATSC3_FDT_TEST_UTILS_ERROR("unable to open filename: %s", filename);
		ret = 1;
		goto cleanup;
	}

	xml_document_t* fdt_xml = xml_open_document(fp);
	if(!fdt_xml) {
		_ATSC3_FDT_TEST_UTILS_ERROR("fdt_xml is null");
		ret = 2;
		goto cleanup;
	}

	if(fdt_xml) {
		//validate our struct
		atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
		if(!atsc3_fdt_instance) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance is null!");
			ret = -1;
			goto cleanup;
		}
		atsc3_fdt_instance_dump(atsc3_fdt_instance);

		//check for baseline values
		if(!(4294967295 == atsc3_fdt_instance->expires)) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->expires is incorrect, expected: %ld, got: %d", 4294967295, atsc3_fdt_instance->expires);
			ret = -2;
			goto cleanup;
		}

		if(!(1 == atsc3_fdt_instance->efdt_version)) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->efdt_vesion is incorrect, expected: %d, got: %d", 1, atsc3_fdt_instance->efdt_version);
			ret = -3;
			goto cleanup;
		}

		if(!(1 == atsc3_fdt_instance->atsc3_fdt_file_v.count)) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->atsc3_fdt_file_v.count is incorrect, expected: %d, got: %d", 1, atsc3_fdt_instance->atsc3_fdt_file_v.count);
			ret = -4;
			goto cleanup;
		}

#define __FDT_CONTENT_LOCATION__ "sls"

		if(!(strncmp(__FDT_CONTENT_LOCATION__, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_location, strlen(__FDT_CONTENT_LOCATION__)))) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->content_location is incorrect, expected: %s, got: %s", __FDT_CONTENT_LOCATION__, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_location);
			ret = -2;
			goto cleanup;
		}

		if(!(196660 == atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->toi)) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->atsc3_fdt_file_v->toi is incorrect, expected: %d, got: %d", 196660, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->toi);
			ret = -5;
			goto cleanup;
		}

		if(!(3560 == atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_length)) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->atsc3_fdt_file_v->content_length is incorrect, expected: %d, got: %d", 3560, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_length);
			ret = -6;
			goto cleanup;
		}

#define __FDT_CONTENT_TYPE__ "application/mbms-envelope+xml"
		if(!(strncmp(__FDT_CONTENT_TYPE__, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_type, __MIN(strlen(__FDT_CONTENT_TYPE__), strlen(atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_type))))) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance->atsc3_fdt_file_v->content_type is incorrect, expected: %s, got: %s", __FDT_CONTENT_TYPE__, atsc3_fdt_instance->atsc3_fdt_file_v.data[0]->content_type);
			ret = -7;
			goto cleanup;
		}
	}

cleanup:

	if(atsc3_fdt_instance) {
		atsc3_fdt_instance_free_atsc3_fdt_file(atsc3_fdt_instance);
	}

	if(fdt_xml) {
		xml_document_free(fdt_xml, true);
	}

	if(fp) {
		fclose(fp);
	}
	return ret;
}

int main(int argc, char* argv[] ) {
	_XML_INFO_ENABLED = 1;
	_XML_DEBUG_ENABLED = 1;
	_XML_TRACE_ENABLED = 1;

	int test_result = 0;

	_ATSC3_FDT_TEST_UTILS_INFO("atsc3_fdt_test: parsing %s", __TEST_SPECIMEN_FDT__);

#ifdef __VALGRIND_LOOP_COUNT__
	for(int i=0; i < __VALGRIND_LOOP_COUNT__; i++) {
#endif

	test_result = parse_fdt(__TEST_SPECIMEN_FDT__); //application/mbms-envelope+xml

#ifdef __VALGRIND_LOOP_COUNT__
	}
#endif

	_ATSC3_FDT_TEST_UTILS_INFO("atsc3_fdt_test: parse_fdt test result: %d (should be 0)", test_result);

	return test_result;
}

