/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../atsc3_utils.h"
#include "../atsc3_logging_externs.h"
#include "../atsc3_mmt_signalling_message_types.h"
#include "../atsc3_mmt_signalling_message.h"
#include "../atsc3_mmtp_parser.h"

#define LEAK_CHECK_RUN_COUNT 1000

#define PHY_FRAME_PAYLOAD_START 42

block_t* get_ip_frame_payload_from_raw_ether_filename(const char* file_name) {
	if( access(file_name, F_OK ) == -1 ) {
		__MMSM_ERROR("alc_get_payload_from_filename: unable to open file: %s", file_name);
		return NULL;
	}

	struct stat st;
	stat(file_name, &st);

	block_t* payload = block_Alloc(st.st_size - PHY_FRAME_PAYLOAD_START);

	FILE* fp = fopen(file_name, "r");
	if(st.st_size < 14) {
		__MMSM_ERROR("alc_get_payload_from_filename: size too small - must be greater than 14 - size: %lld file: %s", st.st_size, file_name);
		return NULL;
	}

	fseek(fp, PHY_FRAME_PAYLOAD_START, SEEK_SET);
	fread(payload->p_buffer, st.st_size - PHY_FRAME_PAYLOAD_START, 1, fp);
    payload->i_pos = 0;

	fclose(fp);
	return payload;
}



int test_parse_mp_item() {

	return 0;
}

int test_parse_mp_table() {


	return 0;
}
/*
 * 239.155.1.1.49152.6425.9816.33024.atsc3_mmt_message.bin
 *
 *
 * 2019-08-12 - valgrind results:
==26527== 799,920 bytes in 9,999 blocks are definitely lost in loss record 60 of 61
==26527==    at 0x1001586EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==26527==    by 0x1000091DF: mmtp_signalling_packet_add_mmt_signalling_message_header_and_payload (atsc3_mmtp_packet_types.c:113)
==26527==    by 0x10000E12D: mmt_signalling_message_parse_id_type (atsc3_mmt_signalling_message.c:171)
==26527==    by 0x10000E014: mmt_signalling_message_parse_packet (atsc3_mmt_signalling_message.c:126)
==26527==    by 0x100000F51: test_parse_atsc3_mmt_message_no_factoy (atsc3_mmt_signalling_packet_test.c:82)
==26527==    by 0x1000010E8: main (atsc3_mmt_signalling_packet_test.c:119)
==26527==
==26527== 3,439,312 bytes in 9,998 blocks are definitely lost in loss record 61 of 61
==26527==    at 0x1001586EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==26527==    by 0x10000FA92: __read_uint32_len_to_string (atsc3_mmt_signalling_message.c:353)
==26527==    by 0x10000F2F6: mmt_atsc3_message_payload_parse (atsc3_mmt_signalling_message.c:578)
==26527==    by 0x10000E50E: mmt_signalling_message_parse_id_type (atsc3_mmt_signalling_message.c:262)
==26527==    by 0x10000E014: mmt_signalling_message_parse_packet (atsc3_mmt_signalling_message.c:126)
==26527==    by 0x100000F51: test_parse_atsc3_mmt_message_no_factoy (atsc3_mmt_signalling_packet_test.c:82)
==26527==    by 0x1000010E8: main (atsc3_mmt_signalling_packet_test.c:119)
==26527==
 *
 *
 */
int test_parse_atsc3_mmt_message_no_factoy() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/signalling_info/239.155.1.1.49152.6425.9816.33024.atsc3_mmt_message.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factoy: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factoy: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);

	//START: method under test

	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		block_Rewind(test_atsc3_mmt_message_payload);
		mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

		mmtp_packet_header_dump(mmtp_packet_header);

		mmtp_signalling_packet_t* mmtp_signalling_packet = mmt_signalling_message_parse_packet_header(mmtp_packet_header, test_atsc3_mmt_message_payload);
		uint8_t mmt_SI_messages_processed = mmt_signalling_message_parse_packet(mmtp_signalling_packet, test_atsc3_mmt_message_payload);

		mmt_signalling_message_dump(mmtp_signalling_packet);

		//free our alloc(s)
		mmt_signalling_message_free(&mmtp_signalling_packet);
		mmtp_packet_header_free(&mmtp_packet_header);
	}


	//END: method under test
	block_Release(&test_atsc3_mmt_message_payload);

	return 0;
}
/*
 * todo add HRBM testing
 */

/**
 * unit tests for
 *
 * _parse
 * _dump
 * _free
 *
 * todo:
 * 	_new 		primiative
 * 	_duplicate	deep clone helper
 *
 */

int main(int argc, char* argv[] ) {
	_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;


	__MMSM_INFO("---starting unit test---");
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factory")
	test_parse_atsc3_mmt_message_no_factoy();

}
