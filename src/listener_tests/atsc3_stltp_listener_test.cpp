/*
 * atsc3_stltp_listener_test.c
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
#include "../atsc3_logging_externs.h"

int PACKET_COUNTER = 0;

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_processed = NULL;
atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = atsc3_ip_udp_rtp_process_packet_from_pcap(user, pkthdr, packet);
	if(!ip_udp_rtp_packet) {
		return;
	}

	//dispatch for LLS extraction and dump
	if(ip_udp_rtp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && ip_udp_rtp_packet->udp_flow.dst_port == *dst_ip_port_filter) {
		atsc3_stltp_tunnel_packet_processed = atsc3_stltp_raw_packet_extract_inner_from_outer_packet(atsc3_stltp_depacketizer_context, ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_processed);

		if(atsc3_stltp_tunnel_packet_processed) {
			if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count) {
				__INFO("stltp atsc3_stltp_baseband_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count);

				//todo - free
			}
			if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count) {
				__INFO("stltp atsc3_stltp_preamble_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count);
				//todo - free
			}
            if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count) {
              __INFO(">>>stltp atsc3_stltp_timing_management_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count);
              for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count; i++) {
                  atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.data[i];
                  atsc3_timing_management_packet_t* atsc3_timing_management_packet = atsc3_stltp_parse_timing_management_packet(atsc3_stltp_timing_management_packet);
                  if(!atsc3_timing_management_packet) {
                      __WARN("atsc3_timing_management_packet is NULL for i: %d", i);
                  }
              }
            }
            
            atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet_processed);

		} else {
            __ERROR("error processing packet: %p, size: %u",  ip_udp_rtp_packet, ip_udp_rtp_packet->data->p_size);
            
		}
	}
    atsc3_ip_udp_rtp_packet_destroy(&ip_udp_rtp_packet);
}

int main(int argc,char **argv) {

    char *dev;
    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;

    if(argc != 4) {
    	println("%s - a udp mulitcast listener test harness for atsc3 stltp payloads", argv[0]);
    	println("---");
    	println("args: dev ip port");
    	println(" dev : device to listen for udp multicast");
    	println(" ip  : ip address for stltp");
    	println(" port: port for single stltp");

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

		//parse port

		dst_port_filter_int = atoi(filter_dst_port);
		dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
		*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;

    }

    atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();

	println("%s - a udp mulitcast listener test harness for atsc3 stltp payloads, listening on dev: %s", argv[0], argv[1]);

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

