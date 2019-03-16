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

atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header(uint8_t* data, uint32_t size) {
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = calloc(1, sizeof(atsc3_rtp_fixed_header_t));
	//total bits needed for rtp_fixed_header == 96
	if(size < 12) {
		__STLTP_PARSER_ERROR("atsc3_stltp_parse_rtp_fixed_header, size is: %u, data: %p", size, data);
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


atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment(uint8_t* raw_packet_data, uint32_t raw_packet_length, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = atsc3_stltp_parse_rtp_fixed_header(raw_packet_data, raw_packet_length);
	assert(atsc3_rtp_fixed_header);

	//97 - tunnel packet
	if(atsc3_rtp_fixed_header->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {

		uint32_t header_packet_offset = 12;

		atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_fragment;

		//marker signifies that we may have a split payload
		if(atsc3_rtp_fixed_header->marker) {
			if(atsc3_stltp_tunnel_packet_fragment) {

				if(atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment) {
					__STLTP_PARSER_WARN("--tunnel packet: patching atsc3_stltp_tunnel_packet_fragment udp_packet_short_fragment, length is: %u, copying short fragment", atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment->data_length);
					assert(header_packet_offset > atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment->data_length);

					header_packet_offset -= atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment->data_length;

					memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment->data, atsc3_stltp_tunnel_packet_fragment->udp_packet->data_length);
					udp_packet_free(&atsc3_stltp_tunnel_packet_fragment->udp_packet_short_fragment);
				}
				//process remaining fragment

				__STLTP_PARSER_DEBUG("--tunnel packet: fragment end, marker present : %u --", ++atsc3_stltp_tunnel_packet_fragment->fragment_count);
				atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header);

				atsc3_stltp_tunnel_packet_fragment->udp_packet->data = &raw_packet_data[header_packet_offset];
				atsc3_stltp_tunnel_packet_fragment->udp_packet->data_length = atsc3_rtp_fixed_header->packet_offset;
				atsc3_stltp_tunnel_packet_fragment = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_fragment);

				if(atsc3_stltp_tunnel_packet_fragment && !atsc3_stltp_tunnel_packet_fragment->is_complete) {
					__STLTP_PARSER_DEBUG(" ----tunnel packet: fragment truncated from marker-----");

					atsc3_stltp_tunnel_packet_fragment->is_truncated_from_marker = true;
				}
			}

			__STLTP_PARSER_DEBUG("--tunnel packet: new--");

			atsc3_stltp_tunnel_packet = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));

			atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel = atsc3_rtp_fixed_header;

			header_packet_offset += atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset;
			assert(header_packet_offset  < raw_packet_length);

			atsc3_stltp_tunnel_packet->first_ip_header = &raw_packet_data[header_packet_offset];
			atsc3_stltp_tunnel_packet->first_ip_header_length = raw_packet_length - header_packet_offset;
			__STLTP_PARSER_DEBUG("  header packet offset: %u, length: %u, first_ip_header: %p",  header_packet_offset, raw_packet_length, atsc3_stltp_tunnel_packet->first_ip_header);
			atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);

			__STLTP_PARSER_DEBUG("  ip header:     0x%x", atsc3_stltp_tunnel_packet->first_ip_header[0]);

			atsc3_stltp_tunnel_packet->udp_packet = process_ip_udp_header(atsc3_stltp_tunnel_packet->first_ip_header, atsc3_stltp_tunnel_packet->first_ip_header_length);
			if(!atsc3_stltp_tunnel_packet->udp_packet) {
				__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp packet from: %p, size: %u", atsc3_stltp_tunnel_packet->first_ip_header, atsc3_stltp_tunnel_packet->first_ip_header_length);
				return NULL;
			}
			__STLTP_PARSER_DEBUG("  dst ip:port :  %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));
			__STLTP_PARSER_DEBUG("  packet length: %u ", atsc3_stltp_tunnel_packet->udp_packet->data_length);
			__STLTP_PARSER_DEBUG("  packet first byte: 0x%x ", atsc3_stltp_tunnel_packet->udp_packet->data[0]);


		} else if(atsc3_stltp_tunnel_packet_fragment){
			//fragment, so replace udp_packet->data
			__STLTP_PARSER_DEBUG("--tunnel packet: fragment: %u --", ++atsc3_stltp_tunnel_packet->fragment_count);
			//atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
			atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header);
			if(atsc3_stltp_tunnel_packet->udp_packet_short_fragment) {
				__STLTP_PARSER_WARN("--tunnel packet: patching udp_packet_short_fragment, length is: %u, copying short fragment", atsc3_stltp_tunnel_packet->udp_packet_short_fragment->data_length);
				assert(header_packet_offset > atsc3_stltp_tunnel_packet->udp_packet_short_fragment->data_length);

				header_packet_offset -= atsc3_stltp_tunnel_packet->udp_packet_short_fragment->data_length;

				memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet->udp_packet_short_fragment->data, atsc3_stltp_tunnel_packet->udp_packet->data_length);
				udp_packet_free(&atsc3_stltp_tunnel_packet->udp_packet_short_fragment);
			}

			atsc3_stltp_tunnel_packet->udp_packet->data = &raw_packet_data[header_packet_offset];
			atsc3_stltp_tunnel_packet->udp_packet->data_length = raw_packet_length - header_packet_offset;
		} else {
			__STLTP_PARSER_ERROR("--tunnel packet: RESET --");
			return NULL;
		}

		if(atsc3_stltp_tunnel_packet->udp_packet && atsc3_stltp_tunnel_packet->udp_packet->data_length >= 12) {
			atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet);
		} else if(atsc3_stltp_tunnel_packet->udp_packet) {
			__STLTP_PARSER_WARN("--tunnel packet: remaining fragment length is: %u, copying to tunnel_packet_buffer", atsc3_stltp_tunnel_packet->udp_packet->data_length);
			atsc3_stltp_tunnel_packet->udp_packet_short_fragment = udp_packet_duplicate(atsc3_stltp_tunnel_packet->udp_packet);
		} else {
			__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp_packet --");
			return NULL;

		}
		return atsc3_stltp_tunnel_packet;
	}

	return NULL;
}

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {
	//parse rtp again....
	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_payload = atsc3_stltp_tunnel_packet_fragment->atsc3_rtp_fixed_header_payload;
	if(!atsc3_rtp_fixed_header_payload) {
		atsc3_rtp_fixed_header_payload = atsc3_stltp_parse_rtp_fixed_header(atsc3_stltp_tunnel_packet_fragment->udp_packet->data, atsc3_stltp_tunnel_packet_fragment->udp_packet->data_length);
		atsc3_stltp_tunnel_packet_fragment->atsc3_rtp_fixed_header_payload = atsc3_rtp_fixed_header_payload;
		atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_payload);
	}

	if(atsc3_rtp_fixed_header_payload->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET) {
		atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_fragment, atsc3_rtp_fixed_header_payload);
	} else if(atsc3_rtp_fixed_header_payload->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET){
		atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_fragment, atsc3_rtp_fixed_header_payload);

	} else {
		__STLTP_PARSER_ERROR("Unknown payload type of 0x%2x", atsc3_rtp_fixed_header_payload->payload_type);
		return NULL;
	}

	return atsc3_stltp_tunnel_packet_fragment;
}



atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel) {

	atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet;
	if(!atsc3_stltp_baseband_packet) {
		atsc3_stltp_baseband_packet = calloc(1, sizeof(atsc3_stltp_baseband_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet = atsc3_stltp_baseband_packet;
	}

	atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_tunnel;
	uint32_t header_packet_offset = 0;
	uint32_t rtp_baseband_header_remaining_length = atsc3_stltp_tunnel_packet->udp_packet->data_length;

	if(atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet->fragment_count) {
		/**
		ATSC A/324:2018 - Section 8.3
		When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
		‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
		field will contain the total length of the Baseband Packet data structure in bytes. This allows
		the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
		 */
		header_packet_offset = 12;
		rtp_baseband_header_remaining_length -= header_packet_offset;

		uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->packet_offset; //SSRC packet length
		assert(baseband_header_packet_length < 65535);
		atsc3_stltp_baseband_packet->baseband_packet_length = baseband_header_packet_length;
		atsc3_stltp_baseband_packet->baseband_packet = calloc(baseband_header_packet_length, sizeof(uint8_t));


		__STLTP_PARSER_DEBUG(" ----baseband packet: new -----");
		__STLTP_PARSER_DEBUG("     total packet length:  %u",  atsc3_stltp_baseband_packet->baseband_packet_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:    %u",  rtp_baseband_header_remaining_length);

	} else {
		__STLTP_PARSER_DEBUG(" ----baseband packet: fragment-----");
		__STLTP_PARSER_DEBUG("     fragment %u length:   %u",   atsc3_stltp_baseband_packet->fragment_count, rtp_baseband_header_remaining_length);
	}

	assert(rtp_baseband_header_remaining_length + atsc3_stltp_baseband_packet->baseband_packet_offset <= atsc3_stltp_baseband_packet->baseband_packet_length);

	atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet->fragment_count++;
	memcpy(&atsc3_stltp_baseband_packet->baseband_packet[atsc3_stltp_baseband_packet->baseband_packet_offset], &atsc3_stltp_tunnel_packet->udp_packet->data[header_packet_offset], rtp_baseband_header_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_baseband_packet->baseband_packet[atsc3_stltp_baseband_packet->baseband_packet_offset], atsc3_stltp_baseband_packet->baseband_packet[atsc3_stltp_baseband_packet->baseband_packet_offset+1]);
	atsc3_stltp_baseband_packet->baseband_packet_offset += rtp_baseband_header_remaining_length;
	__STLTP_PARSER_DEBUG("     packet_offset: %u, baseband_packet_length: %u", atsc3_stltp_baseband_packet->baseband_packet_offset, atsc3_stltp_baseband_packet->baseband_packet_length);

	if(atsc3_stltp_baseband_packet->baseband_packet_offset == atsc3_stltp_baseband_packet->baseband_packet_length) {
		__STLTP_PARSER_DEBUG(" ----baseband packet: complete-----");
		atsc3_stltp_tunnel_packet->is_complete = true;
	}

	return atsc3_stltp_baseband_packet;
}



atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel) {

	atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet;
	if(!atsc3_stltp_preamble_packet) {
		atsc3_stltp_preamble_packet = calloc(1, sizeof(atsc3_stltp_preamble_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet;
	}

	atsc3_stltp_preamble_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_tunnel;
	uint32_t header_packet_offset = 0;
	uint32_t rtp_preamble_header_remaining_length = atsc3_stltp_tunnel_packet->udp_packet->data_length;

	if(atsc3_stltp_preamble_packet->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet->fragment_count) {
		//read the first uint16_t for our preamble length
		header_packet_offset = 12;
		rtp_preamble_header_remaining_length -= header_packet_offset;
		atsc3_stltp_preamble_packet->preamble_payload_length = ntohs(*((uint16_t*)(&atsc3_stltp_tunnel_packet->udp_packet->data[header_packet_offset])));
		__STLTP_PARSER_DEBUG(" ----preamble packet: new -----");
		__STLTP_PARSER_DEBUG("     total packet length:  %u",  atsc3_stltp_preamble_packet->preamble_payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:    %u",  rtp_preamble_header_remaining_length);


		uint32_t preamble_header_packet_length = atsc3_stltp_preamble_packet->preamble_payload_length;
		assert(preamble_header_packet_length < 65535);
		atsc3_stltp_preamble_packet->preamble_payload = calloc(preamble_header_packet_length, sizeof(uint8_t));

	} else {
		__STLTP_PARSER_DEBUG(" ----preamble packet: fragment -----");
		__STLTP_PARSER_DEBUG("     fragment %u length:   %u",   atsc3_stltp_preamble_packet->fragment_count, rtp_preamble_header_remaining_length);

	}


	//	assert(rtp_baseband_header_remaining_length + atsc3_stltp_baseband_packet->baseband_packet_offset <= atsc3_stltp_baseband_packet->baseband_packet_length);
	//
	//	atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet->fragment_count++;

	atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet->fragment_count++;
	memcpy(&atsc3_stltp_preamble_packet->preamble_payload[atsc3_stltp_preamble_packet->preamble_payload_offset], &atsc3_stltp_tunnel_packet->udp_packet->data[header_packet_offset], rtp_preamble_header_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_preamble_packet->preamble_payload[atsc3_stltp_preamble_packet->preamble_payload_offset], atsc3_stltp_preamble_packet->preamble_payload[atsc3_stltp_preamble_packet->preamble_payload_offset+1]);
	atsc3_stltp_preamble_packet->preamble_payload_offset += rtp_preamble_header_remaining_length;
	__STLTP_PARSER_DEBUG("     packet_offset: %u, preamble_payload_length: %u", atsc3_stltp_preamble_packet->preamble_payload_offset, atsc3_stltp_preamble_packet->preamble_payload_length);

	if(atsc3_stltp_preamble_packet->preamble_payload_offset == atsc3_stltp_preamble_packet->preamble_payload_length) {
		__STLTP_PARSER_DEBUG(" ----preamble packet: complete-----");
		atsc3_stltp_tunnel_packet->is_complete = true;
		//todo - parse thsi into proper l1_...strucuts

	}



	return atsc3_stltp_preamble_packet;

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


