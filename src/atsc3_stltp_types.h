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

#define ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET 	0x4D
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


/**
 * ATSC A/324:2018 Section 8.2.1
 *
 *
 *The Preamble data shall be delivered in an RTP/UDP/IP multicast Stream conforming to RFC
3550 [6] with the constraints defined below. The maximum Preamble data structure size can
exceed the typical 1500-byte MTU, so a mechanism is defined herein to allow segmentation of the
Preamble data across multiple RTP/UDP/IP packets. Note that such segmentation is only required
to conform with typical MTU sizes of 1500 bytes. If the local Network allows larger multicast
packets, this segmentation may not be needed.
The payload data for each Preamble Stream RTP/UDP/IP packet shall be a fragment of the
Preamble Payload data structure described in Table 8.1. To provide validation that the
L1_Basic_signaling and L1_Detail_signaling structures are delivered correctly over the STL, a
16-bit cyclic redundancy check is provided. The CRC is applied to the combined length field,
L1_Basic_signaling and L1_Detail_signaling, and appended as the last 16 bits of the payload data.
The resultant Stream of Preamble Payload packets shall have destination address 239.0.51.48 and
destination port 30064.


Table 8.1 Preamble Payload
Syntax 							No. of Bits		Format
	Preamble_Payload () {
		length 					16			 	uimsbf
		L1_Basic_signaling() 	200 			Table 9.2 of [3]
		L1_Detail_signaling() 	var 			Table 9.12 of [3]
		crc16 					16 				uimsbf
}


 *
 * Preamble Payload data structure as described in Table 8.1. The Data Consumer can accumulate
RTP packets until it has received all of the bytes defined by the length field in the first packet. If a
packet is missed, as determined by a missing sequence number, or if a packet with the marker (M)
bit set to ‘1’ is received prematurely, indicating the start of the next Preamble Payload Packet Set,
then one or more packets have been lost and the entire Preamble Payload data set has been lost.
Any accumulated data shall be discarded.
The RTP header fields shall follow the syntax defined in RFC 3550 [6], with the following
additional constraints:
The Padding (P) bit shall conform to the RFC 3550 [6] specification.
The Extension (X) bit shall be set to zero ‘0’, indicating the header contains no extension.
The CSRC Count (CC) shall be set to zero ‘0’, as no CSRC fields are necessary.
The marker (M) bit shall be set to one ‘1’ to indicate that the first byte of the payload is the start of
the Preamble Payload data. A zero ‘0’ value shall indicate that the payload is a continuation of
the Preamble Payload data from the previous packet.
The Payload Type (PT) shall be set to 77 (0x4d), indicating the Preamble Payload type.
The Sequence Number shall conform to the RFC 3550 [6] specification.
The Timestamp shall be defined as in Table 8.2. The timestamp shall be set to the same value for
all of the Preamble Payload Packet Set.
The Synchronization Source (SSRC) Identifier shall be set to zero ‘0’. There shall be no other sources
of Preamble Payload data carried within an STLTP Stream. Any redundant sources can be
managed using IGMP Source-Specific Multicast (SSM) mechanisms.

**/



typedef struct atsc3_stltp_preamble_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header;
	udp_packet_t* udp_packet;
	uint32_t fragment_count;

} atsc3_stltp_preamble_packet_t;





typedef struct atsc3_stltp_tunnel_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel;
	uint8_t* first_ip_header;
	uint32_t first_ip_header_length;

	udp_packet_t* udp_packet;
	udp_packet_t* udp_packet_short_fragment;

	uint32_t fragment_count;

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_payload;

	atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet;
	atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet;

	bool is_truncated_from_marker;
	bool is_complete;

} atsc3_stltp_tunnel_packet_t;


#endif /* ATSC3_STLTP_TYPES_H_ */
