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


//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

//jjustman-2019-10-03 - context event callbacks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context;
lls_slt_monitor_t* lls_slt_monitor;

//mmtp/sls flow management
mmtp_flow_t* mmtp_flow;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

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
            __WARN("atsc3_phy_mmt_player_bridge_set_a331_service_id: service_id: %d - sls_protocol == ROUTE, TODO!", service_id);
            atsc3_slt_broadcast_svc_signalling_route = atsc3_slt_broadcast_svc_signalling;
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


    }

    return atsc3_lls_slt_service;
}

//void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
void atsc3_phy_mmt_player_bridge_process_packet_phy(block_t* packet) {

    mmtp_packet_header_t* mmtp_packet_header = NULL;
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = NULL;
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


void atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
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

    if(!block_Len(mmt_mfu_sample)) {
        __WARN("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: block len is 0 for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        return;
    }


    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME && packet_id == global_video_packet_id) {
        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, __INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
        if(mmt_mfu_sample_rbsp && block_Len(mmt_mfu_sample_rbsp)) {
            uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
            uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

            if((global_mfu_proccessed_count++ % 600) == 0) {
                at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, orig len: %d, len: %d",
                                        global_mfu_proccessed_count,
                                        packet_id, mpu_sequence_number, sample_number, block_Len(mmt_mfu_sample),
                                        block_len);

            }

            at3DrvIntf_ptr->atsc3_onMfuPacketCorrupt(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Release(&mmt_mfu_sample_rbsp);
         } else {
            __ERROR("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p (len: %d) - returned null mmt_mfu_sample_rbsp!", mmt_mfu_sample, mmt_mfu_sample ? mmt_mfu_sample->p_size : -1);
        }
    } else {
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        //audio and stpp don't need NAL start codes
        if(packet_id == 19) {
            at3DrvIntf_ptr->LogMsgF(
                    "atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, block: %p, len: %d, char: %c %c %c %c",
                    packet_id, mpu_sequence_number, block_ptr, block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);
        }
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
        __WARN("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        //__ERROR("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        //return;
    }
    //TODO: jjustman-2019-10-23: determine if we can still extract NAL's from this payload...

    if(__INTERNAL_LAST_NAL_PACKET_TODO_FIXME && packet_id == global_video_packet_id) {
        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, __INTERNAL_LAST_NAL_PACKET_TODO_FIXME);
        if(mmt_mfu_sample_rbsp) {
            uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
            uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

            if((global_mfu_proccessed_count++ % 600) == 0) {
                at3DrvIntf_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, orig len: %d, len: %d",
                                        global_mfu_proccessed_count,
                                        packet_id, mpu_sequence_number, sample_number, block_Len(mmt_mfu_sample),
                                        block_len);

            }

            at3DrvIntf_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Release(&mmt_mfu_sample_rbsp);
        }
    } else {
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        //audio and stpp don't need NAL start codes
        at3DrvIntf_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    }
}

void atsc3_mmt_mpu_mfu_on_sample_missing_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    at3DrvIntf_ptr->atsc3_onMfuSampleMissing(packet_id, mpu_sequence_number, sample_number);
}



void atsc3_phy_mmt_player_bridge_init(At3DrvIntf* At3DrvIntf_ptr) {
    at3DrvIntf_ptr = At3DrvIntf_ptr;

    //set global logging levels
    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 0;

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

    at3DrvIntf_ptr->LogMsg("atsc3_phy_mmt_player_bridge_init - completed");

}





