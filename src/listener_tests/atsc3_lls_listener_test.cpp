/*
 * atsc3_lls_listener_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

  to invoke test driver, run ala:

  ./atsc3_lls_listener_test vnic1 | grep 224.0.23.60

  output should look like


atsc3_lls_listener_test.c:100:DEBUG:Destination Address		239.255.10.4

atsc3_lls_listener_test.c:99:DEBUG:Source Address			192.168.0.4

atsc3_lls_listener_test.c:153:DEBUG:Dst. Address : 224.0.23.60 (3758102332)	Dst. Port    : 4937  	Data length: 193
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

#include "../atsc3_lls.h"
#include "../atsc3_lls_types.h"
#include "../atsc3_lls_slt_parser.h"

#include "../atsc3_logging_externs.h"

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);

	if(!udp_packet) {
		return;
	}

	//dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {

		lls_table_t* lls_table = __lls_table_create(udp_packet->data);

		if(lls_table) {
			printf("---\n");
			lls_dump_instance_table(lls_table);
			printf("---\n\n");
			lls_table_free(&lls_table);
		} else {
			__WARN("Error parsing LLS payload, data len: %u", block_Remaining_size(udp_packet->data));
		}
	}

	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
}

int main(int argc,char **argv) {

	_LLS_INFO_ENABLED = 1;

#ifdef __LOTS_OF_DEBUGGING__

	_LLS_DEBUG_ENABLED = 1;
	_LLS_TRACE_ENABLED = 1;

#endif

    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;

    if(argc == 2) {
    	dev = argv[1];

    } else {
    	println("%s - a udp mulitcast listener test harness for atsc3 LLS messages, defaults to: 224.0.23.60:4937:", argv[0]);
		println("---");
    	println("args: dev");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");

		exit(1);
    }

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

	println("Starting with dev: %s", dev);

    pcap_loop(descr,-1,process_packet,NULL);

    return 0;
}

