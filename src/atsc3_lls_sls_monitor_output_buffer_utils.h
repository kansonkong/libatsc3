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

//4k gets pretty big pretty quick
#define _LLS_SLS_MONITOR_OUTPUT_MAX_RECON_BUFFER 16384000


void lls_sls_monitor_output_buffer_reset_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

void lls_sls_monitor_output_buffer_copy_video_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header);

void lls_sls_monitor_output_buffer_copy_audio_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header);

#endif /* atsc3_lls_sls_monitor_output_buffer_h */
