/*
 * atsc3_mmt_listener_to_http_hls_fmp4.cpp
 *
 *  Created on: Aug 30th, 2019
 *      Author: jjustman
 *
 * MMT to HLS fmp4 transmux service with HTTP endpoint
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

#include "../atsc3_lls.h"
#include "../atsc3_lls_alc_utils.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"
#include "../atsc3_mmt_reconstitution_from_media_sample.h"

#include "../alc_channel.h"
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

// lls and sls glue for slt, contains lls_table_slt for MMT montior
lls_slt_monitor_t* lls_slt_monitor = NULL;
lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

mmtp_flow_t* mmtp_flow;

//todo: refactor me out for mpu recon persitance

udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;


/**
 *
 * httpd listener testing
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
            lls_sls_mmt_monitor->service_id = my_service_id;
            
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

void process_mmtp_payload(udp_packet_t *udp_packet, lls_sls_mmt_session_t* matching_lls_sls_mmt_session) {

	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

	if(!mmtp_packet_header) {
		goto error;
	}

	//for filtering MMT flows by a specific packet_id
	if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
		goto cleanup;
	}

	if(mmtp_packet_header->mmtp_payload_type == 0x0) {
		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, udp_packet->data);
		if(!mmtp_mpu_packet) {
			goto error;
		}

        //hack for mpu re-assembly
        
        
		if(mmtp_mpu_packet->mpu_timed_flag == 1) {
            mmtp_process_from_payload(mmtp_mpu_packet, mmtp_flow, lls_slt_monitor, udp_packet, udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);
//
//            block_Destroy(&mmtp_mpu_packet->du_mpu_metadata_block);
//            block_Destroy(&mmtp_mpu_packet->du_mfu_block);
//            block_Destroy(&mmtp_mpu_packet->du_movie_fragment_block);

			//mmtp_mpu_dump_header(mmtp_mpu_packet);
		} else {
			//non-timed
			__ATSC3_WARN("mmtp_packet_parse: non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
		}
	} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

        
		mmtp_signalling_packet_t* mmtp_signalling_packet = mmt_signalling_message_parse_packet_header(mmtp_packet_header, udp_packet->data);
		uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
		if(parsed_count) {
			mmt_signalling_message_dump(mmtp_signalling_packet);
            
            //update our sls_mmt_session info
            mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);

            mmtp_signalling_packet_free(&mmtp_signalling_packet);
		} else {
			goto error;
		}

	} else {
		__ATSC3_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
		goto error;
	}
    
    goto cleanup;

 error:
		__ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
				__toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
				udp_packet->udp_flow.src_ip_addr,
				__toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
				udp_packet->udp_flow.dst_ip_addr);
    
 cleanup:

 	 ;

}

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);

	if(!udp_packet) {
		return;
	}
    
	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		//printf("setting dns current_bytes_rx: %d, packets_rx: %d", global_bandwidth_statistics->interval_filtered_current_bytes_rx, global_bandwidth_statistics->interval_filtered_current_packets_rx);
		return udp_packet_free(&udp_packet);
	}

	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		//process as lls.sst, dont free as we keep track of our object in the lls_slt_monitor
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
        if(lls_table) {
			if(lls_table->lls_table_id == SLT) {
				int retval = lls_slt_table_perform_update(lls_table, lls_slt_monitor);
                
                if(!retval) {
                    lls_dump_instance_table(lls_table);
                    for(int i=0; i < lls_table->slt_table.service_entry_n; i++) {
                        lls_service_t* lls_service = lls_table->slt_table.service_entry[i];
                        if(lls_service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_MMTP) {
                            if(lls_sls_mmt_monitor) {
                                //re-configure
                            } else {
                                __INFO("Adding service: %d", lls_service->service_id);

                                lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
                                
                                //we may not be initialized yet, so re-check again later
                                lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_service->service_id);
                                lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
                                lls_sls_mmt_monitor->service_id = lls_service->service_id;
                            }
                        }
                    }
                }
            }
		}
        
        //recheck video_packet_id/audio_packet_id
        if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
            if(!lls_sls_mmt_monitor->video_packet_id) {
                lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);
                lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
                lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;
            }
            
            if(lls_sls_mmt_monitor->video_packet_id) {
                lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = false;
                lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;
                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer = (http_output_buffer_t*)calloc(1, sizeof(http_output_buffer_t));
                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex = lls_sls_monitor_reader_mutext_create();
                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled = true;
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
        return udp_packet_free(&udp_packet);
    }

	//find our matching MMT flow and push it to reconsitution
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
    if(matching_lls_sls_mmt_session) {
        __TRACE("data len: %d", udp_packet->data_length);
        process_mmtp_payload(udp_packet, matching_lls_sls_mmt_session);

        return udp_packet_free(&udp_packet);
	}

    //if we get here, we don't know what type of packet it is..
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
    _MPU_DEBUG_ENABLED = 1;
    _MMTP_DEBUG_ENABLED = 1;
    _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;

#define __LOTS_OF_DEBUGGING__
#ifdef __LOTS_OF_DEBUGGING__
	_MPU_DEBUG_ENABLED = 0;
	_MMTP_DEBUG_ENABLED = 0;
	_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 0;

	_MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED = 1;
	_MMT_RECON_FROM_SAMPLE_TRACE_ENABLED = 1;

    _LLS_DEBUG_ENABLED = 1;
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
    	println("%s - a udp mulitcast listener for MMT and transmux to HLS fmp4", argv[0]);
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
    mmtp_flow = mmtp_flow_new();
    
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    /** ncurses support - valgrind on osx will fail in pthread_create...**/

#ifndef _TEST_RUN_VALGRIND_OSX_

	//block sigpipe before creating our threads
	sigemptyset (&player_signal_mask);
	sigaddset (&player_signal_mask, SIGPIPE);
	int rc = pthread_sigmask (SIG_BLOCK, &player_signal_mask, NULL);
	if(!rc) {
		  __WARN("Unable to block SIGPIPE, this may result in a runtime crash when closing ffplay!");
	}

	pthread_t global_http_thread_id;
	pthread_create(&global_http_thread_id, NULL, global_httpd_run_thread, (void*)lls_slt_monitor);

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);
    
    pthread_t global_autoplay_thread_id;
    pthread_create(&global_autoplay_thread_id, NULL, global_autoplay_run_thread, NULL);

	pthread_join(global_pcap_thread_id, NULL);

#else
	pcap_loop_run_thread(dev);
#endif

    return 0;
}

