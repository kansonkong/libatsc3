/*
 * atsc3_stltp_types.h
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_STLTP_TYPES_H_
#define ATSC3_STLTP_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include "atsc3_listener_udp.h"

#define ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL 			0x61
#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET 	0x4e

typedef struct atsc3_rtp_fixed_header {
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t extension:1;
	uint8_t csrc_count:4;
	uint8_t marker:1;
	uint8_t payload_type:7;
	uint16_t sequence_number;
	uint32_t timestamp;
	uint32_t packet_offset;
} atsc3_rtp_fixed_header_t;


typedef struct atsc3_stltp_baseband_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header;
	uint8_t* baseband_packet;
	uint32_t baseband_packet_offset;
	uint32_t baseband_packet_length;
	udp_packet_t* udp_packet;
	uint32_t fragment_count;

} atsc3_stltp_baseband_packet_t;


typedef struct atsc3_stltp_tunnel_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel;
	uint8_t* first_ip_header;
	uint32_t first_ip_header_length;

	udp_packet_t* udp_packet;
	uint32_t fragment_count;

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_payload;

	atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet;
	bool is_truncated_from_marker;
	bool is_complete;

} atsc3_stltp_tunnel_packet_t;


#endif /* ATSC3_STLTP_TYPES_H_ */
