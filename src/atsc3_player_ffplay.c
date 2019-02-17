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
	int ret = sem_post(pipe_ffplay_buffer->pipe_buffer_semaphore);
	__PLAYER_FFPLAY_DEBUG_READER("sem post: %d ", ret);
}

void pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_lock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_unlock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void* pipe_buffer_writer_thread(void* pipe_ffplay_buffer_pointer) {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer = (pipe_ffplay_buffer_t*) pipe_ffplay_buffer_pointer;

	while(1) {

await_semaphore:

		sem_wait(pipe_ffplay_buffer->pipe_buffer_semaphore);
		__PLAYER_FFPLAY_ERROR("--> AFTER PIPE_BUFFER_SEMAPHORE")
		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

		if(pipe_ffplay_buffer->pipe_buffer_reader_to_shutdown) {
			__PLAYER_FFPLAY_INFO("shutting down pipe_buffer_writer thread");
			pipe_ffplay_buffer->pipe_buffer_reader_is_shutdown = true;
			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
			return 0;
		}

		//wait until we have accumulated at least 5 fragments or we have BUFFER/2 available for writing
		if(pipe_ffplay_buffer->writer_unlock_count++ < __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT || (!pipe_ffplay_buffer->has_met_minimum_startup_buffer_threshold && pipe_ffplay_buffer->pipe_buffer_reader_pos < __PLAYER_INITIAL_BUFFER_TARGET)) {
			__PLAYER_FFPLAY_INFO("skipping signal, mutex count %d < %d, buffer reader pos: %u",  pipe_ffplay_buffer->writer_unlock_count, __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT, pipe_ffplay_buffer->pipe_buffer_reader_pos);
			goto unlock_from_error;
		}

		if(pipe_ffplay_buffer->pipe_buffer_reader_pos > __PLAYER_INITIAL_BUFFER_TARGET) {
			pipe_ffplay_buffer->has_met_minimum_startup_buffer_threshold = true;
		} else {
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

		if(pipe_ffplay_buffer->pipe_buffer_writer_pos < 512000) {
			__PLAYER_FFPLAY_WARN("WARNING - remaining buffer is less than ~512KB, this may cause player underflow! ffplay BEFORE WRITE, to write to pipe: %p, from %p, pos: %d to %d",
							pipe_ffplay_buffer->player_pipe,
							pipe_ffplay_buffer->pipe_buffer_writer,
							0,
							pipe_ffplay_buffer->pipe_buffer_writer_pos);
		} else {
			__PLAYER_FFPLAY_WARN("ffplay BEFORE WRITE, to write to pipe: %p, from %p, pos: %d to %d",
				pipe_ffplay_buffer->player_pipe,
				pipe_ffplay_buffer->pipe_buffer_writer,
				0,
				pipe_ffplay_buffer->pipe_buffer_writer_pos);
		}

		for(int i=0; i < pipe_ffplay_buffer->pipe_buffer_writer_pos; i+=__PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE) {
			int bytes_to_write_remaining = pipe_ffplay_buffer->pipe_buffer_writer_pos - i;
			int to_write_blocksize = bytes_to_write_remaining > __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE ? __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE : bytes_to_write_remaining;

			__PLAYER_FFPLAY_WARN("WRITING from %p, pos: %d, blocksize: %d, total size: %d", &pipe_ffplay_buffer->pipe_buffer_writer[i], i, to_write_blocksize, pipe_ffplay_buffer->pipe_buffer_writer_pos);
			int fwrite_ret = fwrite(&pipe_ffplay_buffer->pipe_buffer_writer[i], to_write_blocksize, 1, pipe_ffplay_buffer->player_pipe);
			if(fwrite_ret != 1) {
				__PLAYER_FFPLAY_WARN("short fwrite! at pos: %u, total buffer length: %u", i, pipe_ffplay_buffer->pipe_buffer_writer_pos);
				//TODO - handle ffmpeg shutdown cases here


//				atsc3_player_ffplay.c:94:WARN:1550422471.1440: short fwrite! at pos: 0, total buffer length: 614888
//				atsc3_player_ffplay.c:94:WARN:1550422471.1441: short fwrite! at pos: 131070, total buffer length: 614888
//				atsc3_player_ffplay.c:94:WARN:1550422471.1441: short fwrite! at pos: 262140, total buffer length: 614888
//				atsc3_player_ffplay.c:94:WARN:1550422471.1441: short fwrite! at pos: 393210, total buffer length: 614888
//				Process 15790 stopped
//				* thread #1, queue = 'com.apple.main-thread', stop reason = signal SIGPIPE
//				    frame #0: 0x00007fff5c80ad82 libsystem_kernel.dylib`__semwait_signal + 10
			}
			int fflush_ret = fflush(pipe_ffplay_buffer->player_pipe);
			if(fflush_ret != 0) {
				__PLAYER_FFPLAY_WARN("fflush returned: %d", fflush_ret);
			}
		}

		__PLAYER_FFPLAY_WARN("ffplay AFTER write, to write to pipe: %p complete",
				pipe_ffplay_buffer->player_pipe);

		pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;
		goto await_semaphore;


		//unlock if we aren't ready to swap and write
unlock_from_error:

		__PLAYER_FFPLAY_INFO("..player before mutex_unlock");
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
		__PLAYER_FFPLAY_INFO("..player before pthread_cond_wait");
		goto await_semaphore;
	}
}

pipe_ffplay_buffer_t* pipe_create_ffplay() {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer = calloc(1, sizeof(pipe_ffplay_buffer_t));

	if (pthread_mutex_init(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock, NULL) != 0) {
		__PLAYER_FFPLAY_ERROR("pipe_buffer_reader_mutex_lock init failed");
		abort();
	}

	pipe_ffplay_buffer->pipe_buffer_semaphore = sem_open("/atsc3_player_ffplay", O_CREAT, 0644, 0);
	__PLAYER_FFPLAY_ERROR("sem_init returned: %p", pipe_ffplay_buffer->pipe_buffer_semaphore);
	assert(pipe_ffplay_buffer->pipe_buffer_semaphore);

	pipe_ffplay_buffer->pipe_buffer_reader = calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_reader);
	pipe_ffplay_buffer->pipe_buffer_reader_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_reader_pos = 0;

	pipe_ffplay_buffer->pipe_buffer_writer = calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_writer);
	pipe_ffplay_buffer->pipe_buffer_writer_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;

	//-infbuf -max_delay 5000000 -loglevel debug
	//ffmpeg -i input -vf "drawtext=fontfile=Arial.ttf: text='%{frame_num}': start_number=1: x=(w-tw)/2: y=h-(2*lh): fontcolor=black: fontsize=20: box=1: boxcolor=white: boxborderw=5" -c:a copy output

	/* Failed to set value 'drawtext=fontfile=/System/Library/Fonts/Helveticza.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\:00\:00\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\:n/59.94*90000\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10' for option 'filter_complex': Option not found
	 *-framedrop -infbuf setpts=N/(59.94*TB), genpts
	 */
	char* cmd = "ffplay -infbuf -err_detect ignore_err -hide_banner -nostats -vf \"drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\\:00\\:00\\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\\:n/59.94*90000\\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10\"  - > ffplay.errors 2>&1";
	if ( !(pipe_ffplay_buffer->player_pipe = popen(cmd, "w")) ) {
		__PLAYER_FFPLAY_ERROR("unable to create pipe for cmd: %s", cmd);
		goto error;
	}
	/**
	 *
	 * char* cmd = "ffplay -infbuf -err_detect ignore_err -hide_banner -nostats -vf \"drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\\:00\\:00\\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\\:n/59.94*90000\\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10\"  - > ffplay.errors 2>&1";
	if ( !(pipe_ffplay_buffer->player_pipe = popen(cmd, "w")) ) {
		__PLAYER_FFPLAY_ERROR("unable to create pipe for cmd: %s", cmd);
		goto error;
	}
	 */

	//pipe_ffplay_buffer->player_pipe = fopen("mpu/recon.m4v", "w");


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


	__PLAYER_FFPLAY_TRACE_READER("pipe_push_block with reader buffer: %p, to_write %d, current buffer len: %d, total buffer size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_pos, pipe_ffplay_buffer->pipe_buffer_reader_size);
	//make sure we have enough space, otherwise realloc...
	if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > pipe_ffplay_buffer->pipe_buffer_reader_size) {
		__PLAYER_FFPLAY_WARN("pipe_push_block, calling realloc due to with reader buffer: %p, to_write %d, current size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_size);
		int new_size = pipe_ffplay_buffer->pipe_buffer_reader_size * 2;
		if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > new_size) {
			__PLAYER_FFPLAY_WARN("realloc of 2x isn't enough! reallocing exactly to: %d", pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size);
			new_size = pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size;
		}
		pipe_ffplay_buffer->pipe_buffer_reader = realloc(pipe_ffplay_buffer->pipe_buffer_reader, new_size);
		pipe_ffplay_buffer->pipe_buffer_reader_size = new_size;

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

