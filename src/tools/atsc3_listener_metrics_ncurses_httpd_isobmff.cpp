/*
 * atsc3_listener_metrics_ncurses_httpd_isobmff.c
 *
 *  Created on: Mar 17, 2019
 *      Author: jjustman
 *
 * NOTE: MPU-reassembly for MMT, TODO: move to MFU emmission to decoder buffer
 * 
 * global listener driver for LLS, MMT and ROUTE / DASH with refragmented http output on port 8888
 *
 *
 * note: to use local playback with ffmpeg as the box is building (since we dont interlave samples fully), use:
 * 	ffplay  cache:http://127.0.0.1:8888/video.m4s -loglevel trace
 *
 *
 */


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
#include "../atsc3_logging_externs.h"


#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"
#include "../atsc3_mmt_reconstitution_from_media_sample.h"

#include "../atsc3_alc_rx.h"
#include "../atsc3_alc_utils.h"

#include "../atsc3_bandwidth_statistics.h"
#include "../atsc3_packet_statistics.h"

#include "../atsc3_output_statistics_ncurses.h"


#define _ENABLE_DEBUG true


//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

//jjustman-2019-09-18: refactored MMTP flow collection management
mmtp_flow_t* mmtp_flow;

//todo: jjustman-2019-09-18 refactor me out for mpu recon persitance
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;

// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session
lls_slt_monitor_t* lls_slt_monitor;

extern atsc3_global_statistics_t* atsc3_global_statistics;

/**
 *
 * httpd listener integration
 *
 * jjustman-2019-09-18: TODO: refactor this out
 */


#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <dirent.h>
#include <microhttpd.h>
#include <unistd.h>

#define PORT 8888

#define FILENAME "test.mp4"
#define MIMETYPE "video/mp4"

#define PAGE "<html><head><title>File not found</title></head><body>File not found</body></html>"

static ssize_t http_output_response_from_player_pipe_reader_callback (void *cls, uint64_t pos, char *buf, size_t max)
{
	__INFO("http_output_response_from_player_pipe_reader_callback: enter: pos: %llu, buf: %p, max_size: %lu", pos, buf, max);
	if(!lls_slt_monitor->lls_sls_mmt_monitor) {
			__WARN("http_output_response_from_player_pipe_reader_callback: not lls_sls_mmt_monitor yet");
			//sleep so we don't spinlock too fast
			usleep(100000);
			return 0;
		}
	if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer) {
		__WARN("http_output_response_from_player_pipe_reader_callback: not http_output_buffer yet");
		//sleep so we don't spinlock too fast
		usleep(100000);
		return 0;
	}
	lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_output_conntected = true;
	if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output &&
			!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming) {
		__WARN("http_output_response_from_player_pipe_reader_callback: both buffers are null, returning 0");
		//sleep so we don't spinlock too fast
		usleep(100000);
		return 0;
	}
	if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output &&
			lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->total_fragments_incoming_written < 4) {
			__WARN("http_output_response_from_player_pipe_reader_callback: total incoming fragments written is less than 4, returning 0 ");
			//sleep so we don't spinlock too fast
			usleep(100000);
			return 0;
		}

	lls_sls_monitor_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);
	//swap
	if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output && lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming) {
		lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output = lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming;
		lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming = NULL;
		lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->i_pos = 0;
	}

	//block copy accordingly
	uint32_t block_pos = lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->i_pos;
	uint32_t block_size = __MIN(max, lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->p_size - lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->i_pos);

	__INFO("http_output_response_from_player_pipe_reader_callback: copying from %p, block_pos (i_pos): %u, block_size: %u, p_size: %u",
			lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->p_buffer,
			block_pos,
			block_size,
			lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->p_size);



	memcpy(buf, &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->p_buffer
			[block_pos], block_size);
	lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->i_pos += block_size;

	if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->i_pos == lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output->p_size) {
		__INFO("http_output_response_from_player_pipe_reader_callback: end of output buffer, setting null");
		block_Release(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_client_output);

	}

	lls_sls_monitor_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);

	__INFO("http_output_response_from_player_pipe_reader_callback: return: returning size: %u, total incoming fragments written: %u", block_size, lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->total_fragments_incoming_written);

	return block_size;
}


static void http_output_response_from_player_pipe_reader_free_callback (void *cls)
{
	lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_output_conntected = false;
	__INFO("http_output_response_from_player_pipe_reader_free_callback: closing: %p", cls);
}


static int http_output_response_from_player_pipe (void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data,
	  size_t *upload_data_size, void **ptr)
{
	static int aptr;
	struct MHD_Response *response;
	int ret;
	FILE *file;
	int fd;
	//  DIR *dir;
	struct stat buf;
	char emsg[1024];
	(void)cls;               /* Unused. Silent compiler warning. */
	(void)version;           /* Unused. Silent compiler warning. */
	(void)upload_data;       /* Unused. Silent compiler warning. */
	(void)upload_data_size;  /* Unused. Silent compiler warning. */

	if (0 != strcmp (method, MHD_HTTP_METHOD_GET))
	return MHD_NO;              /* unexpected method */

  	response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN, 512 * 1024,     /* 512k page size */
                                                    &http_output_response_from_player_pipe_reader_callback,
                                                    NULL,
                                                    &http_output_response_from_player_pipe_reader_free_callback);

	if (NULL == response){
		return MHD_NO;
	}
	MHD_add_response_header(response, "Content-Type", MIMETYPE);

	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	//not sure if this is needed here or not..
	MHD_destroy_response (response);

	return ret;
}

void* global_autoplay_run_thread(void*p) {
    uint16_t my_service_id = 3;
    lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

    while(true) {
        sleep(1);
        lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, my_service_id);
        if(lls_sls_mmt_session) {
            lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
            lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
            //TODO - jjustman-2019-10-03 - fix this hack
            lls_sls_mmt_monitor->atsc3_lls_slt_service = lls_sls_mmt_session->atsc3_lls_slt_service;
            
            lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
            lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;
            
            lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = false;
            lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;
            sleep(3);

            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer = pipe_create_ffplay_resolve_fps(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff);
            
            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = true;
            
            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer = (http_output_buffer_t*)calloc(1, sizeof(http_output_buffer_t));
            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex = lls_sls_monitor_reader_mutext_create();
            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled = true;
            break;
        }
    }
    
    return NULL;
}

void* global_httpd_run_thread(void* lls_slt_monitor_ptr) {

//
//    lls_slt_monitor_t* lls_slt_monitor = (lls_slt_monitor_t*)lls_slt_monitor_ptr;
//    lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
//    lls_sls_alc_monitor* lls_sls_alc_monitor = NULL;

    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG,
                           PORT,
                           NULL, NULL, &http_output_response_from_player_pipe, (void*)PAGE, MHD_OPTION_END);

    if (NULL == daemon) return NULL;
    MHD_run(daemon);

    while(true) {
    	sleep(1);
    }
}


void count_packet_as_filtered(udp_packet_t* udp_packet) {
	atsc3_global_statistics->packet_counter_filtered_ipv4++;
	global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data->p_size;
	global_bandwidth_statistics->interval_filtered_current_packets_rx++;
}

void update_global_mmtp_statistics_from_udp_packet_t(lls_sls_mmt_session_t* matching_lls_sls_mmt_session, udp_packet_t *udp_packet) {
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
            mmtp_mpu_packet = mmtp_process_from_payload(mmtp_mpu_packet, mmtp_flow, lls_slt_monitor, udp_packet, udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);

		} else {
			//non-timed
			__ATSC3_WARN("update_global_mmtp_statistics_from_udp_packet_t: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
		}
	} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

		mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
		uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
		if(parsed_count) {
			mmt_signalling_message_dump(mmtp_signalling_packet);
			//temp hack until we are managing flows better
			    /* keep this packet around for processing **/
            //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
            mmtp_asset_flow_t* mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
            mmtp_asset_t* mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
           
            //TODO: FIX ME!!! HACK - jjustman-2019-09-05
            mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_new();
            mmtp_mpu_packet->mmtp_packet_id = mmtp_signalling_packet->mmtp_packet_id;
            
            mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
            mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);
            
            //TODO: FIX ME!!! HACK - jjustman-2019-09-05
            mmtp_mpu_packet_free(&mmtp_mpu_packet);
            
            //update our sls_mmt_session info
            mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);
 
		} else {
            //jjustman-2019-09-05 - unsupported signalling message type, so free immediately
            mmtp_signalling_packet_free(&mmtp_signalling_packet);

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
    if(mmtp_packet_header) {
        mmtp_packet_header_free(&mmtp_packet_header);
    }
}

/**
 * only build our atsc3_isobmff_build_joined_alc_isobmff_fragment if we have ffplay output active
 *
 * TODO: jjustman-2020-06-02: fixme to use proper lls_alc monitor pattern
 */

static void route_process_from_alc_packet(udp_flow_t* udp_flow, alc_packet_t **alc_packet) {
	/**
	 * jdj-2019-05-29: TODO - refactor out for EXT_FTI processing that may be missing a close object flag,
	 * 							 use a sparse array lookup (https://github.com/ned14/nedtries) for resolution to proper transfer_object_length to back-patch close flag
	 */
	if((*alc_packet)->use_start_offset && lls_slt_monitor->lls_sls_alc_monitor &&
				atsc3_sls_alc_flow_get_first_tsi(lls_slt_monitor->lls_sls_alc_monitor->atsc3_sls_alc_video_flow_v) &&
				atsc3_sls_alc_flow_get_first_tsi(lls_slt_monitor->lls_sls_alc_monitor->atsc3_sls_alc_audio_flow_v)) {



		atsc3_alc_persist_route_ext_attributes_per_lls_sls_alc_monitor_essence(*alc_packet, lls_slt_monitor->lls_sls_alc_monitor);

	}

    atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(udp_flow,
                                                                                alc_packet,
                                                                                lls_slt_monitor->lls_sls_alc_monitor);
    
    if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer) {

    	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_alc_isobmff_fragment(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);

		if(!lls_sls_monitor_output_buffer_final_muxed_payload) {
			lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = false;
			__ERROR("lls_sls_monitor_output_buffer_final_muxed_payload was NULL!");
			return;
		}

        if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

        	pipe_ffplay_buffer_t* pipe_ffplay_buffer = lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer;

        	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
        
        	pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);
        
        	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);
        
			//check to see if we have shutdown
			lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);

			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
			//reset our buffer pos and should_flush = false;
        }

        if(true || lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
        	//don't double write to disk for route objects as we do this already in the route alc refrag client
            lls_sls_monitor_output_buffer_alc_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "route/",
            		lls_slt_monitor->lls_sls_alc_monitor->last_completed_flushed_audio_toi,
					lls_slt_monitor->lls_sls_alc_monitor->last_completed_flushed_video_toi);
        }

		lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer);
    }
}

alc_packet_t* route_parse_from_udp_packet(lls_sls_alc_session_t *matching_lls_slt_alc_session, udp_packet_t *udp_packet) {
    alc_packet_t* alc_packet = NULL;

    //sanity check
    if(matching_lls_slt_alc_session->alc_session) {
        //re-inject our alc session

        //process ALC streams
        int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &alc_packet);
        if(!retval) {
            atsc3_global_statistics->packet_counter_alc_packets_parsed++;
            
            //don't dump unless this is pointing to our monitor session
            if(lls_slt_monitor->lls_sls_alc_monitor &&  lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session && lls_slt_monitor->lls_sls_alc_monitor->lls_alc_session->service_id == matching_lls_slt_alc_session->service_id) {
                goto ret;
            } else {
               // __ATSC3_TRACE("ignoring service_id: %u", matching_lls_slt_alc_session->service_id);
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
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if(matching_lls_sls_mmt_session) {
        __TRACE("data len: %d", udp_packet->data_length);
        update_global_mmtp_statistics_from_udp_packet_t(matching_lls_sls_mmt_session, udp_packet);

        return udp_packet_free(&udp_packet);
	}

    //if we get here, we don't know what type of packet it is..
    atsc3_global_statistics->packet_counter_udp_unknown++;
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


/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {
    _MMT_MPU_PARSER_DEBUG_ENABLED = 0;
    _MMTP_DEBUG_ENABLED = 0;
    _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 0;
    
    _AEAT_PARSER_DEBUG_ENABLED = 1;
    _AEAT_PARSER_TRACE_ENABLED = 1;
    
    _LLS_INFO_ENABLED = 1;


#ifdef __LOTS_OF_DEBUGGING__
	_MMT_MPU_PARSER_DEBUG_ENABLED = 0;
	_MMTP_DEBUG_ENABLED = 0;
	_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 0;

	_MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED = 1;
	_MMT_RECON_FROM_SAMPLE_TRACE_ENABLED = 1;

	_LLS_DEBUG_ENABLED = 0;
    _ISOBMFF_TOOLS_DEBUG_ENABLED = 1;
    _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
    _PLAYER_FFPLAY_TRACE_ENABLED = 0;

    _XML_INFO_ENABLED = 1;
   	_XML_DEBUG_ENABLED = 0;
   	_XML_TRACE_ENABLED = 0;

    _ALC_UTILS_IOTRACE_ENABLED = 1;
    _ALC_UTILS_DEBUG_ENABLED = 1;
    _ALC_UTILS_TRACE_ENABLED = 1;
    _ALC_RX_DEBUG_ENABLED = 1;
    _ALC_RX_TRACE_ENABLED = 1;

    _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED = 1;
    _FDT_PARSER_DEBUG_ENABLED=1;

    //if this is disabled, be sure to run this driver with stderr redirect
    _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 1;

    //recon debugging
    _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED = 1;
    _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED = 1;

#endif

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
    // mkdir("route", 0777);
    
    /** setup global structs **/
    lls_slt_monitor = lls_slt_monitor_create();
    mmtp_flow = mmtp_flow_new();
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

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

	pthread_t global_http_thread_id;
	pthread_create(&global_http_thread_id, NULL, global_httpd_run_thread, (void*)lls_slt_monitor);

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);

#ifdef __LIBATSC3_AUTOPLAY__
    pthread_t global_autoplay_thread_id;
    pthread_create(&global_autoplay_thread_id, NULL, global_autoplay_run_thread, NULL);
#endif

	pthread_join(global_pcap_thread_id, NULL);
	pthread_join(global_ncurses_input_thread_id, NULL);

#else
	pcap_loop_run_thread(dev);
#endif

    return 0;
}

