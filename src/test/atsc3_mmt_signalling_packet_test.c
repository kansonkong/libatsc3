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
#include "../atsc3_mmt_signalling_message_types.h"
#include "../atsc3_mmt_signalling_message.h"
#include "../atsc3_mmtp_parser.h"

#define LEAK_CHECK_RUN_COUNT 10000

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
 */
int test_parse_atsc3_mmt_message_no_factoy() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/signalling_info/239.155.1.1.49152.6425.9816.33024.atsc3_mmt_message.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factoy: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factoy: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);


	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

	mmtp_packet_header_dump(mmtp_packet_header);

	mmtp_packet_header_free(&mmtp_packet_header);

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


	__MMSM_INFO("---starting unit test---");
	__MMSM_INFO("test_parse_atsc3_mmt_message_no_factory")
	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		test_parse_atsc3_mmt_message_no_factoy();
	}
}
