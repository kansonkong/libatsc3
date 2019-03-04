//
//  atsc3_lls_sls_monitor_output_buffer.h
//  libatsc3
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef atsc3_lls_sls_monitor_output_buffer_h
#define atsc3_lls_sls_monitor_output_buffer_h

#include <stdbool.h>
#include "atsc3_utils.h"
#include "atsc3_player_ffplay.h"

//4k gets pretty big pretty quick
#define _LLS_SLS_MONITOR_OUTPUT_MAX_RECON_BUFFER 16384000


typedef struct lls_sls_monitor_buffer {
    bool has_written_init_box;
    bool should_flush_output_buffer;
    
    uint8_t* audio_output_buffer;
    uint32_t audio_output_buffer_pos;
    uint8_t* video_output_buffer;
    uint32_t video_output_buffer_pos;
} lls_sls_monitor_output_buffer_t;


typedef struct lls_sls_monitor_buffer_mode {
    bool file_dump_enabled;
    bool ffplay_output_enabled;
    pipe_ffplay_buffer_t* pipe_ffplay_buffer;

} lls_sls_monitor_output_buffer_mode_t;


#endif /* atsc3_lls_sls_monitor_output_buffer_h */
