/*
 * atsc3_route_dash_utils.h
 *
 *  Created on: Jul 13, 2020
 *      Author: jjustman
 */



#ifndef ATSC3_ROUTE_DASH_UTILS_H
#define ATSC3_ROUTE_DASH_UTILS_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_utils.h"
#include "atsc3_vector_builder.h"

#include "atsc3_logging_externs.h"
#include "atsc3_route_s_tsid.h"
#include "atsc3_lls_types.h"
#include "atsc3_pcre2_regex_utils.h"

#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_PATTERN "(<Representation.*?id=\"(.*?)\".*?>.*?<SegmentTemplate.*?startNumber=\"(.*?)\".*?<\\/Representation>)"
#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_FLAGS "msg"
#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_REPRESENTATION_ID_CAPTURE_REFERENCE 2
#define ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_START_OFFSET_CAPTURE_REFERENCE 3

/*
 * jjustman-2020-07-14 - these are "soft" references, do NOT free them
 */
typedef struct atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match {
	atsc3_preg2_regex_match_capture_t* 							atsc3_preg2_regex_match_capture_start_number;
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* 	atsc3_route_s_content_info_media_info;
	atsc3_sls_alc_flow_t* 										atsc3_sls_alc_flow;
} atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t;


typedef struct atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match);
} atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector, atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE_ITEM_FREE(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector);
//void atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t** atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_p);

atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector, atsc3_sls_alc_flow_v* atsc3_sls_alc_flow);

block_t* atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector, block_t* original_mpd);






#define __ROUTE_DASH_UTILS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ROUTE_DASH_UTILS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ROUTE_DASH_UTILS_INFO(...)  if(_ROUTE_DASH_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __ROUTE_DASH_UTILS_DEBUG(...) if(_ROUTE_DASH_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ROUTE_DASH_UTILS_TRACE(...) if(_ROUTE_DASH_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_PCRE2_REGEX_UTILS_H_ */
