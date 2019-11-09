/*
 * atsc3_phy_mmt_player_bridge.cpp
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 *
 * Android MMT MFU Playback with SLS event driven callbacks
 *
 *
 * Note: At3DrvIntf - Android NDK Binding against Lowasys API are not included
 */

#include "../jni/At3DrvIntf.h"
At3DrvIntf* at3DrvIntf_ptr;

#include "atsc3_phy_mmt_player_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <limits.h>

#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"

#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_mpu_utils.h"

#include "atsc3_logging_externs.h"

#include "atsc3_mmt_context_mfu_depacketizer.h"

#include "atsc3_hevc_nal_extractor.h"
#include "atsc3_lls_types.h"

#include "atsc3_lls_alc_utils.h"
#include "atsc3_alc_rx.h"
#include "atsc3_alc_utils.h"



//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

//jjustman-2019-10-03 - context event callbacks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context;
lls_slt_monitor_t* lls_slt_monitor;

void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk(uint32_t tsi, uint32_t toi, char* content_location);
void atsc3_lls_sls_alc_on_route_mpd_patched_ndk(uint16_t service_id);

//mmtp/sls flow management
mmtp_flow_t* mmtp_flow;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

//route/alc specific parameters
lls_sls_alc_monitor_t* lls_sls_alc_monitor = NULL;
alc_channel_t ch;
alc_arguments_t* alc_arguments;

std::string atsc3_ndk_cache_temp_folder_path;


//these should actually be referenced from mmt_sls_monitor for proper flow references
uint16_t global_video_packet_id = 0;
uint16_t global_audio_packet_id = 0;
uint16_t global_stpp_packet_id = 0;

uint32_t global_mfu_proccessed_count = 0;

mmtp_packet_header_t*  mmtp_parse_header_from_udp_packet(udp_packet_t* udp_packet) {

    mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

    if(!mmtp_packet_header) {
        __ERROR("mmtp_parse_header_from_udp_packet: mmtp_packet_header_parse_from_block_t: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
                __toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
                udp_packet->udp_flow.src_ip_addr,
                __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
                udp_packet->udp_flow.dst_ip_addr);
        return NULL;
    }

    return mmtp_packet_header;
}

void mmtp_process_sls_from_payload(udp_packet_t *udp_packet, mmtp_signalling_packet_t* mmtp_signalling_packet, lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {

    __INFO("mmtp_process_sls_from_payload: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: %d, signalling message: %p",
           __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
           mmtp_signalling_packet->mmtp_packet_id,
           mmtp_signalling_packet);

    mmt_signalling_message_dump(mmtp_signalling_packet);
}

atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id(int service_id) {
    __INFO("atsc3_phy_mmt_player_bridge_set_a331_service_id: with service_id: %d", service_id);
    //find our matching LLS service, then assign a monitor reference

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ERROR("atsc3_phy_mmt_player_bridge_set_a331_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    //jjustman-2019-10-19: todo: refactor this out
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = NULL;
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_mmt = NULL;
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_route = NULL;

    for(int i=0; i < atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count; i++) {
        atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[i];
        if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_MMTP) {
            atsc3_slt_broadcast_svc_signalling_mmt = atsc3_slt_broadcast_svc_signalling;
        } else if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_ROUTE) {
            atsc3_slt_broadcast_svc_signalling_route = atsc3_slt_broadcast_svc_signalling;
            atsc3_slt_broadcast_svc_signalling_mmt = NULL;
        }
    }

    if(atsc3_slt_broadcast_svc_signalling_mmt != NULL) {
        __INFO("atsc3_phy_mmt_player_bridge_set_a331_service_id: service_id: %d - using MMT with flow: sip: %s, dip: %s:%s",
               service_id,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_source_ip_address,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_destination_ip_address,
               atsc3_slt_broadcast_svc_signalling_mmt->sls_destination_udp_port);

        //clear any active SLS monitors
        lls_slt_monitor_clear_lls_sls_mmt_monitor(lls_slt_monitor);
        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);

        lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
        lls_sls_mmt_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!
        lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

        lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

        //we may not be initialized yet, so re-check again later
        //this should _never_happen...
        lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);

        if(!lls_sls_mmt_session) {
            __WARN("lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
        }
        lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
        lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

        lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
    } else {
        lls_slt_monitor_clear_lls_sls_mmt_monitor(lls_slt_monitor);
        if(lls_slt_monitor->lls_sls_mmt_monitor) {
            lls_sls_mmt_monitor_free(&lls_slt_monitor->lls_sls_mmt_monitor);
        }

    }

    if(atsc3_slt_broadcast_svc_signalling_route != NULL) {
        __INFO("atsc3_phy_mmt_player_bridge_set_a331_service_id: service_id: %d - using ROUTE with flow: sip: %s, dip: %s:%s",
               service_id,
               atsc3_slt_broadcast_svc_signalling_route->sls_source_ip_address,
               atsc3_slt_broadcast_svc_signalling_route->sls_destination_ip_address,
               atsc3_slt_broadcast_svc_signalling_route->sls_destination_udp_port);

        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);

        lls_sls_alc_monitor = lls_sls_alc_monitor_create();
        lls_sls_alc_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service;
        lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
        lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
        lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

        lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
        if(!lls_sls_alc_session) {
            __WARN("lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
        }
        lls_sls_alc_monitor->lls_alc_session = lls_sls_alc_session;
        lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;

        lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);

        //wire up event callback for alc close_object notification
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;

        //write up event callback for alc MPD patching
        lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched = &atsc3_lls_sls_alc_on_route_mpd_patched_ndk;


    } else {
        lls_slt_monitor_clear_lls_sls_alc_monitor(lls_slt_monitor);
        if(lls_slt_monitor->lls_sls_alc_monitor) {
            lls_sls_alc_monitor_free(&lls_slt_monitor->lls_sls_alc_monitor);
        }

    }

    return atsc3_lls_slt_service;
}

atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(int service_id) {
    __INFO("atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id: with service_id: %d", service_id);

    atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, service_id);
    if(!atsc3_lls_slt_service) {
        __ERROR("atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id: unable to find service_id: %d", service_id);
        return NULL;
    }

    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = NULL;
    atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling_route_to_add_monitor = NULL;

    for(int i=0; i < atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count; i++) {
        atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[i];
         if(atsc3_slt_broadcast_svc_signalling->sls_protocol == SLS_PROTOCOL_ROUTE) {
             atsc3_slt_broadcast_svc_signalling_route_to_add_monitor = atsc3_slt_broadcast_svc_signalling;
        }
    }

    if(atsc3_slt_broadcast_svc_signalling_route_to_add_monitor == NULL) {
        __WARN("atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id: unable to find ALC service_id: %d",
              service_id);
        return NULL;
    }

    __INFO("atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id: service_id: %d - adding ROUTE with flow: sip: %s, dip: %s:%s",
           service_id,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_source_ip_address,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_destination_ip_address,
           atsc3_slt_broadcast_svc_signalling_route_to_add_monitor->sls_destination_udp_port);

    lls_sls_alc_monitor = lls_sls_alc_monitor_create();
    lls_sls_alc_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service;

    lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
    if(!lls_sls_alc_session) {
        __WARN("lls_slt_alc_session_find_from_service_id: %d, lls_sls_alc_session is NULL!", service_id);
        return NULL;
    }
    lls_sls_alc_monitor->lls_alc_session = lls_sls_alc_session;
    lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);

    //add in supplimentary callback hook for additional ALC emissions
    lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;


    //add a supplimentry sls_alc monitor
    // TODO: fix me? NOTE: do not replace the primary lls_slt_monitor->lls_sls_alc_monitor entry if set
    if(!lls_slt_monitor->lls_sls_alc_monitor) {
        lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;
    }

    return atsc3_lls_slt_service;
}

//TODO: jjustman-2019-11-07: mutex
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(int service_id) {
    if(!lls_slt_monitor->lls_sls_alc_monitor || !lls_slt_monitor->lls_sls_alc_monitor_v.count) {
        __ERROR("atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id: unable to remove service_id: %d, lls_slt_monitor->lls_sls_alc_monitor is: %p, lls_slt_monitor->lls_sls_alc_monitor_v.count is: %d",
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

lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_get_from_service_id(int service_id) {
    if(!lls_slt_monitor->lls_sls_alc_monitor || !lls_slt_monitor->lls_sls_alc_monitor_v.count) {
        __ERROR("atsc3_lls_sls_alc_monitor_get_from_service_id: error searching for service_id: %d, alc_monitor is null: lls_slt_monitor->lls_sls_alc_monitor is: %p, lls_slt_monitor->lls_sls_alc_monitor_v.count is: %d",
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
        __ERROR("atsc3_lls_sls_alc_monitor_get_from_service_id: service_id: %d, lls_sls_alc_monitor_to_return is null!",
                service_id);
        return NULL;
    }

    __INFO("atsc3_lls_sls_alc_monitor_get_from_service_id: %d, returning lls_sls_alc_monitor_to_return: %p",
           service_id,
           lls_sls_alc_monitor_to_return);

    return lls_sls_alc_monitor_to_return;
}

atsc3_sls_metadata_fragments_t* atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(int service_id) {
    lls_sls_alc_monitor_t* lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_get_from_service_id(service_id);

    if(!lls_sls_alc_monitor || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
        __ERROR("atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id: service_id: %d, alc_monitor or fragments were null, lls_sls_alc_monitor: %p", service_id, lls_sls_alc_monitor);
        return NULL;
    }

    __INFO("atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id: %d, returning atsc3_sls_metadata_fragments: %p",
           service_id,
           lls_sls_alc_monitor->atsc3_sls_metadata_fragments);

    return lls_sls_alc_monitor->atsc3_sls_metadata_fragments;
}

atsc3_route_s_tsid_t* atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(int service_id) {
    lls_sls_alc_monitor_t* lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_get_from_service_id(service_id);

    if(!lls_sls_alc_monitor || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments || !lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid) {
        __ERROR("atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id: service_id: %d, alc_monitor or fragments or route_s_tsid were null, lls_sls_alc_monitor: %p", service_id, lls_sls_alc_monitor);
        return NULL;
    }

    __INFO("atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id: %d, returning atsc3_route_s_tsid: %p",
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
//void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
void atsc3_phy_mmt_player_bridge_process_packet_phy(block_t* packet) {

    mmtp_packet_header_t* mmtp_packet_header = NULL;
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = NULL;


    alc_packet_t* alc_packet = NULL;
    lls_sls_alc_session_t* matching_lls_slt_alc_session = NULL;

    //lowasys hands off the ip packet header, not phy eth frame

    //udp_packet_t* udp_packet = udp_packet_process_from_ptr_raw_ethernet_packet(block_Get(packet), packet->p_size);
    udp_packet_t* udp_packet = udp_packet_process_from_ptr(block_Get(packet), packet->p_size);

    if(!udp_packet) {
        return;
    }

    //drop mdNS
    if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
        goto cleanup;
    }

    if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
        //at3DrvIntf_ptr->LogMsgF("atsc3_phy_mmt_player_bridge_process_packet_phy got packet: LLS, %p", udp_packet);

        //auto-monitor code here for MMT
        //process as lls.sst, dont free as we keep track of our object in the lls_slt_monitor
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
        if(lls_table) {
            if(lls_table->lls_table_id == SLT) {
                int retval = lls_slt_table_perform_update(lls_table, lls_slt_monitor);

                if(!retval) {
                    lls_dump_instance_table(lls_table);
                    for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
                        atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];
                        if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_MMTP) {
                            if(lls_sls_mmt_monitor) {
                                //re-configure
                            } else {
                                //jjustman-2019-10-19: TODO: refactor this out from above also ^^^ - atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id
                                //TODO:  make sure we don't early free this...
                                //lls_service->broadcast_svc_signaling.sls_destination_ip_address && lls_service->broadcast_svc_signaling.sls_destination_udp_port
                                //match our dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter and port filter
                                __INFO("Adding service: %d", atsc3_lls_slt_service->service_id);

                                lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
                                lls_sls_mmt_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!
                                lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

                                lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

                                //we may not be initialized yet, so re-check again later
                                //this should _never_happen...
                                lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
                                if(!lls_sls_mmt_session) {
                                    __WARN("lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
                                }
                                lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
                                lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

                                lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
                            }
                        }
                    }
                }
            }
        }

        __TRACE("Checking lls_sls_mmt_monitor: %p,", lls_sls_mmt_monitor);

        if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
            __TRACE("Checking lls_sls_mmt_monitor->lls_mmt_session: %p,", lls_sls_mmt_monitor->lls_mmt_session);
        }

        //recheck video_packet_id/audio_packet_id
        if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
            if(!lls_sls_mmt_monitor->video_packet_id) {
                lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);
                lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
                lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;
				lls_sls_mmt_monitor->stpp_packet_id  = lls_sls_mmt_session->stpp_packet_id;
                __TRACE("setting audio_packet_id/video_packet_id/stpp: %u, %u, %u",
						lls_sls_mmt_monitor->audio_packet_id,
						lls_sls_mmt_monitor->video_packet_id,
						lls_sls_mmt_monitor->stpp_packet_id);
            }

        }
        goto cleanup;
    }

    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        goto cleanup;
    }

    //todo: fix me not filtering
    matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if((lls_sls_alc_monitor && matching_lls_slt_alc_session && lls_sls_alc_monitor->atsc3_lls_slt_service &&  (lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == matching_lls_slt_alc_session->atsc3_lls_slt_service->service_id))  ||
       ((dst_ip_addr_filter != NULL && dst_ip_port_filter != NULL) && (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter))) {
        //process ALC streams
        int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &ch, &alc_packet);
        if(!retval) {
            atsc3_alc_persist_route_ext_attributes_per_lls_sls_alc_monitor_essence(alc_packet, lls_slt_monitor->lls_sls_alc_monitor);
            //dump out for fragment inspection
            alc_packet_dump_to_object(&udp_packet->udp_flow, &alc_packet, lls_slt_monitor->lls_sls_alc_monitor);
        } else {
            __ERROR("Error in ALC decode: %d", retval);
        }
        goto cleanup;
    }



    //TODO: jjustman-2019-10-03 - packet header parsing to dispatcher mapping
    matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    __TRACE("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

	if(matching_lls_sls_mmt_session && lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id == lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id) {

        mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

        //at3DrvIntf_ptr->LogMsgF("mmtp_packet_header: %p", mmtp_packet_header);

        if(!mmtp_packet_header) {
            goto cleanup;
        }

        //for filtering MMT flows by a specific packet_id
        if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
            goto cleanup;
        }

        mmtp_packet_header_dump(mmtp_packet_header);

        //dump header, then dump applicable packet type
        if(mmtp_packet_header->mmtp_payload_type == 0x0) {
            //mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t();
            mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            if(!mmtp_mpu_packet) {
                goto error;
            }

            if(mmtp_mpu_packet->mpu_timed_flag == 1) {
                mmtp_mpu_dump_header(mmtp_mpu_packet);

                //TODO: jjustman-2019-10-03 - handle context parameters better
                // mmtp_flow, lls_slt_monitor, , udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);

                atsc3_mmt_mfu_context->mmtp_flow = mmtp_flow;
                atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container;
                atsc3_mmt_mfu_context->lls_slt_monitor = lls_slt_monitor;
                atsc3_mmt_mfu_context->matching_lls_sls_mmt_session = matching_lls_sls_mmt_session;

                __TRACE("process_packet: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,",
                        udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

               // at3DrvIntf_ptr->LogMsgF("process_packet: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,", udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);
                mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

            } else {
                //non-timed
                __ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
            }
        } else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

            mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
            if(parsed_count) {
                mmt_signalling_message_dump(mmtp_signalling_packet);

                __TRACE("process_packet: calling mmt_signalling_message_process_with_context with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
                        udp_packet,
                        mmtp_signalling_packet,
                        atsc3_mmt_mfu_context);

                mmt_signalling_message_process_with_context(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);


                //internal hacks below


                //TODO: jjustman-2019-10-03 - if signalling_packet == MP_table, set atsc3_mmt_mfu_context->mp_table_last;
                mmtp_asset_flow_t* mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
                mmtp_asset_t* mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);

                //TODO: FIX ME!!! HACK - jjustman-2019-09-05
                mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_new();
                mmtp_mpu_packet->mmtp_packet_id = mmtp_signalling_packet->mmtp_packet_id;

                mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
                mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);

                //TODO: FIX ME!!! HACK - jjustman-2019-09-05
                mmtp_mpu_packet_free(&mmtp_mpu_packet);

                //update our sls_mmt_session info
                mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);

                //TODO - remap this
                //add in flows 				lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);

                if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session && matching_lls_sls_mmt_session) {
                	__TRACE("mmt_signalling_information: from atsc3 service_id: %u, patching: seting audio_packet_id/video_packet_id/stpp_packet_id: %u, %u, %u",
                								matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id,
                								matching_lls_sls_mmt_session->audio_packet_id,
                								matching_lls_sls_mmt_session->video_packet_id,
                								matching_lls_sls_mmt_session->stpp_packet_id);

                    if(matching_lls_sls_mmt_session->audio_packet_id) {
                        lls_sls_mmt_monitor->audio_packet_id = matching_lls_sls_mmt_session->audio_packet_id;
                    }

                    if(matching_lls_sls_mmt_session->video_packet_id) {
                        lls_sls_mmt_monitor->video_packet_id = matching_lls_sls_mmt_session->video_packet_id;
                    }
					if(matching_lls_sls_mmt_session->stpp_packet_id) {
						lls_sls_mmt_monitor->stpp_packet_id = matching_lls_sls_mmt_session->stpp_packet_id;
					}

                }
            }

        } else {
            __ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
            goto cleanup;
        }
    }

cleanup:

	if(mmtp_packet_header) {
		mmtp_packet_header_free(&mmtp_packet_header);
	}
	if(alc_packet) {
	    alc_packet_free(&alc_packet);
	}

	if(udp_packet) {
        udp_packet_free(&udp_packet);
    }
    return;

error:
    __ATSC3_WARN("process_packet: error, bailing loop!");
    return;
}



void atsc3_lls_on_sls_table_present_ndk(lls_table_t* lls_table) {
    if(!lls_table) {
        at3DrvIntf_ptr->LogMsg("E: atsc3_lls_on_sls_table_present_ndk: no lls_table for SLS!");
        return;
    }
    if(!lls_table->raw_xml.xml_payload || !lls_table->raw_xml.xml_payload_size) {
        at3DrvIntf_ptr->LogMsg("E: atsc3_lls_on_sls_table_present_ndk: no raw_xml.xml_payload for SLS!");
        return;
    }

    //copy over our xml_payload.size +1 with a null
    int len_aligned = lls_table->raw_xml.xml_payload_size + 1;
    len_aligned += 8-(len_aligned%8);
    char* xml_payload_copy = (char*)calloc(len_aligned , sizeof(char));
    strncpy(xml_payload_copy, (char*)lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);

    at3DrvIntf_ptr->LogMsg((const char*)xml_payload_copy);
    free(xml_payload_copy);

}

//TODO: jjustman-2019-11-08: wire up the service_id in which this alc_emission originated from in addition to tsi/toi
void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk(uint32_t tsi, uint32_t toi, char* content_location) {
    at3DrvIntf_ptr->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(tsi, toi, content_location);
}

void atsc3_lls_sls_alc_on_route_mpd_patched_ndk(uint16_t service_id) {
    at3DrvIntf_ptr->atsc3_lls_sls_alc_on_route_mpd_patched_jni(service_id);
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

void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {
    atsc3_hevc_nals_record_dump("mmt_mpu_metadata", mmt_mpu_metadata);

    if (global_video_packet_id && global_video_packet_id == packet_id) {
        //manually extract our NALs here
        video_decoder_configuration_record_t *video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(
                mmt_mpu_metadata);

        //we will get either avc1 (avcC) NAL or hevc (hvcC) nals back
        if (video_decoder_configuration_record) {
            //set width/height to player
            if(video_decoder_configuration_record->width && video_decoder_configuration_record->height) {
                at3DrvIntf_ptr->atsc3_setVideoWidthHeightFromTrak(video_decoder_configuration_record->width, video_decoder_configuration_record->height);
            }

            if (video_decoder_configuration_record->hevc_decoder_configuration_record) {

                block_t* hevc_nals_combined = atsc3_hevc_extract_extradata_nals_combined_ffmpegImpl(video_decoder_configuration_record->hevc_decoder_configuration_record->box_data_original);

                if(hevc_nals_combined->p_size) {
                    //todo - jjustman-2019-10-12 - lock this for race conditions and allocate per flow
                    block_Rewind(hevc_nals_combined);
                    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME) {
                        block_Destroy(&__INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
                    }
                    __INTERNAL_LAST_NAL_PACKET_TODO_FIXME = block_Duplicate(hevc_nals_combined);

                    at3DrvIntf_ptr->atsc3_onInitHEVC_NAL_Extracted(packet_id, mpu_sequence_number,  block_Get(hevc_nals_combined), hevc_nals_combined->p_size);
                } else {
                    at3DrvIntf_ptr->LogMsg("atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk - error, no NALs returned!");

                }
            }
        }
    } else {
        //if audio, dump the ESDS box for android MediaCodec

        /*
         *
         *  [mp4a] size=8+67
              data_reference_index = 1
              channel_count = 2
              sample_size = 16
              sample_rate = 48000
              [esds] size=12+27
                [ESDescriptor] size=2+25
                  es_id = 0
                  stream_priority = 0
                  [DecoderConfig] size=2+17
                    stream_type = 5
                    object_type = 64
                    up_stream = 0
                    buffer_size = 8192
                    max_bitrate = 128000
                    avg_bitrate = 128000
                    DecoderSpecificInfo = 11 90
                  [Descriptor:06] size=2+1

                  https://wiki.multimedia.cx/index.php/Understanding_AAC
                  https://developer.android.com/reference/android/media/MediaCodecs
         */
    }
}


void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    uint64_t last_mpu_timestamp = mpu_presentation_time_seconds * 1000000 + mpu_presentation_time_microseconds;
    global_video_packet_id = video_packet_id;

     at3DrvIntf_ptr->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    uint64_t last_mpu_timestamp = mpu_presentation_time_seconds * 1000000 + mpu_presentation_time_microseconds;
    global_audio_packet_id = audio_packet_id;

    at3DrvIntf_ptr->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    uint64_t last_mpu_timestamp = mpu_presentation_time_seconds * 1000000 + mpu_presentation_time_microseconds;
    global_stpp_packet_id = stpp_packet_id;

    at3DrvIntf_ptr->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void atsc3_mmt_mpu_mfu_on_sample_complete_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
    //    void onMfuPacket(bool is_video, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs);
//    at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, size: %u, last_mpu_timestamp: %llu",
//                packet_id,
//                mpu_sequence_number,
//                sample_number,
//                mmt_mfu_sample->p_size,
//                last_mpu_timestamp);

    //cant process MFU's without the NAL... we should ALWAYS listen for at least mpu metadata
    //in as many MMT flows as possible

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, packet_id, mpu_sequence_number);

    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        __WARN("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        //__ERROR("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        //return;
    }

    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME && packet_id == global_video_packet_id) {
        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, __INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
        uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
        uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

//        at3DrvIntf_ptr->LogMsgF(
//                "atsc3_mmt_mpu_mfu_on_sample_complete_ndk: NAL, packet_id: %d, mpu_sequence_number: %d, block: %p, len: %d, pts_us: %lu",
//                packet_id, mpu_sequence_number, block_ptr, block_len, mpu_timestamp_descriptor);

        if((global_mfu_proccessed_count++ % 600) == 0) {
            at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, orig len: %d, len: %d",
                                    global_mfu_proccessed_count,
                                    packet_id, mpu_sequence_number, sample_number, block_Len(mmt_mfu_sample),
                                    block_len);

        }
        at3DrvIntf_ptr->atsc3_onMfuPacket(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_rebuilt);

        block_Release(&mmt_mfu_sample_rbsp);
    } else {
        block_Rewind(mmt_mfu_sample);
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        if(packet_id == 19) {
            at3DrvIntf_ptr->LogMsgF(
                    "atsc3_mmt_mpu_mfu_on_sample_complete_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, block: %p, len: %d, char: %c %c %c %c",
                    packet_id, mpu_sequence_number, block_ptr, block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);
        }
        //audio and stpp don't need NAL start codes
        at3DrvIntf_ptr->atsc3_onMfuPacket(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_rebuilt);
    }
}

/*
 * jjustman-2019-10-29 - todo: fixme:
 *
 *
    --------- beginning of crash
2019-10-29 06:32:01.833 20009-20089/org.ngbp.libatsc3 A/libc: Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x2a81800000 in tid 20089 (Thread-3769), pid 20009 (g.ngbp.libatsc3)
2019-10-29 06:32:01.869 20009-20009/org.ngbp.libatsc3 D/main:     -> lines trimmed to 37
2019-10-29 06:32:01.889 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:01.889 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 1	1332	mpu:	7754	sample:	39	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345410761	size	8091
2019-10-29 06:32:01.890 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 2	1333	mpu:	7754	sample:	40	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345444127	size	7353
2019-10-29 06:32:01.904 20140-20140/? E/DEBUG: failed to readlink /proc/20089/fd/148: No such file or directory
2019-10-29 06:32:01.905 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 9	1334	mpu:	7754	sample:	41	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345477493	size	49590
2019-10-29 06:32:01.905 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:01.906 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 3	1335	mpu:	7754	sample:	42	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345510859	size	15812
2019-10-29 06:32:01.907 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 0	1336	mpu:	7754	sample:	43	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345544225	size	7257
2019-10-29 06:32:01.907 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 5	1337	mpu:	7754	sample:	44	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345577591	size	7153
2019-10-29 06:32:01.908 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 6	1338	mpu:	7754	sample:	45	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345610957	size	59180
2019-10-29 06:32:01.955 20140-20140/? I/crash_dump64: obtaining output fd from tombstoned, type: kDebuggerdTombstone
2019-10-29 06:32:01.955 3500-3500/? I//system/bin/tombstoned: received crash request for pid 20089
2019-10-29 06:32:01.957 20140-20140/? I/crash_dump64: performing dump of process 20009 (target tid = 20089)
2019-10-29 06:32:01.966 20140-20140/? A/DEBUG: *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG: Build fingerprint: 'NVIDIA/darcy/darcy:9/PPR1.180610.011/4086637_1697.8089:user/release-keys'
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG: Revision: '0'
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG: ABI: 'arm64'
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG: pid: 20009, tid: 20089, name: Thread-3769  >>> org.ngbp.libatsc3 <<<
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG: signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x2a81800000
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x0  000000007a12500c  x1  0000002a817fffd2  x2  000000000005535a  x3  000000007a139600
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x4  0000002a8185537c  x5  000000007a18e9ea  x6  0000000000000000  x7  0000000000000000
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x8  0000000000000000  x9  0000000000000000  x10 0000000000000000  x11 0000000000000000
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x12 0000000000000000  x13 0000000000000000  x14 0000000000000000  x15 0000000000000000
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x16 0000002a5a6dbce0  x17 00000029d21e1d00  x18 0000000000000000  x19 0000002a7d720980
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x20 0000002a733f8218  x21 0000000000000000  x22 000000007a12500c  x23 00000000000699de
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x24 0000000000000000  x25 0000000000000000  x26 0000002a817eb99e  x27 0000000000000000
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     x28 00000000000699de  x29 0000002a733f81f0
2019-10-29 06:32:01.967 20140-20140/? A/DEBUG:     sp  0000002a733f81b0  lr  0000002a5a6b0bdc  pc  00000029d21e1c8c
2019-10-29 06:32:01.979 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 7	1339	mpu:	7754	sample:	46	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345644323	size	15368
2019-10-29 06:32:01.979 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:01.980 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 8	1340	mpu:	7754	sample:	47	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345677689	size	6577
2019-10-29 06:32:01.988 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:02.004 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:02.005 20009-20125/org.ngbp.libatsc3 D/VideoDecoder.onInputBufferAvailable: video_global_input_count:	buf idx: 4	1341	mpu:	7754	sample:	48	mpu_presentation_timestamp:	1344142853	videoRenderTs	1345711055	size	6696
2019-10-29 06:32:02.020 3494-20121/? D/NvOsDebugPrintf: NvRmPrivFlush: NvRmChannelSubmit failed (err = 196623, SyncPointIdx = 32, SyncPointValue = 0)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG: backtrace:
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #00 pc 000000000001dc8c  /system/lib64/libc.so (memcpy+284)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #01 pc 0000000000034bd8  /system/lib64/libjavacore.so (Memory_memmove(_JNIEnv*, _jclass*, _jobject*, int, _jobject*, int, long)+204)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #02 pc 000000000007d0c0  /system/framework/arm64/boot-core-libart.oat (offset 0x78000) (libcore.io.Memory.memmove+224)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #03 pc 00000000000b7260  /dev/ashmem/dalvik-jit-code-cache (deleted) (java.nio.ByteBuffer.put+560)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #04 pc 00000000000bebd8  /dev/ashmem/dalvik-jit-code-cache (deleted) (org.ngbp.libatsc3.media.sync.mmt.MfuByteBufferFragment.<init>+168)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #05 pc 0000000000554b88  /system/lib64/libart.so (art_quick_invoke_stub+584)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #06 pc 00000000000cf6c8  /system/lib64/libart.so (art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)+200)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #07 pc 000000000027f450  /system/lib64/libart.so (art::interpreter::ArtInterpreterToCompiledCodeBridge(art::Thread*, art::ArtMethod*, art::ShadowFrame*, unsigned short, art::JValue*)+344)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #08 pc 000000000027a57c  /system/lib64/libart.so (bool art::interpreter::DoCall<true, false>(art::ArtMethod*, art::Thread*, art::ShadowFrame&, art::Instruction const*, unsigned short, art::JValue*)+748)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #09 pc 0000000000527470  /system/lib64/libart.so (MterpInvokeDirectRange+244)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #10 pc 0000000000547614  /system/lib64/libart.so (ExecuteMterpImpl+15252)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #11 pc 0000000000015af2  /dev/ashmem/dalvik-classes2.dex extracted in memory from /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/base.apk!classes2.dex (deleted) (com.lowasis.at3drv.At3DrvIntf.atsc3_onMfuPacketCorrupt+46)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #12 pc 000000000025315c  /system/lib64/libart.so (_ZN3art11interpreterL7ExecuteEPNS_6ThreadERKNS_20CodeItemDataAccessorERNS_11ShadowFrameENS_6JValueEb.llvm.442261360+488)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #13 pc 0000000000515190  /system/lib64/libart.so (artQuickToInterpreterBridge+1020)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #14 pc 000000000055dcfc  /system/lib64/libart.so (art_quick_to_interpreter_bridge+92)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #15 pc 0000000000554b88  /system/lib64/libart.so (art_quick_invoke_stub+584)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #16 pc 00000000000cf6c8  /system/lib64/libart.so (art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)+200)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #17 pc 000000000045ca2c  /system/lib64/libart.so (art::(anonymous namespace)::InvokeWithArgArray(art::ScopedObjectAccessAlreadyRunnable const&, art::ArtMethod*, art::(anonymous namespace)::ArgArray*, art::JValue*, char const*)+104)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #18 pc 000000000045ddc0  /system/lib64/libart.so (art::InvokeVirtualOrInterfaceWithVarArgs(art::ScopedObjectAccessAlreadyRunnable const&, _jobject*, _jmethodID*, std::__va_list)+440)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #19 pc 00000000003398a8  /system/lib64/libart.so (art::JNI::CallIntMethodV(_JNIEnv*, _jobject*, _jmethodID*, std::__va_list)+656)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #20 pc 00000000000ffbc4  /system/lib64/libart.so (art::(anonymous namespace)::CheckJNI::CallMethodV(char const*, _JNIEnv*, _jobject*, _jclass*, _jmethodID*, std::__va_list, art::Primitive::Type, art::InvokeType)+2184)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #21 pc 00000000000ecb14  /system/lib64/libart.so (art::(anonymous namespace)::CheckJNI::CallIntMethodV(_JNIEnv*, _jobject*, _jmethodID*, std::__va_list)+92)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #22 pc 000000000002a6dc  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (_JNIEnv::CallIntMethod(_jobject*, _jmethodID*, ...)+208)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #23 pc 000000000002a8e0  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (At3DrvIntf::atsc3_onMfuPacketCorrupt(unsigned short, unsigned int, unsigned int, unsigned char*, unsigned int, unsigned long, unsigned int, unsigned int)+272)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #24 pc 00000000000bc5f8  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(unsigned short, unsigned int, unsigned int, atsc3_block*, unsigned int, unsigned int)+1008)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #25 pc 0000000000080c50  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number+4044)
2019-10-29 06:32:02.033 20140-20140/? A/DEBUG:     #26 pc 000000000007fa14  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (mmtp_mfu_process_from_payload_with_context+1792)
2019-10-29 06:32:02.034 20140-20140/? A/DEBUG:     #27 pc 00000000000ba3d0  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (atsc3_phy_mmt_player_bridge_process_packet_phy(atsc3_block*)+1796)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #28 pc 0000000000028574  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (At3DrvIntf::RxCallbackJJ(S_RX_DATA*)+172)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #29 pc 00000000000284b8  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (At3DrvIntf::RxCallbackStaticJJ(S_RX_DATA*, unsigned long)+36)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #30 pc 0000000000063840  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (non-virtual thunk to CAt3MediaReceiver::OnRxIpData(unsigned char*, int, S_ALP_PLP_CTX*)+196)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #31 pc 000000000008f710  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3ParserALP::_ParseRawAlp(S_ALP_RAW_INFO*, int)+1204)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #32 pc 0000000000090cf4  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3ParserALP::ReceiveRawAlpData(unsigned char*, int, S_ALP_RAW_INFO*)+1768)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #33 pc 0000000000063240  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3MediaReceiver::OnRxBbpData(unsigned char*, S_BBP_CONTAINER*, S_BBP_HDR*)+168)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #34 pc 00000000000844ac  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3ParserBBP::_ProcessOnePayload(unsigned char*, S_BBP_CONTAINER*)+2152)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #35 pc 0000000000085c28  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3ParserBBP::ProcessRawData(S_BBP_RAW_INFO*)+2816)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #36 pc 0000000000061be8  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (CAt3Device::_ReceiveBbpCntPacket()+272)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #37 pc 000000000004d938  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libat3drv.so (AT3DRV_HandleRxData+924)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #38 pc 0000000000029814  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so (At3DrvIntf::RxThread()+560)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #39 pc 0000000000032fe8  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #40 pc 0000000000032f80  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #41 pc 0000000000032ee0  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #42 pc 0000000000032834  /data/app/org.ngbp.libatsc3-rjIE7XWVP_4ievVjm-S-Lg==/lib/arm64/libAt3DrvIntf.so
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #43 pc 0000000000081978  /system/lib64/libc.so (__pthread_start(void*)+36)
2019-10-29 06:32:02.036 20140-20140/? A/DEBUG:     #44 pc 00000000000234b8  /system/lib64/libc.so (__start_thread+68)
2019-10-29 06:32:02.120 3494-20121/? I/chatty: uid=1046(mediacodec) HwBinder:3494_2 identical 3 lines
 */

void atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    //    at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, size: %u, last_mpu_timestamp: %llu",
    //                packet_id,
    //                mpu_sequence_number,
    //                sample_number,
    //                mmt_mfu_sample->p_size,
    //                last_mpu_timestamp);

    //cant process MFU's without the NAL... we should ALWAYS listen for at least mpu metadata
    //in as many MMT flows as possible

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, packet_id, mpu_sequence_number);
    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        __WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
    }

    if(!block_Len(mmt_mfu_sample)) {
        __WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p: block len is 0 for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        return;
    }

    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME && packet_id == global_video_packet_id) {
        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, __INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
        if(mmt_mfu_sample_rbsp && block_Len(mmt_mfu_sample_rbsp)) {
            uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
            uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

            //if((global_mfu_proccessed_count++ % 600) == 0) {
                __INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, sample ptr: %p, orig len: %d (i_pos: %d, p_size: %d), nal ptr: %p, len: %d (i_pos: %d, p_size: %d)",
                                        global_mfu_proccessed_count,
                                        packet_id,
                                        mpu_sequence_number,
                                        sample_number,
                                        mmt_mfu_sample,
                       mmt_mfu_sample->i_pos,

                       mmt_mfu_sample->p_size,
                                        block_Len(mmt_mfu_sample),
                                        block_ptr,
                                        block_len,
                       mmt_mfu_sample_rbsp->i_pos,
                                        mmt_mfu_sample_rbsp->p_size);

            //}

            at3DrvIntf_ptr->atsc3_onMfuPacketCorrupt(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Release(&mmt_mfu_sample_rbsp);
         } else {
            __ERROR("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p (len: %d) - returned null mmt_mfu_sample_rbsp!", mmt_mfu_sample, mmt_mfu_sample ? mmt_mfu_sample->p_size : -1);
        }
    } else {
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        //audio and stpp don't need NAL start codes
        __INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, sample_number: %d, block: %p, len: %d, char: %c %c %c %c",
                    packet_id, mpu_sequence_number, sample_number,  block_ptr, block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);

        at3DrvIntf_ptr->atsc3_onMfuPacketCorrupt(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    }
}


void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    //    void onMfuPacket(bool is_video, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs);
//    at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, size: %u, last_mpu_timestamp: %llu",
//                packet_id,
//                mpu_sequence_number,
//                sample_number,
//                mmt_mfu_sample->p_size,
//                last_mpu_timestamp);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, packet_id, mpu_sequence_number);
    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        __WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
    }
    //TODO: jjustman-2019-10-23: determine if we can still extract NAL's from this payload...

    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME && packet_id == global_video_packet_id) {
        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, __INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
        if(mmt_mfu_sample_rbsp) {
            uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
            uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

            //if((global_mfu_proccessed_count++ % 600) == 0) {
            __INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, sample ptr: %p, orig len: %d (i_pos: %d, p_size: %d), nal ptr: %p, len: %d (i_pos: %d, p_size: %d)",
                                        global_mfu_proccessed_count,
                                        packet_id,
                                        mpu_sequence_number,
                                        sample_number,
                                        mmt_mfu_sample,
                                        block_Len(mmt_mfu_sample),
                   mmt_mfu_sample->i_pos,
                   mmt_mfu_sample->p_size,
                                        block_ptr,
                                        block_len,
                   mmt_mfu_sample_rbsp->i_pos,
                   mmt_mfu_sample_rbsp->p_size);
            //}

            at3DrvIntf_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Release(&mmt_mfu_sample_rbsp);
        }
    } else {
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        __INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, sample: %d, block: %p, len: %d, char: %c %c %c %c",
               packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);

        //audio and stpp don't need NAL start codes
        at3DrvIntf_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    }
}

void atsc3_mmt_mpu_mfu_on_sample_missing_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    at3DrvIntf_ptr->atsc3_onMfuSampleMissing(packet_id, mpu_sequence_number, sample_number);
}

void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata) {
    if(!mmt_movie_fragment_metadata || !mmt_movie_fragment_metadata->p_size) {
        __WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: returned null or no length!",
                packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
        return;
    }

    uint32_t extracted_sample_duration_us = atsc3_mmt_movie_fragment_extract_sample_duration(mmt_movie_fragment_metadata);

    at3DrvIntf_ptr->atsc3_onExtractedSampleDuration(packet_id, mpu_sequence_number, extracted_sample_duration_us);
}


void atsc3_phy_mmt_player_bridge_init(At3DrvIntf* At3DrvIntf_ptr) {
    at3DrvIntf_ptr = At3DrvIntf_ptr;

    //set global logging levels
    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 0;
    _ALC_UTILS_IOTRACE_ENABLED = 0;


    lls_slt_monitor = lls_slt_monitor_create();
    //wire up a lls event for SLS table
    lls_slt_monitor->atsc3_lls_on_sls_table_present = &atsc3_lls_on_sls_table_present_ndk;

    mmtp_flow = mmtp_flow_new();
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();


    //MMT/MFU callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_new();

    //wire up atsc3_mmt_mpu_on_sequence_mpu_metadata_present to parse out our NALs as needed for android MediaCodec init
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present = &atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk;

    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt = &atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk;
    //todo: search thru NAL's as needed here and discard anything that intra-NAL..
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header = &atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing = &atsc3_mmt_mpu_mfu_on_sample_missing_ndk;

    /*
     * TODO: jjustman-2019-10-20 - extend context callback interface with service_id
     */
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor  = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk;

    //extract out one trun sampleduration for essence timing
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present = &atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk;

    atsc3_ndk_cache_temp_folder_path = at3DrvIntf_ptr->get_android_temp_folder();
    chdir(atsc3_ndk_cache_temp_folder_path.c_str());
    at3DrvIntf_ptr->LogMsgF("atsc3_phy_mmt_player_bridge_init - completed, temp folder path: %s", atsc3_ndk_cache_temp_folder_path.c_str());

}

string atsc3_ndk_cache_temp_folder_path_get() {
    return atsc3_ndk_cache_temp_folder_path;
}


string atsc3_route_service_context_temp_folder_name(int service_id) {
    return __ALC_DUMP_OUTPUT_PATH__ + to_string(service_id) + "/";
}




