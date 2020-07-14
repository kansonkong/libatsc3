/*
 * atsc3_route_dash_utils.c
 *
 *  Created on: Jul 6, 2020
 *      Author: jjustman
 */

#include "atsc3_route_dash_utils.h"

int _ROUTE_DASH_UTILS_INFO_ENABLED = 1;
int _ROUTE_DASH_UTILS_DEBUG_ENABLED = 0;
int _ROUTE_DASH_UTILS_TRACE_ENABLED = 0;

void atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector, atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_v) {

	__ROUTE_DASH_UTILS_DEBUG("atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches, capture group count: %d", atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count);

	for(int i=0; i < atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count; i++) {
		atsc3_preg2_regex_match_capture_group_t* atsc3_preg2_regex_match_capture_group = atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.data[i];
		char* mpd_representation_id = atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.data[ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_CAPTURE_REFERENCE]->substring->p_buffer;

		for(int j=0; j < atsc3_sls_alc_flow_v->count; j++) {
			atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_v->data[j];

			atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = atsc3_sls_alc_flow->media_info;
			__ROUTE_DASH_UTILS_DEBUG("mpd_representation_id: %s, rep_id: %s", mpd_representation_id, media_info->rep_id);

			if(media_info && media_info->rep_id) {
				if(strcasecmp(mpd_representation_id, media_info->rep_id) == 0) {
					__ROUTE_DASH_UTILS_INFO("Found match with %s to rep_id: %s, contentType: %s, tsi: %d, last_closed_toi: %d",
							mpd_representation_id, media_info->rep_id, media_info->content_type, atsc3_sls_alc_flow->tsi, atsc3_sls_alc_flow->last_closed_toi);

				}
			}
		}
	}

}
