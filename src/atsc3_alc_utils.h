/*
 * atsc3_alc_utils.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <inttypes.h>


#ifndef ATSC3_ALC_UTILS_H_
#define ATSC3_ALC_UTILS_H_

#include "atsc3_utils.h"
#include "atsc3_alc_rx.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_lls_types.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_route_sls_processor.h"


#if defined (__cplusplus)
extern "C" {
#endif
  
extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;

//zero out this slab of memory for a single TOI when pre-allocating
#define  __TO_PREALLOC_ZERO_SLAB_SIZE 8192000


void atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

atsc3_route_object_t* atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);
int atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_object_t* atsc3_route_object);



/**
 * deubg toi dump methods
 */


//this must be set to 1 for dumps to be written to disk
extern int _ALC_PACKET_DUMP_TO_OBJECT_ENABLED;


//get <LS> element for matching flow and packet
atsc3_route_s_tsid_RS_LS_t* atsc3_alc_packet_get_RS_LS_element(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

//get <fdt:File> element for matching TOI from RS_LS
atsc3_fdt_file_t* atsc3_alc_RS_LS_get_matching_toi_file_instance(atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS, uint32_t search_toi);


char* alc_packet_dump_to_object_get_s_tsid_filename(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);
char* alc_packet_dump_to_object_get_temporary_filename(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet);

FILE* atsc3_alc_object_open(char* file_name);

FILE* alc_object_pre_allocate(char* file_name, atsc3_alc_packet_t* alc_packet);
int alc_packet_write_fragment(FILE* f, char* file_name, uint32_t offset, atsc3_alc_packet_t* alc_packet);
FILE* alc_object_open_or_pre_allocate(char* file_name, atsc3_alc_packet_t* alc_packet);

void alc_recon_file_ptr_set_tsi_toi(FILE* file_ptr, uint32_t tsi, uint32_t toi_init);
char* alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow_t* udp_flow, uint32_t tsi, uint32_t toi);

block_t* alc_get_payload_from_filename(char*);



//jjustman-2020-03-11
//deprecated - used for isobmff de-fragmentation to handoff a standalone media presentation unit from alc media fragment
void alc_recon_file_buffer_struct_set_tsi_toi(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint32_t tsi, uint32_t toi_init);
void alc_recon_file_ptr_fragment_with_init_box(FILE* output_file_ptr, udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, uint32_t to_match_toi_init);
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet);
void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_slt_monitor);
void __alc_prepend_fragment_with_init_box(char* file_name, atsc3_alc_packet_t* alc_packet);
void __alc_recon_fragment_with_init_box(char* file_name, atsc3_alc_packet_t* alc_packet, uint32_t tsi, uint32_t toi_init, const char* to_write_filename);
//end deprecated

#if defined (__cplusplus)
}
#endif

#define __ALC_UTILS_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ALC_UTILS_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ALC_UTILS_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __ALC_UTILS_DEBUG(...)   if(_ALC_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ALC_UTILS_TRACE(...)   if(_ALC_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }
#define __ALC_UTILS_IOTRACE(...) if(_ALC_UTILS_IOTRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif /* ATSC3_ALC_UTILS_H_ */
