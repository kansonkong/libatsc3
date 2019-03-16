/*
 * atsc3_stltp_types.h
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_STLTP_TYPES_H_
#define ATSC3_STLTP_TYPES_H_

#include <stdint.h>
#include "atsc3_listener_udp.h"

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

typedef struct atsc3_stltp_tunnel_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header;
	uint8_t* first_ip_header;
	uint32_t first_ip_header_length;
	udp_packet_t* udp_packet;

} atsc3_stltp_tunnel_packet_t;

#endif /* ATSC3_STLTP_TYPES_H_ */
