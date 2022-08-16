/*
 * atsc3_sony_ts_alp_reader_reflector_test.c
 *
 *  Created on: Aug 8, 2022
 *      Author: jjustman
 
 
 Samples:
 -rw-------@ 1 jjustman  staff  5242944 Dec 20 21:40 DVB-gps-and-video.ts
 -rw-------@ 1 jjustman  staff  5242944 Dec 20 21:48 DVB-separate-plps-with-audio-video-gps.ts
 -rw-------@ 1 jjustman  staff  5242944 Dec 20 21:48 DVB-single-plp-with-audio-video-gps.ts
 -rw-------@ 1 jjustman  staff   240452 Dec 20 21:40 DVB.ts

 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>


#include "../atsc3_utils.h"
#include "../atsc3_listener_udp.h"
#include "../atsc3_pcap_type.h"
#include "../atsc3_alp_parser.h"


#define _ATSC3_SONY_TS_ALP_READER_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SONY_TS_ALP_READER_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

static const int i = 1;
#define is_bigendian()  'ABCD' != 0x41424344
//is_bigendian() ( (*(char*)&i) == 0 )

#define ATSC3_SONY_TS_ALP_FRAME_LENGTH 				188
#define ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN		4


/*
 jjustman-2021-12-20 - special TS ALP padding observed on Endeveour dongle:
 
 [8:06 PM, 12/20/2021] Jason: [8:04 PM, 12/20/2021] Jason:  think i got it:

 47402D 82 -> 130 bytes that belong to the previous packet, then

 a.) it will contain a (varlen?) padding starting with 0x80 10 01 FF with and ending of 0xE7 E6 (usually 32 + 4, but I have seen payloads of 36 + 4),

 or,

 b.) it will contain padding starting with 0x08 00 05 F0 with and ending of 0xE7 0xE6 (usually 9 bytes +4 checksum?)
 [8:07 PM, 12/20/2021] Jason: if the next 4 bytes of your read don't look like that (e.g. after the +4 checksum), then you have a new ALP header
 
 */
#if is_bigendian()

	//0x47402D00 -> 0x47 40 2D 00
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG     0x47402D00
	//0x47002D00 -> 0x47 00 2D 00
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG  0x47002D00
	//0xFFF7FF00 -> 0xFF F7 FF 00
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK           0xFFF7FF00

#else
	//flip when we mask as uint32_t

	//0x47402D00
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_START_FLAG     0x002D4047
	//0x47002D00 -> 0x002D0047
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG  0x002D0047
	//0xFFF7FF00
	#define ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK           0x00FF7FFF

#endif

pcap_t* descrInject = NULL;
atsc3_alp_packet_collection_t* atsc3_alp_packet_collection = NULL;


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

//    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/DVB-gps-and-video.ts";
//    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/DVB.ts";
//    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/DVB-separate-plps-with-audio-video-gps.ts";
//    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/DVB-single-plp-with-audio-video-gps.ts";
    //2022-02-16-725-sony.ts
//    const char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/2022-02-16-725-sony.ts";
    char* SONY_TS_ALP_READER_TEST_FILENAME = "testdata/2022-08-08-sea-533mhz-plp0-sony.ts";

    
    if(argc < 2) {
        println("%s - test sony TLV atsc3 udp mulitcast reflector ", argv[0]);
        println("---");
        println("args: devInject (ts_filename)");
        println(" devInject : device to inject for ALP IP reflection");
        println(" (ts_filename) : ts filename to replay, defaults to %s", SONY_TS_ALP_READER_TEST_FILENAME);

        exit(1);
    }
    
    char *devInject = argv[1];
    if(argc == 3) {
        SONY_TS_ALP_READER_TEST_FILENAME = argv[2];
    }
    char errbufInject[PCAP_ERRBUF_SIZE];

    struct bpf_program fpInject;
    bpf_u_int32 maskpInject;
    bpf_u_int32 netpInject;

    pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 1, errbufInject);
    
    _ATSC3_SONY_TS_ALP_READER_TEST_INFO("starting reflection on: %s (pcap i/f: %p), file: %s",
            devInject, descrInject, SONY_TS_ALP_READER_TEST_FILENAME);


    atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();

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
											
	_ATSC3_SONY_TS_ALP_READER_TEST_DEBUG("Opening ts: %s, length: %d, context is: %p, is_bigendian: %d", atsc3_sony_ts_alp_replay_context->ts_file_name, atsc3_sony_ts_alp_replay_context->ts_file_len, atsc3_sony_ts_alp_replay_context, is_bigendian());

	uint8_t temp_alp_packet_buffer[185] = { 0 };
	int  temp_alp_packet_buffer_read_len = 0;
    
    uint8_t my_plp = 0xFF;
    uint64_t l1dTimeNs_value_last = 0;
	
	while(atsc3_sony_ts_alp_replay_context->ts_file_pos  < (atsc3_sony_ts_alp_replay_context->ts_file_len - ATSC3_SONY_TS_ALP_SYNC_HEADER_MAX_LEN)) {
			
		bool	ts_payload_is_sync = false;
        bool    ts_payload_tei_flag = false;
        
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
            ts_payload_tei_flag = (atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[1] >> 7) & 0x1;

            _ATSC3_SONY_TS_ALP_READER_TEST_TRACE("ts_payload_is_sync_start_flag\ttei flag: %d, at s_file_pos: %d", ts_payload_tei_flag, atsc3_sony_ts_alp_replay_context->ts_file_pos);
			ts_payload_is_sync = true;
			ts_payload_is_sync_start_flag = true;
			ts_payload_alp_start = atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[3];
			atsc3_sony_ts_alp_replay_context->ts_file_pos += 4;
			
			temp_alp_packet_buffer_read_len = 184;
			
		} else if((*(uint32_t*)atsc3_sony_ts_alp_replay_context->current_ts_header_bytes & ATSC3_SONY_TS_ALP_SYNC_HEADER_MASK) == ATSC3_SONY_TS_ALP_SYNC_HEADER_NO_START_FLAG) {
            ts_payload_tei_flag = (atsc3_sony_ts_alp_replay_context->current_ts_header_bytes[1] >> 7) & 0x1;

            _ATSC3_SONY_TS_ALP_READER_TEST_TRACE("ts_payload_is_continuation\ttei flag: %d, at s_file_pos: %d", ts_payload_tei_flag, atsc3_sony_ts_alp_replay_context->ts_file_pos);
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
			//if(ts_payload_alp_start) {
                bool process_any_small_alp_packet = true;
                int ts_remaining_bytes = temp_alp_packet_buffer_read_len - ts_payload_alp_start;
				
				uint8_t* temp_alp_packet_buffer_starting_header = (temp_alp_packet_buffer + ts_payload_alp_start);

				if(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size) {
					_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: pending alp packet size is %d, sync frame at pos: %d", atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size, atsc3_sony_ts_alp_replay_context->ts_file_pos);

                    //append any trailing bytes before the start of our next frame
                    if(ts_payload_alp_start) {
                        block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, ts_payload_alp_start);
                    }
                    block_Rewind(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet);
					
					//close this packet out and parse out ALP
					_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Closing out pending alp packet, length: %d, first four bytes of ALP packet are: 0x%02x 0x%02x 0x%02x 0x%02x",
														atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size,
														*(uint8_t*)(block_Get(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet)),
														*(uint8_t*)(block_Get(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet) + 1),
														*(uint8_t*)(block_Get(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet) + 2),
														*(uint8_t*)(block_Get(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet) + 3)
														);
                    
                    //jjustman-2021-12-21 - TODO: IN processing for "pending_alp_packet", check for internal ALP packet len vs remaining size
                    // e.g. ReadState.PROCESS_ANY_SMALL_PACKETS here by peeking at first 11 LSB of temp_alp_packet_buffer_starting_header to see if value < ts remaining size
                    
                    //jjustman-2022-08-08 - todo - parse PLP from TS header...
                    block_Rewind(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet);
                    while(process_any_small_alp_packet && block_Remaining_size(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet) >= 8) {
                        atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(my_plp, atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet);
                        if(atsc3_alp_packet) {
                            if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.HEF) {
                                _ATSC3_SONY_TS_ALP_READER_TEST_INFO("header_extension: HEF extension type is: 0x%02x", atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type);
                                if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type == 0xF1) {
                                    
                                    my_plp = (atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte[0] >> 2) & 0x3F;
                                    _ATSC3_SONY_TS_ALP_READER_TEST_INFO("header_extension: myplp is now: 0x%02x", my_plp);
                                } else if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type == 0xF0) {
                                    
                                    block_t* l1d_timeinfo_blockt = block_Duplicate_from_ptr(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte, atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_length_minus1 + 1);
                                    block_Rewind(l1d_timeinfo_blockt);
                                    
                                    uint8_t  time_info_flag = block_Read_uint8_bitlen(l1d_timeinfo_blockt, 2);
                                    uint64_t time_info_sec  = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 32);
                                    uint64_t time_info_msec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);
                                    uint64_t time_info_usec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);
                                    uint64_t time_info_nsec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);
                                    
                                    uint64_t l1dTimeNs_value = (time_info_sec * 1000000000) + (time_info_msec * 1000000) + (time_info_usec * 1000) + time_info_nsec;
                                    uint64_t l1dTimeNs_subframe_delta = l1dTimeNs_value - l1dTimeNs_value_last;
                                    
                                    _ATSC3_SONY_TS_ALP_READER_TEST_INFO("header_extension: l1d_time_info_flag: 0x%01x, time_sec: %0.3d, time_msec: %0.3d, time_usec: %0.3d, time_nsec: %0.3d, l1d_time_val_ns: %" PRIu64 ", frame duration_ns: %" PRIu64 "",
                                                                        time_info_flag,
                                                                        time_info_sec,
                                                                        time_info_msec,
                                                                        time_info_usec,
                                                                        time_info_nsec,
                                                                        l1dTimeNs_value,
                                                                        l1dTimeNs_subframe_delta);
                                    l1dTimeNs_value_last = l1dTimeNs_value;

                                }
                                        
                            } else {
                                if(atsc3_alp_packet->is_alp_payload_complete) {
                                    atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                                } else {
                                    process_any_small_alp_packet = false;
                                }
                            }
                        }
                    }
                    atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection, descrInject);

                    for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
                         atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
                         atsc3_alp_packet_free_alp_payload(atsc3_alp_packet);
                     }
                     atsc3_alp_packet_collection_clear_atsc3_alp_packet(atsc3_alp_packet_collection);
                     //todo: refactor to _free(..) for vector_t
                     if(atsc3_alp_packet_collection->atsc3_alp_packet_v.data) {
                         free(atsc3_alp_packet_collection->atsc3_alp_packet_v.data);
                         atsc3_alp_packet_collection->atsc3_alp_packet_v.data = NULL;
                     }
                }
                
                if(!process_any_small_alp_packet || block_Remaining_size(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet) > 0) {
                    //truncate our pending_alp_packet at our small inner-fragmented packet, as we have a small alp packet that is split across TS frame
                    //hack-ish...
                    block_t* new_pending_alp_packet = block_Duplicate_from_position(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet);
                    block_Seek(new_pending_alp_packet, block_Remaining_size(new_pending_alp_packet));
                    
                    block_Destroy(&atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet);
                    atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet = new_pending_alp_packet;
                } else {
                    block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, 0);
                }
                
                _ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Starting new alp packet at TS start packet at pos: %d, remaining TS len: %d, ptr: %p, first 4 bytes: 0x%08x",
                                                atsc3_sony_ts_alp_replay_context->ts_file_pos,
                                                ts_remaining_bytes,
                                                temp_alp_packet_buffer_starting_header,
                                                *(uint32_t*)temp_alp_packet_buffer_starting_header);
				
			
        			
				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer_starting_header, ts_remaining_bytes);
                        				
			//}
//                else {
//				//start new at 0?
//				_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_sync_start_flag: Starting new alp packet at offset: 0 ?! TODO: scan if we are 0x80 or 0x08 for padding: %d", atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);
//				block_Resize(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, 0);
//				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, temp_alp_packet_buffer_read_len);
//			}
				
		} else if(ts_payload_is_continuation) {
			//read all 188-3 = 185 bytes into here
			
			if(!atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size) {
				_ATSC3_SONY_TS_ALP_READER_TEST_WARN("ts_payload_is_continuation: pending alp packet size is 0! sync frame at pos: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos);
			} else {
				_ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_payload_is_continuation: Reading inner TS packet at pos: %d, pending_alp_packet len is: %d", atsc3_sony_ts_alp_replay_context->ts_file_pos, atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet->p_size);

				block_Write(atsc3_sony_ts_alp_replay_context->atsc3_sony_ts_alp_packet_instance.pending_alp_packet, temp_alp_packet_buffer, temp_alp_packet_buffer_read_len);
			}
		}
        
update_fpos_and_loop:
		atsc3_sony_ts_alp_replay_context->ts_file_pos += temp_alp_packet_buffer_read_len;
        
        long actual_ts_file_pos = ftell(atsc3_sony_ts_alp_replay_context->ts_fp);
        
        _ATSC3_SONY_TS_ALP_READER_TEST_INFO("ts_file_pos: %d, actual ftell pos: %ld", atsc3_sony_ts_alp_replay_context->ts_file_pos, actual_ts_file_pos);


	}
    return 0;
}

