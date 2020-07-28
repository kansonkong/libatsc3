/*
 * atsc3_sls_alc_flow.c
 *
 *  Created on: Jul 28, 2020
 *      Author: jjustman
 */

#include "atsc3_sls_alc_flow.h"

int _SLS_ALC_FLOW_INFO_ENABLED = 0;
int _SLS_ALC_FLOW_DEBUG_ENABLED = 0;
int _SLS_ALC_FLOW_TRACE_ENABLED = 0;


ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_IMPLEMENTATION(atsc3_sls_alc_flow);

void atsc3_sls_alc_flow_typedef_free(atsc3_sls_alc_flow_t** atsc3_sls_alc_flow_p) {
	if(atsc3_sls_alc_flow_p) {
		atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = *atsc3_sls_alc_flow_p;
		if(atsc3_sls_alc_flow) {
			if(atsc3_sls_alc_flow->media_info) {
				atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_free(&atsc3_sls_alc_flow->media_info);
			}
			if(atsc3_sls_alc_flow->fdt_file_content_type) {
				free(atsc3_sls_alc_flow->fdt_file_content_type);
				atsc3_sls_alc_flow->fdt_file_content_type = NULL;
			}

			free(atsc3_sls_alc_flow);
			atsc3_sls_alc_flow = NULL;
		}
		*atsc3_sls_alc_flow_p = NULL;
	}
}

//for matching contentInfo.mediaInfo@repId
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi(atsc3_sls_alc_flow, tsi);
	if(matching_atsc3_sls_alc_flow == NULL) {
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi: adding new entry to %p, tsi: %d\n", &atsc3_sls_alc_flow, tsi);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_SLS_ALC_FLOW_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d", &atsc3_sls_alc_flow, tsi);
		}

		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	}

	return matching_atsc3_sls_alc_flow;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow, tsi, toi_init);
	if(matching_atsc3_sls_alc_flow == NULL) {
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: adding new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;
		matching_atsc3_sls_alc_flow->toi_init = toi_init;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_SLS_ALC_FLOW_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
		}

		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	}

	return matching_atsc3_sls_alc_flow;
}

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow->data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == tsi && to_check_atsc3_sls_alc_flow->toi_init == toi_init) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}

	return matching_atsc3_sls_alc_flow;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow->data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == tsi) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}
	_ATSC3_SLS_ALC_FLOW_TRACE("atsc3_sls_alc_flow_find_entry_tsi: couldn't find flow in %p, count: %d, tsi: %d\n", atsc3_sls_alc_flow, atsc3_sls_alc_flow->count, tsi);
	return matching_atsc3_sls_alc_flow;
}

// jjustman-2020-07-14: removed - use atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone intead

//void atsc3_sls_alc_flow_set_rep_id_if_null(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, char* rep_id) {
//	if(atsc3_sls_alc_flow && atsc3_sls_alc_flow->rep_id == NULL && rep_id != NULL) {
//		atsc3_sls_alc_flow->rep_id = strndup(rep_id, strlen(rep_id));
//	}
//}
//
//void atsc3_sls_alc_flow_set_lang_if_null(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, char* lang) {
//	if(atsc3_sls_alc_flow && atsc3_sls_alc_flow->lang == NULL && lang != NULL) {
//		atsc3_sls_alc_flow->lang = strndup(lang, strlen(lang));
//	}
//}



atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_nrt(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow_find_entry_tsi_toi_nrt(atsc3_sls_alc_flow, tsi, toi);
	if(matching_atsc3_sls_alc_flow_nrt == NULL) {
		//create a new entry
		matching_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow_nrt->tsi = tsi;
		matching_atsc3_sls_alc_flow_nrt->toi = toi;
		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow_nrt);
	}

	return matching_atsc3_sls_alc_flow_nrt;
}

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_nrt(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow_nrt = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow_nrt = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->count && !matching_atsc3_sls_alc_flow_nrt; i++) {
		to_check_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow->data[i];
		if(to_check_atsc3_sls_alc_flow_nrt->tsi == tsi && to_check_atsc3_sls_alc_flow_nrt->toi == toi) {
			matching_atsc3_sls_alc_flow_nrt = to_check_atsc3_sls_alc_flow_nrt;
		}
	}

	return matching_atsc3_sls_alc_flow_nrt;
}

void atsc3_sls_alc_flow_nrt_set_fdt_file_content_type_if_null(atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow_nrt, char* fdt_file_content_type) {
	if(matching_atsc3_sls_alc_flow_nrt && matching_atsc3_sls_alc_flow_nrt->fdt_file_content_type == NULL && fdt_file_content_type != NULL) {
		matching_atsc3_sls_alc_flow_nrt->fdt_file_content_type = strndup(fdt_file_content_type, strlen(fdt_file_content_type));
	}
}


uint32_t atsc3_sls_alc_flow_get_first_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_tsi = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_tsi = atsc3_sls_alc_flow->data[0]->tsi;
	}

	return matching_tsi;
}


uint32_t atsc3_sls_alc_flow_get_last_closed_toi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_last_closed_toi = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_last_closed_toi = atsc3_sls_alc_flow->data[0]->last_closed_toi;
	}

	return matching_last_closed_toi;
}



uint32_t atsc3_sls_alc_flow_get_first_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_toi_init = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_toi_init = atsc3_sls_alc_flow->data[0]->toi_init;
	}

	return matching_toi_init;
}
