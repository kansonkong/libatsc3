/*
 * atsc3_lls_types.c
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_types.h"

int _LLS_TYPES_INFO_ENABLED = 1;
int _LLS_TYPES_DEBUG_ENABLED = 0;
int _LLS_TYPES_TRACE_ENABLED = 0;


//vector impl's for lls slt management

//atsc3_lls_slt_service
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_service, atsc3_slt_simulcast_tsid);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_service, atsc3_slt_svc_capabilities);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_service, atsc3_slt_broadcast_svc_signalling);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_service, atsc3_slt_svc_inet_url);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_service, atsc3_slt_other_bsid);

//atsc3_lls_slt_table
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_table, atsc3_slt_capabilities);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_table, atsc3_slt_ineturl);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_lls_slt_table, atsc3_lls_slt_service);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_signed_multi_table, atsc3_signed_multi_table_lls_payload);
//default for now
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_signed_multi_table_lls_payload);

//lls_sls_alc_session_vector
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_alc_session_flows, lls_sls_alc_session);


//lls_sls_alc_session_vector
//TODO: jjustman-2019-10-03
//lls_sls_mmt_monitor_new needs 	lls_slt_mmt_session->sls_relax_source_ip_check = 1;

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_mmt_session_flows, lls_sls_mmt_session);

//lls_slt_service_id_group_id_cache
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_service_id_group_id_cache, atsc3_lls_slt_service_cache);

//lls_slt_monitor
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_slt_service_id);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_sls_mmt_monitor);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_sls_mmt_session_flows);

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(lls_slt_monitor);
//TODO: jjustman-2019-11-09 -  pass this by &lls_sls_alc_monitor so we can properly null our callees reference
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_sls_alc_monitor);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_sls_alc_session_flows);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_slt_monitor, lls_slt_service_id_group_id_cache);

//tracking for ROUTE object recovery
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_sls_alc_flow, atsc3_route_object);


/**
 *
 * jjustman-2019-10-03 - todo: _free methods:
 *
 */

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_simulcast_tsid); //no pointers present in struct

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_svc_capabilities); //no pointers present in struct

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_broadcast_svc_signalling);
void atsc3_slt_broadcast_svc_signalling_free(atsc3_slt_broadcast_svc_signalling_t** atsc3_slt_broadcast_svc_signalling_p) {
    if(atsc3_slt_broadcast_svc_signalling_p) {
        atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = *atsc3_slt_broadcast_svc_signalling_p;
        if(atsc3_slt_broadcast_svc_signalling) {
            freeclean((void**)&atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
            freeclean((void**)&atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
            freeclean((void**)&atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);
            
            //other interior members here
            freesafe(atsc3_slt_broadcast_svc_signalling);
            atsc3_slt_broadcast_svc_signalling = NULL;
        }
        *atsc3_slt_broadcast_svc_signalling_p = NULL;
    }
}


//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_svc_inet_url);
void atsc3_slt_svc_inet_url_free(atsc3_slt_svc_inet_url_t** atsc3_slt_svc_inet_url_p) {
    if(atsc3_slt_svc_inet_url_p) {
        atsc3_slt_svc_inet_url_t* atsc3_slt_svc_inet_url = *atsc3_slt_svc_inet_url_p;
        if(atsc3_slt_svc_inet_url) {
            freeclean((void**)&atsc3_slt_svc_inet_url->url);
            freesafe(atsc3_slt_svc_inet_url);
            atsc3_slt_svc_inet_url = NULL;
        }
        *atsc3_slt_svc_inet_url_p = NULL;
    }
}

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_other_bsid);   //no pointers
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_capabilities); //no pointers

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_slt_ineturl);
void atsc3_slt_ineturl_free(atsc3_slt_ineturl_t** atsc3_slt_ineturl_p) {
    if(atsc3_slt_ineturl_p) {
        atsc3_slt_ineturl_t* atsc3_slt_ineturl = *atsc3_slt_ineturl_p;
        if(atsc3_slt_ineturl) {
            freeclean((void**)&atsc3_slt_ineturl->url);
            freesafe(atsc3_slt_ineturl);
            atsc3_slt_ineturl = NULL;
        }
        *atsc3_slt_ineturl_p = NULL;
    }
}

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_lls_slt_service);
void atsc3_lls_slt_service_free(atsc3_lls_slt_service_t** atsc3_lls_slt_service_p) {
    if(atsc3_lls_slt_service_p) {
        atsc3_lls_slt_service_t* atsc3_lls_slt_service = *atsc3_lls_slt_service_p;
        if(atsc3_lls_slt_service) {
            freeclean((void**)&atsc3_lls_slt_service->global_service_id);
            freeclean((void**)&atsc3_lls_slt_service->short_service_name);
            freeclean((void**)&atsc3_lls_slt_service->drm_system_id);
            atsc3_lls_slt_service_free_atsc3_slt_simulcast_tsid(atsc3_lls_slt_service);
            atsc3_lls_slt_service_free_atsc3_slt_svc_capabilities(atsc3_lls_slt_service);
            atsc3_lls_slt_service_free_atsc3_slt_broadcast_svc_signalling(atsc3_lls_slt_service);
            atsc3_lls_slt_service_free_atsc3_slt_svc_inet_url(atsc3_lls_slt_service);
            atsc3_lls_slt_service_free_atsc3_slt_other_bsid(atsc3_lls_slt_service);
            
            freesafe(atsc3_lls_slt_service);
            atsc3_lls_slt_service = NULL;
        }
        *atsc3_lls_slt_service_p = NULL;
    }
}


ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_slt_service_id); //no pointers

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_sls_mmt_monitor);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_sls_mmt_session_flows);

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_sls_alc_monitor);
void lls_sls_alc_monitor_free(lls_sls_alc_monitor_t** lls_sls_alc_monitor_p) {
    if(lls_sls_alc_monitor_p) {
        lls_sls_alc_monitor_t* lls_sls_alc_monitor = *lls_sls_alc_monitor_p;
        if(lls_sls_alc_monitor) {


        	//jjustman-2020-07-14 - TODO: clear
        	//atsc3_lls_slt_service
        	//lls_alc_session

        	atsc3_fdt_instance_free(&lls_sls_alc_monitor->atsc3_fdt_instance);
            atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);

            if(lls_sls_alc_monitor->last_mpd_payload) {
                block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
            }
            if(lls_sls_alc_monitor->last_mpd_payload_patched) {
                block_Destroy(&lls_sls_alc_monitor->last_mpd_payload_patched);
            }
            //todo: jjustman-2019-11-05: should free? atsc3_fdt_instance_t
            //atsc3_sls_metadata_fragments_t?
            
            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_all_mediainfo_flow_v);

            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_audio_flow_v);
            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_video_flow_v);
            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_subtitles_flow_v);
            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_data_flow_v);

            free(lls_sls_alc_monitor);
            lls_sls_alc_monitor = NULL;
        }
        *lls_sls_alc_monitor_p = NULL;
    }
    
}

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_sls_alc_session_flows);

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_lls_slt_service_cache);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(lls_slt_service_id_group_id_cache);


/*
 *
 * accessors for lls_slt_monitor and group_id/slt_id caching
 */

lls_slt_service_id_t* lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new();
	lls_slt_service_id->service_id = atsc3_lls_slt_service->service_id;
	gettimeofday(&lls_slt_service_id->time_added, NULL);
	gettimeofday(&lls_slt_service_id->time_last_slt_update, NULL);

	return lls_slt_service_id;
}

lls_slt_service_id_group_id_cache_t* lls_slt_monitor_find_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor_t* lls_slt_monitor, uint8_t lls_group_id) {
    lls_slt_service_id_group_id_cache_t* lls_slt_service_id_group_id_cache = NULL;
    for(int i=0; i < lls_slt_monitor->lls_slt_service_id_group_id_cache_v.count; i++) {
        lls_slt_service_id_group_id_cache = lls_slt_monitor->lls_slt_service_id_group_id_cache_v.data[i];
        if(lls_slt_service_id_group_id_cache->lls_group_id == lls_group_id) {
            return lls_slt_service_id_group_id_cache;
        }
    }
    return NULL;
}

lls_slt_service_id_group_id_cache_t* lls_slt_monitor_find_or_create_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor_t* lls_slt_monitor, uint8_t lls_group_id) {
    lls_slt_service_id_group_id_cache_t* lls_slt_service_id_group_id_cache = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor, lls_group_id);
    if(!lls_slt_service_id_group_id_cache) {
        lls_slt_service_id_group_id_cache = lls_slt_service_id_group_id_cache_new();
        lls_slt_service_id_group_id_cache->lls_group_id = lls_group_id;
        lls_slt_monitor_add_lls_slt_service_id_group_id_cache(lls_slt_monitor, lls_slt_service_id_group_id_cache);
    }

    return lls_slt_service_id_group_id_cache;
}


atsc3_lls_slt_service_t* lls_slt_monitor_add_or_update_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor_t* lls_slt_monitor, uint16_t lls_group_id, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {

    lls_slt_service_id_group_id_cache_t* lls_slt_service_id_group_id_cache = lls_slt_monitor_find_or_create_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor, lls_group_id);
    atsc3_lls_slt_service_t* atsc3_lls_slt_service_existing = NULL;
    bool has_service_to_update = false;

    for(int i=0; i < lls_slt_service_id_group_id_cache->atsc3_lls_slt_service_cache_v.count && !has_service_to_update; i++) {
        atsc3_lls_slt_service_existing = lls_slt_service_id_group_id_cache->atsc3_lls_slt_service_cache_v.data[i];
        if(atsc3_lls_slt_service_existing->service_id == atsc3_lls_slt_service->service_id) {
            has_service_to_update = true;
            //jjustman-2020-02-28 - hack-ish
            lls_slt_service_id_group_id_cache->atsc3_lls_slt_service_cache_v.data[i] = atsc3_lls_slt_service;
        }
    }

    if(!has_service_to_update) {
        lls_slt_service_id_group_id_cache_add_atsc3_lls_slt_service_cache(lls_slt_service_id_group_id_cache, atsc3_lls_slt_service);
    }
    return atsc3_lls_slt_service;

}


/*  lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry
 *
 *  find a LLS SLT service by service_id from lls_slt_monitor cached entries
 *
 *  note: we walk thru all group_id entries first, then find matching sls service_id
 */
atsc3_lls_slt_service_t* lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id) {
    lls_slt_service_id_group_id_cache_t* lls_slt_service_id_group_id_cache = NULL;
    atsc3_lls_slt_service_t* atsc3_lls_slt_service = NULL;

    for(int i=0; i < lls_slt_monitor->lls_slt_service_id_group_id_cache_v.count; i++) {
        lls_slt_service_id_group_id_cache = lls_slt_monitor->lls_slt_service_id_group_id_cache_v.data[i];
        for(int j=0; j < lls_slt_service_id_group_id_cache->atsc3_lls_slt_service_cache_v.count; j++) {
            atsc3_lls_slt_service = lls_slt_service_id_group_id_cache->atsc3_lls_slt_service_cache_v.data[j];
            if(atsc3_lls_slt_service->service_id == service_id) {
                return atsc3_lls_slt_service;
            }
        }
    }
    return NULL;
}

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
		_ATSC3_LLS_TYPES_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi: adding new entry to %p, tsi: %d\n", &atsc3_sls_alc_flow, tsi);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_LLS_TYPES_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_LLS_TYPES_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d", &atsc3_sls_alc_flow, tsi);
		}

		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	}

	return matching_atsc3_sls_alc_flow;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow, tsi, toi_init);
	if(matching_atsc3_sls_alc_flow == NULL) {
		_ATSC3_LLS_TYPES_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: adding new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;
		matching_atsc3_sls_alc_flow->toi_init = toi_init;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_LLS_TYPES_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_LLS_TYPES_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
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
	_ATSC3_LLS_TYPES_TRACE("atsc3_sls_alc_flow_find_entry_tsi: couldn't find flow in %p, count: %d, tsi: %d\n", atsc3_sls_alc_flow, atsc3_sls_alc_flow->count, tsi);
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


void atsc3_route_object_free(atsc3_route_object_t** atsc3_route_object_p) {
	if(atsc3_route_object_p) {
		atsc3_route_object_t* atsc3_route_object = *atsc3_route_object_p;
		if(atsc3_route_object) {
			if(atsc3_route_object->toi_received_source_bytes) {
				free(atsc3_route_object->toi_received_source_bytes);
			}
			free(atsc3_route_object);
			atsc3_route_object = NULL;
		}
		*atsc3_route_object_p = NULL;
	}
}


#define ATSC3_ROUTE_OBJECT_RECEVIED_SOURCE_BYTES_INDEX_LENGTH(toi_length) ((toi_length / 256) + 1)

void atsc3_route_object_set_toi_and_length(atsc3_route_object_t* atsc3_route_object, uint32_t toi, uint32_t toi_length) {
	atsc3_route_object->toi = toi;
	atsc3_route_object->toi_length = toi_length;
	int max_received_bytes_count = ATSC3_ROUTE_OBJECT_RECEVIED_SOURCE_BYTES_INDEX_LENGTH(toi_length);
	atsc3_route_object->toi_received_source_bytes = calloc(max_received_bytes_count, sizeof(uint8_t));
	
}
void atsc3_route_object_mark_received_byte_range(atsc3_route_object_t* atsc3_route_object,uint32_t source_byte_range_start, uint32_t source_byte_range_end) {
	//make sure we're in the bounding range
	int max_received_bytes_count = ATSC3_ROUTE_OBJECT_RECEVIED_SOURCE_BYTES_INDEX_LENGTH(atsc3_route_object->toi_length);

}

bool atsc3_route_object_is_recovered(atsc3_route_object_t* atsc3_route_object) {
	bool has_missing_source_block_bytes = false;
	int max_received_bytes_count = ATSC3_ROUTE_OBJECT_RECEVIED_SOURCE_BYTES_INDEX_LENGTH(atsc3_route_object->toi_length);
	
	for(int i=0; i < max_received_bytes_count && !has_missing_source_block_bytes; i++) {
		if(atsc3_route_object->toi_received_source_bytes[i] != 0xFF) {
			has_missing_source_block_bytes = true;
		}
	}
	
	//handle any last remaining bytes that aren't %256
	for(int j=0; j < (atsc3_route_object->toi_length - ((max_received_bytes_count-1) * 256 )); j++) {
		if(atsc3_route_object->toi_received_source_bytes[max_received_bytes_count-1] >> (7-j) != 0x1) {
			has_missing_source_block_bytes = true;
		}
	}
	
	
	return has_missing_source_block_bytes;
}

