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
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_NO_CCTOR(atsc3_sls_alc_flow, atsc3_route_object);


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
            
            atsc3_sls_alc_flow_free_v(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v);


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


void atsc3_lls_sls_alc_monitor_increment_lct_packet_received_count(lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	lls_sls_alc_monitor->lct_packets_received_count++;
}

#define _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_INTERVAL_TO_CHECK_GIVEN_UP_COUNT 5000
#define _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_GIVEN_UP_SECONDS 10

//how long to keep media fragments on disk for snap-back as needed
#define _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_RECOVERY_COMPLETE_PURGE_SECONDS 60

void atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects(lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	if(lls_sls_alc_monitor->lct_packets_received_count % _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_INTERVAL_TO_CHECK_GIVEN_UP_COUNT == 0) {
		long now = gtl();

		for(int i=0; i < lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v.count; i++) {
			atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v.data[i];
			if(atsc3_sls_alc_flow->atsc3_route_object_v.count) {
				for(int j=0; j < atsc3_sls_alc_flow->atsc3_route_object_v.count; j++) {
					atsc3_route_object_t* atsc3_route_object = atsc3_sls_alc_flow->atsc3_route_object_v.data[j];

					bool should_free_and_unlink = false;

					if(atsc3_route_object->recovery_complete_timestamp) {
						if(atsc3_route_object->recovery_complete_timestamp < (now - _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_RECOVERY_COMPLETE_PURGE_SECONDS * 1000)) {
							should_free_and_unlink = true;
							_ATSC3_LLS_TYPES_INFO("atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects: recovery complete candidate: route_object: %p, recovery_complete_timestamp: %.4f (delta: %.4f), tsi: %d, toi: %d, object_length: %d, final_object_recovery_filename_for_eviction: %s",
									atsc3_route_object,
									atsc3_route_object->recovery_complete_timestamp / 1000.0,
									(now - atsc3_route_object->recovery_complete_timestamp) / 1000.0,
									atsc3_route_object->tsi,
									atsc3_route_object->toi,
									atsc3_route_object->object_length,
									atsc3_route_object->final_object_recovery_filename_for_eviction);

						}
					}


					//has given up flow - _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_GIVEN_UP_SECONDS
					if(!should_free_and_unlink && atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received) {
						if(atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received->most_recent_received_timestamp < (now - _ATSC3_LLS_SLS_ALC_MONITOR_LCT_PACKETS_GIVEN_UP_SECONDS * 1000)) {

							uint32_t computed_payload_received_size = 0;
							for(int k=0; k < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count; k++) {
								computed_payload_received_size += atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[k]->packet_len;
							}

							//jjustman: TODO: 2020-08-04 - flag objects with no length...
							if(!atsc3_route_object->object_length || computed_payload_received_size < atsc3_route_object->object_length) {
								should_free_and_unlink = true;

								_ATSC3_LLS_TYPES_INFO("atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects: give_up candidate route_object: %p, given up timestamp: %.4f (delta: %.4f), tsi: %d, toi: %d, object_length: %d, computed_payload_received_size: %d, lct_packets_received: %d, expected: %d",
										atsc3_route_object,
										atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received->most_recent_received_timestamp / 1000.0,
										(now - atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received->most_recent_received_timestamp) / 1000.0,
										atsc3_route_object->tsi,
										atsc3_route_object->toi,
										atsc3_route_object->object_length,
										computed_payload_received_size,
										atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
										atsc3_route_object->expected_route_object_lct_packet_count);
							} else if(!atsc3_route_object->recovery_complete_timestamp) {
								_ATSC3_LLS_TYPES_WARN("atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects: STALE rotue object? give_up candidate route_object: %p, given up timestamp: %.4f (delta: %.4f), tsi: %d, toi: %d, object_length: %d, computed_payload_received_size: %d, lct_packets_received: %d, expected: %d",
																		atsc3_route_object,
																		atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received->most_recent_received_timestamp / 1000.0,
																		(now - atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received->most_recent_received_timestamp) / 1000.0,
																		atsc3_route_object->tsi,
																		atsc3_route_object->toi,
																		atsc3_route_object->object_length,
																		computed_payload_received_size,
																		atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
																		atsc3_route_object->expected_route_object_lct_packet_count);
							}
						}
					}

					if(should_free_and_unlink) {
						atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received(atsc3_route_object);
						atsc3_sls_alc_flow_remove_atsc3_route_object(atsc3_sls_alc_flow, atsc3_route_object);
						atsc3_route_object_free(&atsc3_route_object);
						j = 0; //start us back at the beginning...
					}
				}
			}

			_ATSC3_LLS_TYPES_DEBUG("atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects: completed atsc3_sls_alc_flow: %p, with atsc3_route_objects.count: %d",
					atsc3_sls_alc_flow,
					atsc3_sls_alc_flow->atsc3_route_object_v.count);
		}
	}

}

bool atsc3_lls_sls_alc_monitor_sls_metadata_fragements_has_held_changed(lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor, atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_pending) {
	bool has_changed = false;
	atsc3_sls_held_fragment_t* atsc3_sls_held_fragment = NULL;
	atsc3_sls_held_fragment_t* atsc3_sls_held_fragment_pending = NULL;

	if(atsc3_lls_sls_alc_monitor && atsc3_lls_sls_alc_monitor->atsc3_sls_metadata_fragments && atsc3_lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment) {
		atsc3_sls_held_fragment = atsc3_lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment;
	}

	if(atsc3_sls_metadata_fragments_pending && atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment) {
		atsc3_sls_held_fragment_pending = atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment;
	}

	has_changed = atsc3_sls_held_fragment_has_changed(atsc3_sls_held_fragment, atsc3_sls_held_fragment_pending); //nulls are checked in atsc3_sls_held_fragment_has_changed

	return has_changed;
}

/*
 * atsc3_sls_metadata_fragements_get_sls_held_fragment_duplicate_raw_xml_or_empty
 *
 * jjustman-2020-08-05
 * empty HELD payload will just be
 *
 * caller must invoke block_Destroy(&block) when complete...
 */
static const char* __ATSC3_HELD_FRAGMENT_EMPTY_PAYLOAD__ = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<HELD xmlns=\"tag:atsc.org,2016:XMLSchemas/ATSC3/AppSignaling/HELD/1.0/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n</HELD>\n";
block_t* atsc3_sls_metadata_fragements_get_sls_held_fragment_duplicate_raw_xml_or_empty(atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_pending) {
	block_t* atsc3_sls_held_fragment_raw_xml = NULL;

	if(atsc3_sls_metadata_fragments_pending && atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment && atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment->raw_xml_fragment && atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment->raw_xml_fragment->p_size) {
		atsc3_sls_held_fragment_raw_xml = block_Duplicate(atsc3_sls_metadata_fragments_pending->atsc3_sls_held_fragment->raw_xml_fragment);
	} else {
		atsc3_sls_held_fragment_raw_xml = block_Promote(__ATSC3_HELD_FRAGMENT_EMPTY_PAYLOAD__);
	}

	block_Rewind(atsc3_sls_held_fragment_raw_xml);

	return atsc3_sls_held_fragment_raw_xml;
}



