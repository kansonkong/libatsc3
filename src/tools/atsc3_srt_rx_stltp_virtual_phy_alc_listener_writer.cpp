/*
 * atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp
 *
 *  Created on: Aug 18, 2020
 *      Author: jjustman
 *
 *      notes: for running under lldb w/ asan build:
 *      	env ASAN_OPTIONS=detect_container_overflow=0
 *
 */


#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include <atsc3_logging_externs.h>

#include <atsc3_listener_udp.h>

#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"
#include "../atsc3_alc_rx.h"
#include "../atsc3_alc_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"
#include "../atsc3_mmt_context_mfu_depacketizer.h"

#include <phy/virtual/SRTRxSTLTPVirtualPHY.h>

#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_TRACE(...)   __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);

// #define __DEBUGGING_DUMP_TIMING_MANAGEMENT_AND_PREAMBLE_PACKETS 1

#define LLS_FRAGMENT_SLT_FILENAME "slt.xml"
#define LLS_FRAGMENT_AEAT_FILENAME "aeat.xml"

uint64_t rx_udp_invocation_count = 0;
SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = NULL;
double srt_thread_run_start_time = 0;

lls_slt_monitor_t* lls_slt_monitor;
lls_sls_alc_monitor_t* lls_sls_alc_monitor;
atsc3_alc_arguments_t* alc_arguments;
atsc3_alc_session_t* atsc3_alc_session;

//jjustman-2021-04-16 - adding support for MMT w/ ROUTEComponent extraction, use atsc3_mmt_mfu_context as referenced from lls_sls_mmt_session
lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

uint32_t alc_packet_received_count = 0;


//NOTE: this will _not_ by default update the PHY with a new list of PLPs to listen, this needs to performed in the atsc3_core_service_player_bridge.cpp
void atsc3_lls_sls_alc_on_metadata_fragments_updated_callback_internal_add_monitor_and_alc_session_flows(lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	int ip_mulitcast_flows_added_count = 0;

	ip_mulitcast_flows_added_count = lls_sls_alc_add_additional_ip_flows_from_route_s_tsid(lls_slt_monitor, lls_sls_alc_monitor, lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid);

	if(ip_mulitcast_flows_added_count) {
		__INFO("atsc3_lls_sls_alc_on_metadata_fragments_updated_callback_internal_add_monitor_and_alc_session_flows: added %d ip mulitcast flows for alc monitor and session", ip_mulitcast_flows_added_count);
	}
}


void atsc3_reset_context() {
	_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("atsc3_reset_context");

	if (lls_slt_monitor) {
		atsc3_lls_slt_monitor_free(&lls_slt_monitor);
		/*
		   atsc3_lls_slt_monitor_free chains calls to:
				lls_slt_monitor_free_lls_sls_mmt_monitor(lls_slt_monitor); ->  MISSING IMPL lls_sls_mmt_monitor_free(), but not needed as only ptr ref's are transient
			-and-
				lls_slt_monitor_free_lls_sls_alc_monitor(lls_slt_monitor); -> lls_sls_alc_monitor_free

			so just clear out our local ref:
				lls_sls_mmt_monitor and lls_sls_alc_monitor
		*/
		lls_sls_mmt_monitor = NULL;
		lls_sls_alc_monitor = NULL;
	}

	lls_slt_monitor = lls_slt_monitor_create();
	
	//wire up a lls event for SLS table
//	lls_slt_monitor->atsc3_lls_on_sls_table_present_callback = &atsc3_lls_on_sls_table_present_ndk;
//	lls_slt_monitor->atsc3_lls_on_aeat_table_present_callback = &atsc3_lls_on_aeat_table_present_ndk;

}

void atsc3_mmt_signalling_information_on_held_message_present(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_held_message_t* mmt_atsc3_held_message) {
	if(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
		
		uint16_t service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
		block_t* held_payload = mmt_atsc3_held_message->held_message;
				
		block_Rewind(held_payload);
		char* service_held_fragment_filename = (char*) calloc(64, sizeof(char));
		
		snprintf(service_held_fragment_filename, 64, "route/%d/held.xml", service_id);
		
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("atsc3_mmt_signalling_information_on_held_message_present: HELD: writing to fragment %s, payload:\n%s", service_held_fragment_filename, block_Get(held_payload));

		block_Write_to_filename(held_payload, service_held_fragment_filename);
		
		free(service_held_fragment_filename);
	}
}

bool atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_route_component_t* mmt_atsc3_route_component) {
	//jjustman-2020-12-08 - TODO - add this route_component into our SLT monitoring
	//borrowed from atsc3_core_service_player_bridge_set_single_monitor_a331_service_id
	if(atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored) {
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk: atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored is set, ignoring!");
		return false;
	}

	lls_sls_alc_monitor = lls_sls_alc_monitor_create();
	lls_sls_alc_monitor->atsc3_lls_sls_alc_on_metadata_fragments_updated_callback = &atsc3_lls_sls_alc_on_metadata_fragments_updated_callback_internal_add_monitor_and_alc_session_flows;

	lls_sls_alc_monitor->atsc3_lls_slt_service = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service;
	lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
	lls_sls_alc_monitor->has_discontiguous_toi_flow = true; //jjustman-2020-07-27 - hack-ish

	lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service);
	lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

	lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_or_create_from_ip_udp_values(lls_slt_monitor, atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->atsc3_lls_slt_service, mmt_atsc3_route_component->stsid_destination_ip_address, mmt_atsc3_route_component->stsid_destination_udp_port, mmt_atsc3_route_component->stsid_source_ip_address);
	if(!lls_sls_alc_session) {
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
	}
	lls_sls_alc_monitor->lls_alc_session = lls_sls_alc_session;
	lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;

	lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);

//
//	//wire up event callback for alc close_object notification
//	lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback = &atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk;
//
//	//wire up event callback for alc MPD patching
//	lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_callback = &atsc3_lls_sls_alc_on_route_mpd_patched_ndk;
//	//jjustman-2020-08-05 - also atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback
//
//	lls_sls_alc_monitor->atsc3_lls_sls_alc_on_package_extract_completed_callback = &atsc3_lls_sls_alc_on_package_extract_completed_callback_ndk;

//#1569 - jjustman-2021-04-16 - not needed, as HELD for MMT is a SI message
//	lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback = &atsc3_sls_on_held_trigger_received_callback_impl;

	return true;
}



void process_from_udp_packet(udp_packet_t* udp_packet) {
	atsc3_alc_packet_t* alc_packet = NULL;
	
	//mmt types
	lls_sls_mmt_session_t*                  matching_lls_sls_mmt_session = NULL;
	atsc3_mmt_mfu_context_t*				atsc3_mmt_mfu_context = NULL;
	mmtp_packet_header_t*                   mmtp_packet_header = NULL;
	mmtp_asset_t*                           mmtp_asset = NULL;
	mmtp_packet_id_packets_container_t*     mmtp_packet_id_packets_container = NULL;

	mmtp_mpu_packet_t*                      mmtp_mpu_packet = NULL;
	mmtp_signalling_packet_t*               mmtp_signalling_packet = NULL;
	int8_t                                  mmtp_si_parsed_message_count = 0;
	

    //dispatch for LLS extraction and dump
    if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);

        //auto-assign our first ROUTE service id here
        if(lls_table && lls_table->lls_table_id == SLT) {
            _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("process_from_udp_packet: LLS table is:\n%s", lls_table->raw_xml.xml_payload);
			
			//reset our atsc3 context here
			//jjustman-2021-04-16 - TODO - fixme - this will wipe out lls_table
			//atsc3_reset_context();
			
			//jjustman-2021-04-16 - write out our lls.xml fragment
			block_t* lls_table_payload = block_Duplicate_from_ptr(lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);
			block_Write_to_filename(lls_table_payload, LLS_FRAGMENT_SLT_FILENAME);
			block_Destroy(&lls_table_payload);

            for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
                atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];
                
				//build our ROUTE monitor
				if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_ROUTE) {
					lls_sls_alc_monitor_t* lls_sls_alc_monitor_local = lls_sls_alc_monitor_create();
					lls_sls_alc_monitor_local->atsc3_lls_sls_alc_on_metadata_fragments_updated_callback = &atsc3_lls_sls_alc_on_metadata_fragments_updated_callback_internal_add_monitor_and_alc_session_flows;

					lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
					lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

					lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
					if(!lls_sls_alc_session) {
						_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("process_from_udp_packet: lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
					}
					lls_sls_alc_monitor_local->lls_alc_session = lls_sls_alc_session;
					lls_sls_alc_monitor_local->atsc3_lls_slt_service = atsc3_lls_slt_service;
					lls_sls_alc_monitor_local->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;

					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("process_from_udp_packet: adding lls_sls_alc_monitor: %p to lls_slt_monitor: %p, service_id: %d",
						   lls_sls_alc_monitor_local, lls_slt_monitor, lls_sls_alc_session->service_id);

					lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor_local);

					if(!lls_sls_alc_monitor) {
						lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor_local;
						lls_sls_alc_monitor =  lls_sls_alc_monitor_local;

					} else {
						//only swap out this lls_sls_alc_monitor if this alc flow is "retired"
					}
                } else if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_MMTP) {
										
					lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
					lls_sls_mmt_monitor->transients.atsc3_lls_slt_service = atsc3_lls_slt_service; //transient HACK!
					lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

					lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

					//we may not be initialized yet, so re-check again later
					//this should _never_happen...
					lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);

					if(!lls_sls_mmt_session) {
						_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("process_from_udp_packet: atsc3_core_service_player_bridge_set_single_monitor_a331_service_id: lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
					} else {
						atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_internal_flows_new(); //instead of jni callbacks - atsc3_mmt_mfu_context_callbacks_default_jni_new();
						lls_sls_mmt_session->atsc3_mmt_mfu_context = atsc3_mmt_mfu_context;
						
						//jjustman-2020-12-08 - wire up atsc3_mmt_signalling_information_on_routecomponent_message_present and atsc3_mmt_signalling_information_on_held_message_present
						lls_sls_mmt_session->atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_routecomponent_message_present = &atsc3_mmt_signalling_information_on_routecomponent_message_present_ndk;
						lls_sls_mmt_session->atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_held_message_present = &atsc3_mmt_signalling_information_on_held_message_present;
					}
					
					lls_sls_mmt_monitor->transients.lls_mmt_session = lls_sls_mmt_session;
					lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

					lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
				}
            }
        } else if(lls_table && lls_table->lls_table_id == AEAT) {
			_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("process_from_udp_packet: AEAT table is:\n%s", lls_table->raw_xml.xml_payload);
			//jjustman-2021-04-16 - write out our lls.xml fragment
			block_t* lls_table_payload = block_Duplicate_from_ptr(lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);
			block_Write_to_filename(lls_table_payload, LLS_FRAGMENT_AEAT_FILENAME);
			block_Destroy(&lls_table_payload);
		}
		
        return udp_packet_free(&udp_packet);
    }


    lls_sls_alc_monitor_t* matching_lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	//only used for service id filtering.
    lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);

    if(matching_lls_sls_alc_monitor && matching_lls_slt_alc_session) {

		//process ALC streams
		int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &alc_packet);

		if(!retval) {
			//check our alc_packet for a wrap-around TOI value, if it is a monitored TSI, and re-patch the MBMS MPD for updated availabilityStartTime and startNumber with last closed TOI values
			atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(alc_packet, matching_lls_sls_alc_monitor);

			//keep track of our EXT_FTI and update last_toi as needed for TOI length and manual set of the close_object flag
			atsc3_route_object_t* atsc3_route_object = atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(alc_packet, matching_lls_sls_alc_monitor);

			//persist to disk, process sls mbms and/or emit ROUTE media_delivery_event complete to the application tier if
			//the full packet has been recovered (e.g. no missing data units in the forward transmission)
			if(atsc3_route_object) {
				atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(&udp_packet->udp_flow, alc_packet, matching_lls_sls_alc_monitor, atsc3_route_object);
				alc_packet_received_count++;

			} else {
				_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR("process_from_udp_packet: Error in ALC persist, atsc3_route_object is NULL!");

			}

		} else {
			_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR("process_from_udp_packet: Error in ALC decode: %d", retval);
		}
	} else {

		
		//jjustman-2021-04-16 - todo - refactor me
		//MMT: Find a matching SLS service from this packet flow, and if the selected atsc3_lls_slt_service is monitored, enqueue for MFU DU re-constituion and emission
		matching_lls_sls_mmt_session = lls_sls_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

		if(matching_lls_sls_mmt_session && lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id == lls_slt_monitor->lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id) {

			atsc3_mmt_mfu_context = matching_lls_sls_mmt_session->atsc3_mmt_mfu_context;
			
			if(!atsc3_mmt_mfu_context) {
				goto error;
			}

			mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

			if(!mmtp_packet_header) {
				goto error;
			}
			mmtp_packet_header_dump(mmtp_packet_header);

			mmtp_asset = atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context, udp_packet, lls_slt_monitor, matching_lls_sls_mmt_session);
			mmtp_packet_id_packets_container = atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context, mmtp_asset, mmtp_packet_header);

			if(mmtp_packet_header->mmtp_payload_type == 0x0) {
				
#ifdef __ENABLE_MMT_MPU_PROCESSING_
				mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
				if(!mmtp_mpu_packet) {
					goto error;
				}

				if(mmtp_mpu_packet->mpu_timed_flag == 1) {
					mmtp_mpu_dump_header(mmtp_mpu_packet);

					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,", udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

					mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);
					goto cleanup;

				} else {
					//non-timed -
					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_mpu_packet->mmtp_packet_id);
					goto error;
				}
#else
				//just release this packet header
				mmtp_packet_header_free(&mmtp_packet_header);
#endif
			} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

				mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
				if(!mmtp_signalling_packet) {
					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet_parse_and_free_packet_header_from_block_t - mmtp_signalling_packet was NULL for udp_packet: %p, udp_packet->p_size: %d", udp_packet, udp_packet->data->p_size);
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
						_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet->si_fragmentation_indicator is: 0x%02x while mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count is 0, discarding!", mmtp_signalling_packet->si_fragmentation_indicator);

						goto error;
					}

					//push our first mmtp_signalling_packet for re-assembly (explicit mmtp_signalling_packet->si_fragmentation_indicator == 0x1 (01))
					if(mmtp_signalling_packet->si_fragmentation_indicator == 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
						_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_id_packets_container_add_mmtp_signalling_packet - adding first entry, mmtp_signalling_packet: %p, mmtp_signalling_packet->si_fragmentation_indicator: 0x%02x, mmtp_signalling_packet->si_fragmentation_counter: %d (A: %d, H: %d)",
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
							_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: packet_sequence_number mismatch, discarding! last_mmtp_signalling_packet: %p, psn: %d (next: %d), frag: 0x%02x, frag_counter: %d, mmtp_signalling_packet: %p, psn: %d, frag: 0x%02x, frag_counter: %d",
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
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp is NULL, bailing!");
								mmtp_signalling_packet_vector_valid = false;
								break;
							}

							if(mmtp_signalling_packet_last_temp && mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id) {
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id, %d != %d",
											 mmtp_signalling_packet_last_temp->mmtp_packet_id, mmtp_signalling_packet_temp->mmtp_packet_id);
								mmtp_signalling_packet_vector_valid = false;
								break;
							}

							if(i == 0 && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x1) { //sanity check (01)
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, i == 0 but mmtp_signalling_packet_temp->si_fragmentation_indicator is: 0x%02x", mmtp_signalling_packet_temp->si_fragmentation_indicator);
								mmtp_signalling_packet_vector_valid = false;
								break;
							}

							if(mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i)) {
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i), %d != %d, bailing!",
											 mmtp_signalling_packet_temp->si_fragmentation_counter,
											 mmtp_signalling_packet_vector_count - 1 - i);

								mmtp_signalling_packet_vector_valid = false;
								break;
							}

							//anything less than the "last" packet should be fi==0x2 (10) (fragment that is neither the first nor the last fragment)
							if(i < (mmtp_signalling_packet_vector_count - 2) && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x2) {
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_indicator: 0x%02x at index: %d, vector_count: %d",
											 mmtp_signalling_packet_temp->si_fragmentation_indicator, i, mmtp_signalling_packet_vector_count);

								mmtp_signalling_packet_vector_valid = false;
								break;
							}

							mmtp_message_payload_final_size += mmtp_signalling_packet_temp->udp_packet_inner_msg_payload->p_size;

							//if we are the last index in the vector AND our fi==0x3 (11) (last fragment of a signalling message), then mark us as complete, otherwise
							if(i == (mmtp_signalling_packet_vector_count - 1) && mmtp_signalling_packet_temp->si_fragmentation_indicator == 0x3) {
								mmtp_signalling_packet_vector_complete = true;
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector is complete, packet_id: %d, vector size: %d",
											 mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
							} else {
								_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector not yet complete, i: %d, packet_id: %d, vector size: %d",
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

							_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet msg_payload re-assembly is complete, using first mmtp_signalling_packet: %p, udp_packet_inner_msg_payload size: %d, value: %s",
										 mmtp_signalling_packet,
										 mmtp_signalling_packet->udp_packet_inner_msg_payload->p_size,
										 mmtp_signalling_packet->udp_packet_inner_msg_payload->p_buffer);

							mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, mmtp_signalling_packet->udp_packet_inner_msg_payload);

						} else {
							//noop: continue to accumulate
							_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet - adding to vector, size: %d", mmtp_signalling_packet_vector_count + 1);
							mmtp_signalling_packet = NULL; //so we don't free pending accumulated packets
						}
					}
				}

				if(mmtp_si_parsed_message_count > 0) {
					mmt_signalling_message_dump(mmtp_signalling_packet);

					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_TRACE("process_packet: calling mmt_signalling_message_dispatch_context_notification_callbacks with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
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
				_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
				goto error;
			}

			goto cleanup;
		}
		
	}

udp_packet_free:
    if(alc_packet) {
        alc_packet_free(&alc_packet);
        alc_packet = NULL;
    }
error:
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

    return udp_packet_free(&udp_packet);
}

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {

    udp_packet_t* udp_packet = udp_packet_process_from_ptr(block_Get(packet), packet->p_size);

    if((rx_udp_invocation_count % 1000) == 0) {
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d, udp_packet: %p",
					plp_num, rx_udp_invocation_count++, packet, packet->p_size, udp_packet);
    }

	if(!udp_packet) {
		return;
	}

	process_from_udp_packet(udp_packet);
}


#ifdef __DEBUGGING_DUMP_TIMING_MANAGEMENT_AND_PREAMBLE_PACKETS

void atsc3_stltp_timing_management_packet_collection_callback(atsc3_stltp_timing_management_packet_tv* atsc3_stltp_timing_management_packet_v) {
	int _LAST_STLTP_PARSER_DUMP_ENABLED = _STLTP_PARSER_DUMP_ENABLED;
	_STLTP_PARSER_DUMP_ENABLED = 1;

	for(int i=0; i < atsc3_stltp_timing_management_packet_v->count; i++) {

		atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_timing_management_packet_v->data[i];
		atsc3_timing_management_packet_t* atsc3_timing_management_packet = atsc3_stltp_timing_management_packet->timing_management_packet;

		atsc3_timing_management_packet_dump(atsc3_timing_management_packet);
	}
	_STLTP_PARSER_DUMP_ENABLED = _LAST_STLTP_PARSER_DUMP_ENABLED;

}

void atsc3_stltp_preamble_packet_collection_callback(atsc3_stltp_preamble_packet_tv* atsc3_stltp_preamble_packet_v) {
	int _LAST_STLTP_PARSER_DUMP_ENABLED = _STLTP_PARSER_DUMP_ENABLED;
	_STLTP_PARSER_DUMP_ENABLED = 1;

	for(int i=0; i < atsc3_stltp_preamble_packet_v->count; i++) {

		atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet_v->data[i];
		atsc3_preamble_packet_t* atsc3_preamble_packet = atsc3_stltp_preamble_packet->preamble_packet;

		atsc3_preamble_packet_dump(atsc3_preamble_packet);
	}

	_STLTP_PARSER_DUMP_ENABLED = _LAST_STLTP_PARSER_DUMP_ENABLED;
}

#endif

int start_srt_rx_stltp_virtual_phy(string srt_connection_string) {
	int res = -1;
	srtRxSTLTPVirtualPHY = new SRTRxSTLTPVirtualPHY(srt_connection_string);

	srtRxSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);

	//jjustman-2020-10-06 - dump T&M and Preamble packet for extra debugging

#ifdef __DEBUGGING_DUMP_TIMING_MANAGEMENT_AND_PREAMBLE_PACKETS
	atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = srtRxSTLTPVirtualPHY->get_atsc3_stltp_depacketizer_context();

	atsc3_stltp_depacketizer_context->atsc3_stltp_timing_management_packet_collection_callback = atsc3_stltp_timing_management_packet_collection_callback;
	atsc3_stltp_depacketizer_context->atsc3_stltp_preamble_packet_collection_callback = atsc3_stltp_preamble_packet_collection_callback;
#endif


	res = srtRxSTLTPVirtualPHY->run();

	srt_thread_run_start_time = gt();

	return res;
}

int stop_srt_rx_stltp_virtual_phy() {
	int res = -1;
	if(srtRxSTLTPVirtualPHY) {
		res = srtRxSTLTPVirtualPHY->stop();

		delete srtRxSTLTPVirtualPHY;
	}
	return res;
}

void configure_lls_sls_monitor() {

    lls_slt_monitor = lls_slt_monitor_create();
	//jjustman-2021-07-01 - do not add this dummy entry, wait till we have a proper SLT for alc session and alc monitor

/*
	alc_arguments = (atsc3_alc_arguments_t*)calloc(1, sizeof(atsc3_alc_arguments_t));
    atsc3_alc_session = atsc3_open_alc_session(alc_arguments);

    lls_sls_alc_monitor = lls_sls_alc_monitor_create();
    lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);
 */

}

int main(int argc, char* argv[] ) {

#ifdef __PENDANTIC__
	_ALP_PARSER_INFO_ENABLED = 1;
	_ALP_PARSER_DEBUG_ENABLED = 1;

	_ATSC3_STLTP_DEPACKETIZER_INFO_ENABLED = 1;
	_ATSC3_STLTP_DEPACKETIZER_DEBUG_ENABLED = 1;
	_ATSC3_STLTP_DEPACKETIZER_TRACE_ENABLED = 1;

	_STLTP_PARSER_INFO_ENABLED = 1;
	_STLTP_PARSER_DEBUG_ENABLED = 1;
	_STLTP_PARSER_TRACE_ENABLED = 1;

	_STLTP_TYPES_DEBUG_ENABLED = 1;
	_STLTP_TYPES_TRACE_ENABLED = 1;
	_FDT_PARSER_DEBUG_ENABLED = 1;
	
#endif
	string srt_connection_string = "srt://las.srt.atsc3.com:31351?passphrase=6E35F28D-21B8-46A4-8081-F3232D150728&packetfilter=fec";

	//jjustman-2021-04-16 - testing for mmt ba extraction
	//SRT_LISTEN_PORT="31346"
	//SRT_PASSPHRASE="055E0771-97B2-4447-8B5C-3B2497D0DE32"
	// srt://sea.srt.atsc3.com:31346?passphrase=055E0771-97B2-4447-8B5C-3B2497D0DE32&packetfilter=fec


	
	if(argc > 1) {
		srt_connection_string = string(argv[1]);
	}

	configure_lls_sls_monitor();

	_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("starting atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer with connection: %s", srt_connection_string.c_str());
	start_srt_rx_stltp_virtual_phy(srt_connection_string);


	int loop_count = 0;
	bool should_break = false;
	usleep(1000000);
	while(!should_break) {
		usleep(10000000);
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("srt_is_running: %d", srtRxSTLTPVirtualPHY->is_running());
		if(loop_count++ > 10) {
			should_break = !srtRxSTLTPVirtualPHY->is_running();
		}
	}

	double srt_thread_run_end_time = gt();

	_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("srtRxSTLTPVirtualPHY !running, start time: %0.3f, end time: %0.3f, duration: %0.3f",
			srt_thread_run_start_time,
			srt_thread_run_end_time,
			srt_thread_run_end_time - srt_thread_run_start_time);

	stop_srt_rx_stltp_virtual_phy();


    return 0;
}




