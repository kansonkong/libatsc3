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

#ifndef ATSC3_ALC_UTILS_H_
#define ATSC3_ALC_UTILS_H_

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;


//ALC dump object output path
#define __ALC_DUMP_OUTPUT_PATH__ "route/"

//prepend fragments with init box (ftyp, mmpu, moov, meta)
//#define __TESTING_PREPEND_TSI__ 			"003000"
//#define __TESTING_PREPEND_TOI_INIT__ 		"0000002"

//reconsitiute fragments into single file, init box, for pipe playback...
#define __TESTING_RECONSTITUTED_TSI__ 		003000
#define __TESTING_RECONSTITUTED_TOI_INIT__ 	0000002

//write out to file recon
//#define __TESTING_RECON_FILE_POINTER__
#define __TESTING_RECONSITIUTED_FILE_NAME__ "3.m4v"


/**
 * deubg toi dump methods
 */

//this must be set to 1 for dumps to be written to disk
extern int _ALC_PACKET_DUMP_TO_OBJECT_ENABLED;

char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet);

int alc_packet_dump_to_object(alc_packet_t** alc_packet_ptr);

//deprecated
void alc_recon_file_ptr_set_tsi_toi(FILE* file_ptr, uint32_t tsi, uint32_t toi_init);
void alc_recon_file_ptr_fragment_with_init_box(FILE*,  alc_packet_t* alc_packet);

//new
void alc_recon_file_buffer_struct_set_monitor(pipe_ffplay_buffer_t* pipe_ffplay_buffer, lls_sls_alc_monitor_t* lls_sls_alc_monitor);
void alc_recon_file_buffer_struct_set_tsi_toi(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint32_t tsi, uint32_t toi_init);
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, alc_packet_t* alc_packet);

char* alc_packet_dump_to_object_get_filename_tsi_toi(uint32_t tsi, uint32_t toi);
void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, lls_sls_alc_monitor_t* lls_slt_monitor, alc_packet_t* alc_packet);
void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);
void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);


#define __ALC_UTILS_ERROR(...)   printf("%s:%d:ERROR:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");
#define __ALC_UTILS_WARN(...)    printf("%s:%d:WARN:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");
#define __ALC_UTILS_INFO(...)    printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");

#define __ALC_UTILS_DEBUG(...)   if(_ALC_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __ALC_UTILS_TRACE(...)   if(_ALC_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }


#endif /* ATSC3_ALC_UTILS_H_ */
