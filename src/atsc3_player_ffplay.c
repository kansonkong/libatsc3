/*
 * atsc3_player_ffplay.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include "atsc3_player_ffplay.h"

int _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
int _PLAYER_FFPLAY_TRACE_ENABLED = 0;

void pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	int ret = sem_post(pipe_ffplay_buffer->pipe_buffer_semaphore);
	__PLAYER_FFPLAY_DEBUG_READER("sem post: %d ", ret);
}

void pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_lock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer_t* pipe_ffplay_buffer) {
	pthread_mutex_unlock(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock);
}

void* pipe_buffer_writer_thread(void* pipe_ffplay_buffer_p) {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer = (pipe_ffplay_buffer_t*)pipe_ffplay_buffer_p;

	while(1) {


await_semaphore:

		sem_wait(pipe_ffplay_buffer->pipe_buffer_semaphore);
		__PLAYER_FFPLAY_DEBUG("PlayerOutputThread, sem wait posted");

		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

		if(pipe_ffplay_buffer->pipe_buffer_reader_to_shutdown) {
			__PLAYER_FFPLAY_INFO("shutting down pipe_buffer_writer thread");
			goto thread_shutdown;
		}

		//wait until we have accumulated at least 5 fragments or we have BUFFER/2 available for writing
		if(pipe_ffplay_buffer->writer_unlock_count++ < __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT || (!pipe_ffplay_buffer->has_met_minimum_startup_buffer_threshold && pipe_ffplay_buffer->pipe_buffer_reader_pos < __PLAYER_INITIAL_BUFFER_TARGET)) {
			__PLAYER_FFPLAY_INFO("skipping signal, mutex count %d < %d, buffer reader pos: %u",  pipe_ffplay_buffer->writer_unlock_count, __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT, pipe_ffplay_buffer->pipe_buffer_reader_pos);
			//ick
			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

			goto await_semaphore;
		}

		if(!pipe_ffplay_buffer->has_met_minimum_startup_buffer_threshold && pipe_ffplay_buffer->pipe_buffer_reader_pos < __PLAYER_INITIAL_BUFFER_TARGET) {
			__PLAYER_FFPLAY_INFO("!has_met_minimum_startup_buffer_threshold, buffer level: %d < buffer target: %d", pipe_ffplay_buffer->pipe_buffer_reader_pos,__PLAYER_INITIAL_BUFFER_TARGET );
			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

			goto await_semaphore;
		} else {
			pipe_ffplay_buffer->has_met_minimum_startup_buffer_threshold = true;
			__PLAYER_FFPLAY_INFO("has_met_minimum_startup_buffer_threshold, buffer level: %d > buffer target: %d", pipe_ffplay_buffer->pipe_buffer_reader_pos,__PLAYER_INITIAL_BUFFER_TARGET );
		}

		if(pipe_ffplay_buffer->pipe_buffer_reader_pos < 512) {
			__PLAYER_FFPLAY_WARN("short read! skipping signal as pipe_buffer_reader_pos is %u", pipe_ffplay_buffer->pipe_buffer_reader_pos);
			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

			pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

			goto await_semaphore;
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

			__PLAYER_FFPLAY_DEBUG("WRITING from %p, pos: %d, blocksize: %d, total size: %d", &pipe_ffplay_buffer->pipe_buffer_writer[i], i, to_write_blocksize, pipe_ffplay_buffer->pipe_buffer_writer_pos);

			//we may get a sigpipe from this fwrite or flush call, unwind our thread as that usually means ffplay has exited...

			int fwrite_ret = fwrite(&pipe_ffplay_buffer->pipe_buffer_writer[i], to_write_blocksize, 1, pipe_ffplay_buffer->player_pipe);
			pipe_ffplay_buffer->pipe_write_counts++;

			if(pipe_ffplay_buffer->pipe_write_counts < 500) {
				usleep(10000); 				//TODO - throttle up our buffer as we catch up to live
			}


			//failure example where ffplay is shutdown:
			//we may get a sigpipe from th

			if(fwrite_ret != 1) {
				__PLAYER_FFPLAY_WARN("short fwrite! at pos: %u, total buffer length: %u", i, pipe_ffplay_buffer->pipe_buffer_writer_pos);
				goto thread_shutdown;
			}

			int fflush_ret = fflush(pipe_ffplay_buffer->player_pipe);
			if(fflush_ret != 0) {
				__PLAYER_FFPLAY_WARN("fflush returned: %d", fflush_ret);
				goto thread_shutdown;
			}
		}

		__PLAYER_FFPLAY_WARN("ffplay AFTER write, to write to pipe: %p complete", pipe_ffplay_buffer->player_pipe);

		pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;
		goto await_semaphore;


		//unlock if we aren't ready to swap and write
unlock_from_error:

		__PLAYER_FFPLAY_INFO("unlock_from_error, player before mutex_unlock");
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
		__PLAYER_FFPLAY_INFO("unlock_from_error, player before await_semaphore");
		goto await_semaphore;
	}

thread_shutdown:

	__PLAYER_FFPLAY_ERROR("exiting pipe_buffer_writer_thread, setting is_shutdown = true");

	pipe_ffplay_buffer->pipe_buffer_reader_is_shutdown = true;
	//don't free any resources here, just keep a reference that we're actually shut down and the writer thread will free
	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
	return 0;
}

pipe_ffplay_buffer_t** __last_pipe_ffplayer_buffer_t_p = NULL;
static sigset_t player_signal_mask;
static pthread_t player_signal_thread_id;

//wire up our sigpipe dispatcher to tell the active thread(s) to turn down
void *player_signal_thread_handler (void *arg) {
    int       sig_caught;
    int       res;

	while(1) {
		res = sigwait (&player_signal_mask, &sig_caught);
		if (res) {
			__PLAYER_FFPLAY_ERROR("Unable to poll for signal thread, res is: %d", res);
			return NULL;
		}

		switch (sig_caught)	{
			case SIGPIPE:     /* process SIGINT  */
				if(*__last_pipe_ffplayer_buffer_t_p) {
					__PLAYER_FFPLAY_WARN("Got SIGPIPE, setting pipe_buffer_reader_to_shutdown on %p:", *__last_pipe_ffplayer_buffer_t_p);
					(*__last_pipe_ffplayer_buffer_t_p)->pipe_buffer_reader_to_shutdown = true;
					//trigger a semaphore so we can unwind the thread early
					pipe_buffer_notify_semaphore_post(*__last_pipe_ffplayer_buffer_t_p);

				} else {
					__PLAYER_FFPLAY_ERROR("Got SIGPIPE, __last_pipe_ffplayer_buffer_t_p is NULL!");
				}
				break;

			default:         /* should normally not happen */
				__PLAYER_FFPLAY_WARN("\nUnexpected signal %d\n", sig_caught);
				break;
		}
	}
}

bool __has_applied_sigpipe_sigmask = false;

void sigpipe_register_action_handler(pipe_ffplay_buffer_t**  pipe_ffplay_buffer_p) {

	if(!__has_applied_sigpipe_sigmask) {
		sigemptyset (&player_signal_mask);
		sigaddset (&player_signal_mask, SIGPIPE);
		int rc = pthread_sigmask (SIG_BLOCK, &player_signal_mask, NULL);
		if (rc != 0) {
		  __PLAYER_FFPLAY_WARN("Unable to block SIGPIPE, this may result in a runtime crash when closing ffplay!");
		} else {
			__has_applied_sigpipe_sigmask = true;
			rc = pthread_create (&player_signal_thread_id, NULL, player_signal_thread_handler, NULL);
		}
	}

	//keep track of the pointer to our last pipe play, so we can NULL it out when the pthread is completed.
	__last_pipe_ffplayer_buffer_t_p = pipe_ffplay_buffer_p;
}

/**
 * create our output pipe for pushing mpu's/fragments to ffmpeg
 *
 *     //TODO: determine proper fps from trun box
 *
 */
pipe_ffplay_buffer_t* pipe_create_ffplay() {

	pipe_ffplay_buffer_t* pipe_ffplay_buffer = (pipe_ffplay_buffer_t*)calloc(1, sizeof(pipe_ffplay_buffer_t));

	if (pthread_mutex_init(&pipe_ffplay_buffer->pipe_buffer_reader_mutex_lock, NULL) != 0) {
		__PLAYER_FFPLAY_ERROR("pipe_buffer_reader_mutex_lock init failed");
		abort();
	}

	//since ffplay can be killed externally, wire up sigpipe handler while we are pushing data so we don't crash

	sigpipe_register_action_handler(&pipe_ffplay_buffer);

	pipe_ffplay_buffer->pipe_buffer_semaphore = sem_open("/atsc3_player_ffplay", O_CREAT, 0644, 0);
	__PLAYER_FFPLAY_ERROR("sem_init returned: %p", pipe_ffplay_buffer->pipe_buffer_semaphore);
	assert(pipe_ffplay_buffer->pipe_buffer_semaphore);

	pipe_ffplay_buffer->pipe_buffer_reader = (uint8_t*)calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_reader);
	pipe_ffplay_buffer->pipe_buffer_reader_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_reader_pos = 0;

	pipe_ffplay_buffer->pipe_buffer_writer = (uint8_t*)calloc(__PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE, sizeof(uint8_t));
	assert(pipe_ffplay_buffer->pipe_buffer_writer);
	pipe_ffplay_buffer->pipe_buffer_writer_size = __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE;
	pipe_ffplay_buffer->pipe_buffer_writer_pos = 0;

	//char* tmpAudioFifo = tmpnam(NULL);

	//make a 2nd fifo for audio
//    int res = mkfifo(tmpAudioFifo, S_IRUSR| S_IWUSR);
//    if(!res) {
//        __PLAYER_FFPLAY_ERROR("unable to prepare pipe for audio playback at: %s", tmpAudioFifo);
//        goto error;
//    }


	//flags=bicubic
	//-infbuf -max_delay 5000000 -loglevel debug
	//ffmpeg -i input -vf "drawtext=fontfile=Arial.ttf: text='%{frame_num}': start_number=1: x=(w-tw)/2: y=h-(2*lh): fontcolor=black: fontsize=20: box=1: boxcolor=white: boxborderw=5" -c:a copy output

	/* Failed to set value 'drawtext=fontfile=/System/Library/Fonts/Helveticza.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\:00\:00\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\:n/59.94*90000\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10' for option 'filter_complex': Option not found
	 *-framedrop -infbuf setpts=N/(59.94*TB), genpts -fs
	 */
	//setpts=N/(25*TB)
//	char* cmd = "ffplay -loglevel debug -infbuf -err_detect ignore_err -hide_banner -nostats -noborder -left 0 -top 0  -vf \"fps=25,drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\\:00\\:00\\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\\:n/25*90000\\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10, scale=iw*.5:ih*.5:flags=bicubic\"  - > ffplay.errors 2>&1";

	char* cmd = "ffplay -loglevel debug -infbuf -err_detect ignore_err -hide_banner -nostats -noborder -left 0 -top 0  -vf \"drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: timecode_rate=59.94: timecode='00\\:00\\:00\\:00': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=550:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pts}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-50, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{eif\\:n/25*90000\\:d}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=500-tw:y=h-th-150, drawtext=fontfile=/System/Library/Fonts/Helvetica.ttc: fix_bounds=1: shadowx=2: shadowy=2: text='%{pict_type}': fontcolor=white: fontsize=96: box=1: boxcolor=black@0.4: x=10-tw:y=th+10, scale=iw*.5:ih*.5:flags=bicubic\"  - > ffplay.errors 2>&1";
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

	//use this for capturing the reconstitued mpu for troubleshooting
//	pipe_ffplay_buffer->player_pipe = fopen("mpu/recon.m4v", "w");


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

/**TODO: add a shutdown hook so we can switch between flows for observation ***/


void pipe_release_ffplay(pipe_ffplay_buffer_t** pipe_ffplay_buffer_p) {
	pipe_ffplay_buffer_t* pipe_ffplay_buffer_t = *pipe_ffplay_buffer_p;

	if(pipe_ffplay_buffer_t->player_pipe) {
		fclose(pipe_ffplay_buffer_t->player_pipe);
		pipe_ffplay_buffer_t->player_pipe = NULL;
	}
	if(pipe_ffplay_buffer_t->pipe_buffer_semaphore) {
		sem_close(pipe_ffplay_buffer_t->pipe_buffer_semaphore);
		pipe_ffplay_buffer_t->pipe_buffer_semaphore = NULL;
	}

	if(pipe_ffplay_buffer_t->pipe_buffer_reader) {
		free(pipe_ffplay_buffer_t->pipe_buffer_reader);
		pipe_ffplay_buffer_t->pipe_buffer_reader = NULL;
	}

	if(pipe_ffplay_buffer_t->pipe_buffer_writer) {
		free(pipe_ffplay_buffer_t->pipe_buffer_writer);
		pipe_ffplay_buffer_t->pipe_buffer_writer = NULL;
	}
}

bool pipe_buffer_reader_check_if_shutdown(pipe_ffplay_buffer_t** pipe_ffplay_buffer_p) {
	if((*pipe_ffplay_buffer_p)->pipe_buffer_reader_is_shutdown) {
		pipe_release_ffplay(pipe_ffplay_buffer_p);
		pipe_ffplay_buffer_p = NULL;
		return true;
	}
	return false;
}

/*
 * note: you must lock externally, as we don't want to lock mid-fragment
 * 	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
 *
 * 	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
 *
 * return codes: 0: success
 * 				-1: reader thread has shutdown
 */

int pipe_buffer_unsafe_push_block(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint8_t* block, uint32_t block_size)  {

	//first, check to make sure that our thread isn't shut down, dont process this payload, and free if we are
	if(pipe_ffplay_buffer->pipe_buffer_reader_to_shutdown || pipe_ffplay_buffer->pipe_buffer_reader_is_shutdown) {
		return -1;
	}

	__PLAYER_FFPLAY_TRACE_READER("pipe_push_block with reader buffer: %p, to_write %d, current buffer len: %d, total buffer size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_pos, pipe_ffplay_buffer->pipe_buffer_reader_size);
	//make sure we have enough space, otherwise realloc...
	if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > pipe_ffplay_buffer->pipe_buffer_reader_size) {
		__PLAYER_FFPLAY_WARN("pipe_push_block, calling realloc due to with reader buffer: %p, to_write %d, current size: %d", pipe_ffplay_buffer->pipe_buffer_reader, block_size, pipe_ffplay_buffer->pipe_buffer_reader_size);
		int new_size = pipe_ffplay_buffer->pipe_buffer_reader_size * 2;
		if(pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size > new_size) {
			__PLAYER_FFPLAY_WARN("realloc of 2x isn't enough! reallocing exactly to: %d", pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size);
			new_size = pipe_ffplay_buffer->pipe_buffer_reader_pos + block_size;
		}
		pipe_ffplay_buffer->pipe_buffer_reader = (uint8_t*)realloc(pipe_ffplay_buffer->pipe_buffer_reader, new_size);
		pipe_ffplay_buffer->pipe_buffer_reader_size = new_size;

		if(!pipe_ffplay_buffer->pipe_buffer_reader) {
			__PLAYER_FFPLAY_WARN("realloc failed! exiting");
			exit(1);
		}
	}

	memcpy(&pipe_ffplay_buffer->pipe_buffer_reader[pipe_ffplay_buffer->pipe_buffer_reader_pos], block, block_size);
	pipe_ffplay_buffer->pipe_buffer_reader_pos += block_size;

	return 0;
}

