/*
 *
 * atsc3_mmt_signaling_message_test.c:  driver for MMT signaling message mapping MPU sequence numbers to MPU presentatino time
 *
 */

#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_signaling_message.h"
#include "atsc3_logging_externs.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "atsc3_mmtp_packet_types.h"


#define __UNIT_TEST 1
#ifdef __UNIT_TEST

//system_time_message with packet_id=1
static char* __get_test_mmt_signaling_message_mpu_timestamp_descriptor()	{ return "62020023afb90000002b4f2f00351058a40000000012ce003f12ce003b04010000000000000000101111111111111111111111111111111168657631fd00ff00015f9001000023000f00010c000016cedfc2afb8d6459fff"; }

static char* __get_test_mmt_signaling_message_mpt_table_mpu_timestamp_descriptor_with_ac4_audio() {
   // return "40020015d7e0e525008c768403800000001300003413ed0030fc0100000000010000000b617564696f61737365743061632d34fe01000015000f00010c000705ede02a5662bc249800";
	return "400200140fef814f066a856903800000001200003412be0030fc0100000000010000000b766964656f61737365743068657631fe01000014000f00010c00055dbee02a8e7189cd4c00";
}

//

void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, int * binary_payload_size);
int test_mmt_signaling_message_mpu_timestamp_descriptor_table(char* base64_payload);

int main(int argc, char* argv[]) {
	_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
	_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;

	if(argc==2) {
		test_mmt_signaling_message_mpu_timestamp_descriptor_table(argv[1]);
	} else {
		printf("%s - base64_mmt_signaling_message_here", argv[0]);
	}

    return 0;
}


int test_mmt_signaling_message_mpu_timestamp_descriptor_table(char* base64_payload) {

	uint8_t* binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);

	mmtp_payload_fragments_union_t* mmtp_payload_fragments = calloc(1, sizeof(mmtp_payload_fragments_union_t));

	uint8_t* raw_packet_ptr = NULL;
	raw_packet_ptr = mmtp_packet_header_parse_from_raw_packet(mmtp_payload_fragments, binary_payload, binary_payload_size);

	if(!raw_packet_ptr) {
		_MMSM_ERROR("test_mmt_signaling_message_mpu_timestamp_descriptor_table - raw packet ptr is null!");
		return -1;
	}
	uint8_t new_size = binary_payload_size - (raw_packet_ptr - binary_payload);
	raw_packet_ptr = mmt_signaling_message_parse_packet_header(mmtp_payload_fragments, raw_packet_ptr, new_size);

	new_size = binary_payload_size - (raw_packet_ptr - binary_payload);
	raw_packet_ptr = mmt_signaling_message_parse_packet(mmtp_payload_fragments, raw_packet_ptr, new_size);

	signaling_message_dump(mmtp_payload_fragments);

	return 0;
}


void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, int * binary_payload_size) {
	int test_payload_base64_length = strlen(test_payload_base64);
	int test_payload_binary_size = test_payload_base64_length/2;

	uint8_t *test_payload_binary = calloc(test_payload_binary_size, sizeof(uint8_t));

	for (size_t count = 0; count < test_payload_binary_size; count++) {
	        sscanf(test_payload_base64, "%2hhx", &test_payload_binary[count]);
	        test_payload_base64 += 2;
	}

	*binary_payload = test_payload_binary;
	*binary_payload_size = test_payload_binary_size;
}



#endif



