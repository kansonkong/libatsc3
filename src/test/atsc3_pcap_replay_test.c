/*
 * atsc3_pcap_replay_test.c
 *
 *  Created on: Oct 10, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_pcap_type.h"

#define _ATSC3_PCAP_REPLAY_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


int main(int argc, char* argv[] ) {

	const char* PCAP_REPLAY_TEST_FILENAME = "testdata/pcap_replay_test.pcap";
	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context = NULL;

	atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(PCAP_REPLAY_TEST_FILENAME);
	_ATSC3_PCAP_REPLAY_TEST_DEBUG("Opening pcap: %s, context is: %p", PCAP_REPLAY_TEST_FILENAME, atsc3_pcap_replay_context);

	if(atsc3_pcap_replay_context) {
		while((atsc3_pcap_replay_context = atsc3_pcap_replay_iterate_packet(atsc3_pcap_replay_context))) {
			_ATSC3_PCAP_REPLAY_TEST_DEBUG("Got packet len: %d, ts_sec: %u, ts_usec: %u",
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance.current_pcap_packet->p_size,
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_sec,
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_usec);


		}
	}

	return 0;
}

