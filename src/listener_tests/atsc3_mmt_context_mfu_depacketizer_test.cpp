/*
 * atsc3_mmt_sls_listener_test.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 *
 * sample listener for MMT flow(s) to extract packet_id=0 sls data for testing
 */

//#define _ENABLE_TRACE 1
//#define _SHOW_PACKET_FLOW 1

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
//#include "../atsc3_lls_alc_utils.h"
//#include "alc_channel.h"
//#include "../atsc3_alc_rx.h"
//#include "../atsc3_alc_utils.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"

#include "../atsc3_logging_externs.h"
//#include "../stubs/atsc3_alc_stubs.h"

#include "../atsc3_mmt_context_mfu_depacketizer.h"

#define _ENABLE_DEBUG true

//commandline stream filtering

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

		__INFO("Checking lls_sls_mmt_monitor: %p,", lls_sls_mmt_monitor);

		if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
			__INFO("Checking lls_sls_mmt_monitor->lls_mmt_session: %p,", lls_sls_mmt_monitor->lls_mmt_session);
		}

		//recheck video_packet_id/audio_packet_id
		if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
			if(!lls_sls_mmt_monitor->video_packet_id) {
				lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);
				lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
				lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;
				__INFO("setting audio_packet_id/video_packet_id: %u, %u", lls_sls_mmt_monitor->audio_packet_id, lls_sls_mmt_monitor->video_packet_id);
			}

			if(lls_sls_mmt_monitor->video_packet_id) {
				lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = false;
				lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;

				//todo - jjustman-2019-09-05 - refactor this logic out

				if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer) {
					lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer = (http_output_buffer_t*)calloc(1, sizeof(http_output_buffer_t));
					lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex = lls_sls_monitor_reader_mutext_create();
				}
				lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled = true;
			}
		}
		return udp_packet_free(&udp_packet);
	}

    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        return udp_packet_free(&udp_packet);
    }


    //TODO: jjustman-2019-10-03 - packet header parsing to dispatcher mapping
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	__INFO("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

	if(matching_lls_sls_mmt_session) {

		mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

		if(!mmtp_packet_header) {
			return udp_packet_free(&udp_packet);
		}

		//for filtering MMT flows by a specific packet_id
		if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
			goto cleanup;
		}

		mmtp_packet_header_dump(mmtp_packet_header);

		//dump header, then dump applicable packet type
		if(mmtp_packet_header->mmtp_payload_type == 0x0) {
			mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, udp_packet->data);
			if(mmtp_mpu_packet->mpu_timed_flag == 1) {
				mmtp_mpu_dump_header(mmtp_mpu_packet);

				//TODO: jjustman-2019-10-03 - handle context parameters better
				// mmtp_flow, lls_slt_monitor, , udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);

				atsc3_mmt_mfu_context->mmtp_flow = mmtp_flow;
				atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container;
				atsc3_mmt_mfu_context->lls_slt_monitor = lls_slt_monitor;
				atsc3_mmt_mfu_context->matching_lls_sls_mmt_session = matching_lls_sls_mmt_session;

				__INFO("context_dispatcher: calling mmtp_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,",
						udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

				mmtp_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);
//				            if(mmtp_mpu_packet && matching_lls_sls_mmt_session && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video) {
//
//				                __ATSC3_DEBUG("audio flow: packet_id: %d, mpu_sequence_number: %d, updated: %d, video flow: packet_id: %d, mpu_sequence_number: %d, updated: %d",
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed,
//
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
//				                         matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed);
//
//				                if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed || matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed) {
//				                    atsc3_mmt_hls_fmp4_update_manifest(matching_lls_sls_mmt_session);
//				                }
//				            }
			} else {
				//non-timed
				__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
			}
		} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

			mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
			uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
			if(parsed_count) {
				mmt_signalling_message_dump(mmtp_signalling_packet);

				__INFO("context_dispatcher: calling mmt_signalling_message_process_with_context with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
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
						__INFO("HACK: seting audio_packet_id/video_packet_id: %u, %u",
								matching_lls_sls_mmt_session->audio_packet_id,
								matching_lls_sls_mmt_session->video_packet_id);

						if(matching_lls_sls_mmt_session->audio_packet_id) {
							lls_sls_mmt_monitor->audio_packet_id = matching_lls_sls_mmt_session->audio_packet_id;
						}

						if(matching_lls_sls_mmt_session->video_packet_id) {
							lls_sls_mmt_monitor->video_packet_id = matching_lls_sls_mmt_session->video_packet_id;
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

	_MMTP_DEBUG_ENABLED = 1;
	_MMT_MPU_PARSER_DEBUG_ENABLED = 1;

	_LLS_DEBUG_ENABLED = 1;

	_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
	_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;

	_LLS_SLT_PARSER_INFO_MMT_ENABLED = 1;
	_LLS_MMT_UTILS_TRACE_ENABLED = 1;

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
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt sls", argv[0]);
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
    mmtp_flow = mmtp_flow_new();
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    //callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_new();


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




