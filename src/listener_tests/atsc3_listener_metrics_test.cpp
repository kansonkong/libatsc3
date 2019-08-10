/*
 * atsc3_listener_metrics_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
*/

//#define _ENABLE_TRACE 1
//#define _SHOW_PACKET_FLOW 1
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
#include <time.h>

#include "../atsc3_listener_udp.h"
#include "../atsc3_utils.h"

#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmt_mpu_utils.h"

#include "../atsc3_mmtp_ntp32_to_pts.h"

#include "../alc_channel.h"
#include "../alc_rx.h"
#include "../atsc3_alc_utils.h"

#include "../atsc3_bandwidth_statistics.h"
#include "../atsc3_packet_statistics.h"
#include "../atsc3_logging_externs.h"

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _LLS_DEBUG_ENABLED;

//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;


// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session

lls_sls_alc_session_t* lls_session;

int lls_slt_table_process_update(lls_table_t* lls) {

	if(lls_session->lls_table_slt) {
		lls_table_free(lls_session->lls_table_slt);
		lls_session->lls_table_slt = NULL;
	}
	lls_session->lls_table_slt = lls;


	for(int i=0; i < lls->slt_table.service_entry_n; i++) {
		lls_service_t* service = lls->slt_table.service_entry[i];

		if(service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_ROUTE) {
			//TODO - we probably need to clear out the ALC session?
			if(!lls_session->lls_slt_alc_session->alc_session) {
				lls_dump_instance_table(lls_session->lls_table_slt);

				lls_session->lls_slt_alc_session->lls_slt_service_id_alc = service->service_id;
				lls_session->lls_slt_alc_session->alc_arguments = calloc(1, sizeof(alc_arguments_t));

				lls_session->lls_slt_alc_session->sls_source_ip_address = parseIpAddressIntoIntval(service->broadcast_svc_signaling.sls_source_ip_address);

				lls_session->lls_slt_alc_session->sls_destination_ip_address = parseIpAddressIntoIntval(service->broadcast_svc_signaling.sls_destination_ip_address);
				lls_session->lls_slt_alc_session->sls_destination_udp_port = parsePortIntoIntval(service->broadcast_svc_signaling.sls_destination_udp_port);

				__INFO("adding sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
						service->broadcast_svc_signaling.sls_source_ip_address,
						__toipnonstruct(lls_session->lls_slt_alc_session->sls_source_ip_address),
						service->broadcast_svc_signaling.sls_destination_ip_address,
						service->broadcast_svc_signaling.sls_destination_udp_port,
						__toipandportnonstruct(lls_session->lls_slt_alc_session->sls_destination_ip_address, lls_session->lls_slt_alc_session->sls_destination_udp_port),
						lls_session->lls_slt_alc_session->sls_destination_ip_address, lls_session->lls_slt_alc_session->sls_destination_udp_port);

				lls_session->lls_slt_alc_session->alc_session = open_alc_session(lls_session->lls_slt_alc_session->alc_arguments);

				if(!lls_session->lls_slt_alc_session->alc_session) {
				  __ERROR("Unable to instantiate alc session for service_id: %d via SLS_PROTOCOL_ROUTE", service->service_id);
					goto cleanup;
				}

		  	}
		}
	}
	global_stats->packet_counter_lls_slt_update_processed++;
	return 0;

cleanup:
	if(lls_session->lls_slt_alc_session->alc_arguments) {
		free(lls_session->lls_slt_alc_session->alc_arguments);
		lls_session->lls_slt_alc_session->alc_arguments = NULL;
	}

	if(lls_session->lls_slt_alc_session->alc_session) {
		free(lls_session->lls_slt_alc_session->alc_session);
		lls_session->lls_slt_alc_session->alc_session = NULL;
	}
	return -1;
}



//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector;


void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}


	//compute total_rx on all packets
	__TRACE("updating interval_total_current_rx: %d", udp_packet->data_length)

	global_bandwidth_statistics->interval_total_current_bytes_rx += udp_packet->data_length;
	global_stats->packet_counter_total_received++;

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		global_stats->packet_counter_filtered_ipv4++;
		__TRACE("setting %s,  %d+=%d,", "interval_filtered_current_rx", global_bandwidth_statistics->interval_filtered_current_rx, udp_packet->data_length);
		global_bandwidth_statistics->interval_filtered_current_rx += udp_packet->data_length;

		goto cleanup;
	}


	//dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		global_stats->packet_counter_lls_packets_received++;
		global_bandwidth_statistics->interval_lls_current_bytes_rx += udp_packet->data_length;

		lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data, udp_packet->data_length);
		if(!lls_table) {
			global_stats->packet_counter_lls_packets_parsed_error++;

		}
	}



	//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255

	if(udp_packet->udp_flow.dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->udp_flow.dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
		//out of range, so drop
		global_stats->packet_counter_filtered_ipv4++;
		global_bandwidth_statistics->interval_filtered_current_rx += udp_packet->data_length;

		goto cleanup;
	}

	//ALC (ROUTE) - If this flow is registered from the SLT, process it as ALC, otherwise run the flow thru MMT
	if(lls_session->lls_slt_alc_session->alc_session &&	(lls_session->lls_slt_alc_session->sls_relax_source_ip_check || lls_session->lls_slt_alc_session->sls_source_ip_address == udp_packet->udp_flow.src_ip_addr) &&
			lls_session->lls_slt_alc_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr && lls_session->lls_slt_alc_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port) {
		global_stats->packet_counter_alc_recv++;

		global_bandwidth_statistics->interval_alc_current_rx += udp_packet->data_length;

		if(lls_session->lls_slt_alc_session->alc_session) {
			//re-inject our alc session
			alc_packet_t* alc_packet = NULL;
			alc_channel_t ch;
			ch.s = lls_session->lls_slt_alc_session->alc_session;

			//process ALC streams
			int retval = alc_rx_analyze_packet((char*)udp_packet->data, udp_packet->data_length, &ch, &alc_packet);
			if(!retval) {
				global_stats->packet_counter_alc_packets_parsed++;
				alc_packet_dump_to_object(alc_packet);
				goto cleanup;
			} else {
				__ERROR("Error in ALC decode: %d", retval);
				global_stats->packet_counter_alc_packets_parsed_error++;
				goto cleanup;
			}
		} else {
			__WARN("Have matching ALC session information but ALC client is not active!");
			goto cleanup;
		}
	}

	//Process flow as MMT, we should only have MMT packets left at this point..
	if((dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) || (udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter)) {

		global_bandwidth_statistics->interval_mmt_current_rx += udp_packet->data_length;

		global_stats->packet_counter_mmtp_packets_received++;

		__ALC_UTILS_DEBUG("data len: %d", udp_packet->data_length)
		mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);

		if(!mmtp_payload) {
			global_stats->packet_counter_mmtp_packets_parsed_error++;
			__ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5hu \t ->  %d.%d.%d.%d\t(%-10u)\t:%-5hu",
					ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->udp_flow.src_ip_addr,
					(uint16_t)((udp_header[0] << 8) + udp_header[1]),
					ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->udp_flow.dst_ip_addr,
					(uint16_t)((udp_header[2] << 8) + udp_header[3])
					);
			goto cleanup;
		}
		atsc3_packet_statistics_mmt_stats_populate(udp_packet, mmtp_payload);

		//dump header, then dump applicable packet type
		//mmtp_packet_header_dump(mmtp_payload);

		if(mmtp_payload->mmtp_packet_header->mmtp_payload_type == 0x0) {
			global_stats->packet_counter_mmt_mpu++;

			if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {
				global_stats->packet_counter_mmt_timed_mpu++;

				//timed
				//mpu_dump_flow(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_payload);
				//mpu_dump_reconstitued(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_payload);

			} else {
				//non-timed
				global_stats->packet_counter_mmt_nontimed_mpu++;

			}

		} else if(mmtp_payload->mmtp_packet_header->mmtp_payload_type == 0x2) {

			signaling_message_dump(mmtp_payload);
			global_stats->packet_counter_mmt_signaling++;

		} else {
			_MMTP_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header->mmtp_payload_type);
			global_stats->packet_counter_mmt_unknown++;
			goto cleanup;
		}

		atsc3_packet_statistics_dump_global_stats();
		mmtp_payload_fragments_union_free(&mmtp_payload);

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


#define MAX_PCAP_LEN 1514
/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

	_MPU_DEBUG_ENABLED = 0;
	_MMTP_DEBUG_ENABLED = 0;
	_LLS_DEBUG_ENABLED = 0;

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
    	__INFO("listening on dev: %s", dev);
    } else if(argc==4) {
    	//listen to a selected flow
    	dev = argv[1];
    	dst_ip = argv[2];
    	dst_port = argv[3];

    	dst_ip_addr_filter = calloc(1, sizeof(uint32_t));
    	char* pch = strtok (dst_ip,".");
    	int offset = 24;
    	while (pch != NULL && offset>=0) {
    		uint8_t octet = atoi(pch);
    		*dst_ip_addr_filter |= octet << offset;
    		offset-=8;
    	    pch = strtok (NULL, " ,.-");
    	  }

    	dst_port_filter_int = atoi(dst_port);
    	dst_ip_port_filter = calloc(1, sizeof(uint16_t));
    	*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;

    	__INFO("listening on dev: %s, dst_ip: %s, dst_port: %s", dev, dst_ip, dst_port);

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

    /** setup global structs **/

    mmtp_sub_flow_vector = calloc(1, sizeof(*mmtp_sub_flow_vector));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);
    lls_session = lls_sls_alc_session_create();

    global_stats = calloc(1, sizeof(*global_stats));
    gettimeofday(&global_stats->program_timeval_start, 0);

    global_bandwidth_statistics = calloc(1, sizeof(*global_bandwidth_statistics));
	gettimeofday(&global_bandwidth_statistics->program_timeval_start, NULL);


    //create our background thread for bandwidth calculation

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, print_bandwidth_statistics_thread, NULL);



    mkdir("mpu", 0777);

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
    pthread_join(thread_id, NULL);

    return 0;
}

