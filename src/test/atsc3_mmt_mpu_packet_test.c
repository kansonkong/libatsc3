/*
 * atsc3_mmt_mpu_packet_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../atsc3_utils.h"
#include "../atsc3_logging_externs.h"
#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmt_mpu_parser.h"
#include "../atsc3_mmt_mpu_utils.h"

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


/*
 */

int test_parse_atsc3_mmt_mpu_init_fragment_1() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/mpu/239.255.1.1.49152.packet_id.18.mpu_sequence_number.39.ft.0.fi.01.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_1: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_1: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);

	//START: method under test

	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		block_Rewind(test_atsc3_mmt_message_payload);
		mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

		mmtp_packet_header_dump(mmtp_packet_header);

		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, test_atsc3_mmt_message_payload);
		mmtp_mpu_dump_header(mmtp_mpu_packet);

		//free our alloc(s)
		mmtp_mpu_packet_free(&mmtp_mpu_packet);
		mmtp_packet_header_free(&mmtp_packet_header);
	}


	//END: method under test
	block_Release(&test_atsc3_mmt_message_payload);

	return 0;
}

/*
 *
 */


int test_parse_atsc3_mmt_mpu_init_fragment_2() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/mpu/239.255.1.1.49152.packet_id.18.mpu_sequence_number.39.ft.0.fi.03.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_2: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_2: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);

	//START: method under test

	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		block_Rewind(test_atsc3_mmt_message_payload);
		mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

		mmtp_packet_header_dump(mmtp_packet_header);

		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, test_atsc3_mmt_message_payload);
		mmtp_mpu_dump_header(mmtp_mpu_packet);

		//free our alloc(s)
		mmtp_mpu_packet_free(&mmtp_mpu_packet);
		mmtp_packet_header_free(&mmtp_packet_header);
	}

	//END: method under test
	block_Release(&test_atsc3_mmt_message_payload);

	return 0;
}

/*
 *
 * 1358 bytes
 */

int test_parse_atsc3_mmt_mpu_mpu_sample_1() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/mpu/239.255.1.1.49152.packet_id.18.mpu_sequence_number.39.ft.2.sample.1.fc.68.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_1: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_1: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);

	//START: method under test

	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		block_Rewind(test_atsc3_mmt_message_payload);
		mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

		mmtp_packet_header_dump(mmtp_packet_header);

		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, test_atsc3_mmt_message_payload);
		mmtp_mpu_dump_header(mmtp_mpu_packet);

		//free our alloc(s)
		mmtp_mpu_packet_free(&mmtp_mpu_packet);
		mmtp_packet_header_free(&mmtp_packet_header);
	}


	//END: method under test
	block_Release(&test_atsc3_mmt_message_payload);

	return 0;
}



/*
 *
 * 1358 bytes
 */

int test_parse_atsc3_mmt_mpu_mpu_sample_2() {

	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/mpu/239.255.1.1.49152.packet_id.18.mpu_sequence_number.39.ft.2.sample.1.fc.69.bin";
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_2: opening file: %s", TEST_ATSC3_MMT_MESSAGE_FILENAME);

	block_t* test_atsc3_mmt_message_payload = get_ip_frame_payload_from_raw_ether_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);
	assert(test_atsc3_mmt_message_payload);
	__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_2: i_pos: %d, p_size: %d", test_atsc3_mmt_message_payload->i_pos, test_atsc3_mmt_message_payload->p_size);

	//START: method under test

	for(int i=0; i < LEAK_CHECK_RUN_COUNT; i++) {
		block_Rewind(test_atsc3_mmt_message_payload);
		mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(test_atsc3_mmt_message_payload);

		mmtp_packet_header_dump(mmtp_packet_header);

		mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header, test_atsc3_mmt_message_payload);
		mmtp_mpu_dump_header(mmtp_mpu_packet);

		//free our alloc(s)
		mmtp_mpu_packet_free(&mmtp_mpu_packet);
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
	_MMT_MPU_DEBUG_ENABLED = 1;


	__MMSM_INFO("---starting unit test---");
	if(false) {
		__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_1");
		test_parse_atsc3_mmt_mpu_init_fragment_1();
	}

	if(false) {
		__MMSM_INFO("test_parse_atsc3_mmt_mpu_init_fragment_2");
		test_parse_atsc3_mmt_mpu_init_fragment_2();
	}

	if(false) {
		__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_1");
		test_parse_atsc3_mmt_mpu_mpu_sample_1();
	}

	//test_parse_atsc3_mmt_mpu_mpu_sample_2

	if(true) {
		__MMSM_INFO("test_parse_atsc3_mmt_mpu_mpu_sample_2");
		test_parse_atsc3_mmt_mpu_mpu_sample_2();
	}
}
