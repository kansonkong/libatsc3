/*
 * atsc3_tolka_ts_alp_reader_test.c
 *
 *  Created on: May 23rd, 2022
 *      Author: jjustman
 
 
 Samples:

 2022-05-19-tolka-tlv-testing.ts.tlv
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>


#include "../atsc3_utils.h"
#include "../atsc3_pcap_type.h"

#define _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

static const int i = 1;
#define is_bigendian()  'ABCD' != 0x41424344
//is_bigendian() ( (*(char*)&i) == 0 )

#define ATSC3_TOLKA_TS_TLV_ALP_FRAME_LENGTH             188
#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MAX_LEN		4

#if is_bigendian()

    //0x47136824 -> 0x47 13 68 24
	#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_START_FLAG     0x47136824
	//0x479E4500 -> 0x47 9E 45 00 ?
	#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_START_FLAG  0x47002D00
	//0xFFF7FF00 -> 0xFF F7 FF 00
	#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MASK           0xFFFFFFFF

	//0x801001ff -> 0x80 0x10 0x01 0xff -> 0xff 0x01 0x10 0x80
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_START_LONG 0x801001ff
	//0x080005f0 -> 0x08 0x00 0x05 0xf0 -> 0xf0 0x05 0x00 0x08
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_START_SHORT 0x080005f0

	//0xE7E6
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_END           0xe7e6
#else
	//flip when we mask as uint32_t

	//0x24681347
	#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_START_FLAG     0x24681347
    #define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MASK           0xFFFFFFFF

	//0x47002D00 -> 0x002D0047
	#define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_TLV_START_FLAG  0x00000047
    #define ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_TLV_MASK        0x000000FF

	//0xFFF7FF00

	//0x801001ff -> 0x80 0x10 0x01 0xff -> 0xff 0x01 0x10 0x80
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_START_LONG 0xff011080
	//0x080005f0 -> 0x08 0x00 0x05 0xf0 -> 0xf0 0x05 0x00 0x08
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_START_SHORT 0xf0050008

	//0xE7E6 -> 0xE6 0xE7
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_PAD_END        0xE6E7


#endif


typedef struct atsc3_tolka_ts_tlv_alp_packet_instance {
    block_t*                        pending_tlv_packet_data;
    
	block_t*						pending_alp_packet;
    uint32_t                        pending_alp_packet_tlv_len;
    uint8_t                         pending_alp_packet_tlv_plp;
    
    uint8_t                         ts_transfer_size;
    uint8_t                         tlv_header_size;
	
} atsc3_tolka_ts_tlv_alp_packet_instance_t;

void atsc3_tolka_ts_tlv_alp_packet_instance_reset(atsc3_tolka_ts_tlv_alp_packet_instance_t* atsc3_tolka_ts_tlv_alp_packet_instance) {
    block_Resize(atsc3_tolka_ts_tlv_alp_packet_instance->pending_tlv_packet_data, 0);
    block_Resize(atsc3_tolka_ts_tlv_alp_packet_instance->pending_alp_packet, 0);
    atsc3_tolka_ts_tlv_alp_packet_instance->pending_alp_packet_tlv_len = 0;
    atsc3_tolka_ts_tlv_alp_packet_instance->pending_alp_packet_tlv_plp = 0;
    atsc3_tolka_ts_tlv_alp_packet_instance->ts_transfer_size = 0;
    atsc3_tolka_ts_tlv_alp_packet_instance->ts_transfer_size = 0;
    atsc3_tolka_ts_tlv_alp_packet_instance->tlv_header_size = 0;
}

typedef struct atsc3_tolka_ts_tlv_alp_replay_context {
	char* 								        ts_file_name;

	FILE* 								        ts_fp;
	uint32_t 							        ts_fd_start;

	uint32_t							        ts_file_len;
	uint32_t							        ts_file_pos;

	uint32_t							        ts_read_packet_count;
	
	uint8_t		 					            current_ts_header_bytes[4]; //allocate this as a word for now...

    atsc3_tolka_ts_tlv_alp_packet_instance_t    atsc3_tolka_ts_tlv_alp_packet_instance;

} atsc3_tolka_ts_tlv_alp_replay_context_t;

atsc3_tolka_ts_tlv_alp_replay_context_t* atsc3_tolka_ts_tlv_alp_replay_context_new() {
    atsc3_tolka_ts_tlv_alp_replay_context_t* atsc3_tolka_ts_tlv_alp_replay_context = calloc(1, sizeof(atsc3_tolka_ts_tlv_alp_replay_context_t));
 //   atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.current_ts_packet  = block_Alloc(ATSC3_TOLKA_TS_TLV_ALP_FRAME_LENGTH);
	//block_Resize(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.current_ts_packet, 0);
    atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_tlv_packet_data = block_Alloc(188);

    atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet = block_Alloc(MAX_ATSC3_PHY_IP_DATAGRAM_SIZE);
	block_Resize(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet, 0);
	return atsc3_tolka_ts_tlv_alp_replay_context;
}

int main(int argc, char* argv[] ) {

    const char* TOLKA_TS_TLV_ALP_READER_TEST_FILENAME = "testdata/2022-05-19-tolka-testing.ts.tlv";


    atsc3_tolka_ts_tlv_alp_replay_context_t* atsc3_tolka_ts_tlv_alp_replay_context = atsc3_tolka_ts_tlv_alp_replay_context_new();
	//hack-ish for now
	atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name = strdup(TOLKA_TS_TLV_ALP_READER_TEST_FILENAME);
	
	struct stat st;
	stat(atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name, &st);
	if(!st.st_size) {
		_ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_WARN("atsc3_tolka_ts_tlv_alp_reader_test: %s, ERROR: st.st_size is 0!", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name);
		return -1;
	}
				
	atsc3_tolka_ts_tlv_alp_replay_context->ts_file_len = st.st_size;
	atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos = 0;
											
	atsc3_tolka_ts_tlv_alp_replay_context->ts_fp = fopen(atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name, "r");
	if(!atsc3_tolka_ts_tlv_alp_replay_context->ts_fp) {
		_ATSC3_PCAP_TYPE_WARN("atsc3_tolka_ts_tlv_alp_reader_test: %s -  fopen() for read returned NULL!", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name);
		return -1;
	}
											
	_ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_DEBUG("Opening ts: %s, length: %d, context is: %p, is_bigendian: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_name, atsc3_tolka_ts_tlv_alp_replay_context->ts_file_len, atsc3_tolka_ts_tlv_alp_replay_context, is_bigendian());

	char temp_alp_packet_buffer[185] = { 0 };
	int  temp_alp_packet_buffer_read_len = 0;
	
	while(atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos  < (atsc3_tolka_ts_tlv_alp_replay_context->ts_file_len - ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MAX_LEN)) {
			
		bool	ts_payload_is_sync = false;
		        
		bool 	ts_payload_is_sync_alp_start_flag = false;

		bool 	ts_payload_is_continuation = false;
		
		//peek to start by reading first 4 sync bytes
		fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes, ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MAX_LEN, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
		
		_ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_TRACE("parsing loop: reading as ts_file_pos: %d, 4 bytes are: 0x%02x, 0x%02x, 0x%02x, 0x%02x, ts_file_len: %d, 0x%08x, 0x%08x, 0x%08x",
											 atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos,
											 atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[0],
											 atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[1],
											 atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[2],
											 atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[3],
											 atsc3_tolka_ts_tlv_alp_replay_context->ts_file_len,
											 (*(uint32_t*)atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes & ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MASK),
											 ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_START_FLAG,
                                                  ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_TLV_START_FLAG
											 );

		//check for our sync byte and SL TLV start code - ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_START_FLAG
		if((*(uint32_t*)atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes & ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MASK) == ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_START_FLAG) {
			_ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_TRACE("ts_payload_is_sync_start_flag at s_file_pos: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos);
			ts_payload_is_sync = true;
			ts_payload_is_sync_alp_start_flag = true;
            //B4-B7 - next 4 bytes should be our ALP packet len
            
            fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len, 4, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            //jjustman-2022-05-24 - super hack? remove +2 bytes for no supplied for ALP header?
            atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len -= 2;
            
            block_Resize(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet, 0);

            //B8 - uint8 PLP num
            fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_plp, 1, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            //B9 - TS transfer size, should be 188
            fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.ts_transfer_size, 1, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            //B10 - TLV header size, should be 24
            fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.tlv_header_size, 1, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            uint8_t temp_tlv_packet_read_buffer_len = 188 - 11;
            
            block_Resize(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_tlv_packet_data, temp_tlv_packet_read_buffer_len);
            fread((void*)atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_tlv_packet_data->p_buffer, temp_tlv_packet_read_buffer_len, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos += 11 + temp_tlv_packet_read_buffer_len; //we should have read 11 + tlv remaing payload for the start frame
                    
            //now, read the next byte, which should be the TS start frame id, for our continuation which contains our actual ALP data after byte 1
            fread((void*)&atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes, 1, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);
            atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos++;
            if(atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[0] == 0x47) {
                temp_alp_packet_buffer_read_len = 187;
                _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_TRACE("ts_tlv_payload_is_sync_start_flag at s_file_pos: %d, pending_alp_packet_tlv_len: %d, pending_alp_packet_tlv_plp: %d, ts_transfer_size: %d, tlv_header_size: %d",
                                                          atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos,
                                                          atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len,
                                                          atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_plp,
                                                          atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.ts_transfer_size,
                                                          atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.tlv_header_size);

            } else {
                _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_WARN("ts_tlv_payload_is_sync_start_flag, pos: %d is not 0x47! val: 0x%02x", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos, atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes[0]);
            }
            

			
		} else if((*(uint32_t*)atsc3_tolka_ts_tlv_alp_replay_context->current_ts_header_bytes & ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_TLV_MASK) == ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_NO_TLV_START_FLAG) {
			_ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_TRACE("ts_payload_is_continuation sync bit at s_file_pos: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos);
            if(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len > 0 && atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size < atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len) {
                ts_payload_is_sync = true;
                ts_payload_is_continuation = true;
                fseek(atsc3_tolka_ts_tlv_alp_replay_context->ts_fp, -2 , SEEK_CUR);
                atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos += 2;
                 
                temp_alp_packet_buffer_read_len = 186;
            } else {
                _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_WARN("ts_payload_is_continuation sync bit at s_file_pos: %d, but pending_alp_packet_tlv_len: %d with p_size: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos,
                                                         atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len,
                                                         atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size);
                temp_alp_packet_buffer_read_len = 0;
            }
			
		} else {
			//walk thru +1
			fseek(atsc3_tolka_ts_tlv_alp_replay_context->ts_fp, 1 - ATSC3_TOLKA_TS_TLV_ALP_SYNC_HEADER_MAX_LEN, SEEK_CUR);
			atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos++;
			
			continue;
		}
		
        if(temp_alp_packet_buffer_read_len) {
            fread((void*)&temp_alp_packet_buffer, temp_alp_packet_buffer_read_len, 1, atsc3_tolka_ts_tlv_alp_replay_context->ts_fp);

            if(ts_payload_is_sync_alp_start_flag) {
                _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_INFO("ts_payload_is_sync_alp_start_flag: Reading sync frame at pos: %d, ts_payload_alp_start: %d, pending_alp_packet len is: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos, 0, atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size);

                block_Write(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, temp_alp_packet_buffer_read_len);
            } else if(ts_payload_is_continuation) {
                //read all 188-3 = 185 bytes into here
                
                if(!atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size) {
                    _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_WARN("ts_payload_is_continuation: pending alp packet size is 0! sync frame at pos: %d", atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos);
                } else {
                    //hack for casting
                    uint32_t to_read_remaining_alp_packet_len = __MAX(0, atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len - atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size);
                    uint32_t to_append_alp_packet_buffer_len = __MIN(temp_alp_packet_buffer_read_len, to_read_remaining_alp_packet_len);
                    _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_INFO("ts_payload_is_continuation: Reading inner TS packet at pos: %d, pending_alp_packet_tlv_len: %d, pending_alp_packet len is: %d, appending: %d bytes",
                                                             atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos,
                                                             atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len,
                                                             atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size,
                                                             to_append_alp_packet_buffer_len);

                    block_Write(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, to_append_alp_packet_buffer_len);
                }
            }
        }
        
        if(atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len > 0 && atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len == atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_size) {
            //flush out this completed ALP packet..
            _ATSC3_TOLKA_TS_TLV_ALP_READER_TEST_INFO("pending_alp_packet_tlv_len complete check: done! pending_alp_packet_tlv_len == pending_alp_packet->p_size: %d, plp: %d, first 4 bytes: 0x%02x 0x%02x 0x%02x 0x%02x",
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_len,
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet_tlv_plp,
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_buffer[0],
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_buffer[1],
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_buffer[2],
                                                     atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance.pending_alp_packet->p_buffer[3]

                                                     );

            //jjustman-2022-05-24 - dispatch to alp packet handler here
            
            atsc3_tolka_ts_tlv_alp_packet_instance_reset(&atsc3_tolka_ts_tlv_alp_replay_context->atsc3_tolka_ts_tlv_alp_packet_instance);
        }
        
		
		atsc3_tolka_ts_tlv_alp_replay_context->ts_file_pos += temp_alp_packet_buffer_read_len;
				
	}
								
    return 0;
}

