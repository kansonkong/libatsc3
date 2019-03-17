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

#include "atsc3_utils.h"
#include "atsc3_alc_rx.h"
#include "alc_channel.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_lls_types.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"

#ifndef ATSC3_ALC_UTILS_H_
#define ATSC3_ALC_UTILS_H_


#if defined (__cplusplus)
extern "C" {
#endif
  
extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;

//zero out this slab of memory for a single TOI when pre-allocating
#define  __TO_PREALLOC_ZERO_SLAB_SIZE 8192000


//ALC dump object output path
#define __ALC_DUMP_OUTPUT_PATH__ "route/"
/**
 * deubg toi dump methods
 */




//this must be set to 1 for dumps to be written to disk
extern int _ALC_PACKET_DUMP_TO_OBJECT_ENABLED;

char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet);

int alc_packet_dump_to_object(alc_packet_t** alc_packet_ptr);
    
FILE* alc_object_pre_allocate(char* file_name, alc_packet_t* alc_packet);
int alc_packet_write_fragment(FILE* f, char* file_name, uint32_t offset, alc_packet_t* alc_packet);
FILE* alc_object_open_or_pre_allocate(char* file_name, alc_packet_t* alc_packet);


//deprecated
void alc_recon_file_ptr_set_tsi_toi(FILE* file_ptr, uint32_t tsi, uint32_t toi_init);
void alc_recon_file_ptr_fragment_with_init_box(FILE*,  alc_packet_t* alc_packet, uint32_t toi_init);

//new
void alc_recon_file_buffer_struct_set_monitor(lls_sls_alc_monitor_t* lls_sls_alc_monitor);
void alc_recon_file_buffer_struct_set_tsi_toi(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint32_t tsi, uint32_t toi_init);
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, alc_packet_t* alc_packet);

char* alc_packet_dump_to_object_get_filename_tsi_toi(uint32_t tsi, uint32_t toi);
void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(lls_sls_alc_monitor_t* lls_slt_monitor, alc_packet_t* alc_packet);
void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);
void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet, uint32_t tsi, uint32_t toi_init, const char* to_write_filename);

block_t* alc_get_payload_from_filename(char*);

#if defined (__cplusplus)
}
#endif

#define __ALC_UTILS_ERROR(...)   printf("%s:%d:ERROR:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n");
#define __ALC_UTILS_WARN(...)    printf("%s:%d:WARN:%.4f : ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n");
#define __ALC_UTILS_INFO(...)    printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n");

#define __ALC_UTILS_DEBUG(...)   if(_ALC_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n"); }
#define __ALC_UTILS_TRACE(...)   if(_ALC_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n"); }
#define __ALC_UTILS_IOTRACE(...) if(_ALC_UTILS_IOTRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\r%s","\n"); }


#endif /* ATSC3_ALC_UTILS_H_ */
