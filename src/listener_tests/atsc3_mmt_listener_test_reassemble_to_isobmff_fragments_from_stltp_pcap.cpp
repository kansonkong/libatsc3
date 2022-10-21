/*
 * atsc3_mmt_listener_test_reassemble_to_isobmff_fragments_from_stltp_pcap.cpp
 *
 *  Created on: 2022-10-20
 *      Author: jjustman
 *
 *      reassemble mmt emission into corresponding isobmff fragments per packet_id (e.g. v/a/c essence)
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

#include <atsc3_utils.h>
#include <atsc3_listener_udp.h>
#include <atsc3_pcap_type.h>

#include <atsc3_lls.h>
#include <atsc3_lls_slt_parser.h>
#include <atsc3_lls_sls_monitor_output_buffer_utils.h>
#include <atsc3_mmtp_packet_types.h>
#include <atsc3_mmtp_parser.h>
#include <atsc3_ntp_utils.h>
#include <atsc3_mmt_mpu_utils.h>
#include <atsc3_logging_externs.h>

#include <atsc3_mmt_context_mfu_depacketizer.h>
#include <atsc3_mmt_context_mfu_depacketizer_callbacks_noop.h>

#include <atsc3_stltp_types.h>

#include <phy/virtual/PcapSTLTPVirtualPHY.h>

#define _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;
lls_slt_monitor_t* lls_slt_monitor;

//jjustman-2019-10-03 - context event callbacks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context;


void process_packet(block_t* ip_blockt) {
    mmtp_packet_header_t* mmtp_packet_header = NULL;
    
    udp_packet_t* udp_packet = udp_packet_process_from_block_t(ip_blockt);
    if(!udp_packet) {
        return;
    }
    
    //dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		atsc3_lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
        goto cleanup;
	}

	if(udp_packet->udp_flow.dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->udp_flow.dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
		//out of range, so drop

		goto cleanup;
	}

	if((dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) || (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter)) {

		lls_sls_mmt_session_t* matching_lls_slt_mmt_session = lls_sls_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	    if(matching_lls_slt_mmt_session) {

	    	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

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
				if(!mmtp_mpu_packet) {
					__ATSC3_ERROR("process_packet: mmtp_mpu_packet_parse_from_block_t: returned mmtp_mpu_packet is NULL!");
					goto cleanup;
					
				}
				
                mmtp_mpu_persist_to_isobmff_fragments_per_flow(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet);

				if(mmtp_mpu_packet->mpu_timed_flag == 1) {
					mmtp_mpu_dump_header(mmtp_mpu_packet);
										
				} else {
					//non-timed
					__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
				}
			} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

				mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
                uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
                if(parsed_count) {
                    mmt_signalling_message_dump(mmtp_signalling_packet);
                }

			} else {
				__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
				goto cleanup;
			}
		}
	}

cleanup:


	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
}

uint64_t rx_udp_invocation_count = 0;

void atsc3_stltp_baseband_alp_packet_collection_callback(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

    for(int i=0; i < atsc3_alp_packet_collection->atsc3_baseband_packet_v.count; i++) {
        atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_alp_packet_collection->atsc3_baseband_packet_v.data[i];

        //atsc3_baseband_packet_dump(atsc3_baseband_packet);
    }

    for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
        atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
        block_Rewind(atsc3_alp_packet->alp_payload);
        atsc3_alp_packet_dump(atsc3_alp_packet);
    }
}

void atsc3_stltp_timing_management_packet_collection_callback(atsc3_stltp_timing_management_packet_tv* atsc3_stltp_timing_management_packet_v) {
    for(int i=0; i < atsc3_stltp_timing_management_packet_v->count; i++) {

        atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_timing_management_packet_v->data[i];
        atsc3_timing_management_packet_t* atsc3_timing_management_packet = atsc3_stltp_timing_management_packet->timing_management_packet;

        //atsc3_timing_management_packet_dump(atsc3_timing_management_packet);
    }

}

void atsc3_stltp_preamble_packet_collection_callback(atsc3_stltp_preamble_packet_tv* atsc3_stltp_preamble_packet_v) {
    for(int i=0; i < atsc3_stltp_preamble_packet_v->count; i++) {

        atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet_v->data[i];
        atsc3_preamble_packet_t* atsc3_preamble_packet = atsc3_stltp_preamble_packet->preamble_packet;

        //atsc3_preamble_packet_dump(atsc3_preamble_packet);
    }
}

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {
    if((rx_udp_invocation_count++ % 10000) == 0) {
        _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_DEBUG("PLP: %d, packet number: %" PRIu64 ", packet: %p, len: %d",
                plp_num, rx_udp_invocation_count, packet, packet->p_size);
    }
    
    process_packet(packet);
}

int process_stltp_pcap_file(char* filename, char* stltp_dst_ip, char* stltp_dst_port) {

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;
    atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

    PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = new PcapSTLTPVirtualPHY();

    pcapSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);
    atsc3_stltp_depacketizer_context = pcapSTLTPVirtualPHY->get_atsc3_stltp_depacketizer_context();

    //wire up specific callbacks for premable and t&m packets here
    //atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback = atsc3_stltp_baseband_alp_packet_collection_callback;
    //atsc3_stltp_depacketizer_context->atsc3_stltp_timing_management_packet_collection_callback = atsc3_stltp_timing_management_packet_collection_callback;
    //atsc3_stltp_depacketizer_context->atsc3_stltp_preamble_packet_collection_callback = atsc3_stltp_preamble_packet_collection_callback;

    pcapSTLTPVirtualPHY->atsc3_pcap_stltp_listen_ip_port_plp(stltp_dst_ip, stltp_dst_port, ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_VALUE);

    pcapSTLTPVirtualPHY->atsc3_pcap_replay_open_file(filename);

    pcapSTLTPVirtualPHY->atsc3_pcap_thread_run();

    double pcap_thread_run_start_time = gt();
    sleep(1);
    while(pcapSTLTPVirtualPHY->is_pcap_replay_running()) {
        usleep(1000000);
        atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();
        //not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
        if(atsc3_pcap_replay_context_volitale) {
            _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
                    atsc3_pcap_replay_context_volitale->pcap_file_pos,
                    atsc3_pcap_replay_context_volitale->pcap_file_len);
        }
    }
    double pcap_thread_run_end_time = gt();
    atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();

    double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
    double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_usec / 1000000.0));

    _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",
            pcap_thread_run_start_time,
            pcap_thread_run_end_time,
            pcap_thread_run_end_time - pcap_thread_run_start_time,
            first_packet_ts_s,
            last_packet_ts_s,
            (last_packet_ts_s - first_packet_ts_s)
            );

    pcapSTLTPVirtualPHY->atsc3_pcap_thread_stop();

    delete pcapSTLTPVirtualPHY;

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
    _MMTP_TRACE_ENABLED = 0;
    _MMT_MPU_PARSER_DEBUG_ENABLED = 0;
    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;

    _LLS_DEBUG_ENABLED = 0;

    _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
    _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;
    _MMT_SIGNALLING_MESSAGE_DESCRIPTOR_TRACE_ENABLED = 1;
    
    //jjustman-2021-09-15 - next level deeper is #define _MMT_SIGNALLING_MESSAGE_DUMP_HEX_PAYLOAD
    
    char* PCAP_FILENAME = "";
    
    char* stltp_dst_ip = "239.239.239.239";
    char* stltp_dst_port = "30000";

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_context = NULL;

    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    if(argc == 1) {
        println("%s - STLTP pcap replay test harness for atsc3 mmt essence extraction into ISOBMFF fragments", argv[0]);
        println("---");
        println("args: stltp_pcap_file_name (stltp_ip_address) (stltp_port) (dst_ip) (dst_port) (packet_id)");
        println(" stltp_pcap_file_name: pcap demuxed file to process MMT SLS emissions - note: defaults to stltp ip: 239.239.239.239 and port: 30000");
        println(" (stltp_ip_address): optional, filter to specific ip address, or * for wildcard");
        println("");
        println(" (stltp_port): optional, filter to specific port, or * for wildcard");
        println(" (dst_ip): optional, filter to specific ip address, or * for wildcard");
        println(" (dst_port): optional, filter to specific port, or * for wildcard");

        println("");
        exit(1);
    }
    
    //listen to all flows
    if(argc >= 2) {
        PCAP_FILENAME = argv[1];
    }
    
    if(argc==4) {
        stltp_dst_ip = argv[2];
        stltp_dst_port = argv[3];
    }
    
    if(argc>=5) {
        //listen to a selected flow
        filter_dst_ip = argv[4];

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

        if(argc>=5) {
            filter_dst_port = argv[5];
            if(!(strncmp("*", filter_dst_port, 1) == 0 || strncmp("-", filter_dst_port, 1) == 0)) {

                dst_port_filter_int = atoi(filter_dst_port);
                dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
                *dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
            }
        }

        _ATSC3_MMT_LISTENER_TEST_REASSEMBLE_TO_ISOBMFF_FRAGMENTS_FROM_STLTP_PCAP_TEST_INFO("reading file %s, stltp ip: %s, stltp port: %s, dst_ip: %s (%p), dst_port: %s (%p)",
                                                                 PCAP_FILENAME,
                                                                 stltp_dst_ip,
                                                                 stltp_dst_port,
                                                                 filter_dst_ip,
                                                                 dst_ip_addr_filter,
                                                                 filter_dst_port,
                                                                 dst_ip_port_filter);
                                                            

    }


    /** setup global structs **/

    lls_slt_monitor = lls_slt_monitor_create();
    
    //callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_callbacks_noop_new();
    
    process_stltp_pcap_file(PCAP_FILENAME, stltp_dst_ip, stltp_dst_port);

    return 0;
}
