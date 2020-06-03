/*
 * atsc3_lls_types.c
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_types.h"

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

//jjustman-2020-06-02 - adding support for multiple alc tsi flows
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_alc_monitor, atsc3_sls_alc_audio_flow);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_alc_monitor, atsc3_sls_alc_video_flow);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_alc_monitor, atsc3_sls_alc_subtitles_flow);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_alc_monitor, atsc3_sls_alc_data_flow);

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
            if(lls_sls_alc_monitor->last_mpd_payload) {
                block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
            }
            if(lls_sls_alc_monitor->last_mpd_payload_patched) {
                block_Destroy(&lls_sls_alc_monitor->last_mpd_payload_patched);
            }
            //todo: jjustman-2019-11-05: should free? atsc3_fdt_instance_t
            //atsc3_sls_metadata_fragments_t?
            
            atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);
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
ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_ITEM_FREE(atsc3_sls_alc_flow);



void atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(atsc3_sls_alc_flow_v atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow, tsi, toi_init);
	if(matching_atsc3_sls_alc_flow == NULL) {
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;
		matching_atsc3_sls_alc_flow->toi_init = toi_init;

		atsc3_sls_alc_flow_add(&atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	}
}

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow_v atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow.count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow.data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == tsi && to_check_atsc3_sls_alc_flow->toi_init == toi_init) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}

	return matching_atsc3_sls_alc_flow;
}



void atsc3_sls_alc_flow_add_entry_unique_tsi_toi_nrt(atsc3_sls_alc_flow_v atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow_find_entry_tsi_toi_nrt(atsc3_sls_alc_flow, tsi, toi);
	if(matching_atsc3_sls_alc_flow_nrt == NULL) {
		//create a new entry
		matching_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow_nrt->tsi = tsi;
		matching_atsc3_sls_alc_flow_nrt->toi = toi;
		atsc3_sls_alc_flow_add(&atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow_nrt);
	}
}

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_nrt(atsc3_sls_alc_flow_v atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow_nrt = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow_nrt = NULL;

	for(int i=0; i < atsc3_sls_alc_flow.count && !matching_atsc3_sls_alc_flow_nrt; i++) {
		to_check_atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow.data[i];
		if(to_check_atsc3_sls_alc_flow_nrt->tsi == tsi && to_check_atsc3_sls_alc_flow_nrt->toi == toi) {
			matching_atsc3_sls_alc_flow_nrt = to_check_atsc3_sls_alc_flow_nrt;
		}
	}

	return matching_atsc3_sls_alc_flow_nrt;
}


