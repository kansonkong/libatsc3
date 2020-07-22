/*
 * atsc3_sls_metadata_fragment_types_parser.c
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#include "atsc3_sls_metadata_fragment_types_parser.h"


int _SLS_METADATA_FRAGMENT_PARSER_INFO_ENABLED  = 0;
int _SLS_METADATA_FRAGMENT_PARSER_DEBUG_ENABLED = 0;
int _SLS_METADATA_FRAGMENT_PARSER_TRACE_ENABLED = 0;


atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance) {
	if(!atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count) {
		return NULL;

	}
	atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = calloc(1, sizeof(atsc3_sls_metadata_fragments_t));
    atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_instance;
	atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = NULL;

	for(int i=0; i < atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
		atsc3_mime_multipart_related_payload = atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];

		//jjustman-2020-07-22 TODO: switch _parse_from_payload methods to use block_t* rather than char*

		//jjustman-2020-02-28 - avoid null pointer deref
		if(atsc3_mime_multipart_related_payload && atsc3_mime_multipart_related_payload->content_type && atsc3_mime_multipart_related_payload->payload) {
            __SLS_METADATA_FRAGMENT_PARSER_DEBUG("type     : %s", atsc3_mime_multipart_related_payload->content_type);
            __SLS_METADATA_FRAGMENT_PARSER_DEBUG("location : %s", atsc3_mime_multipart_related_payload->content_location);
            __SLS_METADATA_FRAGMENT_PARSER_DEBUG("payload  :\n%s", atsc3_mime_multipart_related_payload->payload->p_buffer);

            //ROUTE MBMS envelope fragment creation
            if(!strncmp(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope = atsc3_mbms_envelope_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_mbms_metadata_envelope_dump(atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope);
            }
            //USBD fragment creation
            if(!strncmp(ATSC3_ROUTE_USD_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_USD_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_route_user_service_bundle_description = atsc3_route_user_service_bundle_description_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_route_usb_dump(atsc3_sls_metadata_fragments->atsc3_route_user_service_bundle_description);
            }

            //S-TSID fragment creation
            if(!strncmp(ATSC3_ROUTE_S_TSID_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_S_TSID_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_route_s_tsid = atsc3_route_s_tsid_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_route_s_tsid_dump(atsc3_sls_metadata_fragments->atsc3_route_s_tsid);
            }

            //ROUTE MPD fragment creation
            if(!strncmp(ATSC3_ROUTE_MPD_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_MPD_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_route_mpd = atsc3_route_mpd_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_route_mpd_dump(atsc3_sls_metadata_fragments->atsc3_route_mpd);
            }


            //HELD fragment creation
            if(!strncmp(ATSC3_SLS_HELD_FRAGMENT_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_SLS_HELD_FRAGMENT_TYPE))) {
                atsc3_sls_metadata_fragments->atsc3_sls_held_fragment = atsc3_sls_held_fragment_parse_from_payload(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->content_location);
                atsc3_sls_held_fragment_dump(atsc3_sls_metadata_fragments->atsc3_sls_held_fragment);

            }
		}
	}
	return atsc3_sls_metadata_fragments;
}


