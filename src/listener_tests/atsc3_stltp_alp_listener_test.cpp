/*
 * atsc3_stltp_alp_listener_test.c
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 * stltp listener for atsc a/324
 *
 * */
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

int PACKET_COUNTER = 0;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_processed = NULL;
atsc3_alp_packet_collection_t* atsc3_alp_packet_collection = NULL;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	atsc3_stltp_depacketizer_from_pcap_frame(user, pkthdr, packet, atsc3_stltp_depacketizer_context);

}

int main(int argc,char **argv) {
    _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 1;
	
	_ALP_PARSER_INFO_ENABLED = 1;
	_ALP_PARSER_DEBUG_ENABLED = 1;
	_ALP_PARSER_TRACE_ENABLED = 1;

    char *dev;
    char *filter_dst_ip = NULL;

    char *filter_dst_port = NULL;
    char *filter_stltp_plp_id = NULL;

    uint32_t* dst_ip_addr_filter = NULL;
    uint16_t* dst_ip_port_filter = NULL;
    uint16_t stltp_ip_port_filter = 30000;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;

    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;
    
    atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();
    atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();

    if(argc < 4) {
    	println("%s - a udp mulitcast listener test harness for atsc3 stltp payloads", argv[0]);
    	println("---");
    	println("args: dev ip port");
    	println(" dev : device to listen for udp multicast");
    	println(" ip  : ip address for stltp");
    	println(" port: port for single stltp");
		println(" 	(optional) PLP_num: PLP number to extract (e.g. 0 -> STLTP inner port: 30000, 63 -> STLTP inner port 300063");

		exit(1);
    } else {
    	dev = argv[1];
		filter_dst_ip = argv[2];
		filter_dst_port = argv[3];


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

		atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr =

		//parse port
		dst_port_filter_int = atoi(filter_dst_port);
		dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
		*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;

        atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr = *dst_ip_addr_filter;
        atsc3_stltp_depacketizer_context->destination_flow_filter.dst_port = *dst_ip_port_filter;

		if(argc == 5) {
			//parse out custom PLP_num
			filter_stltp_plp_id = argv[4];

			uint8_t stltp_plp_id = atoi(filter_stltp_plp_id);
			if(stltp_plp_id >=0 && stltp_plp_id <= 63) {
				atsc3_stltp_depacketizer_context->inner_rtp_port_filter = stltp_plp_id + 30000;;
				println("using PLP: %d (stltp inner port: %d)", stltp_plp_id, stltp_ip_port_filter);
			} else {
				atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
				println("ignoring PLP: %d, defaulting to PLP 0 (stltp inner port: %d)", stltp_plp_id, stltp_ip_port_filter);
			}
		}
    }

    if(!atsc3_stltp_depacketizer_context->inner_rtp_port_filter) {
      	atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
    }

	println("%s - a udp mulitcast listener test harness for atsc3 stltp payloads, listening on dev: %s, plp: %d", argv[0], argv[1], atsc3_stltp_depacketizer_context->inner_rtp_port_filter - 30000);


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

