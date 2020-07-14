/*
 * atsc3_route_dash_utils.c
 *
 *  Created on: Jul 6, 2020
 *      Author: jjustman
 */

#include "atsc3_route_dash_utils.h"

int _ROUTE_DASH_UTILS_INFO_ENABLED = 1;
int _ROUTE_DASH_UTILS_DEBUG_ENABLED = 1;
int _ROUTE_DASH_UTILS_TRACE_ENABLED = 1;


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector);

/*
 * jjustman-2020-07-14 - we must have all matching atsc3_sls_alc_flow objects with last_closed_toi before we will return back the full vector_t
 */
atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector, atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_v) {

	__ROUTE_DASH_UTILS_DEBUG("atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches, capture group count: %d", atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count);
	atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector = atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_new();

	for(int i=0; i < atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count; i++) {
		atsc3_preg2_regex_match_capture_group_t* atsc3_preg2_regex_match_capture_group = atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.data[i];
		atsc3_preg2_regex_match_capture_t* atsc3_preg2_regex_match_capture_representation_id = atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.data[ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_REPRESENTATION_ID_CAPTURE_REFERENCE];
		char* mpd_representation_id = atsc3_preg2_regex_match_capture_representation_id->substring->p_buffer;

		for(int j=0; j < atsc3_sls_alc_flow_v->count; j++) {
			atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_v->data[j];

			atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = atsc3_sls_alc_flow->media_info;
			__ROUTE_DASH_UTILS_TRACE("mpd_representation_id: %s, rep_id: %s", mpd_representation_id, media_info->rep_id);

			//only push this candidate if we have a last_closed_toi value
			if(atsc3_sls_alc_flow->last_closed_toi && media_info && media_info->rep_id) {
				if(strcasecmp(mpd_representation_id, media_info->rep_id) == 0) {
					atsc3_preg2_regex_match_capture_t* atsc3_preg2_regex_match_capture_start_number = atsc3_preg2_regex_match_capture_group->atsc3_preg2_regex_match_capture_v.data[ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_START_OFFSET_CAPTURE_REFERENCE];

					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_new();
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number = atsc3_preg2_regex_match_capture_start_number;
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info = media_info;
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow = atsc3_sls_alc_flow;

					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_add_atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match);

					__ROUTE_DASH_UTILS_DEBUG("Found match with %s to rep_id: %s, contentType: %s, tsi: %d, last_closed_toi: %d, startNumber start: %d, end: %d",
							mpd_representation_id, media_info->rep_id, media_info->content_type, atsc3_sls_alc_flow->tsi, atsc3_sls_alc_flow->last_closed_toi,
							atsc3_preg2_regex_match_capture_start_number->match_start, atsc3_preg2_regex_match_capture_start_number->match_end);

				}
			}
		}
	}

	//sanity check to make sure we have all our match_vector entries against our capture group count
	if(atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count != atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count) {
		__ROUTE_DASH_UTILS_ERROR("atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches error: capture_group_v.count: %d != matching_s_tsid_representation_v.count: %d",
				atsc3_pcre2_regex_match_capture_vector->atsc3_preg2_regex_match_capture_group_v.count,
				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

		atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector);
		return NULL;

	} else {

		return atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector;
	}
}

/*
 * perform MPD patch with startObject number from atsc3_sls_alc_flow->last_closed_toi
 *
 * note: toi is uint_32, so max value and length for this string replacement will be:
 *
 * 			4294967296
 * 			----------
 * 			1234567890
 * 			         1
 *
 * 		10 chars long + null pad == 11
 *  e.g.
 *
 *  	video-796278946.mp4v
 *  	      ---------
 *  	      123456789\0
 *
 *	NOTE: requires that regex will match from 0...N for match_start and match_end bytes, and atsc3_route_dash_find_matching_s.. is independent of repId type and ordering
 *
 */
#define ATSC3_ROUTE_DASH_PATCH_UINT32_LENGTH 11
block_t* atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector, block_t* original_mpd) {
	block_t* patched_mpd = NULL;
	char temp_buffer[ATSC3_ROUTE_DASH_PATCH_UINT32_LENGTH] = { 0 };

	if(!original_mpd || !original_mpd->p_size) {
		__ROUTE_DASH_UTILS_ERROR("original MPD is empty!");
		return NULL;
	}

	block_Rewind(original_mpd);

	patched_mpd = block_Alloc(0);

	__ROUTE_DASH_UTILS_DEBUG("atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector: have %d match tuples of <capture, media_info, alc_flow>",
								match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

	for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
		atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

		__ROUTE_DASH_UTILS_TRACE("s-tsid repId: %s, contentType: %s, startNumber replace start: %d, end: %d, toi value: %d",
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);

		int original_mpd_length_to_copy = atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start - original_mpd->i_pos;

		__ROUTE_DASH_UTILS_TRACE("capture: %d, repId: %s, copying from original mpd at %p, len: %d", i, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
									block_Get(original_mpd), original_mpd_length_to_copy);

		//copy from original_mpd->i_pos to match_start
		block_Write(patched_mpd, block_Get(original_mpd), original_mpd_length_to_copy);
		block_Seek(original_mpd, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end);
		__ROUTE_DASH_UTILS_TRACE("capture: %d, repId: %s, original mpd is now at: %p", i, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
									block_Get(original_mpd));

		snprintf(&temp_buffer, ATSC3_ROUTE_DASH_PATCH_UINT32_LENGTH, "%d", atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);
		__ROUTE_DASH_UTILS_TRACE("capture: %d, repId: %s, writing startNumber as: %s", i, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
									temp_buffer);

		block_Write(patched_mpd, &temp_buffer, strlen(temp_buffer));
	}

	__ROUTE_DASH_UTILS_TRACE("appending remaining original mpd: %p, i_pos: %d, p_size: %d to patched_mpd", block_Get(original_mpd), original_mpd->i_pos, original_mpd->p_size);

	//perform one closing block_write of original mpd
	block_AppendFromSrciPos(patched_mpd, original_mpd);
	block_Rewind(patched_mpd);

	__ROUTE_DASH_UTILS_TRACE("patched mpd is:\n", block_Get(patched_mpd));

	return patched_mpd;
}
