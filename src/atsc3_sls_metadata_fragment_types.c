/*
 * atsc3_sls_metadata_fragment_types.c
 *
 *  Created on: Nov 2, 2019
 *      Author: jjustman
 */



#include "atsc3_sls_metadata_fragment_types.h"

atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_new() {
	atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = calloc(1, sizeof(atsc3_sls_metadata_fragments_t));

	return atsc3_sls_metadata_fragments;
}


void atsc3_mime_multipart_related_instance_free(atsc3_mime_multipart_related_instance_t** atsc3_mime_multipart_related_instance_p) {
	if(atsc3_mime_multipart_related_instance_p) {
		atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = *atsc3_mime_multipart_related_instance_p;
		if(atsc3_mime_multipart_related_instance) {

			freeclean((void**)&atsc3_mime_multipart_related_instance->type);

			freeclean((void**)&atsc3_mime_multipart_related_instance->boundary);

			atsc3_mime_multipart_related_instance_free_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_instance);

			free(atsc3_mime_multipart_related_instance);
			atsc3_mime_multipart_related_instance = NULL;
		}
		*atsc3_mime_multipart_related_instance_p = NULL;
	}
}
void atsc3_mbms_metadata_envelope_free(atsc3_mbms_metadata_envelope_t** atsc3_mbms_metadata_envelope_p) {
	if(atsc3_mbms_metadata_envelope_p) {
		atsc3_mbms_metadata_envelope_t* atsc3_mbms_metadata_envelope = *atsc3_mbms_metadata_envelope_p;
		if(atsc3_mbms_metadata_envelope) {
			atsc3_mbms_metadata_envelope_free_atsc3_mbms_metadata_item(atsc3_mbms_metadata_envelope);
			free(atsc3_mbms_metadata_envelope);
			atsc3_mbms_metadata_envelope = NULL;
		}
		*atsc3_mbms_metadata_envelope_p = NULL;
	}
}
void atsc3_route_user_service_bundle_description_free(atsc3_route_user_service_bundle_description_t** atsc3_route_user_service_bundle_description_p) {
	if(atsc3_route_user_service_bundle_description_p) {
		atsc3_route_user_service_bundle_description_t* atsc3_route_user_service_bundle_description = *atsc3_route_user_service_bundle_description_p;
		if(atsc3_route_user_service_bundle_description) {

			if(atsc3_route_user_service_bundle_description->atsc3_user_service_description) {
				atsc3_user_service_description_free_atsc3_user_service_delivery_method(atsc3_route_user_service_bundle_description->atsc3_user_service_description);

				freeclean((void**)&atsc3_route_user_service_bundle_description->atsc3_user_service_description->service_language);
				freeclean((void**)&atsc3_route_user_service_bundle_description->atsc3_user_service_description);
			}
			free(atsc3_route_user_service_bundle_description);
			atsc3_route_user_service_bundle_description = NULL;
		}
		*atsc3_route_user_service_bundle_description_p = NULL;
	}
}

void atsc3_route_s_tsid_free(atsc3_route_s_tsid_t** atsc3_route_s_tsid_p) {
	if(atsc3_route_s_tsid_p) {
		atsc3_route_s_tsid_t* atsc3_route_s_tsid = *atsc3_route_s_tsid_p;
		if(atsc3_route_s_tsid) {

			atsc3_route_s_tsid_free_atsc3_route_s_tsid_RS(atsc3_route_s_tsid);
			free(atsc3_route_s_tsid);
			atsc3_route_s_tsid = NULL;
		}
		*atsc3_route_s_tsid_p = NULL;
	}
}
void atsc3_route_mpd_free(atsc3_route_mpd_t** atsc3_route_mpd_p) {
	if(atsc3_route_mpd_p) {
		atsc3_route_mpd_t* atsc3_route_mpd = *atsc3_route_mpd_p;
		if(atsc3_route_mpd) {
			freeclean((void**)&atsc3_route_mpd->availability_start_time);
			freeclean((void**)&atsc3_route_mpd->max_segment_duration);
			freeclean((void**)&atsc3_route_mpd->min_buffer_time);
			freeclean((void**)&atsc3_route_mpd->minimum_update_period);
			freeclean((void**)&atsc3_route_mpd->profiles);
			freeclean((void**)&atsc3_route_mpd->publish_time);
			freeclean((void**)&atsc3_route_mpd->time_shift_buffer_depth);
			freeclean((void**)&atsc3_route_mpd->type);

			atsc3_route_mpd_free_atsc3_route_period(atsc3_route_mpd);

			free(atsc3_route_mpd);
			atsc3_route_mpd = NULL;
		}
		*atsc3_route_mpd_p = NULL;
	}
}
void atsc3_sls_held_fragment_free(atsc3_sls_held_fragment_t** atsc3_sls_held_fragment_p) {
	if(atsc3_sls_held_fragment_p) {
		atsc3_sls_held_fragment_t* atsc3_sls_held_fragment = *atsc3_sls_held_fragment_p;
		if(atsc3_sls_held_fragment) {
			atsc3_sls_held_fragment_free_atsc3_sls_html_entry_package(atsc3_sls_held_fragment);
			free(atsc3_sls_held_fragment);
			atsc3_sls_held_fragment = NULL;
		}
		*atsc3_sls_held_fragment_p = NULL;
	}
}


void atsc3_sls_metadata_fragments_free(atsc3_sls_metadata_fragments_t** atsc3_sls_metadata_fragments_p) {
	if(atsc3_sls_metadata_fragments_p) {
		atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = *atsc3_sls_metadata_fragments_p;
		if(atsc3_sls_metadata_fragments) {
			atsc3_mime_multipart_related_instance_free(&atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance);
			atsc3_mbms_metadata_envelope_free(&atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope);
			atsc3_route_user_service_bundle_description_free(&atsc3_sls_metadata_fragments->atsc3_route_user_service_bundle_description);
			atsc3_route_s_tsid_free(&atsc3_sls_metadata_fragments->atsc3_route_s_tsid);
			atsc3_route_mpd_free(&atsc3_sls_metadata_fragments->atsc3_route_mpd);
			atsc3_sls_held_fragment_free(&atsc3_sls_metadata_fragments->atsc3_sls_held_fragment);
			free(atsc3_sls_metadata_fragments);
			atsc3_sls_metadata_fragments = NULL;
		}
		*atsc3_sls_metadata_fragments_p = NULL;
	}
}

