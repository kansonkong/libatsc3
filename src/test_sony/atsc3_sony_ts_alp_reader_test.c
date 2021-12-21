/*
 * atsc3_sony_ts_alp_reader_test.c
 *
 *  Created on: Dec 20, 2021
 *      Author: jjustman
 
 
 Samples:
	 -rw-------@ 1 jjustman  staff  5242944 Dec 20 21:40 DVB-gps-and-video.ts
	 -rw-------@ 1 jjustman  staff   240452 Dec 20 21:40 DVB.ts
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>


#include "../atsc3_utils.h"
#include "../atsc3_pcap_type.h"

#define _ATSC3_SONY_TS_ALP_READER_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


#define ATSC3_SONY_TS_ALP_FRAME_LENGTH 				188

//flip when we mask as uint32_t
//0x47402D00
#define ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG    0x002D4047
//0x47402D00
#define ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG 0x002D0047
#define ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK          0x00FF7FFF
#define ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN		4


typedef struct atsc3_sony_ts_alp_packet_instance {
	block_t*						current_ts_packet; //do NOT memset(0) this block...
	
	block_t*						pending_alp_packet;
	
} atsc3_sony_ts_alp_packet_instance_t;

typedef struct atsc3_sony_ts_alp_replay_context {
	char* 								 ts_file_name;

	FILE* 								 ts_fp;
	uint32_t 							 ts_fd_start;

	uint32_t							 ts_file_len;
	uint32_t							 ts_file_pos;

	uint32_t							 ts_read_packet_count;
	
	uint8_t		 					     current_ts_header_bytes[4]; //allocate this as a word for now...

	atsc3_sony_ts_alp_packet_instance_t  atsc3_sony_ts_alp_packet_instance;

//	atsc3_pcap_packet_instance_t	atsc3_pcap_packet_instance;
//
//	struct timeval 					first_wallclock_timeval;
//	struct timeval 					first_packet_ts_timeval;
//
//	struct timeval 					last_wallclock_timeval;
//	struct timeval 					current_wallclock_timeval;
//
//	uint32_t						delay_delta_behind_rt_replay;
//
//	uint32_t						last_packet_ts_sec;
//	uint32_t						last_packet_ts_usec;
//	uint32_t						current_packet_ts_sec;
//	uint32_t						current_packet_ts_usec;

} atsc3_sony_ts_alp_replay_context_t;

atsc3_sony_ts_alp_replay_context_t* atsc3_sony_ts_alp_replay_context_new() {
	atsc3_sony_ts_alp_replay_context_t* atsc3_sony_ts_alp_replay_context = calloc(1, sizeof(atsc3_sony_ts_alp_replay_context_t));
	atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.current_ts_packet  = block_Alloc(ATSC3_SONY_TS_ALP_FRAME_LENGTH);
	block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.current_ts_packet, 0);

	atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet = block_Alloc(MAX_ATSC3_PHY_IP_DATAGRAM_SIZE);
	block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, 0);
	return atsc3_sony_ts_alp_replay_context;
}

int main(int argc, char* argv[] ) {

    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/DVB-gps-and-video.ts";
	
	atsc3_sony_ts_alp_replay_context_t* atsc3_sony_ts_alp_replay_context = atsc3_sony_ts_alp_replay_context_new();
	//hack-ish for now
	atsc3_sony_ts_alp_replay_context->ts_file_name = strdup(SONY_TS_ALP_READER_TEST_FILENAME);
	
	struct stat st;
	stat(atsc3_sony_ts_alp_replay_context->ts_file_name, &st);
	if(!st.st_size) {
		_ATSC3_SONY_TS_ALP_READER_TEST_WARN("atsc3_sony_ts_alp_reader_test: %s, ERROR: st.st_size is 0!", atsc3_sony_ts_alp_replay_context->ts_file_name);
		return -1;
	}
				
										
	atsc3_sony_ts_alp_replay_context->ts_file_len = st.st_size;
	atsc3_sony_ts_alp_replay_context->ts_file_pos = 0;
											
	atsc3_sony_ts_alp_replay_context->ts_fp = fopen(atsc3_sony_ts_alp_replay_context->ts_file_name, "r");
	if(!atsc3_sony_ts_alp_replay_context->ts_fp) {
		_ATSC3_PCAP_TYPE_WARN("atsc3_sony_ts_alp_reader_test: %s -  fopen() for read returned NULL!", atsc3_sony_ts_alp_replay_context->ts_file_name);
		return -1;
	}
											
	_ATSC3_SONY_TS_ALP_READER_TEST_DEBUG("Opening ts: %s, length: %d, context is: %p", atsc3_sony_ts_alp_replay_context->ts_file_name, atsc3_sony_ts_alp_replay_context->ts_file_len, atsc3_sony_ts_alp_replay_context);

	char temp_alp_packet_buffer[185] = { 0 };
	int  temp_alp_packet_buffer_read_len = 0;
	
	while(atsc3_sony_ts_alp_replay_context->ts_file_pos  < (atsc3_sony_ts_alp_replay_context->ts_file_len - ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN)) {
			
		bool	ts_payload_is_sync = false;
		
		uint8_t ts_payload_alp_start   = 0;
		bool 	ts_payload_is_sync_start_flag = false;

		bool 	ts_payload_is_continuation = false;
		
		//peek to start by reading first 3 sync bytes
		fread((void*)&atsc3_sony_ts_alp_replay_context->current_ts_header_bytes, ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN, 1, atsc3_sony_ts_alp_replay_context->ts_fp);
		
		_ATSC3_SONY_TS_ALP_READER_TEST_TRACE("parsing loop: reading as ts_file_pos: %d, 4 bytes are: 0x%02x, 0x%02x, 0x%02x, 0x%02x, ts_file_len: %d, 0x%08x, 0x%08x, 0x%08x",
											 atsc3_sony_ts_alp_replay_context->ts_file_pos,
											 atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[0],
											 atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[1],
											 atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[2],
											 atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[3],
											 atsc3_sony_ts_alp_replay_context->ts_file_len,
											 (*(uint32_t*)atsc3_sony_ts_alp_replay_context->current_ts_header_bytes & ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK),
											 ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG,
											 ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG
											 );

		//try our start flag first - ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG
		if((*(uint32_t*)atsc3_sony_ts_alp_replay_context->current_ts_header_bytes & ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK) == ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG) {
			_ATSC3_SONY_TS_ALP_READER_TEST_TRACE("ts_payload_is_sync_start_flag at s_file_pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);
			ts_payload_is_sync = true;
			ts_payload_is_sync_start_flag = true;
			ts_payload_alp_start = atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[3];
			atsc3_sony_ts_alp_replay_context->ts_file_pos += 4;
			
			temp_alp_packet_buffer_read_len = 184;
			
		} else if((*(uint32_t*)atsc3_sony_ts_alp_replay_context->current_ts_header_bytes & ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK) == ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG) {
			_ATSC3_SONY_TS_ALP_READER_TEST_TRACE("ts_payload_is_continuation at s_file_pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);
			ts_payload_is_sync = true;
			ts_payload_is_continuation = true;
			fseek(atsc3_sony_ts_alp_replay_context->ts_fp, -1 , SEEK_CUR);
			atsc3_sony_ts_alp_replay_context->ts_file_pos += 3;
			 
			temp_alp_packet_buffer_read_len = 185;
			
		} else {
			//walk thru +1
			fseek(atsc3_sony_ts_alp_replay_context->ts_fp, 1 - ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN, SEEK_CUR);
			atsc3_sony_ts_alp_replay_context->ts_file_pos++;
			
			continue;
		}
		
		fread((void*)&temp_alp_packet_buffer, temp_alp_packet_buffer_read_len, 1, atsc3_sony_ts_alp_replay_context->ts_fp);

		if(ts_payload_is_sync_start_flag) {
			_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Reading sync frame at pos: %d, ts_payload_alp_start: %d, pending_alp_packet len is: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos, ts_payload_alp_start, atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);

			//parse thru our padding data if needed..
			if(ts_payload_alp_start) {
				int ts_remaining_bytes = temp_alp_packet_buffer_read_len - ts_payload_alp_start;

				if(!atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size) {
					_ATSC3_SONY_TS_ALP_READER_TEST_WARN("ts_payload_is_sync_start_flag: pending alp packet size is 0! sync frame at pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);
				} else {
					_ATSC3_SONY_TS_ALP_READER_TEST_WARN("ts_payload_is_sync_start_flag: pending alp packet size is 0! sync frame at pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);


					block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, ts_payload_alp_start);
					//close this packet out and parse out ALP
					_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Closing out pending alp packet, length: %d", atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);
				}
				
				block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, 0);
				_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Reading inner TS start packet at pos: %d, remaining TS len: %d",
													atsc3_sony_ts_alp_replay_context->ts_file_pos,
													ts_remaining_bytes);
				
				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer + ts_payload_alp_start, ts_remaining_bytes);
			} else {
				//start new at 0?
				_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Starting new alp packet at offset, TODO: scan if we are 0x80 or 0x08 for padding: %d", atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);
				block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, 0);
				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, temp_alp_packet_buffer_read_len);
			}
				
		} else if(ts_payload_is_continuation) {
			//read all 188-3 = 185 bytes into here
			
			if(!atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size) {
				_ATSC3_SONY_TS_ALP_READER_TEST_WARN("ts_payload_is_continuation: pending alp packet size is 0! sync frame at pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);
			} else {
				_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_continuation: Reading inner TS packet at pos: %d, pending_alp_packet len is: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos, atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);

				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, temp_alp_packet_buffer_read_len);
			}
		}
		
		atsc3_sony_ts_alp_replay_context->ts_file_pos += temp_alp_packet_buffer_read_len;

				
	}
											
											

											
    return 0;
}

