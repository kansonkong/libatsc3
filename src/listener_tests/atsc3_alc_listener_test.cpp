/*
 * atsc3_alc_listener_test.c
 *
 *  Created on: Jan 29, 2019
 *      Author: jjustman
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
 *
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

 * to invoke test driver, run ala:

  ./atsc3_alc_listener_test vnic1

	ROUTE fragments (no re-assembly or fec support yet) will be available in route

*/

//#define _ENABLE_TRACE 1
#define _SHOW_PACKET_FLOW 1
int PACKET_COUNTER=0;

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

#include "../atsc3_listener_udp.h"
#include "../atsc3_utils.h"
#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"
#include "../atsc3_alc_rx.h"
#include "../alc_channel.h"
#include "../atsc3_alc_utils.h"
#include "../atsc3_listener_udp.h"
#include "../atsc3_logging_externs.h"

//dummy method for avoiding linking bento4 this unit listener
struct trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntries(block_t* moof_box) { return NULL; }

lls_slt_monitor_t* lls_slt_monitor;
uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;

alc_channel_t ch;
alc_arguments_t* alc_arguments;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {


	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}
	__TRACE("process_packet: source: %u.%u.%u.%u:%u", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));

    alc_packet_t* alc_packet = NULL;

	//dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
		return udp_packet_free(&udp_packet);
	}

	lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	if(matching_lls_slt_alc_session ||
			((dst_ip_addr_filter != NULL && dst_ip_port_filter != NULL) && (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter))) {
		//process ALC streams
		int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &ch, &alc_packet);
		if(!retval) {
			//dump out for fragment inspection
			//alc_packet_dump_to_object(&alc_packet, lls_slt_monitor->lls_sls_alc_monitor);
		} else {
			__ERROR("process_packet: Error in ALC decode: %d", retval);
		}
	} else {
		__ERROR("process_packet: Missing ALC session for: %u.%u.%u.%u:%u", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
	}

udp_packet_free:
	alc_packet_free(&alc_packet);
	alc_packet = NULL;

    return udp_packet_free(&udp_packet);
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


int main(int argc,char **argv) {

	_LLS_SLT_PARSER_INFO_ROUTE_ENABLED = 1;
    _ALC_UTILS_DEBUG_ENABLED = 1;
	_ALC_RX_DEBUG_ENABLED = 1;
	_ALC_RX_TRACE_TAB_ENABLED = 1;

#ifdef __LOTS_OF_DEBUGGING__

	_LLS_INFO_ENABLED = 1;
	_LLS_DEBUG_ENABLED = 1;

	_LLS_SLT_PARSER_INFO_ROUTE_ENABLED = 1;
    _ALC_UTILS_DEBUG_ENABLED = 1;
    _ALC_UTILS_TRACE_ENABLED = 1;

	_ALC_UTILS_IOTRACE_ENABLED=1;
	_ALC_RX_DEBUG_ENABLED = 1;
	_ALC_RX_TRACE_ENABLED = 1;
#endif

	char *dev;

    char *dst_ip = NULL;
    char *dst_port = NULL;
    int dst_port_filter_int;

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;


    //listen to all flows
    if(argc == 2) {
    	dev = argv[1];
	    __ALC_UTILS_DEBUG("listening on dev: %s", dev);
    } else if(argc==4) {
    	//listen to a selected flow
		dev = argv[1];
		dst_ip = argv[2];
		dst_port = argv[3];

		uint32_t dst_ip_int;
		dst_ip_int = parseIpAddressIntoIntval(dst_ip);
		dst_ip_addr_filter = &dst_ip_int;

		__INFO("listening on dev: %s, dst_ip: %s, dst_port: %s", dev, dst_ip, dst_port);
		uint16_t dst_port_int = parsePortIntoIntval(dst_port);
		dst_ip_port_filter = &dst_port_int;

    } else {
    	println("%s - a udp mulitcast listener test harness for atsc3 ALC ROUTE FLOW analysis (does not write out ROUTE ALC TOI, see tools/atsc3_alc_listener_mde_writer)", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");
    	println("");
    	exit(1);
    }

    lls_slt_monitor = lls_slt_monitor_create();
	alc_arguments = (alc_arguments_t*)calloc(1, sizeof(alc_arguments_t));

    ch.s = open_alc_session(alc_arguments);


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


