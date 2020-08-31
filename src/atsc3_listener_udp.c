/*
 * atsc3_listener_udp.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 *
 * 2019-08-10 - refactor to using block_t for data payload management
 */

#include "atsc3_listener_udp.h"

udp_packet_t* process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	int i = 0;
	int k = 0;
	u_char ethernet_packet[14];
	u_char ip_header[24];
	u_char udp_header[8];
	int udp_header_start = 34;
	udp_packet_t* udp_packet = NULL;

	for (i = 0; i < 14; i++) {
		ethernet_packet[i] = packet[0 + i];
	}
	//workaround for airwavz pcaps with ethertype=0xc0a8
    if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {
        __LISTENER_UDP_ERROR("udp_packet_process_from_ptr: invalid ethernet frame, expected 0x08 0x00, ethernet_packet[12]=0x%02x, [13]=0x%02x", ethernet_packet[12], ethernet_packet[13]);
		return NULL;
	}

	for (i = 0; i < 20; i++) {
        ip_header[i] = packet[14 + i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: not a UDP packet! ip_header[9]=0x%02x, len: %d, caplen: %d", ip_header[9], pkthdr->len, pkthdr->caplen);
		return NULL;
	}

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 48;
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
	udp_packet->raw_packet_length = pkthdr->len;
    
    uint32_t data_length = pkthdr->len - (udp_header_start + 8);
    if(data_length <=0 || data_length > MAX_ATSC3_PHY_IP_DATAGRAM_SIZE) {
        __LISTENER_UDP_ERROR("process_packet_from_pcap: invalid udp data length: %d, raw phy frame: %d", data_length, udp_packet->raw_packet_length);
        freesafe(udp_packet);
        return NULL;
    }
    
    udp_packet->data = block_Alloc(data_length);
    block_Write(udp_packet->data, (uint8_t*)&packet[udp_header_start + 8], data_length);
    block_Rewind(udp_packet->data);
    return udp_packet;
}

udp_packet_t* udp_packet_process_from_ptr_raw_ethernet_packet(uint8_t* raw_packet, uint32_t raw_packet_length) {

	int i = 0;
	int k = 0;
	u_char ethernet_packet[14];
	u_char ip_header[24];
	u_char udp_header[8];
	int udp_header_start = 34;
	udp_packet_t* udp_packet = NULL;

	for (i = 0; i < 14; i++) {
		ethernet_packet[i] = raw_packet[0 + i];
	}
	if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: invalid ethernet frame");
		return NULL;
	}

	for (i = 0; i < 20; i++) {
		ip_header[i] = raw_packet[14 + i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: not a UDP packet!");

		return NULL;
	}

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 48;
	}

	//malloc our udp_packet_header:
	udp_packet = (udp_packet_t*)calloc(1, sizeof(udp_packet_t));
	udp_packet->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	udp_packet->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = raw_packet[udp_header_start + i];
	}

	udp_packet->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];

	udp_packet->raw_packet_length = raw_packet_length;
	uint32_t data_length = raw_packet_length - (udp_header_start + 8);

	 if(data_length <=0 || data_length > MAX_ATSC3_PHY_IP_DATAGRAM_SIZE) {
		__LISTENER_UDP_ERROR("process_packet_from_pcap: invalid udp data length: %d, raw phy frame: %d", data_length, udp_packet->raw_packet_length);
		freesafe(udp_packet);
		return NULL;
	}

	udp_packet->data = block_Alloc(data_length);
    block_Write(udp_packet->data, (uint8_t*)&raw_packet[udp_header_start + 8], data_length);
    block_Rewind(udp_packet->data);

	return udp_packet;
}

//process ip header and copy packet data
udp_packet_t* udp_packet_process_from_ptr(uint8_t* packet, uint32_t packet_length) {
	int i = 0;
	int k = 0;
	u_char ip_header[24];
	u_char udp_header[8];
	int udp_header_start = 20;
	udp_packet_t* udp_packet = NULL;

	for (i = 0; i < udp_header_start; i++) {
		ip_header[i] = packet[i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: not a UDP packet!");
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
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", data_length);
		return NULL;
	}

	udp_packet->data = block_Alloc(data_length);
	block_Write(udp_packet->data, (uint8_t*)&packet[udp_header_start + 8], data_length);
    block_Rewind(udp_packet->data);

	return udp_packet;
}

udp_packet_t* udp_packet_duplicate(udp_packet_t* udp_packet) {
	if(udp_packet && udp_packet->data) {
		udp_packet_t* udp_packet_new = calloc(1, sizeof(udp_packet_t));
		udp_packet_new->raw_packet_length 		= udp_packet->raw_packet_length;
		udp_packet_new->udp_flow.src_ip_addr 	= udp_packet->udp_flow.src_ip_addr;
		udp_packet_new->udp_flow.src_port	 	= udp_packet->udp_flow.src_port;
		udp_packet_new->udp_flow.dst_ip_addr 	= udp_packet->udp_flow.dst_ip_addr;
		udp_packet_new->udp_flow.dst_port 	 	= udp_packet->udp_flow.dst_port;
		udp_packet_new->data 					= block_Duplicate(udp_packet->data);
		block_Rewind(udp_packet_new->data);

		return udp_packet_new;
	} else {
		return NULL;
	}
}

udp_packet_t* udp_packet_prepend_if_not_null(udp_packet_t* from_packet, udp_packet_t* to_packet) {
	if(!from_packet && !to_packet) {
		return NULL;
	}

	uint32_t new_payload_size = 0;
	udp_packet_t* udp_packet_new = calloc(1, sizeof(udp_packet_t));
	assert(udp_packet_new);


	if(from_packet) {
		new_payload_size += block_Remaining_size(from_packet->data);
	}

	if(to_packet) {
		new_payload_size += block_Remaining_size(to_packet->data);
	}

	if(!new_payload_size) {
		free(udp_packet_new);
		return NULL;
	}

	udp_packet_new->data = block_Alloc(new_payload_size);

	if(from_packet) {
		block_Write(udp_packet_new->data, block_Get(from_packet->data), block_Remaining_size(from_packet->data));
	}

	if(to_packet) {
		block_Write(udp_packet_new->data, block_Get(to_packet->data), block_Remaining_size(to_packet->data));
	}
    block_Rewind(udp_packet_new->data);

	udp_packet_new->raw_packet_length = udp_packet_new->data->p_size;
    
	udp_packet_new->udp_flow.src_ip_addr = to_packet->udp_flow.src_ip_addr;
	udp_packet_new->udp_flow.src_port	 = to_packet->udp_flow.src_port;
	udp_packet_new->udp_flow.dst_ip_addr = to_packet->udp_flow.dst_ip_addr;
	udp_packet_new->udp_flow.dst_port 	 = to_packet->udp_flow.dst_port;

	return udp_packet_new;
}

