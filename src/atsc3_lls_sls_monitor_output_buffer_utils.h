//
//  atsc3_lls_sls_monitor_output_buffer.h
//  libatsc3
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef atsc3_lls_sls_monitor_output_buffer_utils_h
#define atsc3_lls_sls_monitor_output_buffer_utils_h

#include "atsc3_lls_types.h"
#include "atsc3_mmtp_types.h"
#include "atsc3_player_ffplay.h"

extern int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED;
extern int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED;

void lls_sls_monitor_output_buffer_reset_moov_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
void lls_sls_monitor_output_buffer_reset_all_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

int lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);
int lls_sls_monitor_output_buffer_copy_audio_moov_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);
int lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);
int lls_sls_monitor_output_buffer_copy_and_recover_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* audio_data_unit);

int lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);
int lls_sls_monitor_output_buffer_copy_video_moov_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);
int lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);
int lls_sls_monitor_output_buffer_copy_and_recover_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* video_data_unit);

int lls_sls_monitor_buffer_isobmff_moov_patch_mdat_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_to_patch);

block_t* lls_sls_monitor_output_buffer_copy_audio_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
block_t* lls_sls_monitor_output_buffer_copy_audio_moov_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

block_t* lls_sls_monitor_output_buffer_copy_video_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
block_t* lls_sls_monitor_output_buffer_copy_video_moov_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

void lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor_t* lls_slt_monitor);
void lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number);

#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN(...)    printf("%s:%d:WARN :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO(...)    printf("%s:%d:INFO :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__);
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG(...)   if(_LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__); }
#define __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE(...)   if(_LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE :",__FILE__,__LINE__);__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_PRINTLN(__VA_ARGS__); }

#endif /* atsc3_lls_sls_monitor_output_buffer_h */
