/*
 * atsc3_core_service_player_bridge.cpp
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 *
 * Android MMT MFU Playback with SLS event driven callbacks
 *
 *
fv * Note: Atsc3NdkPHYBridge - Android NDK Binding against Lowasys API are not included
 */

#ifndef __JJ_PHY_MMT_PLAYER_BRIDGE_DISABLED

#include "atsc3_core_service_player_bridge.h"

//jjustman-2020-12-02 - restrict this include to local cpp, as downstream projects otherwise would need to have <pcre2.h> on their include path
#include "atsc3_alc_utils.h"

int _ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO_ENABLED = 0;
int _ATSC3_CORE_SERVICE_PLAYER_BRIDGE_DEBUG_ENABLED = 0;
int _ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE_ENABLED = 0;

IAtsc3NdkApplicationBridge* Atsc3NdkApplicationBridge_ptr = NULL;
IAtsc3NdkPHYBridge*         Atsc3NdkPHYBridge_ptr = NULL;

atsc3_link_mapping_table*   atsc3_link_mapping_table_last = NULL;

//commandline stream filtering
uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

recursive_mutex atsc3_core_service_player_bridge_context_mutex;

//jjustman-2019-10-03 - context event callbacks...
lls_slt_monitor_t* lls_slt_monitor = NULL;

//mmtp/sls flow management

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = NULL;

//route/alc specific parameters
lls_sls_alc_monitor_t* lls_sls_alc_monitor = NULL;
atsc3_alc_arguments_t* alc_arguments = NULL;

std::string atsc3_ndk_cache_temp_folder_path = "";

IAtsc3NdkApplicationBridge* atsc3_ndk_application_bridge_get_instance() {
    return Atsc3NdkApplicationBridge_ptr;
}

IAtsc3NdkPHYBridge* atsc3_ndk_phy_bridge_get_instance() {
    return Atsc3NdkPHYBridge_ptr;
}

void atsc3_core_service_application_bridge_init(IAtsc3NdkApplicationBridge* atsc3NdkApplicationBridge) {
    Atsc3NdkApplicationBridge_ptr = atsc3NdkApplicationBridge;
    printf("atsc3_core_service_application_bridge_init with Atsc3NdkApplicationBridge_ptr: %p", Atsc3NdkApplicationBridge_ptr);
    Atsc3NdkApplicationBridge_ptr->LogMsgF("atsc3_core_service_application_bridge_init - Atsc3NdkApplicationBridge_ptr: %p", Atsc3NdkApplicationBridge_ptr);

    //set global logging levels
    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 0;
    _ALC_UTILS_IOTRACE_ENABLED = 0;
    _ROUTE_SLS_PROCESSOR_INFO_ENABLED = 1;
    _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED = 0;
    _ALC_UTILS_IOTRACE_ENABLED = 0;

#ifdef __SIGNED_MULTIPART_LLS_DEBUGGING__
        _LLS_TRACE_ENABLED = 1;
    _LLS_DEBUG_ENABLED = 1;

    _LLS_SLT_PARSER_DEBUG_ENABLED = 1;
    _LLS_SLT_PARSER_TRACE_ENABLED = 1;
#endif
    _LLS_ALC_UTILS_DEBUG_ENABLED = 0;
    _ALC_UTILS_DEBUG_ENABLED = 0;
    _ALC_RX_TRACE_ENABLED = 0;

    //jjustman-2020-04-23 - TLV parsing metrics enable inline ALP parsing
    __ATSC3_SL_TLV_USE_INLINE_ALP_PARSER_CALL__ = 1;

    atsc3_core_service_application_bridge_reset_context();

    atsc3_ndk_cache_temp_folder_path = Atsc3NdkApplicationBridge_ptr->get_android_temp_folder();

    //jjustman-2020-04-16 - hack to clean up cache directory payload and clear out any leftover cache objects (e.g. ROUTE/DASH toi's)
    //no linkage forfs::remove_all(atsc3_ndk_cache_temp_folder_path + "/");
    //https://github.com/android/ndk/issues/609

    atsc3_ndk_cache_temp_folder_purge((char*)(atsc3_ndk_cache_temp_folder_path).c_str());

    chdir(atsc3_ndk_cache_temp_folder_path.c_str());

    Atsc3NdkApplicationBridge_ptr->LogMsgF("atsc3_phy_player_bridge_init - completed, temp folder path: %s", atsc3_ndk_cache_temp_folder_path.c_str());
    /**
     * additional SLS monitor related callbacks wired up in
     *
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;
        //write up event callback for alc MPD patching
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched = &atsc3_lls_sls_alc_on_route_mpd_patched_ndk;
     */
}

/*
 * jjustman-2020-08-31 - todo: refactor this into a context handle
 */
void atsc3_core_service_application_bridge_reset_context() {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    if(atsc3_mmt_mfu_context) {
          atsc3_mmt_mfu_context_free(&atsc3_mmt_mfu_context);
    }

    if (lls_slt_monitor) {
        atsc3_lls_slt_monitor_free(&lls_slt_monitor);
        lls_sls_alc_monitor = NULL;
    }

    lls_slt_monitor = lls_slt_monitor_create();
    //wire up a lls event for SLS table
    lls_slt_monitor->atsc3_lls_on_sls_table_present_callback = &atsc3_lls_on_sls_table_present_ndk;
    lls_slt_monitor->atsc3_lls_on_aeat_table_present_callback = &atsc3_lls_on_aeat_table_present_ndk;

    //MMT/MFU callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_callbacks_default_jni_new();

    //jjustman-2020-12-08 - wire up atsc3_mmt_signalling_information_on_routecomponent_message_present and atsc3_mmt_signalling_information_on_held_message_present
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_routecomponent_message_present = &atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_held_message_present = &atsc3_mmt_signalling_information_on_held_message_present_ndk;
}

void atsc3_core_service_phy_bridge_init(IAtsc3NdkPHYBridge* atsc3NdkPHYBridge) {
	Atsc3NdkPHYBridge_ptr = atsc3NdkPHYBridge;
	if(Atsc3NdkApplicationBridge_ptr) {
	        Atsc3NdkApplicationBridge_ptr->LogMsgF("atsc3_core_service_phy_bridge_init - Atsc3NdkPHYBridge_ptr: %p", Atsc3NdkPHYBridge_ptr);
	}
}



atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_find_from_service_id(uint16_t service_id) {
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = NULL;

    bool found_atsc3_slt_broadcast_svc_signalling = false;

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_slt_broadcast_svc_signalling_find_from_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    //broadcast_svc_signalling has cardinality (0..1), any other signalling location is represented by SvcInetUrl (0..N)
    for(int i=0; i < atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && !found_atsc3_slt_broadcast_svc_signalling; i++) {
        atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[i];

        if(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address && atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port) {
            found_atsc3_slt_broadcast_svc_signalling = true;
        }
    }

    if(found_atsc3_slt_broadcast_svc_signalling) {
        return atsc3_slt_broadcast_svc_signalling;
    } else {
        return NULL;
    }
}

//jjustman-2020-11-17 - TODO: also walk thru lls_slt_monitor->lls_slt_service_id
//jjustman-2020-11-18 - TODO - we also need to iterate over our S-TSID for our monitored service_id's to ensure
//                         all IP flows and their corresponding PLP's are listened for
//jjustman-2020-12-08 - TODO: we will also need to monitor for MMT HELD component to check if we need to add its flow for PLP listening

atsc3_slt_broadcast_svc_signalling_t* atsc3_phy_add_plp_listener_from_service_id(uint16_t service_id) {
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = NULL;

    bool found_atsc3_slt_broadcast_svc_signalling = false;
    bool atsc3_phy_notify_plp_selection_changed_called = false;

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_phy_add_plp_listener_from_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    //broadcast_svc_signalling has cardinality (0..1), any other signalling location is represented by SvcInetUrl (0..N)

    for(int i=0; i < atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && !found_atsc3_slt_broadcast_svc_signalling; i++) {
        atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[i];

        //jjustman-2020-11-18 - relax this check - for sls_protocol to just ensuring that we have a dst_ip and dst_port and then proceed with LMT matching
        // if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_MMTP || atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_ROUTE) {
        if(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address && atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port) {
            //check if we need to add this PLP based upon our LMT to the phy listener
            //if(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address
            if(atsc3_link_mapping_table_last != NULL) {
                vector<uint8_t> plps_to_listen;
                plps_to_listen.push_back(0);

                for(int j=0; j < atsc3_link_mapping_table_last->atsc3_link_mapping_table_plp_v.count && !found_atsc3_slt_broadcast_svc_signalling; j++) {
                    atsc3_link_mapping_table_plp_t* atsc3_link_mapping_table_plp = atsc3_link_mapping_table_last->atsc3_link_mapping_table_plp_v.data[j];
                    for(int k=0; k < atsc3_link_mapping_table_plp->atsc3_link_mapping_table_multicast_v.count && !found_atsc3_slt_broadcast_svc_signalling; k++) {
                        atsc3_link_mapping_table_multicast_t* atsc3_link_mapping_table_multicast = atsc3_link_mapping_table_plp->atsc3_link_mapping_table_multicast_v.data[k];

                        uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
                        uint16_t sls_destination_udp_port = parsePortIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);

                        if(atsc3_link_mapping_table_multicast->dst_ip_add == sls_destination_ip_address &&
                           atsc3_link_mapping_table_multicast->dst_udp_port == sls_destination_udp_port) {
                            Atsc3NdkApplicationBridge_ptr->LogMsgF("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: SLS adding PLP_id: %d (%s: %s)",
                                                                   atsc3_link_mapping_table_plp->PLP_ID,
                                                                   atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address,
                                                                   atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);
                            found_atsc3_slt_broadcast_svc_signalling = true;

                            bool should_add = true;
                            for(int i=0; i < plps_to_listen.size(); i++) {
                                if(plps_to_listen.at(i) == atsc3_link_mapping_table_plp->PLP_ID) {
                                    should_add = false;
                                }
                            }
                            if(should_add) {
                                plps_to_listen.push_back(atsc3_link_mapping_table_plp->PLP_ID);
                            }
                        }
                    }

                }
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_phy_add_plp_listener_from_service_id: before atsc3_phy_notify_plp_selection_changed: with %lu plp's", plps_to_listen.size());
                Atsc3NdkApplicationBridge_ptr->atsc3_phy_notify_plp_selection_changed(plps_to_listen);
                atsc3_phy_notify_plp_selection_changed_called = true;
            } else {
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_phy_add_plp_listener_from_service_id: No LMT to support serviceID selection change!");
            }
        }
    }

    if(found_atsc3_slt_broadcast_svc_signalling && atsc3_phy_notify_plp_selection_changed_called) {
        return atsc3_slt_broadcast_svc_signalling;
    } else {
        return NULL;
    }
}

/*
 * jjustman-2020-11-18 - TODO: refactor this to avoid juggling lls_slt_monitor and sls_alc and sls_mmt monitors
 */
atsc3_lls_slt_service_t* atsc3_core_service_player_bridge_set_single_monitor_a331_service_id(int service_id) {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    //clear out our lls_slt_monitor->lls_slt_service_id
    if(lls_slt_monitor) {
        lls_slt_monitor_clear_lls_slt_service_id(lls_slt_monitor);
    }

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: with service_id: %d", service_id);

    //find our PLP from LMT with our slt_broadcast_svc_signalling ip/port to update PHY
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_plp_selection_changed = NULL;
    atsc3_slt_broadcast_svc_signalling_plp_selection_changed = atsc3_phy_add_plp_listener_from_service_id(service_id);

    if(!atsc3_slt_broadcast_svc_signalling_plp_selection_changed) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: atsc3_phy_add_plp_listener_from_service_id: unable to find LMT mapping for service_id: %d, acquisition may fail!", service_id);
    }

    //find our matching LLS service, then assign a monitor reference
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = NULL;
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_mmt = NULL;
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_route = NULL;

    atsc3_slt_broadcast_svc_signalling = atsc3_slt_broadcast_svc_signalling_find_from_service_id(service_id);
    if(!atsc3_slt_broadcast_svc_signalling) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: atsc3_slt_broadcast_svc_signalling_find_from_service_id: unable to find atsc3_slt_broadcast_svc_signalling_t for service_id: %d", service_id);
        return NULL;
    }

    if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_MMTP) {
        atsc3_slt_broadcast_svc_signalling_mmt = atsc3_slt_broadcast_svc_signalling;
    } else if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_ROUTE) {
        atsc3_slt_broadcast_svc_signalling_route = atsc3_slt_broadcast_svc_signalling;
    }

    //wire up MMT, watch out for potentally free'd sessions that aren't NULL'd out properly..
    if(atsc3_slt_broadcast_svc_signalling_mmt != NULL) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: service_id: %d - using MMT with flow: sip: %s, dip: %s:%s",
               service_id,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_source_ip_address,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_destination_ip_address,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_destination_udp_port);

        //clear any active SLS monitors, don't destroy our serviceId flows
        lls_slt_monitor_clear_lls_sls_mmt_monitor(lls_slt_monitor);

        //TODO - remove this logic to a unified process...
        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);
        lls_slt_monitor->lls_sls_alc_monitor = NULL;
        lls_sls_alc_monitor = NULL; //make sure to clear out our ref

        lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
        lls_sls_mmt_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!
        lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

        lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

        //we may not be initialized yet, so re-check again later
        //this should _never_happen...
        lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);

        if(!lls_sls_mmt_session) {
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
        }
        lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
        lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

        lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);

        //clear out atsc3_mmt_mfu_context elements, e.g. our ROUTEComponent entry (if present)
        if(atsc3_mmt_mfu_context) {
            if(atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored) {
                atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored->__is_pinned_to_context = false;
                mmt_atsc3_route_component_free(&atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored);
            }
        }

    } else {
        //jjustman-2020-09-17 - use _clear, but keep our lls_sls_mmt_session_flows
        //todo: release any internal lls_sls_mmt_monitor handles
        lls_slt_monitor_clear_lls_sls_mmt_monitor(lls_slt_monitor);
        lls_slt_monitor->lls_sls_mmt_monitor = NULL;
        lls_sls_mmt_monitor = NULL;
    }

    //wire up ROUTE
    if(atsc3_slt_broadcast_svc_signalling_route != NULL) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: service_id: %d - using ROUTE with flow: sip: %s, dip: %s:%s",
               service_id,
               atsc3_slt_broadcast_svc_signalling_route->sls_source_ip_address,
               atsc3_slt_broadcast_svc_signalling_route->sls_destination_ip_address,
               atsc3_slt_broadcast_svc_signalling_route->sls_destination_udp_port);

        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);

        lls_sls_alc_monitor = lls_sls_alc_monitor_create();
        lls_sls_alc_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service;
        lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
        lls_sls_alc_monitor->has_discontiguous_toi_flow = true; //jjustman-2020-07-27 - hack-ish

        lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
        lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

        lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
        if(!lls_sls_alc_session) {
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
        }
        lls_sls_alc_monitor->lls_alc_session = lls_sls_alc_session;
        lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;

        lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);


        //wire up event callback for alc close_object notification
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;

        //wire up event callback for alc MPD patching
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_callback = &atsc3_lls_sls_alc_on_route_mpd_patched_ndk;
        //jjustman-2020-08-05 - also atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback

        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_package_extract_completed_callback = &atsc3_lls_sls_alc_on_package_extract_completed_callback_ndk;

        //#1569
        lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback = &atsc3_sls_on_held_trigger_received_callback_impl;
        //jjustman-2020-08-05 - also atsc3_sls_on_held_trigger_received_with_version_callback

    } else {
        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);
        if(lls_slt_monitor->lls_sls_alc_monitor) {
            lls_sls_alc_monitor_free(&lls_slt_monitor->lls_sls_alc_monitor);
        }
        lls_sls_alc_monitor = NULL;
    }

    return atsc3_lls_slt_service;
}
//jjustman-2020-11-17 - todo: add in additional PLP listener for this service (e.g. for ESG service acquisition)
atsc3_lls_slt_service_t* atsc3_core_service_player_bridge_add_monitor_a331_service_id(int service_id) {
    lls_sls_alc_monitor_t* lls_sls_alc_monitor_to_add = NULL;

    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_player_bridge_add_monitor_a331_service_id: with service_id: %d", service_id);

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_player_bridge_add_monitor_a331_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    //find our PLP from LMT with our slt_broadcast_svc_signalling ip/port to update PHY
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_plp_selection_changed = NULL;
    atsc3_slt_broadcast_svc_signalling_plp_selection_changed = atsc3_phy_add_plp_listener_from_service_id(service_id);

    if(!atsc3_slt_broadcast_svc_signalling_plp_selection_changed) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_add_monitor_a331_service_id: atsc3_phy_add_plp_listener_from_service_id: unable to find LMT mapping for service_id: %d, acquisition may fail!", service_id);
    }

    //find our matching LLS service, then assign a monitor reference
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_route_to_add_monitor = NULL;

    atsc3_slt_broadcast_svc_signalling_route_to_add_monitor = atsc3_slt_broadcast_svc_signalling_find_from_service_id(service_id);
    if(!atsc3_slt_broadcast_svc_signalling_route_to_add_monitor) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_player_bridge_add_monitor_a331_service_id: atsc3_slt_broadcast_svc_signalling_find_from_service_id: unable to find atsc3_slt_broadcast_svc_signalling_route_to_add_monitor for service_id: %d", service_id);
        return NULL;
    }

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_player_bridge_add_monitor_a331_service_id: service_id: %d - adding ROUTE with flow: sip: %s, dip: %s:%s",
           service_id,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_source_ip_address,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_destination_ip_address,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_destination_udp_port);

    lls_sls_alc_monitor_to_add = lls_sls_alc_monitor_create();
    lls_sls_alc_monitor_to_add->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true; //jjustman-2020-11-17 - todo: fix me
    lls_sls_alc_monitor_to_add->atsc3_lls_slt_service = atsc3_lls_slt_service;

    lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
    lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

    //jjustman-2020-11-17 - add lls_sls_alc_session_flows_v

    lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
    if(!lls_sls_alc_session) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_add_monitor_a331_service_id: lls_slt_alc_session_find_from_service_id: %d, lls_sls_alc_session is NULL!", service_id);
        return NULL;
    }
    lls_sls_alc_monitor_to_add->lls_alc_session = lls_sls_alc_session;
    lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor_to_add);

    //add in supplimentary callback hook for additional ALC emissions
    lls_sls_alc_monitor_to_add->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;


    //add a supplimentry sls_alc monitor
    // TODO: fix me? NOTE: do not replace the primary lls_slt_monitor->lls_sls_alc_monitor entry if set
    if(!lls_slt_monitor->lls_sls_alc_monitor) {
        lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;
    }

    return atsc3_lls_slt_service;
}

atsc3_lls_slt_service_t* atsc3_core_service_player_bridge_remove_monitor_a331_service_id(int service_id) {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    if(!lls_slt_monitor->lls_sls_alc_monitor || !lls_slt_monitor->lls_sls_alc_monitor_v.count) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_player_bridge_remove_monitor_a331_service_id: unable to remove service_id: %d, lls_slt_monitor->lls_sls_alc_monitor is: %p, lls_slt_monitor->lls_sls_alc_monitor_v.count is: %d",
                service_id,
                lls_slt_monitor->lls_sls_alc_monitor,
                lls_slt_monitor->lls_sls_alc_monitor_v.count);
        return NULL;
    }

    atsc3_lls_slt_service_t* lls_service_removed_lls_sls_alc_monitor = NULL;
    lls_sls_alc_monitor_t* my_lls_sls_alc_monitor_entry_to_release = NULL;
    if(lls_slt_monitor->lls_sls_alc_monitor && lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == service_id) {
        my_lls_sls_alc_monitor_entry_to_release = lls_slt_monitor->lls_sls_alc_monitor;
        lls_slt_monitor->lls_sls_alc_monitor = NULL;
    }

    //remove lls_sls_alc_monitor entry
    //TODO: jjustman-2019-11-07: hack, move this into atsc3_vector_builder.h
    for(int i=0; i < lls_slt_monitor->lls_sls_alc_monitor_v.count && !my_lls_sls_alc_monitor_entry_to_release; i++) {
        lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_slt_monitor->lls_sls_alc_monitor_v.data[i];
        if(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == service_id) {
            my_lls_sls_alc_monitor_entry_to_release = lls_sls_alc_monitor;
            for(int j=i + 1; j < lls_slt_monitor->lls_sls_alc_monitor_v.count; j++) {
                lls_slt_monitor->lls_sls_alc_monitor_v.data[j-1] = lls_slt_monitor->lls_sls_alc_monitor_v.data[j];
            }
            lls_slt_monitor->lls_sls_alc_monitor_v.data[lls_slt_monitor->lls_sls_alc_monitor_v.count-1] = NULL;
            lls_slt_monitor->lls_sls_alc_monitor_v.count--;
            //clear out last entry
        }
    }

    if(my_lls_sls_alc_monitor_entry_to_release) {
        lls_service_removed_lls_sls_alc_monitor = my_lls_sls_alc_monitor_entry_to_release->atsc3_lls_slt_service;
        lls_sls_alc_monitor_free(&my_lls_sls_alc_monitor_entry_to_release);
    }

    return lls_service_removed_lls_sls_alc_monitor;
}

//jjustman-2020-09-02 - note: non-mutex protected method, expects caller to already own a mutex for lls_slt_monitor
lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_get_from_service_id(int service_id) {

    if(!lls_slt_monitor->lls_sls_alc_monitor || !lls_slt_monitor->lls_sls_alc_monitor_v.count) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_lls_sls_alc_monitor_get_from_service_id: error searching for service_id: %d, alc_monitor is null: lls_slt_monitor->lls_sls_alc_monitor is: %p, lls_slt_monitor->lls_sls_alc_monitor_v.count is: %d",
                service_id,
                lls_slt_monitor->lls_sls_alc_monitor,
                lls_slt_monitor->lls_sls_alc_monitor_v.count);
        return NULL;
    }

    lls_sls_alc_monitor_t* lls_sls_alc_monitor_to_return = NULL;
    for(int i=0; i < lls_slt_monitor->lls_sls_alc_monitor_v.count && !lls_sls_alc_monitor_to_return; i++) {
        lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_slt_monitor->lls_sls_alc_monitor_v.data[i];
        if(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == service_id) {
            lls_sls_alc_monitor_to_return = lls_sls_alc_monitor;
            continue;
        }
    }

    if(!lls_sls_alc_monitor_to_return) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_lls_sls_alc_monitor_get_from_service_id: service_id: %d, lls_sls_alc_monitor_to_return is null!",
                service_id);
        return NULL;
    }

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_lls_sls_alc_monitor_get_from_service_id: %d, returning lls_sls_alc_monitor_to_return: %p",
           service_id,
           lls_sls_alc_monitor_to_return);

    return lls_sls_alc_monitor_to_return;
}

atsc3_sls_metadata_fragments_t* atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(int service_id) {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    lls_sls_alc_monitor_t* lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_get_from_service_id(service_id);

    if(!lls_sls_alc_monitor || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id: service_id: %d, alc_monitor or fragments were null, lls_sls_alc_monitor: %p", service_id, lls_sls_alc_monitor);
        return NULL;
    }

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id: %d, returning atsc3_sls_metadata_fragments: %p",
           service_id,
           lls_sls_alc_monitor->atsc3_sls_metadata_fragments);

    return lls_sls_alc_monitor->atsc3_sls_metadata_fragments;
}

atsc3_route_s_tsid_t* atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(int service_id) {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    lls_sls_alc_monitor_t* lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_get_from_service_id(service_id);

    if(!lls_sls_alc_monitor || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id: service_id: %d, alc_monitor or fragments or route_s_tsid were null, lls_sls_alc_monitor: %p", service_id, lls_sls_alc_monitor);
        return NULL;
    }

    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id: %d, returning atsc3_route_s_tsid: %p",
           service_id,
           lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid);

    return lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid;
}

/**
 * else {
        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);
        if(lls_slt_monitor->lls_sls_alc_monitor) {
            lls_sls_alc_monitor_free(&lls_slt_monitor->lls_sls_alc_monitor);
        }

 * @param packet
 */


//jjustman-2020-08-18 - todo: keep track of plp_num's?
void atsc3_core_service_bridge_process_packet_from_plp_and_block(uint8_t plp_num, block_t* block) {
	atsc3_core_service_bridge_process_packet_phy(block);
}

//void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
void atsc3_core_service_bridge_process_packet_phy(block_t* packet) {
    lock_guard<recursive_mutex> atsc3_core_service_player_bridge_context_mutex_local(atsc3_core_service_player_bridge_context_mutex);

    //alc types
    lls_sls_alc_session_t*                  matching_lls_slt_alc_session = NULL;
    atsc3_alc_packet_t*                     alc_packet = NULL;
    bool                                    has_matching_lls_slt_service_id = false;

    //mmt types
    lls_sls_mmt_session_t*                  matching_lls_sls_mmt_session = NULL;
    mmtp_packet_header_t*                   mmtp_packet_header = NULL;
    mmtp_asset_t*                           mmtp_asset = NULL;
    mmtp_packet_id_packets_container_t*     mmtp_packet_id_packets_container = NULL;

    mmtp_mpu_packet_t*                      mmtp_mpu_packet = NULL;
    mmtp_signalling_packet_t*               mmtp_signalling_packet = NULL;
    int8_t                                  mmtp_si_parsed_message_count = 0;

    //udp_packet_t* udp_packet = udp_packet_process_from_ptr_raw_ethernet_packet(block_Get(packet), packet->p_size);
    udp_packet_t* udp_packet = udp_packet_process_from_ptr(block_Get(packet), packet->p_size);

    if(!udp_packet) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_bridge_process_packet_phy: after udp_packet_process_from_ptr: unable to extract packet size: %d, i_pos: %d, 0x%02x 0x%02x",
                packet->p_size,
                packet->i_pos,
                packet->p_buffer[0],
                packet->p_buffer[1]);

        goto error;
    }

    //drop mdNS
    if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
        goto cleanup;
    }

    //don't auto-select service here, let the lls_slt_monitor->atsc3_lls_on_sls_table_present event callback trigger in a service selection
    if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
        if(lls_table) {
            if(lls_table->lls_table_id == SLT) {
                //capture our SLT services into alc and mmt session flows
                int retval = lls_slt_table_perform_update(lls_table, lls_slt_monitor);

                if(!retval) {
                    lls_dump_instance_table(lls_table);
                }
            } else {
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("atsc3_core_service_bridge_process_packet_phy: lls_table_id: %d", lls_table->lls_table_id);
            }
        } else {
            //LLS_table may not have been updated (e.g. lls_table_version has not changed)
        }
        goto cleanup;
    }

    __DEBUG("IP flow packet: dst_ip_addr: %u.%u.%u.%u:%u, pkt_len: %d",
           __toipnonstruct(udp_packet->udp_flow.dst_ip_addr),
           udp_packet->udp_flow.dst_port,
           udp_packet->data->p_size);

    //ALC: Find a matching SLS service from this packet flow, and if the selected atsc3_lls_slt_service is monitored, write MBMS/MPD and MDE's out to disk
    matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);

    //jjustman-2020-11-17 - TODO: filter this out to lls_slt_monitor->lls_slt_service_id_v for discrimination
    //(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == matching_lls_slt_alc_session->atsc3_lls_slt_service->service_id)
//       ((dst_ip_addr_filter != NULL && dst_ip_port_filter != NULL) && (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter))) {
//    if((lls_sls_alc_monitor && matching_lls_slt_alc_session && lls_sls_alc_monitor->atsc3_lls_slt_service && has_matching_lls_slt_service_id)  ||

    if(lls_sls_alc_monitor && matching_lls_slt_alc_session) {
        for (int i = 0; i < lls_slt_monitor->lls_slt_service_id_v.count && !has_matching_lls_slt_service_id; i++) {
            lls_slt_service_id_t *lls_slt_service_id_to_check = lls_slt_monitor->lls_slt_service_id_v.data[i];
            if (lls_slt_service_id_to_check->service_id == matching_lls_slt_alc_session->atsc3_lls_slt_service->service_id) {
                has_matching_lls_slt_service_id = true;
            }
        }

        if (has_matching_lls_slt_service_id) {

            //parse and process ALC flow
            int retval = alc_rx_analyze_packet_a331_compliant((char *) block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &alc_packet);
            if (!retval) {
                //__DEBUG("atsc3_core_service_bridge_process_packet_phy: alc_packet: %p, tsi: %d, toi: %d", alc_packet, alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

                //check our alc_packet for a wrap-around TOI value, if it is a monitored TSI, and re-patch the MBMS MPD for updated availabilityStartTime and startNumber with last closed TOI values
                atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(alc_packet, lls_slt_monitor->lls_sls_alc_monitor);

                //keep track of our EXT_FTI and update last_toi as needed for TOI length and manual set of the close_object flag
                atsc3_route_object_t *atsc3_route_object = atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(alc_packet, lls_slt_monitor->lls_sls_alc_monitor);

                //persist to disk, process sls mbms and/or emit ROUTE media_delivery_event complete to the application tier if
                //the full packet has been recovered (e.g. no missing data units in the forward transmission)
                if (atsc3_route_object) {
                    atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(&udp_packet->udp_flow, alc_packet, lls_slt_monitor->lls_sls_alc_monitor, atsc3_route_object);
                    goto cleanup;

                } else {
                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_bridge_process_packet_phy: Error in ALC persist, atsc3_route_object is NULL!");
                }
            } else {
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_bridge_process_packet_phy: Error in ALC decode: %d", retval);
            }
            goto error;
        }
    }

    //jjustman-2020-11-12 - TODO: extract this core MMT logic out for ExoPlayer MMT native depacketization

    //MMT: Find a matching SLS service from this packet flow, and if the selected atsc3_lls_slt_service is monitored, enqueue for MFU DU re-constituion and emission
    matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

	if(matching_lls_sls_mmt_session && lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id == lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id) {

        if(!atsc3_mmt_mfu_context) {
            goto error;
        }

        mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

        if(!mmtp_packet_header) {
            goto error;
        }
        mmtp_packet_header_dump(mmtp_packet_header);

        //for filtering MMT flows by a specific packet_id
        if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
            goto error;
        }

        mmtp_asset = atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context, udp_packet, lls_slt_monitor, matching_lls_sls_mmt_session);
        mmtp_packet_id_packets_container = atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context, mmtp_asset, mmtp_packet_header);

        if(mmtp_packet_header->mmtp_payload_type == 0x0) {
            mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            if(!mmtp_mpu_packet) {
                goto error;
            }

            if(mmtp_mpu_packet->mpu_timed_flag == 1) {
                mmtp_mpu_dump_header(mmtp_mpu_packet);

                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,", udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

                mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);
                goto cleanup;

            } else {
                //non-timed -
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_mpu_packet->mmtp_packet_id);
                goto error;
            }
        } else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

            mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            if(!mmtp_signalling_packet) {
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet_parse_and_free_packet_header_from_block_t - mmtp_signalling_packet was NULL for udp_packet: %p, udp_packet->p_size: %d", udp_packet, udp_packet->data->p_size);
                goto error;
            }

            mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmtp_signalling_packet(mmtp_asset, mmtp_signalling_packet);

            if(mmtp_signalling_packet->si_fragmentation_indicator == 0x0) {
                //process this SI message in-line, no need for re-assembly
                mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);

                //but clear out any possible mmtp_signalling_packets being queued for re-assembly in mmtp_packet_id_packets_container
                mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);

            } else {
                //TODO: jjustman-2019-10-03 - if signalling_packet == MP_table, set atsc3_mmt_mfu_context->mp_table_last;

                //if mmtp_signalling_packet.sl_fragmentation_indicator != 00, then
                //  handle any fragmented signallling_information packets by packet_id,
                //  persisting for depacketization into packet_buffer[] when si_fragmentation_indicator:
                //       must start with f_i: 0x1 (01)
                //          any non 0x0 (00) or 0x1 (01) with no packet_buffer[].length should be discarded
                //       next packet contains: congruent packet_sequence_number (eg old + 1 (mod 2^32 for wraparound) == new)
                //          f_i: 0x2 (10) -> append
                //          f_i: 0x3 (11) -> check for completeness
                //                          -->should have packet_buffer[0].fragmentation_counter == packet_buffer[].length
                //   mmtp_signalling_packet_process_from_payload_with_context(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);

                if(mmtp_signalling_packet->si_fragmentation_indicator != 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
                    //we should never have a case where fragmentation_indicator is _not_ the first fragment of a signalling message and have 0 packets in the re-assembly vector,
                    //it means we lost at least one previous DU for this si_messgae, so discard and goto error
                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet->si_fragmentation_indicator is: 0x%02x while mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count is 0, discarding!", mmtp_signalling_packet->si_fragmentation_indicator);

                    goto error;
                }

                //push our first mmtp_signalling_packet for re-assembly (explicit mmtp_signalling_packet->si_fragmentation_indicator == 0x1 (01))
                if(mmtp_signalling_packet->si_fragmentation_indicator == 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_id_packets_container_add_mmtp_signalling_packet - adding first entry, mmtp_signalling_packet: %p, mmtp_signalling_packet->si_fragmentation_indicator: 0x%02x, mmtp_signalling_packet->si_fragmentation_counter: %d (A: %d, H: %d)",
                                 mmtp_signalling_packet,
                                 mmtp_signalling_packet->si_fragmentation_indicator,
                                 mmtp_signalling_packet->si_fragmentation_counter,
                                 mmtp_signalling_packet->si_aggregation_flag,
                                 mmtp_signalling_packet->si_additional_length_header);

                    mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);
                    mmtp_signalling_packet = NULL;
                    goto cleanup; //continue on

                } else {
                    mmtp_signalling_packet_t *last_mmtp_signalling_packet = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count - 1];

                    //make sure our packet_sequence_number is sequential to our last mmtp_signalling_packet
                    if((last_mmtp_signalling_packet->packet_sequence_number + 1) != mmtp_signalling_packet->packet_sequence_number) {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: packet_sequence_number mismatch, discarding! last_mmtp_signalling_packet: %p, psn: %d (next: %d), frag: 0x%02x, frag_counter: %d, mmtp_signalling_packet: %p, psn: %d, frag: 0x%02x, frag_counter: %d",
                                    last_mmtp_signalling_packet,
                                    last_mmtp_signalling_packet->packet_sequence_number,
                                    (uint32_t)(last_mmtp_signalling_packet->packet_sequence_number+1),
                                    last_mmtp_signalling_packet->si_fragmentation_indicator,
                                    last_mmtp_signalling_packet->si_fragmentation_counter,
                                    mmtp_signalling_packet,
                                    mmtp_signalling_packet->packet_sequence_number,
                                    mmtp_signalling_packet->si_fragmentation_indicator,
                                    mmtp_signalling_packet->si_fragmentation_counter);

                        mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);

                        goto error;
                    }

                    bool     mmtp_signalling_packet_vector_valid = true;
                    bool     mmtp_signalling_packet_vector_complete = false;
                    uint32_t mmtp_message_payload_final_size = 0;

                    //check our vector sanity, and if we are "complete"
                    mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);
                    int mmtp_signalling_packet_vector_count = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count;

                    mmtp_signalling_packet_t* mmtp_signalling_packet_temp = NULL;
                    mmtp_signalling_packet_t* mmtp_signalling_packet_last_temp = NULL;

                    for(int i=0; i < mmtp_signalling_packet_vector_count && mmtp_signalling_packet_vector_valid; i++) {
                        mmtp_signalling_packet_t* mmtp_signalling_packet_temp = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[i];

                        if(!mmtp_signalling_packet_temp || !mmtp_signalling_packet_temp->udp_packet_inner_msg_payload) {
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp is NULL, bailing!");
                            mmtp_signalling_packet_vector_valid = false;
                            break;
                        }

                        if(mmtp_signalling_packet_last_temp && mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id) {
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id, %d != %d",
                                         mmtp_signalling_packet_last_temp->mmtp_packet_id, mmtp_signalling_packet_temp->mmtp_packet_id);
                            mmtp_signalling_packet_vector_valid = false;
                            break;
                        }

                        if(i == 0 && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x1) { //sanity check (01)
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, i == 0 but mmtp_signalling_packet_temp->si_fragmentation_indicator is: 0x%02x", mmtp_signalling_packet_temp->si_fragmentation_indicator);
                            mmtp_signalling_packet_vector_valid = false;
                            break;
                        }

                        if(mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i)) {
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i), %d != %d, bailing!",
                                         mmtp_signalling_packet_temp->si_fragmentation_counter,
                                         mmtp_signalling_packet_vector_count - 1 - i);

                            mmtp_signalling_packet_vector_valid = false;
                            break;
                        }

                        //anything less than the "last" packet should be fi==0x2 (10) (fragment that is neither the first nor the last fragment)
                        if(i < (mmtp_signalling_packet_vector_count - 2) && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x2) {
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_indicator: 0x%02x at index: %d, vector_count: %d",
                                         mmtp_signalling_packet_temp->si_fragmentation_indicator, i, mmtp_signalling_packet_vector_count);

                            mmtp_signalling_packet_vector_valid = false;
                            break;
                        }

                        mmtp_message_payload_final_size += mmtp_signalling_packet_temp->udp_packet_inner_msg_payload->p_size;

                        //if we are the last index in the vector AND our fi==0x3 (11) (last fragment of a signalling message), then mark us as complete, otherwise
                        if(i == (mmtp_signalling_packet_vector_count - 1) && mmtp_signalling_packet_temp->si_fragmentation_indicator == 0x3) {
                            mmtp_signalling_packet_vector_complete = true;
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector is complete, packet_id: %d, vector size: %d",
                                         mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
                        } else {
                            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector not yet complete, i: %d, packet_id: %d, vector size: %d",
                                         i, mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
                        }

                        mmtp_signalling_packet_last_temp = mmtp_signalling_packet_temp;
                    }

                    if(!mmtp_signalling_packet_vector_valid) {
                        mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                        mmtp_signalling_packet = NULL; //we will have already freed this packet by clearing the container
                        goto error;

                    } else if(mmtp_signalling_packet_vector_complete) {
                        //re-assemble into MSG payload for parsing
                        block_t* msg_payload_final = block_Alloc(mmtp_message_payload_final_size);
                        mmtp_signalling_packet_t* mmtp_signalling_packet_temp = NULL;

                        for(int i=0; i < mmtp_signalling_packet_vector_count; i++) {
                            mmtp_signalling_packet_temp = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[i];

                            block_AppendFromSrciPos(msg_payload_final, mmtp_signalling_packet_temp->udp_packet_inner_msg_payload);
                        }

                        //finally, we can now process our signalling_messagae
                        mmtp_signalling_packet = mmtp_packet_id_packets_container_pop_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                        block_Destroy(&mmtp_signalling_packet->udp_packet_inner_msg_payload);
                        mmtp_signalling_packet->udp_packet_inner_msg_payload = msg_payload_final;
                        block_Rewind(mmtp_signalling_packet->udp_packet_inner_msg_payload);

                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet msg_payload re-assembly is complete, using first mmtp_signalling_packet: %p, udp_packet_inner_msg_payload size: %d, value: %s",
                                     mmtp_signalling_packet,
                                     mmtp_signalling_packet->udp_packet_inner_msg_payload->p_size,
                                     mmtp_signalling_packet->udp_packet_inner_msg_payload->p_buffer);

                        mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, mmtp_signalling_packet->udp_packet_inner_msg_payload);

                    } else {
                        //noop: continue to accumulate
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet - adding to vector, size: %d", mmtp_signalling_packet_vector_count + 1);
                        mmtp_signalling_packet = NULL; //so we don't free pending accumulated packets
                    }
                }
            }

            if(mmtp_si_parsed_message_count > 0) {
                mmt_signalling_message_dump(mmtp_signalling_packet);

                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("process_packet: calling mmt_signalling_message_dispatch_context_notification_callbacks with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
                        udp_packet,
                        mmtp_signalling_packet,
                        atsc3_mmt_mfu_context);


                //dispatch our wired callbacks
                mmt_signalling_message_dispatch_context_notification_callbacks(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);

                //update our internal sls_mmt_session info
                mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);

                //clear and flush out our mmtp_packet_id_packets_container if we came from re-assembly,
                // otherwise, final free of mmtp_signalling_packet packet in :cleanup
                mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                goto cleanup;
            }
        } else {
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
            goto error;
        }

        goto cleanup;
    }

	//catchall - not really an error, just un-processed datagram
	goto cleanup;

error:
    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: error, bailing processing!");

cleanup:

    //jjustman-2020-11-12 - this should be freed already from mmtp_*_free_packet_header_from_block_t, but just in case...
	if(mmtp_packet_header) {
		mmtp_packet_header_free(&mmtp_packet_header);
	}

	//jjustman-2020-11-12 - note: do not free mmtp_mpu_packet or mmtp_signalling_packet as they may have been added to a mmtp_*_packet_collection for re-assembly
	//unless si_fragmentation_indicator == 0x0, then we can safely release, as we do not push single units to the mmtp_packet_id_packets_container->mmtp_signalling_packet_v
	if(mmtp_signalling_packet && mmtp_signalling_packet->si_fragmentation_indicator == 0x0) {
	    mmtp_signalling_packet_free(&mmtp_signalling_packet);
	}

	if(alc_packet) {
	    alc_packet_free(&alc_packet);
	}

	if(udp_packet) {
        udp_packet_free(&udp_packet);
    }
    return;

}

/**
 * NDK to JNI bridiging methods defined here
 * @param lls_table
 */


void atsc3_lls_on_sls_table_present_ndk(lls_table_t* lls_table) {

    printf("atsc3_lls_on_sls_table_present_ndk: lls_table is: %p, val: %s", lls_table, lls_table->raw_xml.xml_payload);
    if(!lls_table) {
        Atsc3NdkApplicationBridge_ptr->LogMsg("E: atsc3_lls_on_sls_table_present_ndk: no lls_table for SLS!");
        return;
    }
    if(!lls_table->raw_xml.xml_payload || !lls_table->raw_xml.xml_payload_size) {
        Atsc3NdkApplicationBridge_ptr->LogMsg("E: atsc3_lls_on_sls_table_present_ndk: no raw_xml.xml_payload for SLS!");
        return;
    }

    //copy over our xml_payload.size +1 with a null
    int len_aligned = lls_table->raw_xml.xml_payload_size + 1;
    len_aligned += 8-(len_aligned%8);
    char* xml_payload_copy = (char*)calloc(len_aligned , sizeof(char));
    strncpy(xml_payload_copy, (char*)lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);

    Atsc3NdkApplicationBridge_ptr->atsc3_onSlsTablePresent((const char*)xml_payload_copy);

    free(xml_payload_copy);
}



void atsc3_lls_on_aeat_table_present_ndk(lls_table_t* lls_table) {
    printf("atsc3_lls_on_aeat_table_present_ndk: lls_table is: %p, val: %s", lls_table, lls_table->raw_xml.xml_payload);
    if(!lls_table) {
        Atsc3NdkApplicationBridge_ptr->LogMsg("E: atsc3_lls_on_aeat_table_present_ndk: no lls_table for AEAT!");
        return;
    }
    if(!lls_table->raw_xml.xml_payload || !lls_table->raw_xml.xml_payload_size) {
        Atsc3NdkApplicationBridge_ptr->LogMsg("E: atsc3_lls_on_aeat_table_present_ndk: no raw_xml.xml_payload for AEAT!");
        return;
    }

    //copy over our xml_payload.size +1 with a null
    int len_aligned = lls_table->raw_xml.xml_payload_size + 1;
    len_aligned += 8-(len_aligned%8);
    char* xml_payload_copy = (char*)calloc(len_aligned , sizeof(char));
    strncpy(xml_payload_copy, (char*)lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);

    Atsc3NdkApplicationBridge_ptr->atsc3_onAeatTablePresent((const char*)xml_payload_copy);

    free(xml_payload_copy);
}


//TODO: jjustman-2019-11-08: wire up the service_id in which this alc_emission originated from in addition to tsi/toi
void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk(uint16_t service_id, uint32_t tsi, uint32_t toi, char* s_tsid_content_location, char* s_tsid_content_type, char* cache_file_path) {
    Atsc3NdkApplicationBridge_ptr->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(service_id, tsi, toi, s_tsid_content_location, s_tsid_content_type, cache_file_path);
}

void atsc3_lls_sls_alc_on_route_mpd_patched_ndk(uint16_t service_id) {
    Atsc3NdkApplicationBridge_ptr->atsc3_lls_sls_alc_on_route_mpd_patched_jni(service_id);
}

void atsc3_lls_sls_alc_on_package_extract_completed_callback_ndk(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload) {
    Atsc3NdkApplicationBridge_ptr->atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload);
}

bool atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_route_component_t* mmt_atsc3_route_component) {
    //jjustman-2020-12-08 - TODO - add this route_component into our SLT monitoring
    //borrowed from atsc3_core_service_player_bridge_set_single_monitor_a331_service_id
    if(atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk: atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored is set, ignoring!");
        return false;
    }

    lls_sls_alc_monitor = lls_sls_alc_monitor_create();
    lls_sls_alc_monitor->atsc3_lls_slt_service = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service;
    lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
    lls_sls_alc_monitor->has_discontiguous_toi_flow = true; //jjustman-2020-07-27 - hack-ish

    lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service);
    lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

    lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_or_create_from_ip_udp_values(lls_slt_monitor, atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service, mmt_atsc3_route_component->stsid_destination_ip_address, mmt_atsc3_route_component->stsid_destination_udp_port, mmt_atsc3_route_component->stsid_source_ip_address);
    if(!lls_sls_alc_session) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
    }
    lls_sls_alc_monitor->lls_alc_session = lls_sls_alc_session;
    lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;

    lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);


    //wire up event callback for alc close_object notification
    lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;

    //wire up event callback for alc MPD patching
    lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_callback = &atsc3_lls_sls_alc_on_route_mpd_patched_ndk;
    //jjustman-2020-08-05 - also atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback

    lls_sls_alc_monitor->atsc3_lls_sls_alc_on_package_extract_completed_callback = &atsc3_lls_sls_alc_on_package_extract_completed_callback_ndk;

    //#1569
    lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback = &atsc3_sls_on_held_trigger_received_callback_impl;

    return true;
}

void atsc3_sls_on_held_trigger_received_callback_impl(uint16_t service_id, block_t* held_payload) {
    block_Rewind(held_payload);
    uint8_t *block_ptr = block_Get(held_payload);
    uint32_t block_len = block_Len(held_payload);

    int len_aligned = block_len + 1;
    len_aligned += 8-(len_aligned%8);
    char* xml_payload_copy = (char*)calloc(len_aligned , sizeof(char));
    strncpy(xml_payload_copy, (char*)block_ptr, block_len);
    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_INFO("HELD: change: %s", xml_payload_copy);

    Atsc3NdkApplicationBridge_ptr->atsc3_onSlsHeldEmissionPresent(service_id, (const char*)xml_payload_copy);

    free(xml_payload_copy);
}

void atsc3_mmt_signalling_information_on_held_message_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_held_message_t* mmt_atsc3_held_message) {
    if(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        atsc3_sls_on_held_trigger_received_callback_impl(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id, mmt_atsc3_held_message->held_message);
    }
}


/*
 *
note for Android MediaCodec:

https://developer.android.com/reference/android/media/MediaCodec

Android uses the following codec-specific data buffers.
These are also required to be set in the track format for proper MediaMuxer track configuration.
Each parameter set and the codec-specific-data sections marked with (*) must start with a start code of "\x00\x00\x00\x01".

CSD buffer #0:

H.265 HEVC	VPS (Video Parameter Sets*) +
SPS (Sequence Parameter Sets*) +
PPS (Picture Parameter Sets*)

*/
block_t* __INTERNAL_LAST_NAL_PACKET_TODO_FIXME = NULL;

atsc3_link_mapping_table_t* atsc3_phy_jni_bridge_notify_link_mapping_table(atsc3_link_mapping_table_t* atsc3_link_mapping_table_pending) {
    atsc3_link_mapping_table_t* atsc3_link_mapping_table_to_free = NULL;

    //no last link mapping table, so take ownership of pending ptr
    if(!atsc3_link_mapping_table_last) {
        atsc3_link_mapping_table_last = atsc3_link_mapping_table_pending;
    } else {
        //if we have a pending table version that matches our last table version, so return our pending version to be freed (discarded)
        if(atsc3_link_mapping_table_pending->alp_additional_header_for_signaling_information_signaling_version ==
            atsc3_link_mapping_table_last->alp_additional_header_for_signaling_information_signaling_version) {
            atsc3_link_mapping_table_to_free = atsc3_link_mapping_table_pending;
        } else {
            //pending table version is not the saemm as our last version, so discard our last ptr ref and update pending to last
            atsc3_link_mapping_table_to_free = atsc3_link_mapping_table_last;
            atsc3_link_mapping_table_last = atsc3_link_mapping_table_pending;
        }
    }
    return atsc3_link_mapping_table_to_free;
}


string atsc3_ndk_cache_temp_folder_path_get(int service_id) {
    return atsc3_ndk_cache_temp_folder_path + to_string(service_id) + "/";
}


string atsc3_route_service_context_temp_folder_name(int service_id) {
    return __ALC_DUMP_OUTPUT_PATH__ + to_string(service_id) + "/";
}

int atsc3_ndk_cache_temp_unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = 0;

    if(strcmp(fpath, atsc3_ndk_cache_temp_folder_path.c_str()) != 0) {
        printf("atsc3_ndk_cache_temp_unlink_cb: removing cache path: %s", fpath);

        rv = remove(fpath);

        if (rv) {
            printf("atsc3_ndk_cache_temp_unlink_cb: unable to remove path: %s, err from remove is: %d", fpath, rv);
        }
    }
    return rv;
}

int atsc3_ndk_cache_temp_folder_purge(char *path)
{
    printf("atsc3_ndk_cache_temp_folder_purge: invoked with path: %s", path);

    return nftw(path, atsc3_ndk_cache_temp_unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}


#endif
