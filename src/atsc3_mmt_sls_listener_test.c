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

#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"
//#include "atsc3_lls_alc_utils.h"
//#include "alc_channel.h"
//#include "atsc3_alc_rx.h"
//#include "atsc3_alc_utils.h"

#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_mpu_utils.h"

#include "atsc3_logging_externs.h"
#include "stubs/atsc3_alc_stubs.h"

#define _ENABLE_DEBUG true

//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session

lls_slt_monitor_t* lls_slt_monitor;

//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t*                          mmtp_sub_flow_vector;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;

/** create some dummy stub symbols so we don't link in the full alc processing
 *
Undefined symbols for architecture x86_64:
  "_lls_sls_alc_session_vector_create", referenced from:
      _lls_slt_monitor_create in atsc3_lls_slt_parser.o
  "_lls_slt_alc_session_find_or_create", referenced from:
      _lls_slt_table_process_update in atsc3_lls_slt_parser.o
  "_lls_slt_alc_session_remove", referenced from:
      _lls_slt_table_process_update in atsc3_lls_slt_parser.o
ld: symbol(s) not found for architecture x86_64
 */

mmtp_payload_fragments_union_t* mmtp_parse_from_udp_packet(udp_packet_t *udp_packet) {

    mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);

    if(!mmtp_payload) {
        __ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
                __toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
                udp_packet->udp_flow.src_ip_addr,
                __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
                udp_packet->udp_flow.dst_ip_addr);
        return NULL;
    }

    return mmtp_payload;
}

void mmtp_process_sls_from_payload(udp_packet_t *udp_packet, mmtp_payload_fragments_union_t** mmtp_payload_p, lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {
	mmtp_payload_fragments_union_t* mmtp_payload = * mmtp_payload_p;
//
//    mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
//    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
//    mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =    NULL;
//    mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
//    mmtp_sub_flow_t* mmtp_sub_flow = NULL;
//
	__INFO("processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
	//mmtp_payload->mmtp_signalling_message_fragments.payload
	signaling_message_dump(mmtp_payload);

}

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		return cleanup(&udp_packet);
	}

	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
				//process as lls
		lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data, udp_packet->data_length);

		return cleanup(&udp_packet);
	}

    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        return cleanup(&udp_packet);
    }

    lls_sls_mmt_session_t* matching_lls_slt_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if(matching_lls_slt_mmt_session) {
        mmtp_payload_fragments_union_t * mmtp_payload = mmtp_parse_from_udp_packet(udp_packet);
        if(mmtp_payload && mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x02 && mmtp_payload->mmtp_signalling_message_fragments.mmtp_packet_id == 0) {
            mmtp_process_sls_from_payload(udp_packet, &mmtp_payload, matching_lls_slt_mmt_session);
        }
        mmtp_payload_fragments_union_free(&mmtp_payload);
        mmtp_payload = NULL;

        return cleanup(&udp_packet);
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


/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

	_MMTP_DEBUG_ENABLED = 0;
	_MPU_DEBUG_ENABLED = 0;

	_LLS_DEBUG_ENABLED = 0;

	_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
	_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;

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

    mmtp_sub_flow_vector = (mmtp_sub_flow_vector_t*)calloc(1, sizeof(*mmtp_sub_flow_vector));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    lls_slt_monitor = lls_slt_monitor_create();


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




