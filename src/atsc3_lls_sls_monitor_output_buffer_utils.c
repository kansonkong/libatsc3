//
//  atsc3_lls_sls_monitor_output_buffer.c
//  cmd
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//


#include "atsc3_lls_sls_monitor_output_buffer_utils.h"


void lls_sls_monitor_output_buffer_reset_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
    lls_sls_monitor_output_buffer->video_output_buffer_pos = 0;
    lls_sls_monitor_output_buffer->audio_output_buffer_pos = 0;
    lls_sls_monitor_output_buffer->should_flush_output_buffer = false;
}

void lls_sls_monitor_output_buffer_copy_video_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header) {
    
    if(!lls_sls_monitor_output_buffer->video_output_buffer) {
        lls_sls_monitor_output_buffer->video_output_buffer = (uint8_t*) calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_RECON_BUFFER, sizeof(uint8_t*));
    }
    
    memcpy(&lls_sls_monitor_output_buffer->video_output_buffer[lls_sls_monitor_output_buffer->video_output_buffer_pos], video_isobmff_header->p_buffer, video_isobmff_header->i_buffer);
    lls_sls_monitor_output_buffer->video_output_buffer_pos += video_isobmff_header->i_buffer;
}

void lls_sls_monitor_output_buffer_copy_audio_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header) {
    
    if(!lls_sls_monitor_output_buffer->audio_output_buffer) {
        lls_sls_monitor_output_buffer->audio_output_buffer = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_RECON_BUFFER, sizeof(uint8_t*));
    }
    memcpy(&lls_sls_monitor_output_buffer->audio_output_buffer[lls_sls_monitor_output_buffer->audio_output_buffer_pos], audio_isobmff_header->p_buffer, audio_isobmff_header->i_buffer);
    lls_sls_monitor_output_buffer->audio_output_buffer_pos += audio_isobmff_header->i_buffer;
}
