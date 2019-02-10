/*
 * atsc3_player_ffplay.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include "atsc3_player_ffplay.h"

int _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
int _PLAYER_FFPLAY_TRACE_ENABLED = 0;

FILE* pipe_create_ffplay() {
	FILE *player_pipe = NULL;

	char* cmd = "ffplay -hide_banner -infbuf -max_delay 5000000 -";
	if ( !(player_pipe = popen(cmd, "w")) ) {
		__PLAYER_FFPLAY_ERROR("unable to create pipe for cmd: %s", cmd);
		return NULL;
	}
	__PLAYER_FFPLAY_DEBUG("pipe created: file* is %p", player_pipe);
	return player_pipe;
}


//
//void push_mfu_block(block_t* block) {
//
////	int output_size = fwrite(block->p_buffer, 1, block->i_buffer, ffplay_pipe);
//
////	printf("in: %d, wrote %d bytes", block->i_buffer, output_size);
//
//	int output_size = fwrite(block->p_buffer, block->i_buffer, 1, file_pipe);
//
//	if(!player_pipe) {
//		create_ffplay_pipe();
//	}
//}

