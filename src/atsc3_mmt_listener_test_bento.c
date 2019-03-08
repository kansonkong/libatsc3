/*
 * atsc3_mmt_listener_test_bento.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 * Demo functional video/audio playback of a re-constituted MMT video stream with bento4.
 * Note: bento4 is only used in the final box-muxing into one MPU of video+audio, if you
 * can provide 2 elementary stream essences to your player, the gpl inclusion of bento4
 * is not needed.
 *
 */

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include "bento4/MPUtoISOBMFFProcessor.h"

extern "C" {

#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"
#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_mpu_parser.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_isobmff_tools.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_vector.h"
#include "atsc3_logging_externs.h"
#include "atsc3_mmt_reconstitution_from_media_sample.h"

}

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

lls_slt_monitor_t* lls_slt_monitor;
global_atsc3_stats_t* global_stats;


//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;

//now that are are performing memory management, we can't use this fragment object once the mpu_sequence has been re-assembeled and free'd
//mmtp_payload_fragments_union_t* mmtp_payload_previous_for_reassembly = NULL;

uint32_t __SEQUENCE_NUMBER_COUNT=0;

pipe_ffplay_buffer_t* pipe_ffplay_buffer = NULL;
uint32_t last_mpu_sequence_number = 0;
uint32_t fragment_count = 0;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}

	//dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data, udp_packet->data_length);
	}

	if(udp_packet->udp_flow.dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->udp_flow.dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
		//out of range, so drop
        return cleanup(&udp_packet);
	}

	if((dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) || (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter)) {

		mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);

		if(!mmtp_payload) {

	        return cleanup(&udp_packet);
		}

		//for filtering MMT flows by a specific packet_id
		if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_payload->mmtp_packet_header.mmtp_packet_id) {
			__TRACE("dropping packet id: %d", mmtp_payload->mmtp_packet_header.mmtp_packet_id);
            mmtp_payload_fragments_union_free(&mmtp_payload);

            return cleanup(&udp_packet);
		}

		//find our matching MMT flow and push it to reconsitution
		lls_sls_mmt_session_t* matching_lls_slt_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
		if(matching_lls_slt_mmt_session) {
			__TRACE("data len: %d", udp_packet->data_length)
    	    mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);
			if(mmtp_payload) {
				mmtp_process_from_payload(mmtp_sub_flow_vector, udp_flow_latest_mpu_sequence_number_container, lls_slt_monitor, udp_packet, &mmtp_payload, matching_lls_slt_mmt_session);
			   // mmtp_payload_fragments_union_free(&mmtp_payload);
			}
			return cleanup(&udp_packet);
		}
	}
cleanup:


	if(udp_packet) {
        if(udp_packet->data) {
            free(udp_packet->data);
            udp_packet->data = NULL;
        }
		free(udp_packet);
		udp_packet = NULL;
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
    descr = pcap_open_live(dev, MAX_PCAP_LEN, 1, 0, errbuf);

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

#define MAX_PCAP_LEN 1514
/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

    char *dev;

    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    _MMT_MPU_DEBUG_ENABLED = 0;
    _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
    _MMTP_DEBUG_ENABLED = 0;
    _MPU_DEBUG_ENABLED = 0;
    _LLS_DEBUG_ENABLED = 0;

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
			dst_ip_addr_filter = (uint32_t*) calloc(1, sizeof(uint32_t));
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
				__INFO("832:");

				dst_port_filter_int = atoi(filter_dst_port);
				dst_ip_port_filter = (uint16_t*)  calloc(1, sizeof(uint16_t));
				*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
			}
		}

		if(argc>=5) {
			filter_packet_id = argv[4];
			if(!(strncmp("*", filter_packet_id, 1) == 0 || strncmp("-", filter_packet_id, 1) == 0)) {
				dst_packet_id_filter_int = atoi(filter_packet_id);
				dst_packet_id_filter = (uint16_t*) calloc(1, sizeof(uint16_t));
				*dst_packet_id_filter |= dst_packet_id_filter_int & 0xFFFF;
			}
		}

		__INFO("listening on dev: %s, dst_ip: %s (%p), dst_port: %s (%p), dst_packet_id: %s (%p)", dev, filter_dst_ip, dst_ip_addr_filter, filter_dst_port, dst_ip_port_filter, filter_packet_id, dst_packet_id_filter);

    } else {
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt messages", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");
    	println("");
    	exit(1);
    }

    mmtp_sub_flow_vector = (mmtp_sub_flow_vector_t*)calloc(1, sizeof(mmtp_sub_flow_vector_t));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();
    lls_slt_monitor = lls_slt_monitor_create();


    mkdir("mpu", 0777);

    pipe_ffplay_buffer = pipe_create_ffplay();

#ifndef _TEST_RUN_VALGRIND_OSX_

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);

	pthread_join(global_pcap_thread_id, NULL);

#else
    pcap_loop_run(dev);
#endif


    return 0;
}

/**
 *
void __trace_dump_ip_header_info(u_char* ip_header) {
    __TRACE("Version\t\t\t\t\t%d", (ip_header[0] >> 4));
    __TRACE("IHL\t\t\t\t\t\t%d", (ip_header[0] & 0x0F));
    __TRACE("Type of Service\t\t\t%d", ip_header[1]);
    __TRACE("Total Length\t\t\t%d", ip_header[2]);
    __TRACE("Identification\t\t\t0x%02x 0x%02x", ip_header[3], ip_header[4]);
    __TRACE("Flags\t\t\t\t\t%d", ip_header[5] >> 5);
    __TRACE("Fragment Offset\t\t\t%d", (((ip_header[5] & 0x1F) << 8) + ip_header[6]));
    __TRACE("Time To Live\t\t\t%d", ip_header[7]);
    __TRACE("Header Checksum\t\t\t0x%02x 0x%02x", ip_header[10], ip_header[11]);
}
 */



