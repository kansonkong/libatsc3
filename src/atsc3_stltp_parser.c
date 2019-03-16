/*
 * atsc3_stltp_parser.c
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */


#include "atsc3_stltp_parser.h"

int _STLTP_PARSER_DEBUG_ENABLED = 1;

atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header(uint8_t* data, uint32_t size) {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = calloc(1, sizeof(atsc3_rtp_fixed_header_t));
	//total bits needed for rtp_fixed_header == 96
	if(size < 12) {
		return NULL;
	}

	uint8_t flags[2];
	flags[0] = data[0];
	flags[1] = data[1];

	atsc3_rtp_fixed_header->version = (flags[0] >> 6) & 0x3;
	atsc3_rtp_fixed_header->padding = (flags[0] >> 5) & 0x1;
	atsc3_rtp_fixed_header->extension = (flags[0] >> 4) & 0x1;
	atsc3_rtp_fixed_header->csrc_count = (flags[0]) & 0xF;

	atsc3_rtp_fixed_header->marker = (flags[1] >> 7 ) & 0x1;
	atsc3_rtp_fixed_header->payload_type = (flags[1]) & 0x7F;

	atsc3_rtp_fixed_header->sequence_number = ntohs(*((uint16_t*)(&data[2])));
	atsc3_rtp_fixed_header->timestamp = ntohl(*((uint32_t*)(&data[4])));
	atsc3_rtp_fixed_header->packet_offset = ntohl(*((uint32_t*)(&data[8])));

	return atsc3_rtp_fixed_header;
}



