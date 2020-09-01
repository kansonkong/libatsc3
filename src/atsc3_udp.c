/*
 * atsc3_udp.c
 *
 *  Created on: Aug 31, 2020
 *      Author: jjustman
 */


#include "atsc3_udp.h"


int _ATSC3_UDP_INFO_ENABLED = 0;
int _ATSC3_UDP_DEBUG_ENABLED = 0;
int _ATSC3_UDP_TRACE_ENABLED = 0;

//process ip header and copy packet data
atsc3_udp_packet_t* atsc3_udp_packet_from_block_t(block_t* block_udp_packet) {
	if(!block_udp_packet) {
		return NULL;
	}

	block_Rewind(block_udp_packet);
	uint8_t* packet = block_Get(block_udp_packet);
	uint32_t packet_length = block_Remaining_size(block_udp_packet);

	int i = 0;
	int k = 0;
	u_char ip_header[24];
	u_char udp_header[8];
	int udp_header_start = 20;

	atsc3_udp_packet_t* udp_packet = NULL;

	for (i = 0; i < udp_header_start; i++) {
		ip_header[i] = packet[i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		_ATSC3_UDP_ERROR("udp_packet_process_from_ptr: not a UDP packet!");
		return NULL;
	}

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 28;
	}

	//malloc our udp_packet_header:
	udp_packet = (udp_packet_t*)calloc(1, sizeof(udp_packet_t));
	udp_packet->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	udp_packet->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = packet[udp_header_start + i];
	}

	udp_packet->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];

	uint32_t data_length = packet_length - (udp_header_start + 8);

	if(data_length <=0 || data_length > MAX_ATSC3_PHY_IP_DATAGRAM_SIZE) {
		_ATSC3_UDP_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", data_length);
		return NULL;
	}

	udp_packet->data = block_Alloc(data_length);
	block_Write(udp_packet->data, (uint8_t*)&packet[udp_header_start + 8], data_length);
    block_Rewind(udp_packet->data);

	return udp_packet;
}

void udp_packet_free(udp_packet_t** udp_packet_p) {
	atsc3_udp_packet_free(udp_packet_p);
}

void atsc3_udp_packet_free(atsc3_udp_packet_t** udp_packet_p) {
	if(udp_packet_p) {
		udp_packet_t* udp_packet = *udp_packet_p;
		if(udp_packet) {

			if(udp_packet->data) {
				block_Destroy(&udp_packet->data);
			}
			freesafe(udp_packet);
			udp_packet = NULL;
		}
		*udp_packet_p = NULL;
	}
}

