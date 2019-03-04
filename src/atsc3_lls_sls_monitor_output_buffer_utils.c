//
//  atsc3_lls_sls_monitor_output_buffer.c
//  cmd
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

//todo: use box_t also

#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED = 0;
int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED = 0;


void lls_sls_monitor_output_buffer_reset_moov_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos = 0;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos = 0;

    lls_sls_monitor_output_buffer->should_flush_output_buffer = false;
}

void lls_sls_monitor_output_buffer_reset_all_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos = 0;
	lls_sls_monitor_output_buffer_reset_moov_and_fragment_position(lls_sls_monitor_output_buffer);
}


int __lls_sls_monitor_output_buffer_check_and_copy(uint8_t* dest_box, uint32_t* dest_box_pos, uint32_t max_alloc_size, block_t* src) {

	uint32_t last_box_pos = *dest_box_pos;

	if(*dest_box_pos + src->i_pos > max_alloc_size) {
		//truncate and rewind if possible
		if(src->i_pos > max_alloc_size) {
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("box: %p, unable to copy from src size: %u, buffer size: %u", dest_box, src->i_pos, max_alloc_size)
			return -1;
		}
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("box: %p, truncating box from pos: %u to 0, src size: %u", dest_box, last_box_pos, src->i_pos);
		*dest_box_pos = 0;
	    memcpy(&dest_box[*dest_box_pos], src->p_buffer, src->i_pos);
	    *dest_box_pos += src->i_pos;

		return -1 * last_box_pos;

	} else {
	    memcpy(&dest_box[*dest_box_pos], src->p_buffer, src->i_pos);

	    *dest_box_pos += src->i_pos;
	    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE("box: %p, copied src size: %u to pos: %u", dest_box, src->i_pos, *dest_box_pos);
		return *dest_box_pos;
	}
}

int lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header) {

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, audio_isobmff_header);
}


int lls_sls_monitor_output_buffer_copy_audio_moov_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_moov) {

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOV_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOV_BUFFER, audio_isobmff_moov);
}


int lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_fragment) {
    
    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, audio_isobmff_fragment);
}


int lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, video_isobmff_header);
}


int lls_sls_monitor_output_buffer_copy_video_moov_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_moov) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOV_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOV_BUFFER, video_isobmff_moov);
}


int lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_fragment) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, video_isobmff_fragment);
}


block_t* lls_sls_monitor_output_buffer_copy_audio_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos || !lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio full box: error: init_box_pos: %u, fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos;
	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio full box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_full_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy audio full box: total size: %u, init size: %u, moov size: %u, fragment size: %u", full_box_size, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos);

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos) {
		block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos);

	}

	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	return isobmff_full_block;
}

block_t* lls_sls_monitor_output_buffer_copy_audio_moov_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio moov_fragment: error: fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t moov_fragment_size = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos;
	if(moov_fragment_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio moov_fragment size <= 0, %u, returning NULL", moov_fragment_size);
		return NULL;
	}
	block_t* isobmff_moov_fragment_block = block_Alloc(moov_fragment_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy audio moov_fragment: total size: %u,  moov size: %u, fragment size: %u", moov_fragment_size,  lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos) {
		block_Write(isobmff_moov_fragment_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moov_box_pos);
	}

	block_Write(isobmff_moov_fragment_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	return isobmff_moov_fragment_block;
}


block_t* lls_sls_monitor_output_buffer_copy_video_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video full box: error: init_box_pos: %u, fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos;
	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video full box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_full_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy video full box: total size: %u, init size: %u, moov size: %u, fragment size: %u", full_box_size, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos) {
		block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos);

	}

	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	return isobmff_full_block;
}

block_t* lls_sls_monitor_output_buffer_copy_video_moov_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video moov_fragment: error: fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t moov_fragment_size = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos;
	if(moov_fragment_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video moov_fragment size <= 0, %u, returning NULL", moov_fragment_size);
		return NULL;
	}
	block_t* isobmff_moov_fragment_block = block_Alloc(moov_fragment_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy video moov_fragment: total size: %u,  moov size: %u, fragment size: %u", moov_fragment_size,  lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos) {
		block_Write(isobmff_moov_fragment_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moov_box_pos);
	}

	block_Write(isobmff_moov_fragment_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	return isobmff_moov_fragment_block;
}

