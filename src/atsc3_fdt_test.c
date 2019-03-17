/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "atsc3_utils.h"
#include "atsc3_fdt.h"
#include "atsc3_fdt_parser.h"
#include "atsc3_logging_externs.h"

#define _ATSC3_FDT_TEST_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

int parse_fdt(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");
	xml_document_t* fdt_xml = xml_open_document(fp);
	if(fdt_xml) {
		//validate our struct
		atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
		if(!atsc3_fdt_instance) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance is null!");
			return -1;
		}
		atsc3_fdt_instance_dump(atsc3_fdt_instance);

	}

	return ret;
}
int main(int argc, char* argv[] ) {

	 _XML_INFO_ENABLED = 1;
	 _XML_DEBUG_ENABLED = 1;
	 _XML_TRACE_ENABLED = 1;



	parse_fdt("../test_data/xml_fdt/phx-fdt-0-0.xml");

	return 0;
}

