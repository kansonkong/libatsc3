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

#define _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER 65535
#define _LLS_SLS_MONITOR_OUTPUT_MAX_MOOV_BUFFER 65535
//4k gets pretty big pretty quick
#define _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER 16384000

typedef struct lls_sls_monitor_buffer_isobmff {
    uint8_t* init_box;
    uint32_t init_box_pos;

    //optional, route-dash contains moov + mdat pre-combined, so use the fragment below
    uint8_t* moov_box;
    uint32_t moov_box_pos;

    //always need a fragment, may be just data unit for track
    uint8_t* fragment_box;
    uint32_t fragment_pos;

} lls_sls_monitor_buffer_isobmff_t;

typedef struct lls_sls_monitor_buffer {
    bool has_written_init_box;
    bool should_flush_output_buffer;
    
    lls_sls_monitor_buffer_isobmff_t audio_output_buffer_isobmff;
    lls_sls_monitor_buffer_isobmff_t video_output_buffer_isobmff;

    block_t* joined_isobmff_block;

} lls_sls_monitor_output_buffer_t;


typedef struct lls_sls_monitor_buffer_mode {
    bool file_dump_enabled;
    bool ffplay_output_enabled;
    pipe_ffplay_buffer_t* pipe_ffplay_buffer;

} lls_sls_monitor_output_buffer_mode_t;


#endif /* atsc3_lls_sls_monitor_output_buffer_h */
