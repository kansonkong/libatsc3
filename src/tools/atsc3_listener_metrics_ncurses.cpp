/*
 * atsc3_listener_metrics_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 * global listener driver for LLS, MMT and ROUTE / DASH
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

  to invoke test driver, run ala:

  ./atsc3_listener_metrics_test vnic1


  	  6.1 IP Address Assignment


LLS shall be transported in IP packets with address 224.0.23.60 and
destination port 4937/udp.1 All IP packets other than LLS IP packets
shall carry a Destination IP address either

	(a) allocated and reserved by a mechanism guaranteeing that the
	 destination addresses in use are unique in a geographic region2,or

	(b) in the range of 239.255.0.0 to 239.255.255.2553, where the
	bits in the third octet shall correspond to a value of
	SLT.Service@majorChannelNo registered to the broadcaster for use
	in the Service Area4 of the broadcast transmission, with the
	following caveats:

	• If a broadcast entity operates transmissions carrying different Services
	on multiple RF frequencies with all or a part of their service area in common,
	each IP address/port combination shall be unique across all such broadcast emissions;

	•In the case that multiple LLS streams (hence, multiple SLTs) are present in a
	given broadcast emission, each IP address/port combination in use for non-LLS streams
	shall be unique across all Services in the aggregate broadcast emission;




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
#include <signal.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <limits.h>
#include <strings.h>

#include "../bento4/ISOBMFFTrackJoiner.h"
#include "../atsc3_isobmff_tools.h"

#include "../atsc3_listener_udp.h"
#include "../atsc3_utils.h"

#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"
#include "../atsc3_mmt_reconstitution_from_media_sample.h"

#include "../atsc3_alc_channel.h"
#include "../atsc3_alc_rx.h"
#include "../atsc3_alc_utils.h"

#include "../atsc3_bandwidth_statistics.h"
#include "../atsc3_packet_statistics.h"

#include "../atsc3_output_statistics_ncurses.h"

#include "../atsc3_logging_externs.h"


#define _ENABLE_DEBUG true


//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session

lls_slt_monitor_t* lls_slt_monitor;

//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
//mmtp_sub_flow_vector_t*                          mmtp_sub_flow_vector;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
extern atsc3_global_statistics_t* atsc3_global_statistics;

void count_packet_as_filtered(udp_packet_t* udp_packet) {
	atsc3_global_statistics->packet_counter_filtered_ipv4++;
	global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data->p_size;
	global_bandwidth_statistics->interval_filtered_current_packets_rx++;
}

void update_global_mmtp_statistics_from_udp_packet_t(udp_packet_t *udp_packet) {
	global_bandwidth_statistics->interval_mmt_current_bytes_rx += udp_packet->data->p_size;

	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

	if(!mmtp_packet_header) {
		goto error;
	}

	//for filtering MMT flows by a specific packet_id
	if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
		count_packet_as_filtered(udp_packet);
		goto cleanup;
	}

	if(mmtp_packet_header->mmtp_payload_type == 0x0) {
		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, udp_packet->data);
		if(!mmtp_mpu_packet) {
			goto error;
		}

		if(mmtp_mpu_packet->mpu_timed_flag == 1) {
		    atsc3_packet_statistics_mmt_stats_populate(udp_packet, mmtp_mpu_packet);

			//mmtp_mpu_dump_header(mmtp_mpu_packet);
		} else {
			//non-timed
			__ATSC3_WARN("update_global_mmtp_statistics_from_udp_packet_t: non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
		}
	} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

		mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
		uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
		if(parsed_count) {
			mmt_signalling_message_dump(mmtp_signalling_packet);
		} else {
			goto error;
		}

	} else {
		__ATSC3_WARN("update_global_mmtp_statistics_from_udp_packet_t: unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
		goto error;
	}
    
    atsc3_global_statistics->packet_counter_mmtp_packets_received++;
    global_bandwidth_statistics->interval_mmt_current_packets_rx++;

    goto cleanup;

 error:
	atsc3_global_statistics->packet_counter_mmtp_packets_parsed_error++;
		__ERROR("update_global_mmtp_statistics_from_udp_packet_t: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
				__toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
				udp_packet->udp_flow.src_ip_addr,
				__toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
				udp_packet->udp_flow.dst_ip_addr);
    
 cleanup:

 	 ;

}

static void route_process_from_alc_packet(udp_flow_t* udp_flow, alc_packet_t **alc_packet) {
    alc_packet_dump_to_object(udp_flow, alc_packet, lls_slt_monitor->lls_sls_alc_monitor);
    
    if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer) {
     
        lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_alc_isobmff_fragment(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);

        if(!lls_sls_monitor_output_buffer_final_muxed_payload) {
        	lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = false;
			__ERROR("lls_sls_monitor_output_buffer_final_muxed_payload was NULL!");
			return;
        }
        
        if(true || lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
        	lls_sls_monitor_output_buffer_alc_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "route/",
        			lls_slt_monitor->lls_sls_alc_monitor->last_completed_flushed_audio_toi, lls_slt_monitor->lls_sls_alc_monitor->last_completed_flushed_video_toi);
        }

        if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

        	pipe_ffplay_buffer_t* pipe_ffplay_buffer = lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer;

        	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
        
        	pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);
        
        	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);
        
			//check to see if we have shutdown
			lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);

			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
        }

		lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);

    }
}

alc_packet_t* route_parse_from_udp_packet(lls_sls_alc_session_t *matching_lls_slt_alc_session, udp_packet_t *udp_packet) {
    alc_packet_t* alc_packet = NULL;

    //sanity check
    if(matching_lls_slt_alc_session->alc_session) {
        //re-inject our alc session

        atsc3_alc_channel_t ch;
        ch.s = matching_lls_slt_alc_session->alc_session;
        
        //process ALC streams
        int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &ch, &alc_packet);
        if(!retval) {
            atsc3_global_statistics->packet_counter_alc_packets_parsed++;
            
            //don't dump unless this is pointing to our monitor session
            if(lls_slt_monitor->lls_sls_alc_monitor &&  lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session && lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session->service_id == matching_lls_slt_alc_session->service_id) {
                goto ret;
            } else {
                __ATSC3_TRACE("ignoring service_id: %u", matching_lls_slt_alc_session->service_id);
            }
            goto cleanup;
        } else {
            __ERROR("Error in ALC decode: %d", retval);
            atsc3_global_statistics->packet_counter_alc_packets_parsed_error++;
            goto cleanup;
        }
    } else {
        __WARN("Have matching ALC session information but ALC client is not active!");
        goto cleanup;
    }
cleanup:
    alc_packet_free(&alc_packet);
    alc_packet = NULL;

ret:
    return alc_packet;
    
}

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);

	if(!udp_packet) {
		return;
	}

	//collect global
	global_bandwidth_statistics->interval_total_current_bytes_rx += udp_packet->raw_packet_length;
	global_bandwidth_statistics->interval_total_current_packets_rx++;
	global_bandwidth_statistics->grand_total_bytes_rx += udp_packet->raw_packet_length;
	global_bandwidth_statistics->grand_total_packets_rx++;
	atsc3_global_statistics->packets_total_received++;

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		atsc3_global_statistics->packet_counter_filtered_ipv4++;
		//printf("setting dns current_bytes_rx: %d, packets_rx: %d", global_bandwidth_statistics->interval_filtered_current_bytes_rx, global_bandwidth_statistics->interval_filtered_current_packets_rx);
		global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data->p_size;
		global_bandwidth_statistics->interval_filtered_current_packets_rx++;

		return udp_packet_free(&udp_packet);
	}

	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		global_bandwidth_statistics->interval_lls_current_bytes_rx += udp_packet->data->p_size;
		global_bandwidth_statistics->interval_lls_current_packets_rx++;

		atsc3_global_statistics->packet_counter_lls_packets_received++;

		//process as lls.sst, dont free as we keep track of our object in the lls_slt_monitor

		lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor_with_metrics(lls_slt_monitor, udp_packet->data, &atsc3_global_statistics->packet_counter_lls_packets_parsed, &atsc3_global_statistics->packet_counter_lls_packets_parsed_update, &atsc3_global_statistics->packet_counter_lls_packets_parsed_error);
		if(lls_table) {

			if(lls_table->lls_table_id == SLT) {

				atsc3_global_statistics->packet_counter_lls_slt_packets_parsed++;
				int retval = lls_slt_table_perform_update(lls_table, lls_slt_monitor);

				if(!retval) {
					atsc3_global_statistics->packet_counter_lls_slt_update_processed++;
				} else {
					atsc3_global_statistics->packet_counter_lls_slt_packets_parsed_error++;
				}
			}
		}

		return udp_packet_free(&udp_packet);
	}


	//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255
//    if(udp_packet->udp_flow.dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->udp_flow.dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
//        //out of range, so drop
//        count_packet_as_filtered(udp_packet);
//
//        //goto cleanup;
//        return udp_packet_free(udp_packet);
//    }
//    
    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        count_packet_as_filtered(udp_packet);
        return udp_packet_free(&udp_packet);
    }

	//ALC (ROUTE) - If this flow is registered from the SLT, process it as ALC, otherwise run the flow thru MMT
	lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	if(matching_lls_slt_alc_session) {
		global_bandwidth_statistics->interval_alc_current_bytes_rx += udp_packet->data->p_size;
		global_bandwidth_statistics->interval_alc_current_packets_rx++;
		atsc3_global_statistics->packet_counter_alc_recv++;

        alc_packet_t* alc_packet = route_parse_from_udp_packet(matching_lls_slt_alc_session, udp_packet);
        if(alc_packet) {
            route_process_from_alc_packet(&udp_packet->udp_flow, &alc_packet);
            alc_packet_free(&alc_packet);
        }
        
        return udp_packet_free(&udp_packet);
	}

	//find our matching MMT flow and push it to reconsitution
    lls_sls_mmt_session_t* matching_lls_slt_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if(matching_lls_slt_mmt_session) {
        __TRACE("data len: %d", udp_packet->data_length);

        update_global_mmtp_statistics_from_udp_packet_t(udp_packet);

	}

    //if we get here, we don't know what type of packet it is..
    atsc3_global_statistics->packet_counter_udp_unknown++;
    
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

	_MMT_MPU_PARSER_DEBUG_ENABLED = 1;
	_MMTP_DEBUG_ENABLED = 1;
	_LLS_DEBUG_ENABLED = 0;
    _ISOBMFF_TOOLS_DEBUG_ENABLED = 1;
    _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
    _PLAYER_FFPLAY_TRACE_ENABLED = 0;
    _ALC_UTILS_DEBUG_ENABLED = 1;
    _ALC_UTILS_TRACE_ENABLED = 1;
    _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 0;

    char *dev;

    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    sigset_t player_signal_mask;


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
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt messages", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port) (packet_id)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");
    	println(" (packet_id): optional, filter to specific packet_id across all streams");

    	println("");
    	exit(1);
    }
    // mkdir("mpu", 0777);

    /** setup global structs **/

    lls_slt_monitor = lls_slt_monitor_create();

    atsc3_global_statistics = (global_atsc3_stats*)calloc(1, sizeof(*atsc3_global_statistics));
    gettimeofday(&atsc3_global_statistics->program_timeval_start, 0);

    global_bandwidth_statistics = (bandwidth_statistics_t*)calloc(1, sizeof(*global_bandwidth_statistics));
	gettimeofday(&global_bandwidth_statistics->program_timeval_start, NULL);


    //create our background thread for bandwidth calculation
    /** ncurses support - valgrind on osx will fail in pthread_create...**/



#ifndef _TEST_RUN_VALGRIND_OSX_

	//block sigpipe before creating our threads
	sigemptyset (&player_signal_mask);
	sigaddset (&player_signal_mask, SIGPIPE);
	int rc = pthread_sigmask (SIG_BLOCK, &player_signal_mask, NULL);
	if(!rc) {
		  __WARN("Unable to block SIGPIPE, this may result in a runtime crash when closing ffplay!");
	}


	pthread_t global_ncurses_input_thread_id;
	int ncurses_input_ret = pthread_create(&global_ncurses_input_thread_id, NULL, ncurses_input_run_thread, (void*)lls_slt_monitor);
	assert(!ncurses_input_ret);

	pthread_t global_bandwidth_thread_id;
	pthread_create(&global_bandwidth_thread_id, NULL, print_bandwidth_statistics_thread, NULL);

	pthread_t global_stats_thread_id;
	pthread_create(&global_stats_thread_id, NULL, print_global_statistics_thread, NULL);

	pthread_t global_slt_thread_id;
	pthread_create(&global_slt_thread_id, NULL, print_lls_instance_table_thread, (void*)lls_slt_monitor);

	
	pthread_t global_pcap_thread_id;
		int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);

    
	pthread_join(global_pcap_thread_id, NULL);
	pthread_join(global_ncurses_input_thread_id, NULL);

#else
	pcap_loop_run_thread(dev);
#endif


    return 0;
}


/***
 *
 *
 *
	//dispatch for LLS extraction and dump
	#ifdef _SHOW_PACKET_FLOW
		__INFO("--- Packet size : %-10d | Counter: %-8d", udp_packet->data_length, PACKET_COUNTER++);
		__INFO("    Src. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->udp_flow.src_ip_addr);
		__INFO("    Src. Port   : %-5hu ", (uint16_t)((udp_header[0] << 8) + udp_header[1]));
		__INFO("    Dst. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->udp_flow.dst_ip_addr);
		__INFO("    Dst. Port   : %-5hu \t", (uint16_t)((udp_header[2] << 8) + udp_header[3]));
	#endif
 *
//dump full packet if needed
#ifdef _ENABLE_TRACE
    for (i = 0; i < pkthdr->len; i++) {
        if ((i % 16) == 0) {
            __TRACE("%03x0\t", k);
            k++;
        }
        __TRACE("%02x ", packet[i]);
    }
    __TRACE("*******************************************************");
#endif
 *
	//4294967295
	//1234567890
	__TRACE("Src. Addr  : %d.%d.%d.%d\t(%-10u)\t", ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->udp_flow.src_ip_addr);
	__TRACE("Src. Port  : %-5hu ", (udp_header[0] << 8) + udp_header[1]);
	__TRACE("Dst. Addr  : %d.%d.%d.%d\t(%-10u)\t", ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->udp_flow.dst_ip_addr);
	__TRACE("Dst. Port  : %-5hu \t", (udp_header[2] << 8) + udp_header[3]);

	__TRACE("Length\t\t\t\t\t%d", (udp_header[4] << 8) + udp_header[5]);
	__TRACE("Checksum\t\t\t\t0x%02x 0x%02x", udp_header[6], udp_header[7]);
 *
 *
 *
 */
