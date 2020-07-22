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


#if defined (__cplusplus)
extern "C" {
#endif

typedef struct atsc3_route_package_extract_payload_multipart_item {
	char*		content_location;
	uint32_t	size;
} atsc3_route_package_extract_payload_multipart_item_t;

/*
 * from atsc3_sls_metadata_fragment_types_parser.c
 *
 *      //ROUTE MBMS envelope fragment creation
            if(!strncmp(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope = atsc3_mbms_envelope_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_mbms_metadata_envelope_dump(atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope);
            }

 */

typedef struct atsc3_route_package_extract_payload_metadata {
	block_t* atsc3_mbms_metadata_envelope_raw_xml;
	atsc3_mbms_metadata_envelope_t* atsc3_mbms_metadata_envelope;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_package_extract_payload_multipart_item);

} atsc3_route_package_extract_payload_metadata_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_package_extract_payload_metadata, atsc3_route_package_extract_payload_multipart_item);

atsc3_route_package_extract_payload_metadata_t* atsc3_route_package_extract_unsigned_payload(const char* filename);




#define __ROUTE_PACKAGE_UTILS_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_DEBUG(...)   if(_ROUTE_PACKAGE_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ROUTE_PACKAGE_UTILS_TRACE(...)   if(_ROUTE_PACKAGE_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }



#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_ROUTE_PACKAGE_UTILS_H_ */
