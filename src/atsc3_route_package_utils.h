/*
 * atsc3_route_package_utils.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 */



#include <stdio.h>
#include <string.h>

#ifndef ATSC3_ROUTE_PACKAGE_UTILS_H_
#define ATSC3_ROUTE_PACKAGE_UTILS_H_


#include "atsc3_logging_externs.h"
#include "atsc3_utils.h"
#include "atsc3_mime_multipart_related_parser.h"
#include "atsc3_mbms_envelope_parser.h"
#include "atsc3_sls_metadata_fragment_types_parser.h"
#include "atsc3_alc_rx.h"

#include "atsc3_utils_sha256.h"

#if defined (__cplusplus)
extern "C" {
#endif


typedef struct atsc3_route_package_extracted_envelope_metadata_and_payload {
	char*										package_name; //TODO: jjustman-2020-07-27 - App.pkg
	uint32_t 									tsi;
	uint32_t									toi;
	char*										app_context_id_list;
	char*										filter_codes;

	//todo: extract elements from atsc3_fdt-instance as needed (max expires delta, etc)
	//todo: add atsc3_fdt_file_t as needed

	char*										package_extract_path;
	block_t* 									atsc3_mbms_metadata_envelope_raw_xml;  					//raw xml contents of the MBMS envelope boundary object w/ header Content-Type:application/mbms-envelope+xml

	atsc3_mbms_metadata_envelope_t* 			atsc3_mbms_metadata_envelope;		//collection of envelope items, should match with item.metadataUri ==  multipart_related.content-location
	atsc3_mime_multipart_related_instance_t*  	atsc3_mime_multipart_related_instance;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mime_multipart_related_payload);	//collection of multipart/related objects and their binary payload from the package extraction

} atsc3_route_package_extracted_envelope_metadata_and_payload_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_mime_multipart_related_payload);
ATSC3_VECTOR_BUILDER_METHODS_PARENT_INTERFACE_FREE(atsc3_route_package_extracted_envelope_metadata_and_payload);

char* atsc3_route_package_generate_path_from_appContextIdList(atsc3_fdt_file_t* atsc3_fdt_file);

void atsc3_route_package_extracted_envelope_metadata_and_payload_set_alc_tsi_toi_from_alc_packet(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload, alc_packet_t* alc_packet);
void atsc3_route_package_extracted_envelope_metadata_and_payload_set_fdt_attributes(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_fdt_file_t* atsc3_fdt_file);

atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extract_unsigned_payload(const char* filename, const char* package_extract_path);

void atsc3_route_package_extract_payload_metadata_dump(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload);



#define __ROUTE_PACKAGE_UTILS_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_DEBUG(...)   if(_ROUTE_PACKAGE_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ROUTE_PACKAGE_UTILS_TRACE(...)   if(_ROUTE_PACKAGE_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }



#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_ROUTE_PACKAGE_UTILS_H_ */
