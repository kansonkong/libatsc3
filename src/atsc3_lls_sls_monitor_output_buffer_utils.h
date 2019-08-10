//
//  atsc3_lls_sls_monitor_output_buffer.h
//  libatsc3
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef atsc3_lls_sls_monitor_output_buffer_utils_h
#define atsc3_lls_sls_monitor_output_buffer_utils_h

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_lls_types.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_isobmff_trun_box.h"
#include "bento4/ISOBMFFTrackJoiner_firewall_gpl.h"


extern int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED;
extern int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED;

void lls_sls_lls_sls_monitor_buffer_persist_moof(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);
void lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, const char* directory_path, uint32_t mpu_sequence_number, const char* prefix);
void ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, const char* directory_path, uint32_t mpu_sequence_number, const char* prefix);
block_t* lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);
void lls_sls_monitor_output_buffer_reset_rebuilt_mpu_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
//extern trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntries(block_t* moof_box);
//trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntriesWithSampleSize(block_t* moof_box);

//end for bento4 support

//void lls_sls_monitor_output_buffer_reset_audio_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
void lls_sls_monitor_output_buffer_reset_trun(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);

void lls_sls_lls_sls_monitor_buffer_isobmff_reset_moof_and_mdat_position(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);

void lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
void lls_sls_monitor_output_buffer_reset_all_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

bool lls_sls_monitor_output_buffer_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, mmtp_mpu_packet_t* mmtp_mpu_packet);

//refragment
int lls_sls_monitor_output_buffer_copy_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block);
int lls_sls_monitor_output_buffer_append_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block);


int lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* isobmff_moof_block);

int lls_sls_monitor_output_buffer_copy_init_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block);

int lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);
int lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);

int lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);
int lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);

//int lls_sls_monitor_output_buffer_copy_and_parse_audio_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* audio_isobmff_moof_fragment);
//int lls_sls_monitor_output_buffer_copy_and_parse_video_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* video_isobmff_moof_fragment);
int lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, mmtp_mpu_packet_t* mmtp_mpu_packet);
//int lls_sls_monitor_output_buffer_recover_from_last_audio_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
//int lls_sls_monitor_output_buffer_recover_from_last_video_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

//mmt: mfu from in/out of order flow
block_t* lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);
block_t* lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);

int lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_to_create_mdat);

block_t* lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);
int lls_sls_monitor_output_buffer_merge_alc_fragment_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* isobmff_fragment);

void 	 lls_sls_monitor_output_buffer_alc_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio,  uint32_t mpu_sequence_number_video);
void 	 lls_sls_monitor_output_buffer_mmt_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio,  uint32_t mpu_sequence_number_video);


void lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor_t* lls_slt_monitor);
//mmt void lls_sls_monitor_output_buffer_alc_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio,  uint32_t mpu_sequence_number_video);

pthread_mutex_t* lls_sls_monitor_reader_mutext_create();
void lls_sls_monitor_reader_mutex_lock(pthread_mutex_t* pthread_mutex);
void lls_sls_monitor_reader_mutex_unlock(pthread_mutex_t* pthread_mutex);

#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN(...)    printf("%s:%d:WARN  :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO(...)    printf("%s:%d:INFO  :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG(...)   if(_LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__); };
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE(...)   if(_LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__); };


#if defined (__cplusplus)
}
#endif
#endif /* atsc3_lls_sls_monitor_output_buffer_h */
