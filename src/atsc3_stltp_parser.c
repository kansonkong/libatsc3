/*
 * atsc3_stltp_parser.c
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 *
 *      https://tools.ietf.org/html/rfc3550
 *
 *
 */


#include "atsc3_stltp_parser.h"

int _STLTP_PARSER_DEBUG_ENABLED = 1;

atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header(udp_packet_t* udp_packet) {

	//total bits needed for rtp_fixed_header == 96, so this is a fragment
	if(!udp_packet) {
		__STLTP_PARSER_ERROR("fragment: atsc3_stltp_parse_rtp_fixed_header, udp_packet is null!");
		return NULL;
	}
	if(udp_packet_get_remaining_bytes(udp_packet) < 12) {
		__STLTP_PARSER_WARN("fragment: atsc3_stltp_parse_rtp_fixed_header, position: %u, size is: %u, data: %p", udp_packet->data_position, udp_packet->data_length, udp_packet->data);
		return NULL;
	}

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = calloc(1, sizeof(atsc3_rtp_fixed_header_t));
	uint8_t* data = udp_packet_get_ptr(udp_packet);
	assert(data);

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

	udp_packet->data_position += 12;
	return atsc3_rtp_fixed_header;
}


udp_packet_t* atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
	udp_packet_seek(atsc3_stltp_tunnel_packet->udp_packet_outer, atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset + 12);

	__STLTP_PARSER_DEBUG("seeking udp_packet_inner: %p, to: %u", atsc3_stltp_tunnel_packet->udp_packet_inner, atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset);

	udp_packet_t* udp_packet_new = udp_packet_prepend_if_not_null(atsc3_stltp_tunnel_packet->udp_packet_inner_last_fragment, atsc3_stltp_tunnel_packet->udp_packet_inner);
	return udp_packet_new;
}


udp_packet_t* atsc3_stltp_udp_packet_fragment_check_marker(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
	udp_packet_seek(atsc3_stltp_tunnel_packet->udp_packet_outer, atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset + 12);

	if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->marker) {
		__STLTP_PARSER_DEBUG("seeking udp_packet_outer: %p, to: %u", atsc3_stltp_tunnel_packet->udp_packet_outer, atsc3_stltp_tunnel_packet->udp_packet_outer->data_position);

		atsc3_stltp_tunnel_packet->udp_packet_inner = udp_packet_process_from_ptr(udp_packet_get_ptr(atsc3_stltp_tunnel_packet->udp_packet_outer), udp_packet_get_remaining_bytes(atsc3_stltp_tunnel_packet->udp_packet_outer));
		if(!atsc3_stltp_tunnel_packet->udp_packet_inner) {
			__STLTP_PARSER_ERROR("unable to parse outer packet");
			return NULL;
		}
	} else {
		atsc3_stltp_tunnel_packet->udp_packet_inner = udp_packet_duplicate(atsc3_stltp_tunnel_packet->udp_packet_outer);
	}
	return atsc3_stltp_tunnel_packet->udp_packet_inner;
}




atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet(udp_packet_t* udp_packet, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {
	__STLTP_PARSER_DEBUG(" ----atsc3_stltp_tunnel_packet_extract_fragment, size: %u, pkt: %p-----", udp_packet->data_length, udp_packet->data);

	atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_fragment;

	if(!atsc3_stltp_tunnel_packet) {
		atsc3_stltp_tunnel_packet = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));
	}

	atsc3_stltp_tunnel_packet->udp_packet_outer = udp_packet_duplicate(udp_packet);
	if(!atsc3_stltp_tunnel_packet->udp_packet_outer) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_fragment: atsc3_stltp_tunnel_packet->udp_packet_outer is null");
		return NULL;

	}
	atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel = atsc3_stltp_parse_rtp_fixed_header(atsc3_stltp_tunnel_packet->udp_packet_outer);
	assert(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
	atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);

	//97 - tunnel packet
	if(!atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel || atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->payload_type != ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_fragment: unknown tunnel packet: %u", (atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel ? atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->payload_type : -1));
		return NULL;
	}

	atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented = atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet);


	//todo - match by payload_type
	if(atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented && atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented->data_length > atsc3_stltp_tunnel_packet->udp_packet_inner->data_length &&
			atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last) {

		//re-fragment
		__STLTP_PARSER_WARN("--tunnel packet: processing from fragment: %u, length is: %u", atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last->payload_type, udp_packet_get_remaining_bytes(atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented));

		atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last);

		atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented, atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last);
	//out of data for refragmenting
		if(udp_packet_get_remaining_bytes(atsc3_stltp_tunnel_packet->udp_packet_inner_refragmented) <= 12) {
			return atsc3_stltp_tunnel_packet;
		}
	}



	atsc3_stltp_udp_packet_fragment_check_marker(atsc3_stltp_tunnel_packet);

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner = NULL;
	while(udp_packet_get_remaining_bytes(atsc3_stltp_tunnel_packet->udp_packet_inner) > 12) {

		//parse header
		atsc3_rtp_fixed_header_inner = atsc3_stltp_parse_rtp_fixed_header(atsc3_stltp_tunnel_packet->udp_packet_inner);

		if(atsc3_rtp_fixed_header_inner) {
			atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_inner);

			__STLTP_PARSER_DEBUG("--tunnel packet: processing from fragment: %u, length is: %u", atsc3_rtp_fixed_header_inner->payload_type, udp_packet_get_remaining_bytes(atsc3_stltp_tunnel_packet->udp_packet_inner));

			atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, atsc3_stltp_tunnel_packet->udp_packet_inner, atsc3_rtp_fixed_header_inner);
		} else {
			__STLTP_PARSER_ERROR("--tunnel packet: error processing udp packet inner");
		}
	}

	if(atsc3_rtp_fixed_header_inner) {

		atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last = atsc3_rtp_fixed_header_inner;
	}

	return atsc3_stltp_tunnel_packet;

//
//		//marker signifies that we may have a split payload
//		if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header->marker) {
//			if(atsc3_stltp_tunnel_packet_fragment && !atsc3_stltp_tunnel_packet_fragment->is_complete) {
//
//				if(atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment) {
//					assert(header_packet_offset > atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data_length);
//
//					header_packet_offset -= atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data_length;
//
//					memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data, atsc3_stltp_tunnel_packet_fragment->udp_packet->data_length);
//					udp_packet_free(&atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment);
//				}
//				//process remaining fragment
//
//				__STLTP_PARSER_DEBUG("--tunnel packet: fragment end, marker present : %u --", ++atsc3_stltp_tunnel_packet_fragment->fragment_count);
//
//				if(atsc3_stltp_tunnel_packet_fragment && !atsc3_stltp_tunnel_packet_fragment->is_complete) {
//					__STLTP_PARSER_DEBUG(" ----tunnel packet: fragment truncated from marker-----");
//
//					atsc3_stltp_tunnel_packet_fragment->is_truncated_from_marker = true;
//				}
//
//
//				__STLTP_PARSER_WARN("--tunnel packet: marker: remaining fragment length is: %u, position is: %u", atsc3_stltp_tunnel_packet->udp_packet->data_length, atsc3_stltp_tunnel_packet->udp_packet_last_position);
//
////			}
//
//			__STLTP_PARSER_DEBUG("--tunnel packet: marker --");
//
//			header_packet_offset += atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset;
//			assert(header_packet_offset  < raw_packet_length);
//
//			atsc3_stltp_tunnel_packet->outer_ip_header = &raw_packet_data[header_packet_offset];
//			atsc3_stltp_tunnel_packet->outer_ip_header_length = raw_packet_length - header_packet_offset;
//			__STLTP_PARSER_DEBUG("  header packet offset: %u, length: %u, first_ip_header: %p",  header_packet_offset, raw_packet_length, atsc3_stltp_tunnel_packet->outer_ip_header);
//			atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
//
//			__STLTP_PARSER_DEBUG("  ip header:     0x%x", atsc3_stltp_tunnel_packet->outer_ip_header[0]);
//
//			atsc3_stltp_tunnel_packet->udp_packet = udp_packet_process_from_ptr(atsc3_stltp_tunnel_packet->outer_ip_header, atsc3_stltp_tunnel_packet->outer_ip_header_length);
//			if(!atsc3_stltp_tunnel_packet->udp_packet) {
//				__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp packet from: %p, size: %u", atsc3_stltp_tunnel_packet->outer_ip_header, atsc3_stltp_tunnel_packet->outer_ip_header_length);
//				return NULL;
//			}
//			__STLTP_PARSER_DEBUG("  dst ip:port :  %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));
//			__STLTP_PARSER_DEBUG("  packet length: %u ", atsc3_stltp_tunnel_packet->udp_packet->data_length);
//			__STLTP_PARSER_DEBUG("  packet first byte: 0x%x ", atsc3_stltp_tunnel_packet->udp_packet->data[0]);
//
//
//		} else if(atsc3_stltp_tunnel_packet_fragment){
//			//fragment, so replace udp_packet->data
//			__STLTP_PARSER_DEBUG("--tunnel packet: fragment: %u --", ++atsc3_stltp_tunnel_packet->fragment_count);
//			//atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
//			atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header);
//			if(atsc3_stltp_tunnel_packet->udp_packet_last_fragment) {
//				__STLTP_PARSER_WARN("--tunnel packet: patching udp_packet_short_fragment, length is: %u, copying short fragment", atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length);
//				assert(header_packet_offset > atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length);
//
//				header_packet_offset -= atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length;
//
//				memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data, atsc3_stltp_tunnel_packet->udp_packet->data_length);
//				udp_packet_free(&atsc3_stltp_tunnel_packet->udp_packet_last_fragment);
//			}
//
//			atsc3_stltp_tunnel_packet->udp_packet->data = &raw_packet_data[header_packet_offset];
//			atsc3_stltp_tunnel_packet->udp_packet->data_length = raw_packet_length - header_packet_offset;
//		} else {
//			__STLTP_PARSER_ERROR("--tunnel packet: RESET --");
//			return NULL;
//		}
//
//		if(atsc3_stltp_tunnel_packet->udp_packet && atsc3_stltp_tunnel_packet->udp_packet->data_length >= 12) {
//			atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet);
//			__STLTP_PARSER_WARN("--tunnel packet: remaining fragment length is: %u, position is: %u", atsc3_stltp_tunnel_packet->udp_packet->data_length, atsc3_stltp_tunnel_packet->udp_packet_last_position);
//
//		} else if(atsc3_stltp_tunnel_packet->udp_packet) {
//			__STLTP_PARSER_WARN("--tunnel packet: remaining fragment length is: %u, copying to tunnel_packet_buffer", atsc3_stltp_tunnel_packet->udp_packet->data_length);
//			atsc3_stltp_tunnel_packet->udp_packet_last_fragment = udp_packet_duplicate(atsc3_stltp_tunnel_packet->udp_packet);
//		} else {
//			__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp_packet --");
//			return NULL;
//
//		}
//		return atsc3_stltp_tunnel_packet;
//	}
//
//	return NULL;
}

//atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {

//parse rtp again....

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, udp_packet_t* udp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {


	if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET) {
		atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet, udp_packet_inner, atsc3_rtp_fixed_header_inner);
	} else if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET){
		atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet, udp_packet_inner, atsc3_rtp_fixed_header_inner);

	} else if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET) {
		atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet, udp_packet_inner, atsc3_rtp_fixed_header_inner);
	} else {
		__STLTP_PARSER_ERROR("Unknown payload type of 0x%2x", atsc3_rtp_fixed_header_inner->payload_type);
		return NULL;
	}

	return atsc3_stltp_tunnel_packet;
}



//atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel) {
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, udp_packet_t* udp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {

	atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet;
	if(!atsc3_stltp_baseband_packet) {
		atsc3_stltp_baseband_packet = calloc(1, sizeof(atsc3_stltp_baseband_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet = atsc3_stltp_baseband_packet;
	}

	atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
	uint32_t header_packet_offset = 0;
	uint32_t rtp_baseband_header_remaining_length = udp_packet_get_remaining_bytes(udp_packet_inner);

	if(atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet->fragment_count) {
		/**
		ATSC A/324:2018 - Section 8.3
		When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
		‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
		field will contain the total length of the Baseband Packet data structure in bytes. This allows
		the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
		 */

		uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->packet_offset; //SSRC packet length
		assert(baseband_header_packet_length < 65535);
		atsc3_stltp_baseband_packet->payload_length = baseband_header_packet_length;
		atsc3_stltp_baseband_packet->payload = calloc(baseband_header_packet_length, sizeof(uint8_t));


		__STLTP_PARSER_DEBUG(" ----baseband packet: new -----");
		__STLTP_PARSER_DEBUG("     total packet length:  %u",  atsc3_stltp_baseband_packet->payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:    %u",  rtp_baseband_header_remaining_length);

	} else {
		__STLTP_PARSER_DEBUG(" ----baseband packet: fragment-----");
		__STLTP_PARSER_DEBUG("     fragment %u length:   %u",   atsc3_stltp_baseband_packet->fragment_count, rtp_baseband_header_remaining_length);
	}

	assert(rtp_baseband_header_remaining_length + atsc3_stltp_baseband_packet->payload_offset <= atsc3_stltp_baseband_packet->payload_length);

	atsc3_stltp_baseband_packet->fragment_count++;
	memcpy(&atsc3_stltp_baseband_packet->payload[atsc3_stltp_baseband_packet->payload_offset], udp_packet_get_ptr(udp_packet_inner), rtp_baseband_header_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_baseband_packet->payload[atsc3_stltp_baseband_packet->payload_offset], atsc3_stltp_baseband_packet->payload[atsc3_stltp_baseband_packet->payload_offset+1]);
	atsc3_stltp_baseband_packet->payload_offset += rtp_baseband_header_remaining_length;
	__STLTP_PARSER_DEBUG("     packet_offset: %u, baseband_packet_length: %u", atsc3_stltp_baseband_packet->payload_offset, atsc3_stltp_baseband_packet->payload_length);

	if(atsc3_stltp_baseband_packet->payload_offset == atsc3_stltp_baseband_packet->payload_length) {
		//todo - parse ALP payloads
		__STLTP_PARSER_DEBUG(" ----baseband packet: complete-----");
		atsc3_stltp_baseband_packet->is_complete = true;
	}
	udp_packet_seek(udp_packet_inner, header_packet_offset + rtp_baseband_header_remaining_length);

	return atsc3_stltp_baseband_packet;
}



atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, udp_packet_t* udp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {
	atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet;
	if(!atsc3_stltp_preamble_packet) {
		atsc3_stltp_preamble_packet = calloc(1, sizeof(atsc3_stltp_preamble_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet;
	}

	atsc3_stltp_preamble_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
	uint32_t header_packet_offset = 0;
	uint32_t rtp_preamble_header_remaining_length = udp_packet_get_remaining_bytes(udp_packet_inner);

	if(atsc3_stltp_preamble_packet->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet->fragment_count) {
		//read the first uint16_t for our preamble length

		atsc3_stltp_preamble_packet->payload_length = ntohs(*((uint16_t*)(udp_packet_get_ptr(udp_packet_inner))));
		__STLTP_PARSER_DEBUG(" ----preamble packet: new -----");
		__STLTP_PARSER_DEBUG("     preamble length:    %u",  atsc3_stltp_preamble_packet->payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:  %u",  rtp_preamble_header_remaining_length);


		uint32_t preamble_header_packet_length = atsc3_stltp_preamble_packet->payload_length;
		assert(preamble_header_packet_length < 65535);
		atsc3_stltp_preamble_packet->payload = calloc(preamble_header_packet_length, sizeof(uint8_t));

	} else {
		__STLTP_PARSER_DEBUG(" ----preamble packet: fragment -----");
		__STLTP_PARSER_DEBUG("     fragment: %u length: %u",   atsc3_stltp_preamble_packet->fragment_count, rtp_preamble_header_remaining_length);
	}

	if(atsc3_stltp_preamble_packet->payload_length > atsc3_stltp_preamble_packet->payload_offset) {
		rtp_preamble_header_remaining_length = __MIN(rtp_preamble_header_remaining_length, atsc3_stltp_preamble_packet->payload_length - atsc3_stltp_preamble_packet->payload_offset);
	}
	__STLTP_PARSER_DEBUG("     remaining length: %u",   rtp_preamble_header_remaining_length);

	atsc3_stltp_preamble_packet->fragment_count++;
	memcpy(&atsc3_stltp_preamble_packet->payload[atsc3_stltp_preamble_packet->payload_offset], udp_packet_get_ptr(udp_packet_inner), rtp_preamble_header_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_preamble_packet->payload[atsc3_stltp_preamble_packet->payload_offset], atsc3_stltp_preamble_packet->payload[atsc3_stltp_preamble_packet->payload_offset+1]);
	atsc3_stltp_preamble_packet->payload_offset += rtp_preamble_header_remaining_length;
	__STLTP_PARSER_DEBUG("     packet_offset: %u, preamble_payload_length: %u", atsc3_stltp_preamble_packet->payload_offset, atsc3_stltp_preamble_packet->payload_length);

	if(atsc3_stltp_preamble_packet->payload_offset == atsc3_stltp_preamble_packet->payload_length) {

		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----preamble packet: complete-----");
		atsc3_stltp_preamble_packet->is_complete = true;

	}
	udp_packet_seek(udp_packet_inner, header_packet_offset + rtp_preamble_header_remaining_length);

	return atsc3_stltp_preamble_packet;

}



atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, udp_packet_t* udp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {
	atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet;
	if(!atsc3_stltp_timing_management_packet) {
		atsc3_stltp_timing_management_packet = calloc(1, sizeof(atsc3_stltp_timing_management_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet = atsc3_stltp_timing_management_packet;
	}

	atsc3_stltp_timing_management_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
	uint32_t header_packet_offset = 0;
	uint32_t rtp_timing_management_header_remaining_length = udp_packet_get_remaining_bytes(udp_packet_inner);

	if(atsc3_stltp_timing_management_packet->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet->fragment_count) {
		//read the first uint16_t for our preamble length

		atsc3_stltp_timing_management_packet->payload_length = ntohs(*((uint16_t*)(udp_packet_get_ptr(udp_packet_inner))));
		__STLTP_PARSER_DEBUG(" ----preamble packet: new -----");
		__STLTP_PARSER_DEBUG("     preamble length:    %u",  atsc3_stltp_timing_management_packet->payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:  %u",  rtp_timing_management_header_remaining_length);


		uint32_t preamble_header_packet_length = atsc3_stltp_timing_management_packet->payload_length;
		assert(preamble_header_packet_length < 65535);
		atsc3_stltp_timing_management_packet->payload = calloc(preamble_header_packet_length, sizeof(uint8_t));

	} else {
		__STLTP_PARSER_DEBUG(" ----preamble packet: fragment -----");
		__STLTP_PARSER_DEBUG("     fragment: %u length: %u",   atsc3_stltp_timing_management_packet->fragment_count, rtp_timing_management_header_remaining_length);
	}

	if(atsc3_stltp_timing_management_packet->payload_length > atsc3_stltp_timing_management_packet->payload_offset) {
		rtp_timing_management_header_remaining_length = __MIN(rtp_timing_management_header_remaining_length, atsc3_stltp_timing_management_packet->payload_length - atsc3_stltp_timing_management_packet->payload_offset);
	}
	__STLTP_PARSER_DEBUG("     remaining length: %u",   rtp_timing_management_header_remaining_length);

	atsc3_stltp_timing_management_packet->fragment_count++;
	memcpy(&atsc3_stltp_timing_management_packet->payload[atsc3_stltp_timing_management_packet->payload_offset], udp_packet_get_ptr(udp_packet_inner), rtp_timing_management_header_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_timing_management_packet->payload[atsc3_stltp_timing_management_packet->payload_offset], atsc3_stltp_timing_management_packet->payload[atsc3_stltp_timing_management_packet->payload_offset+1]);
	atsc3_stltp_timing_management_packet->payload_offset += rtp_timing_management_header_remaining_length;
	__STLTP_PARSER_DEBUG("     packet_offset: %u, preamble_payload_length: %u", atsc3_stltp_timing_management_packet->payload_offset, atsc3_stltp_timing_management_packet->payload_length);

	if(atsc3_stltp_timing_management_packet->payload_offset == atsc3_stltp_timing_management_packet->payload_length) {

		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----preamble packet: complete-----");
		atsc3_stltp_timing_management_packet->is_complete = true;

	}
	udp_packet_seek(udp_packet_inner, header_packet_offset + rtp_timing_management_header_remaining_length);

	return atsc3_stltp_timing_management_packet;

}

void atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header) {

	__STLTP_PARSER_DEBUG("version:         %x", atsc3_rtp_fixed_header->version);
	__STLTP_PARSER_DEBUG("padding:         %x", atsc3_rtp_fixed_header->padding);
	__STLTP_PARSER_DEBUG("extension:       %x", atsc3_rtp_fixed_header->extension);
	__STLTP_PARSER_DEBUG("csrc_count:      %x", atsc3_rtp_fixed_header->csrc_count);
	__STLTP_PARSER_DEBUG("marker:          %x", atsc3_rtp_fixed_header->marker);
	__STLTP_PARSER_DEBUG("payload_type:    0x%x (%hhu)", atsc3_rtp_fixed_header->payload_type, 	atsc3_rtp_fixed_header->payload_type);
	__STLTP_PARSER_DEBUG("sequence_number: 0x%x (%u)", atsc3_rtp_fixed_header->sequence_number, atsc3_rtp_fixed_header->sequence_number);
	__STLTP_PARSER_DEBUG("timestamp:       0x%x (%u)", atsc3_rtp_fixed_header->timestamp, 		atsc3_rtp_fixed_header->timestamp);
	__STLTP_PARSER_DEBUG("packet_offset:   0x%x (%u)", atsc3_rtp_fixed_header->packet_offset, 	atsc3_rtp_fixed_header->packet_offset);

}


