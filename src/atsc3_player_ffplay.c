/*
 * atsc3_player_ffplay.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include "atsc3_player_ffplay.h"

int _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
int _PLAYER_FFPLAY_TRACE_ENABLED = 0;

void pipe_buffer_condition_signal(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	int ret = pthread_cond_signal(&pipe_ffplay_buffer->pipe_buffer_unlocked_cond_signal);
	__PLAYER_FFPLAY_DEBUG("signal condition returned: %d ", ret);
}

void pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_lock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_unlock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void* pipe_buffer_writer_thread(void* pipe_ffplay_buffer_pointer) {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer = (pipe_ffplay_buffer_t*) pipe_ffplay_buffer_pointer;
	int condition_wait_ret = 0;

	while(1) {

	wait_signal:

		//wait on a signal condition

		condition_wait_ret = pthread_cond_wait(&pipe_ffplay_buffer->pipe_buffer_unlocked_cond_signal, &pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
		__PLAYER_FFPLAY_INFO("mutex count %d, buffer reader pos: %u, condition_wait return: %d",  pipe_ffplay_buffer->reader_unlock_count, pipe_ffplay_buffer->pipe_buffer_reader_pos, condition_wait_ret);


		if(pipe_ffplay_buffer->pipe_buffer_reader_to_shutdown) {
			__PLAYER_FFPLAY_INFO("shutting down pipe_buffer_writer thread");
			pipe_ffplay_buffer->pipe_buffer_reader_is_shutdown = true;
			return 0;
		}

		//wait until we have accumulated at least 5 fragments or we have BUFFER/2 available for writing
		if(pipe_ffplay_buffer->reader_unlock_count++ < __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT && (pipe_ffplay_buffer->pipe_buffer_reader_pos < (__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE/2))) {
			__PLAYER_FFPLAY_INFO("skipping signal, mutex count %d < 5, buffer reader pos: %u",  pipe_ffplay_buffer->reader_unlock_count, pipe_ffplay_buffer->pipe_buffer_reader_pos);
			goto unlock_from_error;
		}

		if(pipe_ffplay_buffer->pipe_buffer_reader_pos < 512) {
			__PLAYER_FFPLAY_WARN("short read! skipping signal as pipe_buffer_reader_pos is %u", pipe_ffplay_buffer->pipe_buffer_reader_pos);
			goto unlock_from_error;
		}

		if(pipe_ffplay_buffer->pipe_buffer_writer_pos > 0) {
			__PLAYER_FFPLAY_WARN("short write left in buffer! pipe_buffer_writer_pos is greater than 0, is: %u", pipe_ffplay_buffer->pipe_buffer_writer_pos);
		}

		//swap pointers
		uint8_t* pipe_buffer_swap_to_write = pipe_ffplay_buffer->pipe_buffer_reader;
		uint32_t pipe_buffer_swap_to_write_len = pipe_ffplay_buffer->pipe_buffer_reader_pos;

		//always assume that we will flush out the writer buffer when we are done
		pipe_ffplay_buffer->pipe_buffer_reader = pipe_ffplay_buffer->pipe_buffer_writer;
		pipe_ffplay_buffer->pipe_buffer_reader_pos = 0;

		pipe_ffplay_buffer->pipe_buffer_writer = pipe_buffer_swap_to_write;
		pipe_ffplay_buffer->pipe_buffer_writer_pos = pipe_buffer_swap_to_write_len;

		//unlock now that double buffer is complete
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

		__PLAYER_FFPLAY_INFO("ffplay BEFORE WRITE, to write to pipe: %p, from %p, pos: %d to %d, total size: %d",
				pipe_ffplay_buffer->player_pipe,
				pipe_ffplay_buffer->pipe_buffer_writer,
				0,
				__PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE,
				pipe_ffplay_buffer->pipe_buffer_writer_pos);


		for(int i=0; i < pipe_ffplay_buffer->pipe_buffer_writer_pos; i+=__PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE) {
			int bytes_to_write_remaining = pipe_ffplay_buffer->pipe_buffer_writer_pos - i;
			int to_write_blocksize = bytes_to_write_remaining > __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE ? __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE : bytes_to_write_remaining;

			__PLAYER_FFPLAY_INFO("writing from %p, pos: %d, blocksize: %d, total size: %d", &pipe_ffplay_buffer->pipe_buffer_writer[i], i, to_write_blocksize, pipe_ffplay_buffer->pipe_buffer_writer_pos);
			int fwrite_ret = fwrite(&pipe_ffplay_buffer->pipe_buffer_writer[i], to_write_blocksize, 1, pipe_ffplay_buffer->player_pipe);
			if(fwrite_ret != 1) {
				__PLAYER_FFPLAY_WARN("short fwrite! at pos: %u, total buffer length: %u", i, pipe_ffplay_buffer->pipe_buffer_writer_pos);
			}
			int fflush_ret = fflush(pipe_ffplay_buffer->player_pipe);
			if(fflush_ret != 0) {
				__PLAYER_FFPLAY_WARN("fflush returned: %d", fflush_ret);
			}
		}


		__PLAYER_FFPLAY_INFO("ffplay AFTER write, to write to pipe: %p, from %p, pos: %d to %d, total size: %d",
				pipe_ffplay_buffer->player_pipe,
				pipe_ffplay_buffer->pipe_buffer_writer,
				0,
				__PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE,
				pipe_ffplay_buffer->pipe_buffer_writer_pos);


		pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;

		goto wait_signal;

	//unlock if we aren't ready to swap and write
	unlock_from_error:
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
		goto wait_signal;
	}
}

pipe_ffplay_buffer_t* pipe_create_ffplay() {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer = calloc(1, sizeof(pipe_ffplay_buffer_t));

	if (pthread_mutex_init(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock, NULL) != 0) {
		__PLAYER_FFPLAY_ERROR("pipe_buffer_reader_mutex_lock init failed");
		abort();
	}

	pthread_cond_init(&pipe_ffplay_buffer->pipe_buffer_unlocked_cond_signal, NULL);

	pipe_ffplay_buffer->pipe_buffer_reader = calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_reader);
	pipe_ffplay_buffer->pipe_buffer_reader_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_reader_pos = 0;

	pipe_ffplay_buffer->pipe_buffer_writer = calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_writer);
	pipe_ffplay_buffer->pipe_buffer_writer_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;

	//-infbuf -max_delay 5000000 -loglevel debug
	char* cmd = "ffplay -hide_banner -";
	if ( !(pipe_ffplay_buffer->player_pipe = popen(cmd, "w")) ) {
		__PLAYER_FFPLAY_ERROR("unable to create pipe for cmd: %s", cmd);
		goto error;
	}
	__PLAYER_FFPLAY_DEBUG("pipe created: file* is %p", pipe_ffplay_buffer->player_pipe);

	pthread_create(&pipe_ffplay_buffer->pipe_buffer_thread_id, NULL, pipe_buffer_writer_thread, (void*)pipe_ffplay_buffer);


	return pipe_ffplay_buffer;

error:
	if(pipe_ffplay_buffer->pipe_buffer_reader) {
		free(pipe_ffplay_buffer->pipe_buffer_reader);
		pipe_ffplay_buffer->pipe_buffer_reader = NULL;
	}
	if(pipe_ffplay_buffer->pipe_buffer_writer) {
		free(pipe_ffplay_buffer->pipe_buffer_writer);
		pipe_ffplay_buffer->pipe_buffer_writer = NULL;
	}

	return NULL;
}

/*
 * note: you must lock externally, as we don't want to lock mid-fragment
 * 	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
 *
 * 	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
 *
 */
void pipe_buffer_push_block(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint8_t* block, uint32_t block_size)  {

	__PLAYER_FFPLAY_DEBUG("pipe_push_block with reader buffer: %p, to_write %d, current buffer len: %d, total buffer size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_pos, pipe_ffplay_buffer->pipe_buffer_reader_size);
	//make sure we have enough space, otherwise realloc...
	if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > pipe_ffplay_buffer->pipe_buffer_reader_size) {
		__PLAYER_FFPLAY_WARN("pipe_push_block, calling realloc due to with reader buffer: %p, to_write %d, current size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_size);
		int new_size = pipe_ffplay_buffer->pipe_buffer_reader_size * 2;
		if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > new_size) {
			__PLAYER_FFPLAY_WARN("realloc of 2x isn't enough! reallocing exactly to: %d", pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size);
			new_size = pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size;
		}
		pipe_ffplay_buffer->pipe_buffer_reader = realloc(pipe_ffplay_buffer->pipe_buffer_reader, new_size);
		if(!pipe_ffplay_buffer->pipe_buffer_reader) {
			__PLAYER_FFPLAY_WARN("realloc failed! exiting");
			exit(1);
		}
	}

	memcpy(&pipe_ffplay_buffer->pipe_buffer_reader[pipe_ffplay_buffer->pipe_buffer_reader_pos], block, block_size);
	pipe_ffplay_buffer->pipe_buffer_reader_pos += block_size;
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

