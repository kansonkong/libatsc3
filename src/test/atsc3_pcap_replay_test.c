/*
 * atsc3_pcap_replay_test.c
 *
 *  Created on: Oct 10, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../ats3_pcap_type.h"

#define _ATSC3_PCAP_REPLAY_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_REPLAY_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

int parse_fdt(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");
	xml_document_t* fdt_xml = xml_open_document(fp);
	if(fdt_xml) {
		//validate our struct
		atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
		if(!atsc3_fdt_instance) {
			_ATSC3_FDT_TEST_UTILS_ERROR("atsc3_fdt_instance is null!");
			return -1;
		}
		atsc3_fdt_instance_dump(atsc3_fdt_instance);

	}

	return ret;
}

int main(int argc, char* argv[] ) {


	const char* PCAP_REPLAY_TEST_FILENAME = "tests/pcap_replay_test.pcap";
	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context = NULL;

	atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(PCAP_REPLAY_TEST_FILENAME);
	if(atsc3_pcap_replay_context) {
		while((atsc3_pcap_replay_context = atsc3_pcap_replay_iterate_packet(atsc3_pcap_replay_context))) {
			_ATSC3_PCAP_REPLAY_TEST_DEBUG("Got packet len: %d, ts_sec: %u, ts_usec: %u",
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance->current_pcap_packet->p_size,
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance->atsc3_pcap_packet_header->ts_sec,
					atsc3_pcap_replay_context->atsc3_pcap_packet_instance->atsc3_pcap_packet_header->ts_usec);


		}
	}


	return 0;
}

