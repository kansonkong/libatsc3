/*
 * atsc3_mbms_envelope_parser.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_mbms_envelope_parser.h"


uint32_t* atsc3_mbms_envelope_find_toi_from_fdt(atsc3_fdt_instance_t* atsc3_fdt_instance) {

	for(int i=0; i < atsc3_fdt_instance->atsc3_fdt_file_v.count; i++) {
		atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[i];
		if(!strncasecmp(ATSC3_MBMS_ENVELOPE_CONTENT_TYPE, atsc3_fdt_file->content_type, strlen(ATSC3_MBMS_ENVELOPE_CONTENT_TYPE))) {
			_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("returning matching type: %s for toi: %u",  ATSC3_MBMS_ENVELOPE_CONTENT_TYPE, atsc3_fdt_file->toi);

			return &atsc3_fdt_file->toi;
		} else {
			_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("ignoring fdt_file: toi: %u, type: %s, name: %s", atsc3_fdt_file->toi, atsc3_fdt_file->content_type, atsc3_fdt_file->content_location);
		}
	}

	return NULL;
}


atsc3_sls_metadata_fragments_t* atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(FILE* atsc3_fdt_instance_fp) {

	atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = NULL;

	if(!atsc3_fdt_instance_fp) {
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR("atsc3_fdt_instance_fp is null!");
		return NULL;
	}

	atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_parser(atsc3_fdt_instance_fp);
	if(atsc3_mime_multipart_related_instance) {
		atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance);

		//build out the atsc3_sls_metadata_fragment_types
		atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance);

	} else {
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR("atsc3_mime_multipart_related_instance is null!");
		return NULL;
	}

	return atsc3_sls_metadata_fragments;
}
