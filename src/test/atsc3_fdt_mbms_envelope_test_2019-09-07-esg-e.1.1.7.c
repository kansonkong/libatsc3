/*
 * atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c
 *
 *  Created on: Sept 7th, 2019
 *      Author: jjustman
 *
 *      baseline ESG parsing of mbms envelope and s-tsid payload
 *
 */


#define __VALGRIND_LOOP_COUNT__ 1000

#define __TEST_SPECIMEN_PATH__ "testdata/2019-09-07-esg-e.1.1.7"
#define __TEST_SPECIMEN_FDT__  "testdata/2019-09-07-esg-e.1.1.7/0-0"
#define __TEST_SPECIMEN_MBMS__ "testdata/2019-09-07-esg-e.1.1.7/0-196660"

/*
 *
 *
Content-Type: multipart/related;
 type="application/mbms-envelope+xml"
boundary="j3(c_hegB.HBC9P/9=Re=FkSgyhaTzS_D4_e7a?7EZh(6zXf(-ZmQq+6hDxLu3?uoNww?F"

--j3(c_hegB.HBC9P/9=Re=FkSgyhaTzS_D4_e7a?7EZh(6zXf(-ZmQq+6hDxLu3?uoNww?F
Content-Type: application/mbms-envelope+xml
Content-Location: envelope.xml

<?xml version="1.0" encoding="UTF-8"?>
<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
   <item metadataURI="usbd.xml" version="1" contentType="application/route-usd+xml"/>
   <item metadataURI="stsid.xml" version="1" contentType="application/route-s-tsid+xml"/>
</metadataEnvelope>

..[truncated]..
 *
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_fdt_parser.h"
#include "../atsc3_mbms_envelope_parser.h"
#include "../atsc3_mime_multipart_related_parser.h"

#define _ATSC3_FDT_TEST_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_TEST_UTILS_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

int parse_fdt(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");
	xml_document_t* fdt_xml = xml_open_document(fp);
	if(fdt_xml) {
		//pretend we got our 0-0 tsi/toi here..
		atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
		if(!atsc3_fdt_instance) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance is null!");
			return -1;
		}
		atsc3_fdt_instance_dump(atsc3_fdt_instance);
		//pretend we got our 0/toi here..

		uint32_t* mbms_toi = atsc3_mbms_envelope_find_toi_from_fdt(atsc3_fdt_instance);

		if(!mbms_toi) {
			printf("Unable to find MBMS TOI for %s", filename);
			return -1;
		}

		char* mbms_toi_filename = calloc(65, sizeof(char*));
		snprintf(mbms_toi_filename, 64, "%s/0-%d", __TEST_SPECIMEN_PATH__, *mbms_toi);

		FILE *fp_mbms = fopen(mbms_toi_filename, "r");

		atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);
	}

	return ret;
}
int main(int argc, char* argv[] ) {

	 _XML_INFO_ENABLED = 1;
	 _XML_DEBUG_ENABLED = 1;
	 _XML_TRACE_ENABLED = 1;

	int test_result = 0;

	_ATSC3_FDT_TEST_UTILS_INFO("atsc3_fdt_mbms_envelope_test:\n\tparsing fdt: %s,\n\texpected mbms: %s\n\n", __TEST_SPECIMEN_FDT__, __TEST_SPECIMEN_MBMS__);

	 parse_fdt(__TEST_SPECIMEN_FDT__);
	 return 0;
}

