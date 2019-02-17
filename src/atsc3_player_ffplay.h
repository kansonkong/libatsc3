/*
 * atsc3_player_ffplay.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>

#include "atsc3_utils.h"

#ifndef ATSC3_PLAYER_FFPLAY_H_
#define ATSC3_PLAYER_FFPLAY_H_

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

#define __PLAYER_INITIAL_BUFFER_SEGMENT_COUNT 8  //wait for at least 8 signals
#define __PLAYER_INITIAL_BUFFER_TARGET 1024000  //and 1 MB of payload before starting to stream to ffplay
#define __PLAYER_FFPLAY_PIPE_INTERNAL_BUFFER_SIZE 8192000
#define __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE 131070

typedef struct pipe_ffplay_buffer {
	//used to signal pipe writer new data is present
	sem_t* pipe_buffer_semaphore;

	sig_atomic_t writer_unlock_count;

	//mutex for reader buffer
	pthread_mutex_t pipe_buffer_reader_mutex_lock;

	//processing thread for double buffer pipe writer
	pthread_t pipe_buffer_thread_id;
	bool pipe_buffer_reader_to_shutdown;
	bool pipe_buffer_reader_is_shutdown;

	bool has_written_init_box;
	//use double buffer for reading from fragments from alc_utils/mmt_mpu_utils
	uint32_t pipe_buffer_reader_pos;
	uint32_t pipe_buffer_reader_size;
	uint8_t* pipe_buffer_reader;

	//use double buffer for writing to ffplay
	uint32_t pipe_buffer_writer_pos;
	uint32_t pipe_buffer_writer_size;

	uint8_t* pipe_buffer_writer;

	bool has_met_minimum_startup_buffer_threshold;

	//player pipe for ffplay via popen
	FILE* player_pipe;

	//TODO - refactor out attributes for mmt?
	uint32_t last_mpu_sequence_number;


} pipe_ffplay_buffer_t;

pipe_ffplay_buffer_t* pipe_create_ffplay();

void pipe_buffer_condition_signal(pipe_ffplay_buffer_t* pipe_ffplay_buffer);
void pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer_t* pipe_ffplay_buffer);
void pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer_t* pipe_ffplay_buffer);
void pipe_buffer_push_block(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint8_t* block, uint32_t block_size);


#define __PLAYER_FFPLAY_ERROR(...)   printf("%s:%d:ERROR:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");
#define __PLAYER_FFPLAY_WARN(...)    printf("%s:%d:WARN:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");
#define __PLAYER_FFPLAY_INFO(...)    printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n");

#define __PLAYER_FFPLAY_DEBUG(...)  		if(_PLAYER_FFPLAY_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __PLAYER_FFPLAY_DEBUG_READER(...)   if(_PLAYER_FFPLAY_DEBUG_ENABLED) { printf("%s:%d:DEBUG_READER:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __PLAYER_FFPLAY_DEBUG_WRITER(...)   if(_PLAYER_FFPLAY_DEBUG_ENABLED) { printf("%s:%d:DEBUG_WRITER:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }

#define __PLAYER_FFPLAY_TRACE_READER(...)   if(_PLAYER_FFPLAY_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }

#define __PLAYER_FFPLAY_TRACE(...)   if(_PLAYER_FFPLAY_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __PLAYER_FFPLAY_TRACE_WRITER(...)  if(_PLAYER_FFPLAY_TRACE_ENABLED) { printf("%s:%d:TRACE_WRITER:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#endif /* ATSC3_PLAYER_FFPLAY_H_ */
