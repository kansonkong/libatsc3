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

#define ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL 					0x61

#define ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET 	0x4C
#define ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET 			0x4D
#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET 			0x4E

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

//https://www.atsc.org/wp-content/uploads/2016/10/A322-2018-Physical-Layer-Protocol.pdf

typedef struct atsc3_stltp_baseband_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header;

	uint8_t* 		payload;
	uint32_t 		payload_offset;
	uint32_t 		payload_length;
	bool 			is_complete;

	//other baseband alp attributes here



	udp_packet_t* 	udp_packet;
	uint32_t 		fragment_count;

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
		L1_Basic_signaling() 	200 			Table 9.2 of [3]  //atsc a/322
		L1_Detail_signaling() 	var 			Table 9.12 of [3] //atsc a/322
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
the Preamble Payload data. A zero ‘0’ value shall indicate that the payload is a continuation of the
Preamble Payload data from the previous packet.

The Payload Type (PT) shall be set to 77 (0x4d), indicating the Preamble Payload type.

The Sequence Number shall conform to the RFC 3550 [6] specification.
The Timestamp shall be defined as in Table 8.2. The timestamp shall be set to the same value for
all of the Preamble Payload Packet Set.

The Synchronization Source (SSRC) Identifier shall be set to zero ‘0’. There shall be no other sources
of Preamble Payload data carried within an STLTP Stream.

Any redundant sources can be
managed using IGMP Source-Specific Multicast (SSM) mechanisms.

**/

typedef struct L1_basic_signaling {
	uint8_t raw_payload[25];
} L1_basic_signaling_t;

typedef struct L1_detail_signaling {

} L1_detail_signaling_t;

typedef struct atsc3_stltp_preamble_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header;

	uint8_t* 				payload;
	uint16_t 				payload_offset;
	uint16_t 				payload_length;
	bool is_complete;

	L1_basic_signaling_t 	L1_basic_signaling;
	L1_detail_signaling_t 	L1_detail_signaling;
	uint16_t				crc16;

	udp_packet_t* 			udp_packet;
	uint32_t 				fragment_count;

} atsc3_stltp_preamble_packet_t;


typedef struct timing_management_packet {
	uint8_t		version_major:4;
	uint8_t		version_minor:4;
	uint8_t 	maj_log_rep_cnt_pre:4;
	uint8_t 	maj_log_rep_cnt_tim:4;
	uint8_t 	bootstrap_major:4;
	uint8_t 	bootstrap_minor:4;
	uint8_t 	min_time_to_next:5;
	uint8_t 	system_bandwidth:2;
	uint8_t 	bsr_coefficient:7;
	uint8_t 	preamble_structure:8;
	uint8_t 	ea_wakeup:2;
	uint8_t 	num_emission_tim:6;
	uint8_t 	num_xmtrs_in_group:6;
	uint8_t 	xmtr_group_num:7;
	uint8_t 	maj_log_override:3;
	uint8_t		num_miso_filt_codes:2;
	uint8_t 	tx_carrier_offset:2;
	uint8_t 	_reserved:6; 		//1

} timing_management_packet_t;

typedef struct bootstrap_timing_data_emission {

} bootstrap_timing_data_emission_t;

typedef struct bootstrap_timing_data {
	uint32_t	seconds;
	uint32_t	nanoseconds;
} bootstrap_timing_data_vt;

typedef struct per_transmitter_data {
	uint16_t	xmtr_id;
	uint16_t	tx_time_offset;
	uint8_t		txid_injection_lvl;
	uint8_t		miso_filt_code_index;
	uint32_t	_reserved:29; //1
} per_transmitter_data_vt;

typedef struct packet_release_time {
	uint8_t		pkt_rls_seconds;
	uint16_t	pkt_rls_a_miliseconds;
	uint8_t		_reserved:2;
} packet_release_time_t;

typedef struct error_check_data {
	uint16_t	crc16;
} error_check_data_t;

typedef struct atsc3_stltp_timing_management_packet {
	atsc3_rtp_fixed_header_t* 	atsc3_rtp_fixed_header;

	uint8_t* 					payload;
	uint16_t 					payload_offset;
	uint16_t 					payload_length;
	bool						is_complete;

	timing_management_packet_t 	timing_management_packet;
	bootstrap_timing_data_vt* 	bootstrap_timing_data_v;
	per_transmitter_data_vt*	per_transmitter_data_v;
	packet_release_time_t		packet_release_time;
	error_check_data_t			error_check_data;

	udp_packet_t* 				udp_packet;
	uint32_t 					fragment_count;

} atsc3_stltp_timing_management_packet_t;



typedef struct atsc3_stltp_tunnel_packet {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel;
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner_last;

	uint8_t* outer_ip_header;
	uint32_t outer_ip_header_length;

	udp_packet_t* udp_packet_outer;
	udp_packet_t* udp_packet_inner;
	udp_packet_t* udp_packet_inner_refragmented;

	udp_packet_t* udp_packet_inner_last_fragment;


	uint32_t udp_packet_last_position;

	uint32_t fragment_count;


	atsc3_stltp_baseband_packet_t* 			atsc3_stltp_baseband_packet;
	atsc3_stltp_preamble_packet_t* 			atsc3_stltp_preamble_packet;
	atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet;

	atsc3_rtp_fixed_header_t* 				atsc3_rtp_fixed_header_payload;


} atsc3_stltp_tunnel_packet_t;


#endif /* ATSC3_STLTP_TYPES_H_ */
