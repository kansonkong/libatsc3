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
#include "atsc3_mmtp_types.h"


#define _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER 65535
#define _LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER 65535
//4k gets pretty big pretty quick
#define _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER 16384000


typedef struct lls_sls_monitor_buffer_isobmff {
	uint32_t track_id;

	uint32_t fps_num;
	uint32_t fps_denom;

    uint8_t* init_box;
    uint32_t init_box_pos;

    //optional, route-dash contains moof + mdat pre-combined, so use the fragment below
    uint8_t* moof_box;
    uint32_t moof_box_pos;
    bool moof_box_is_from_last_mpu;

    //for rebuilding moof boxes if they are lost, we don't clear the buffer so we can recover from a lost fragment.
    uint32_t last_moof_box_pos;
    struct trun_sample_entry_vector* moof_box_trun_sample_entry_vector;


    //always need a fragment, may be just data unit for track
    uint8_t* fragment_box;
    uint32_t fragment_pos;

    //for fragment recovery
    mmtp_payload_fragments_union_t* last_fragment;
    //this won't reset unless we do _all
    mmtp_payload_fragments_union_t* last_mpu_sequence_number_last_fragment;

    uint32_t last_fragment_lost_mfu_count;

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
    struct pipe_ffplay_buffer* pipe_ffplay_buffer;

} lls_sls_monitor_output_buffer_mode_t;


#endif /* atsc3_lls_sls_monitor_output_buffer_h */
