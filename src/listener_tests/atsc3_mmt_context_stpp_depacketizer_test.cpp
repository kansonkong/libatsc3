/*
 * atsc3_mmt_context_stpp_depacketizer_test.c
 *
 *  Created on: Oct 19, 2019
 *      Author: jjustman
 *
 * sample listener for MMT stpp (IMSC-1) caption data
 */

int PACKET_COUNTER=0;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <limits.h>

#include "../atsc3_listener_udp.h"
#include "../atsc3_utils.h"

#include "../atsc3_lls.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_ntp_utils.h"
#include "../atsc3_mmt_mpu_utils.h"

#include "../atsc3_logging_externs.h"

#include "../atsc3_mmt_context_mfu_depacketizer.h"
#include "../atsc3_mmt_context_mfu_depacketizer_callbacks_noop.h"

//commandline stream filtering per flow

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;


//jjustman-2019-09-18: refactored MMTP flow collection management
mmtp_flow_t* mmtp_flow;

//todo: jjustman-2019-09-18 refactor me out for mpu recon persitance
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;

lls_slt_monitor_t* lls_slt_monitor;

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

//jjustman-2019-10-03 - context event callbacks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context;

//stpp packet_id under test
//jjustman-2021-09-14 - TODO: resolve this properly instead of a hardcoded value...
uint16_t atsc3_mmt_context_stpp_packet_id_for_testing = 300;

void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump: stpp mp_table packet_id: %u, stpp_packet_id_under_test: %u",
			stpp_packet_id,
			atsc3_mmt_context_stpp_packet_id_for_testing);
}

void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump: stpp mp_table packet_id: %u, stpp_packet_id_under_test: %u, mpu_sequence_number: %d, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
				stpp_packet_id,
				atsc3_mmt_context_stpp_packet_id_for_testing,
				mpu_sequence_number,
				mpu_presentation_time_ntp64,
				mpu_presentation_time_seconds,
				mpu_presentation_time_microseconds);
}




void atsc3_mmt_mpu_mfu_on_sample_complete_dump(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}

	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, mmtp_timestamp: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
			packet_id,
            mpu_sequence_number,
			mmtp_timestamp,
            sample_number,
			mmt_mfu_sample,
			mmt_mfu_sample->p_size);

    char myFileName[128];
    snprintf(myFileName, 128, "mpu/%d.%d.%d.mfu.complete", packet_id, mpu_sequence_number, sample_number);
    FILE* fp = fopen(myFileName, "w");
    if(fp) {
    	block_Rewind(mmt_mfu_sample);
    	fwrite(block_Get(mmt_mfu_sample), mmt_mfu_sample->p_size, 1, fp);
    	fclose(fp);
    } else {
    	  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to complete filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
    			    myFileName,
				  	packet_id,
    	            mpu_sequence_number,
    	            sample_number,
    				mmt_mfu_sample,
    				mmt_mfu_sample->p_size);

    }
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_dump(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_dump: packet_id: %u, mpu_sequence_number: %u, mmtp_timestamp: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
				packet_id,
	            mpu_sequence_number,
				mmtp_timestamp,
	            sample_number,
				mmt_mfu_sample,
				mmt_mfu_sample->p_size);

	char myFileName[128];
	snprintf(myFileName,  128, "mpu/%d.%d.%d.mfu.corrupt", packet_id, mpu_sequence_number, sample_number);
	FILE* fp = fopen(myFileName, "w");
	if(fp) {
		block_Rewind(mmt_mfu_sample);
		fwrite(block_Get(mmt_mfu_sample), mmt_mfu_sample->p_size, 1, fp);
		fclose(fp);
	} else {
		  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to corrupt filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
					myFileName,
					packet_id,
					mpu_sequence_number,
					sample_number,
					mmt_mfu_sample,
					mmt_mfu_sample->p_size);

	}
}



void atsc3_mmt_mpu_mfu_on_sample_missing_dump(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
				packet_id,
	            mpu_sequence_number,
	            sample_number);

	char myFileName[128];
	snprintf(myFileName,  128, "mpu/%d.%d.%d.mfu.missing", packet_id, mpu_sequence_number, sample_number);
	FILE* fp = fopen(myFileName, "w");
	if(fp) {

		fwrite("x", 1, 1, fp);
		fclose(fp);
	} else {
		  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to missing filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
					myFileName,
					packet_id,
					mpu_sequence_number,
					sample_number);

	}
}

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

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	mmtp_packet_header_t* mmtp_packet_header = NULL;

	//jjustman-2021-08-30 - borrowed from atsc3_core_service_player_bridge.cpp
	mmtp_packet_id_packets_container_t*     mmtp_packet_id_packets_container = NULL;
	mmtp_asset_t*                           mmtp_asset = NULL;

	mmtp_signalling_packet_t*               mmtp_signalling_packet = NULL;
	int8_t                                  mmtp_si_parsed_message_count = 0;


	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		return udp_packet_free(&udp_packet);
	}

	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
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
								//TODO:  make sure
								//lls_service->broadcast_svc_signaling.sls_destination_ip_address && lls_service->broadcast_svc_signaling.sls_destination_udp_port
								//match our dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter and port filter
								__INFO("Adding service: %d", atsc3_lls_slt_service->service_id);

								lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
								lls_sls_mmt_monitor->transients.atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!
								lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

								lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

								//we may not be initialized yet, so re-check again later
								//this should _never_happen...
								lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
								if(!lls_sls_mmt_session) {
									__WARN("lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
								}
								lls_sls_mmt_monitor->transients.lls_mmt_session = lls_sls_mmt_session;
								lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

								lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
							}
						}
					}
				}
			}
		}

		__TRACE("Checking lls_sls_mmt_monitor: %p,", lls_sls_mmt_monitor);

		if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->transients.lls_mmt_session) {
			__TRACE("Checking lls_sls_mmt_monitor->lls_mmt_session: %p,", lls_sls_mmt_monitor->lls_mmt_session);
		}

		
		return udp_packet_free(&udp_packet);
	}

    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        return udp_packet_free(&udp_packet);
    }


    //TODO: jjustman-2019-10-03 - packet header parsing to dispatcher mapping
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = lls_sls_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	__TRACE("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

	if(matching_lls_sls_mmt_session && lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id == lls_slt_monitor->lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id) {

		mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);
        
		if(!mmtp_packet_header) {
			return udp_packet_free(&udp_packet);
		}

		//for filtering MMT flows by a specific packet_id
		if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
			goto cleanup;
		}

		mmtp_packet_header_dump(mmtp_packet_header);
		
		mmtp_asset = atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context, udp_packet, lls_slt_monitor, matching_lls_sls_mmt_session);
		mmtp_packet_id_packets_container = atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context, mmtp_asset, mmtp_packet_header);
	
		//dump header, then dump applicable packet type
		if(mmtp_packet_header->mmtp_payload_type == 0x0) {
			//bail if we are not the MFU packet_id under test, but allow SLS/signalling messages to passthru

            mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            if(!mmtp_mpu_packet) {
                goto error;
            }
            
			if(mmtp_mpu_packet->mpu_timed_flag == 1) {
				mmtp_mpu_dump_header(mmtp_mpu_packet);

				//TODO: jjustman-2019-10-03 - handle context parameters better
				// mmtp_flow, lls_slt_monitor, , udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);
				
				atsc3_mmt_mfu_context->matching_lls_sls_mmt_session = matching_lls_sls_mmt_session;
				
				__TRACE("process_packet: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,",
						udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

				if(mmtp_mpu_packet->mmtp_packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
					//discard
					goto cleanup;
				}
				
				mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

			} else {
				//non-timed
				__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
			}
		} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

			//jjustman-2021-08-30 - TODO: refactor this down into atsc3_mmt_signalling_message_depacketizer.c
			
			mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
			if(!mmtp_signalling_packet) {
				__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet_parse_and_free_packet_header_from_block_t - mmtp_signalling_packet was NULL for udp_packet: %p, udp_packet->p_size: %d", udp_packet, udp_packet->data->p_size);
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
					__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet->si_fragmentation_indicator is: 0x%02x while mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count is 0, discarding!", mmtp_signalling_packet->si_fragmentation_indicator);

					goto error;
				}

				//push our first mmtp_signalling_packet for re-assembly (explicit mmtp_signalling_packet->si_fragmentation_indicator == 0x1 (01))
				if(mmtp_signalling_packet->si_fragmentation_indicator == 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
					__ATSC3_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_id_packets_container_add_mmtp_signalling_packet - adding first entry, mmtp_signalling_packet: %p, mmtp_signalling_packet->si_fragmentation_indicator: 0x%02x, mmtp_signalling_packet->si_fragmentation_counter: %d (A: %d, H: %d)",
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
						__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: packet_sequence_number mismatch, discarding! last_mmtp_signalling_packet: %p, psn: %d (next: %d), frag: 0x%02x, frag_counter: %d, mmtp_signalling_packet: %p, psn: %d, frag: 0x%02x, frag_counter: %d",
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
							__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp is NULL, bailing!");
							mmtp_signalling_packet_vector_valid = false;
							break;
						}

						if(mmtp_signalling_packet_last_temp && mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id) {
							__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id, %d != %d",
										 mmtp_signalling_packet_last_temp->mmtp_packet_id, mmtp_signalling_packet_temp->mmtp_packet_id);
							mmtp_signalling_packet_vector_valid = false;
							break;
						}

						if(i == 0 && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x1) { //sanity check (01)
							__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, i == 0 but mmtp_signalling_packet_temp->si_fragmentation_indicator is: 0x%02x", mmtp_signalling_packet_temp->si_fragmentation_indicator);
							mmtp_signalling_packet_vector_valid = false;
							break;
						}

						if(mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i)) {
							__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i), %d != %d, bailing!",
										 mmtp_signalling_packet_temp->si_fragmentation_counter,
										 mmtp_signalling_packet_vector_count - 1 - i);

							mmtp_signalling_packet_vector_valid = false;
							break;
						}

						//anything less than the "last" packet should be fi==0x2 (10) (fragment that is neither the first nor the last fragment)
						if(i < (mmtp_signalling_packet_vector_count - 2) && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x2) {
							__ATSC3_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_indicator: 0x%02x at index: %d, vector_count: %d",
										 mmtp_signalling_packet_temp->si_fragmentation_indicator, i, mmtp_signalling_packet_vector_count);

							mmtp_signalling_packet_vector_valid = false;
							break;
						}

						mmtp_message_payload_final_size += mmtp_signalling_packet_temp->udp_packet_inner_msg_payload->p_size;

						//if we are the last index in the vector AND our fi==0x3 (11) (last fragment of a signalling message), then mark us as complete, otherwise
						if(i == (mmtp_signalling_packet_vector_count - 1) && mmtp_signalling_packet_temp->si_fragmentation_indicator == 0x3) {
							mmtp_signalling_packet_vector_complete = true;
							__ATSC3_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector is complete, packet_id: %d, vector size: %d",
										 mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
						} else {
							__ATSC3_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector not yet complete, i: %d, packet_id: %d, vector size: %d",
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

						__ATSC3_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet msg_payload re-assembly is complete, using first mmtp_signalling_packet: %p, udp_packet_inner_msg_payload size: %d, value: %s",
									 mmtp_signalling_packet,
									 mmtp_signalling_packet->udp_packet_inner_msg_payload->p_size,
									 mmtp_signalling_packet->udp_packet_inner_msg_payload->p_buffer);

						mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, mmtp_signalling_packet->udp_packet_inner_msg_payload);

					} else {
						//noop: continue to accumulate
						__ATSC3_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet - adding to vector, size: %d", mmtp_signalling_packet_vector_count + 1);
						mmtp_signalling_packet = NULL; //so we don't free pending accumulated packets
					}
				}
			}

			if(mmtp_si_parsed_message_count > 0) {
				mmt_signalling_message_dump(mmtp_signalling_packet);

				__ATSC3_TRACE("process_packet: calling mmt_signalling_message_dispatch_context_notification_callbacks with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
						udp_packet,
						mmtp_signalling_packet,
						atsc3_mmt_mfu_context);


				//dispatch our wired callbacks
				mmt_signalling_message_dispatch_context_notification_callbacks(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);

				//update our internal sls_mmt_session info
				bool has_updated_atsc3_mmt_sls_mpt_location_info = false;

				__ATSC3_TRACE("atsc3_core_service_bridge_process_packet_phy - before mmt_signalling_message_update_lls_sls_mmt_session with matching_lls_sls_mmt_session: %p", matching_lls_sls_mmt_session);

				has_updated_atsc3_mmt_sls_mpt_location_info = mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);

				__ATSC3_TRACE("atsc3_core_service_bridge_process_packet_phy - after mmt_signalling_message_update_lls_sls_mmt_session with matching_lls_sls_mmt_session: %p, has_updated_atsc3_mmt_sls_mpt_location_info: %d", matching_lls_sls_mmt_session, has_updated_atsc3_mmt_sls_mpt_location_info);

				//clear and flush out our mmtp_packet_id_packets_container if we came from re-assembly,
				// otherwise, final free of mmtp_signalling_packet packet in :cleanup
				mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
				goto cleanup;
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
	
	//jjustman-2020-11-12 - note: do not free mmtp_mpu_packet or mmtp_signalling_packet as they may have been added to a mmtp_*_packet_collection for re-assembly
	//unless si_fragmentation_indicator == 0x0, then we can safely release, as we do not push single units to the mmtp_packet_id_packets_container->mmtp_signalling_packet_v
	if(mmtp_signalling_packet && mmtp_signalling_packet->si_fragmentation_indicator == 0x0) {
		mmtp_signalling_packet_free(&mmtp_signalling_packet);
	}
	
    return;

error:
    __ATSC3_WARN("process_packet: error, bailing loop!");
    return;
}


void* pcap_loop_run_thread(void* dev_pointer) {
	char* dev = (char*) dev_pointer;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* descr;
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;

	pcap_lookupnet(dev, &netp, &maskp, errbuf);
    descr = pcap_open_live(dev, MAX_PCAP_LEN, 1, 1, errbuf);

    if(descr == NULL) {
        printf("pcap_open_live(): %s",errbuf);
        exit(1);
    }

    char filter[] = "udp";
    if(pcap_compile(descr,&fp, filter,0,netp) == -1) {
        fprintf(stderr,"Error calling pcap_compile");
        exit(1);
    }

    if(pcap_setfilter(descr,&fp) == -1) {
        fprintf(stderr,"Error setting filter");
        exit(1);
    }

    pcap_loop(descr,-1,process_packet,NULL);

    return 0;
}


/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;

#ifdef __lots_of_logging_
    _LLS_DEBUG_ENABLED = 1;

    _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
    _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;

    _LLS_SLT_PARSER_INFO_MMT_ENABLED = 1;
    _LLS_MMT_UTILS_TRACE_ENABLED = 1;
    
    _MMTP_DEBUG_ENABLED = 1;
    _MMT_MPU_PARSER_DEBUG_ENABLED = 1;

    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;
    _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED = 1;

#endif
    
	//jjustman-2021-09-14 - output path: mpu
	mkdir("mpu", 0777);

    
    char *dev;

    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    //listen to all flows
    if(argc == 2) {
    	dev = argv[1];
    	__INFO("listening on dev: %s", dev);
    } else if(argc>=4) {
    	//listen to a selected flow
    	dev = argv[1];
    	filter_dst_ip = argv[2];

		//skip ip address filter if our params are * or -
    	if(!(strncmp("*", filter_dst_ip, 1) == 0 || strncmp("-", filter_dst_ip, 1) == 0)) {
			dst_ip_addr_filter = (uint32_t*)calloc(1, sizeof(uint32_t));
			char* pch = strtok (filter_dst_ip,".");
			int offset = 24;
			while (pch != NULL && offset>=0) {
				uint8_t octet = atoi(pch);
				*dst_ip_addr_filter |= octet << offset;
				offset-=8;
				pch = strtok (NULL, ".");
			}
		}

    	if(argc>=4) {
    		filter_dst_port = argv[3];
        	if(!(strncmp("*", filter_dst_port, 1) == 0 || strncmp("-", filter_dst_port, 1) == 0)) {

				dst_port_filter_int = atoi(filter_dst_port);
				dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
				*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
        	}
    	}

    	if(argc>=5) {
    		filter_packet_id = argv[4];
        	if(!(strncmp("*", filter_packet_id, 1) == 0 || strncmp("-", filter_packet_id, 1) == 0)) {
				dst_packet_id_filter_int = atoi(filter_packet_id);
				dst_packet_id_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
				*dst_packet_id_filter |= dst_packet_id_filter_int & 0xFFFF;
        	}
    	}

    	__INFO("listening on dev: %s, dst_ip: %s (%p), dst_port: %s (%p), dst_packet_id: %s (%p)", dev, filter_dst_ip, dst_ip_addr_filter, filter_dst_port, dst_ip_port_filter, filter_packet_id, dst_packet_id_filter);


    } else {
		//jjustman-2021-08-30 - TODO: update to assign STPP packet_id from command line as needed
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt stpp emissions", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port) (packet_id)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");

    	println("");
    	exit(1);
    }

    /** setup global structs **/

    lls_slt_monitor = lls_slt_monitor_create();

    //callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_callbacks_noop_new();

    //stpp SLS related callbacks from mp_table
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id = &atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump;


	//MFU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt 	= &atsc3_mmt_mpu_mfu_on_sample_corrupt_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing 	= &atsc3_mmt_mpu_mfu_on_sample_missing_dump;

	atsc3_mmt_mfu_context->transients.lls_slt_monitor = lls_slt_monitor;

#ifndef _TEST_RUN_VALGRIND_OSX_

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);


	pthread_join(global_pcap_thread_id, NULL);

#else
	pcap_loop_run_thread(dev);
#endif


    return 0;
}




