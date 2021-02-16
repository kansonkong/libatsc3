/*
 * atsc3_stltp_alp_listener_reflector_test.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 * stltp listener for atsc a/324
 *
 * jjustman: 2020-06-02 - renamed from atsc3_stltp_listener_alp_reflector.cpp to atsc3_stltp_alp_listener_reflector_test.cpp to match makefile build artifact
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

#include "../atsc3_listener_udp.h"
#include "../atsc3_stltp_parser.h"
#include "../atsc3_alp_parser.h"
#include "../atsc3_logging_externs.h"
#include "../atsc3_stltp_depacketizer.h"

#define PREAMBLE_PACKET_PARSE_AND_LOG true
#define TIMING_MANAGEMENT_PACKET_PARSE_AND_LOG true

int PACKET_COUNTER = 0;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context;

extern pcap_t* descrInject;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	atsc3_stltp_depacketizer_from_pcap_frame(user, pkthdr, packet, atsc3_stltp_depacketizer_context);
}

int main(int argc,char **argv) {
    _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 0;
    _IP_UDP_RTP_PARSER_TRACE_ENABLED = 0;

    _ATSC3_UTILS_TRACE_ENABLED = 0;
	
	//jjustman-2020-12-31 - extra debugging
	_STLTP_PARSER_INFO_ENABLED  = 1;
	_STLTP_PARSER_DUMP_ENABLED  = 1;
	_STLTP_PARSER_DEBUG_ENABLED = 1;
	_STLTP_PARSER_TRACE_ENABLED = 1;

	_STLTP_TYPES_DEBUG_ENABLED = 1;
	_STLTP_TYPES_TRACE_ENABLED = 1;
    
    char *dev;
    char *devInject;
    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_stltp_plp_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
	
    char errbuf[PCAP_ERRBUF_SIZE];
    char errbufInject[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;
    
    struct bpf_program fpInject;
    bpf_u_int32 maskpInject;
    bpf_u_int32 netpInject;
    

    uint32_t* dst_ip_addr_filter = NULL;
    uint16_t* dst_ip_port_filter = NULL;


    atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();


    if(argc < 5) {
        println("%s - an atsc3 stltp udp mulitcast reflector ", argv[0]);
        println("---");
        println("args: devListen ip port devInject");
        println(" devListen : device to listen for stltp udp multicast");
        println(" ip        : ip address for stltp");
        println(" port      : port for single stltp");
        println(" devInject : device to inject for ALP IP reflection");
		println(" 	(optional) PLP_num: PLP number to extract (e.g. 0 -> STLTP inner port: 30000, 63 -> STLTP inner port 300063");

        exit(1);
    } else {
        dev = argv[1];
        filter_dst_ip = argv[2];
        filter_dst_port = argv[3];
        devInject = argv[4];
        
        //parse ip
        dst_ip_addr_filter = (uint32_t*)calloc(1, sizeof(uint32_t));
        char* pch = strtok (filter_dst_ip,".");
        int offset = 24;
        while (pch != NULL && offset>=0) {
            uint8_t octet = atoi(pch);
            *dst_ip_addr_filter |= octet << offset;
            offset-=8;
            pch = strtok (NULL, ".");
        }
        
        //parse port
        
        dst_port_filter_int = atoi(filter_dst_port);
        dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
        *dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;

        atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr = *dst_ip_addr_filter;
        atsc3_stltp_depacketizer_context->destination_flow_filter.dst_port = *dst_ip_port_filter;

		if(argc == 6) {
			//parse out custom PLP_num
			filter_stltp_plp_id = argv[5];

			uint8_t stltp_plp_id = atoi(filter_stltp_plp_id);
			if(stltp_plp_id >=0 && stltp_plp_id <= 63) {
				atsc3_stltp_depacketizer_context->inner_rtp_port_filter = stltp_plp_id + 30000;;
				println("using PLP: %d (stltp inner port: %d)", stltp_plp_id, atsc3_stltp_depacketizer_context->inner_rtp_port_filter);
			} else {
				atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
				println("ignoring PLP: %d, defaulting to PLP 0 (stltp inner port: %d)", stltp_plp_id, atsc3_stltp_depacketizer_context->inner_rtp_port_filter);
			}
		}
    }
    
    if(!atsc3_stltp_depacketizer_context->inner_rtp_port_filter) {
    	atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
    }

    
    println("%s -an atsc3 stltp udp mulitcast reflector , listening on dev: %s, reflecting: %s, plp: %d", argv[0], dev, devInject, atsc3_stltp_depacketizer_context->inner_rtp_port_filter - 30000);
    
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
   
    //inject
    pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 1, errbufInject);
    

    atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference = atsc3_reflect_alp_packet_collection;
    atsc3_stltp_depacketizer_context->atsc3_baseband_alp_output_pcap_device_reference = descrInject;


    pcap_loop(descr, -1, process_packet, NULL);
    
    return 0;
}

