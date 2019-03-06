/*
 * atsc3_listener_metrics_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 * global listener driver for LLS, MMT and ROUTE / DASH (coming soon)
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


#include "atsc3_isobmff_tools.h"
#include "bento4/ISOBMFFTrackJoiner.h"


extern "C" {


#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"
#include "atsc3_lls_alc_utils.h"

#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_mpu_utils.h"

#include "alc_channel.h"
#include "atsc3_alc_rx.h"
#include "atsc3_alc_utils.h"

#include "atsc3_bandwidth_statistics.h"
#include "atsc3_packet_statistics.h"

#include "atsc3_output_statistics_ncurses.h"
}

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;
extern int _LLS_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;
extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;

#define MAX_PCAP_LEN 1514

#define _ENABLE_DEBUG true

#define __ERROR(...)   printf("%s:%d:ERROR :","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __WARN(...)    printf("%s:%d:WARN: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __INFO(...)    printf("%s:%d: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __DEBUGF(...)  printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __DEBUGA(...) 	__PRINTF(__VA_ARGS__);
#define __DEBUGN(...)  __PRINTLN(__VA_ARGS__);
#else
#define __DEBUGF(...)
#define __DEBUGA(...)
#define __DEBUGN(...)
#endif


#ifndef _TEST_RUN_VALGRIND_OSX_
//overload printf to write to stderr
int printf(const char *format, ...)  {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
    va_end(argptr);

	return 0;
}
#endif





#ifdef _ENABLE_TRACE
#define __TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

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

#else
#define __TRACE(...)
#endif

//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session

lls_slt_monitor_t* lls_slt_monitor;

//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t*                          mmtp_sub_flow_vector;
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;




global_atsc3_stats_t* global_stats;
void count_packet_as_filtered(udp_packet_t* udp_packet) {
	global_stats->packet_counter_filtered_ipv4++;
	global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data_length;
	global_bandwidth_statistics->interval_filtered_current_packets_rx++;
}


void cleanup(udp_packet_t* udp_packet) {

	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
}

mmtp_payload_fragments_union_t* mmtp_parse_from_udp_packet(udp_packet_t *udp_packet) {
    
    mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);
    
    if(!mmtp_payload) {
        global_stats->packet_counter_mmtp_packets_parsed_error++;
        __ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
                __toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
                udp_packet->udp_flow.src_ip_addr,
                __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
                udp_packet->udp_flow.dst_ip_addr);
        return NULL;
    }
    
    global_bandwidth_statistics->interval_mmt_current_bytes_rx += udp_packet->data_length;
    global_bandwidth_statistics->interval_mmt_current_packets_rx++;
    global_stats->packet_counter_mmtp_packets_received++;
    
    atsc3_packet_statistics_mmt_stats_populate(udp_packet, mmtp_payload);
    
    return mmtp_payload;
}

//todo - use local lls_slt_monitor
mmtp_payload_fragments_union_t* mmtp_process_from_payload(udp_packet_t *udp_packet, mmtp_payload_fragments_union_t** mmtp_payload_p, lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {
    mmtp_payload_fragments_union_t* mmtp_payload = * mmtp_payload_p;
    
    mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
    mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =    NULL;
    mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
    mmtp_sub_flow_t* mmtp_sub_flow = NULL;
    
    //dump header, then dump applicable packet type
    //mmtp_packet_header_dump(mmtp_payload);
    
    if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x0) {
        global_stats->packet_counter_mmt_mpu++;
        
        if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {
            global_stats->packet_counter_mmt_timed_mpu++;
            
            if(lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && lls_slt_monitor->lls_sls_mmt_monitor->service_id == matching_lls_slt_mmt_session->service_id) {

                __TRACE("Starting processing loop, current mpu_sequence_number is: %d, packet_sequence_number: %d", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.packet_sequence_number);
                
                udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload);
                __TRACE("update_last_packet_id_and_mpu_sequence_number: ptr: %p, last dst_ip_addr: %u, last dst_port: %hu, last packet_id: %u, last mpu_sequence_number: %u",
                        last_flow_reference,
                        last_flow_reference->udp_flow.dst_ip_addr,
                        last_flow_reference->udp_flow.dst_port,
                        last_flow_reference->packet_id,
                        last_flow_reference->mpu_sequence_number);

// this happens above also...
//                udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_last_packet_id_mpu_sequence_id = udp_flow_latest_mpu_sequence_number_from_packet_id(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id);
                
                
                //this is pretty large, we could also query udp_flow_latest_mpu_sequence_number_from_packet_id for both of our monitored packet id's
                if( matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio &&
                    matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio &&
                   !matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed &&
                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video &&
                    matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video &&
                   !matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed &&
                   (
                    (lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id &&
                     mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number) ||
                    
                    (lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id &&
                     mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number)
                    )) {
                       
                       uint32_t min_mpu_sequence_number = MIN(matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                       __INFO("Starting re-fragmenting because packet_id:mpu_sequence number changed, from a: %u:%u, v: %u:%u with %u:%u, processing a: %u:%u, v: %u:%u, min: %u",
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
                              mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
                              min_mpu_sequence_number
                              );


                        //major refactoring
                       lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(&udp_packet->udp_flow, udp_flow_latest_mpu_sequence_number_container, min_mpu_sequence_number, min_mpu_sequence_number, mmtp_sub_flow_vector, lls_slt_monitor->lls_sls_mmt_monitor);

                        if(lls_sls_monitor_output_buffer_final_muxed_payload) {
                            //mark both of these flows as having been processed
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;

                            
                            if(true || lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                                //todo, call atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box with forced init box
                            	//lls_sls_monitor_output_buffer_final_muxed_payload
                            	lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/", min_mpu_sequence_number);

                            }
                            
                            if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

                                pipe_buffer_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
                                
                                pipe_buffer_unsafe_push_block(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);

                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = true;

                                pipe_buffer_notify_semaphore_post(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);

                                //check to see if we have shutdown
                                lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);

                                pipe_buffer_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);

                            }
                        }
                    }
                
                
                //update our last references for mpu_sequence rollover until we process packet_id signaling messages only if our mpu_sequence_number has changed due to malloc/copy the flow reference
                if(lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == last_flow_reference->packet_id &&
                   (!matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio || matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number != last_flow_reference->mpu_sequence_number )) {
                    
                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = false;
                    if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio) {
                        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio);
                    }
                    __INFO("Updating audio packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
                           last_flow_reference->packet_id,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio ? matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number : -1,
                           last_flow_reference->mpu_sequence_number,
                           matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio ? matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number : -1,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed
                           );
                    
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);

                } else if(lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == last_flow_reference->packet_id &&
                          (!matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video || matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number != last_flow_reference->mpu_sequence_number )) {
                    
                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = false;
                    if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video) {
                        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video);
                    }
                    __INFO("Updating video packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
                           last_flow_reference->packet_id,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video ? matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number : -1 ,
                           last_flow_reference->mpu_sequence_number,
                           matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video ? matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number : -1,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed
                           );
                    
                   
                    
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
                }
                
            }
        } else {
            //non-timed
            global_stats->packet_counter_mmt_nontimed_mpu++;
        }
            
        
            
#ifdef __REAP
            //only perform evictions if our last_mpu and last_packet are different than the last eviction run...
            if(!udp_flow_last_packet_id_mpu_sequence_id || !udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush || (udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush - udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start <= 0)) {
                //bail on reaping this time...
                goto cleanup;
            }
            
            //clear out our "global" packet_id data_unit_payloads from the mpu fragments
            mpu_fragments_t* mpu_fragments = NULL;
            if(!mmtp_sub_flow) {
                //try and find our packet_id subflow to clean up any intermediate objects
                mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow_last_packet_id_mpu_sequence_id->packet_id);
                __TRACE("mmtp_sub_flow was null, now: %p, resolved from sub_flow_vector and packet_id: %d",
                        mmtp_sub_flow,
                        udp_flow_last_packet_id_mpu_sequence_id->packet_id);
            }
            
            if(mmtp_sub_flow) {
                mpu_fragments = mpu_fragments_get_or_set_packet_id(mmtp_sub_flow, udp_flow_last_packet_id_mpu_sequence_id->packet_id);
            }
            
            if(mpu_fragments && udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start) {
                for(; udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start < udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush; udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start++) {
                    data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mpu_fragments->media_fragment_unit_vector, udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start);
                    
                    if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.data) {
                        data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;
                        if(data_unit_payload_fragments) {
                            //            __INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size)
                            int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
                            //            __INFO("Eviction pass for mpu: %u resulted in %u", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, evicted_count);
                        }
                    }
                }
            }
#endif
        
            
      
        
       
    } else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {
        
        //signaling_message_dump(mmtp_payload);
            global_stats->packet_counter_mmt_signaling++;

    } else {
            _MMTP_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
            global_stats->packet_counter_mmt_unknown++;
            goto cleanup;
    }
    
cleanup:

 //   mmtp_payload_fragments_union_free(mmtp_payload_p);
    mmtp_payload_p = NULL;

ret:
    return mmtp_payload;

}

static void route_process_from_alc_packet(alc_packet_t **alc_packet) {
    alc_packet_dump_to_object(alc_packet);
    
    if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer) {
     
        lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_isobmff_fragment(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);
        
        if(true || lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
            lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "route/", lls_slt_monitor->lls_sls_alc_monitor->processed_toi);
        }

        if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

        	pipe_ffplay_buffer_t* pipe_ffplay_buffer = lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer;

        	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
        
        	pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);
        
        	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);
        
			//check to see if we have shutdown
			lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);

			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
			//reset our buffer pos
			lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);
        }
    }
}

alc_packet_t* route_parse_from_udp_packet(lls_sls_alc_session_t *matching_lls_slt_alc_session, udp_packet_t *udp_packet) {
    alc_packet_t* alc_packet = NULL;

    //sanity check
    if(matching_lls_slt_alc_session->alc_session) {
        //re-inject our alc session
                alc_channel_t ch;
        ch.s = matching_lls_slt_alc_session->alc_session;
        
        //process ALC streams
        int retval = alc_rx_analyze_packet_a331_compliant((char*)udp_packet->data, udp_packet->data_length, &ch, &alc_packet);
        if(!retval) {
            global_stats->packet_counter_alc_packets_parsed++;
            
            //don't dump unless this is pointing to our monitor session
            //lls_slt_monitor->lls_service &&
            if(lls_slt_monitor->lls_sls_alc_monitor &&  lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session && lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session->service_id == matching_lls_slt_alc_session->service_id) {
                goto ret;
            } else {
                __LOG_TRACE("ignoring service_id: %u", matching_lls_slt_alc_session->service_id);
            }
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
cleanup:
    alc_packet_free(&alc_packet);
    alc_packet = NULL;

ret:
    return alc_packet;
    
}

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

  int i = 0;
  int k = 0;
  u_char ethernet_packet[14];
  u_char ip_header[24];
  u_char udp_header[8];
  int udp_header_start = 34;
  udp_packet_t* udp_packet = NULL;

    for (i = 0; i < 14; i++) {
        ethernet_packet[i] = packet[0 + i];
    }
    if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {
        __TRACE("Source MAC Address\t\t\t%02X:%02X:%02X:%02X:%02X:%02X", ethernet_packet[6], ethernet_packet[7], ethernet_packet[8], ethernet_packet[9], ethernet_packet[10], ethernet_packet[11]);
        __TRACE("Destination MAC Address\t\t%02X:%02X:%02X:%02X:%02X:%02X", ethernet_packet[0], ethernet_packet[1], ethernet_packet[2], ethernet_packet[3], ethernet_packet[4], ethernet_packet[5]);
    	__TRACE("Discarding packet with Ethertype unknown");
    	return;
    }

    for (i = 0; i < 20; i++) {
		ip_header[i] = packet[14 + i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__TRACE("Protocol not UDP, dropping");
		return;
	}

	#ifdef _ENABLE_TRACE
        __trace_dump_ip_header_info(ip_header);
	#endif

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 48;
		__TRACE("Options\t\t\t\t\t0x%02x 0x%02x 0x%02x 0x%02x", ip_header[20], ip_header[21], ip_header[22], ip_header[23]);
	}

	//malloc our udp_packet_header:
	udp_packet = (udp_packet_t*)calloc(1, sizeof(udp_packet_t));
	udp_packet->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	udp_packet->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = packet[udp_header_start + i];
	}

	udp_packet->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];

	udp_packet->total_packet_length = pkthdr->len;
	udp_packet->data_length = pkthdr->len - (udp_header_start + 8);

	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__ERROR("invalid data length of udp packet: %d", udp_packet->data_length);
		return;
	}
	__TRACE("Data length: %d", udp_packet->data_length);
	udp_packet->data = (u_char*)malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &packet[udp_header_start + 8], udp_packet->data_length);

	//inefficient as hell for 1 byte at a time, but oh well...
	#ifdef __ENABLE_TRACE
		for (i = 0; i < udp_packet->data_length; i++) {
			__TRACE("%02x ", packet[udp_header_start + 8 + i]);
		}
	#endif

	//collect global
	global_bandwidth_statistics->interval_total_current_bytes_rx += udp_packet->total_packet_length;
	global_bandwidth_statistics->interval_total_current_packets_rx++;
	global_bandwidth_statistics->grand_total_bytes_rx += udp_packet->total_packet_length;
	global_bandwidth_statistics->grand_total_packets_rx++;
	global_stats->packets_total_received++;

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		global_stats->packet_counter_filtered_ipv4++;
		//printf("setting dns current_bytes_rx: %d, packets_rx: %d", global_bandwidth_statistics->interval_filtered_current_bytes_rx, global_bandwidth_statistics->interval_filtered_current_packets_rx);
		global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_filtered_current_packets_rx++;

		return cleanup(udp_packet);
	}
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		global_bandwidth_statistics->interval_lls_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_lls_current_packets_rx++;

		global_stats->packet_counter_lls_packets_received++;

		//process as lls
		lls_table_t* lls_table = lls_table_create(udp_packet->data, udp_packet->data_length);
		if(lls_table) {
			global_stats->packet_counter_lls_packets_parsed++;

			bool should_free_lls = true; //do not free the lls table if we keep a reference in process_lls_table_slt_update
			if(lls_table->lls_table_id == SLT) {

				global_stats->packet_counter_lls_slt_packets_parsed++;

				int retval = lls_slt_table_check_process_update(lls_table, lls_slt_monitor);


				if(!retval) {
					global_stats->packet_counter_lls_slt_update_processed++;
				} else {
					global_stats->packet_counter_lls_packets_parsed_error++;
				}
			}
		}

		//goto cleanup;
		return cleanup(udp_packet);
	}


	//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255
//    if(udp_packet->udp_flow.dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->udp_flow.dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
//        //out of range, so drop
//        count_packet_as_filtered(udp_packet);
//
//        //goto cleanup;
//        return cleanup(udp_packet);
//    }
//    
    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        count_packet_as_filtered(udp_packet);
        return cleanup(udp_packet);
    }

	//ALC (ROUTE) - If this flow is registered from the SLT, process it as ALC, otherwise run the flow thru MMT
	lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	if(matching_lls_slt_alc_session) {
		global_bandwidth_statistics->interval_alc_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_alc_current_packets_rx++;
		global_stats->packet_counter_alc_recv++;

        alc_packet_t* alc_packet = route_parse_from_udp_packet(matching_lls_slt_alc_session, udp_packet);
        if(alc_packet) {
            route_process_from_alc_packet(&alc_packet);
            alc_packet_free(&alc_packet);
        }
        
        return cleanup(udp_packet);
	}

    lls_sls_mmt_session_t* matching_lls_slt_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if(matching_lls_slt_mmt_session) {
        __TRACE("data len: %d", udp_packet->data_length)
        mmtp_payload_fragments_union_t * mmtp_payload = mmtp_parse_from_udp_packet(udp_packet);
        if(mmtp_payload) {
            mmtp_process_from_payload(udp_packet, &mmtp_payload, matching_lls_slt_mmt_session);
           // mmtp_payload_fragments_union_free(&mmtp_payload);
        }
        return cleanup(udp_packet);
	}

    //if we get here, we don't know what type of packet it is..
    global_stats->packet_counter_udp_unknown++;
    
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

	_MPU_DEBUG_ENABLED = 0;
	_MMTP_DEBUG_ENABLED = 0;
	_LLS_DEBUG_ENABLED = 0;
    _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
    _PLAYER_FFPLAY_DEBUG_ENABLED = 0;
    _PLAYER_FFPLAY_TRACE_ENABLED = 0;
    _ALC_UTILS_DEBUG_ENABLED = 0;
    _ALC_UTILS_TRACE_ENABLED = 0;
    _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 0;

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

    mmtp_sub_flow_vector = (mmtp_sub_flow_vector_t*)calloc(1, sizeof(*mmtp_sub_flow_vector));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    lls_slt_monitor = lls_slt_monitor_create();

    global_stats = (global_atsc3_stats*)calloc(1, sizeof(*global_stats));
    gettimeofday(&global_stats->program_timeval_start, 0);

    global_bandwidth_statistics = (bandwidth_statistics_t*)calloc(1, sizeof(*global_bandwidth_statistics));
	gettimeofday(&global_bandwidth_statistics->program_timeval_start, NULL);


    //create our background thread for bandwidth calculation
    /** ncurses support - valgrind on osx will fail in pthread_create...**/

	ncurses_init();


#ifndef _TEST_RUN_VALGRIND_OSX_

	pthread_t global_bandwidth_thread_id;
	pthread_create(&global_bandwidth_thread_id, NULL, print_bandwidth_statistics_thread, NULL);

	pthread_t global_stats_thread_id;
	pthread_create(&global_stats_thread_id, NULL, print_global_statistics_thread, NULL);

	pthread_t global_slt_thread_id;
	pthread_create(&global_slt_thread_id, NULL, print_lls_instance_table_thread, (void*)lls_slt_monitor);

	pthread_t global_ncurses_input_thread_id;
	int ncurses_input_ret = pthread_create(&global_ncurses_input_thread_id, NULL, ncurses_input_run_thread, (void*)lls_slt_monitor);
	assert(!ncurses_input_ret);

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
