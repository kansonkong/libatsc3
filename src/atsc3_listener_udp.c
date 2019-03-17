/*
 * atsc3_listener_udp.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
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
	if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {
		return NULL;
	}

	for (i = 0; i < 20; i++) {
	ip_header[i] = packet[14 + i];
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
		udp_header[i] = packet[udp_header_start + i];
	}

	udp_packet->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];

	udp_packet->raw_packet_length = pkthdr->len;
	udp_packet->data_length = pkthdr->len - (udp_header_start + 8);

	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__LISTENER_UDP_ERROR("invalid data length of udp packet: %d", udp_packet->data_length);
		return NULL;
	}
	udp_packet->data = (u_char*)malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &packet[udp_header_start + 8], udp_packet->data_length);

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
	udp_packet->data_length = raw_packet_length - (udp_header_start + 8);

	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__LISTENER_UDP_ERROR("invalid data length of udp packet: %d", udp_packet->data_length);
		return NULL;
	}
	udp_packet->data = (u_char*)malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &raw_packet[udp_header_start + 8], udp_packet->data_length);

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

	udp_packet->data_length = packet_length - (udp_header_start + 8);
	udp_packet->data_position = 0;

	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__LISTENER_UDP_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", udp_packet->data_length);
		return NULL;
	}
	udp_packet->data = (u_char*)malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &packet[udp_header_start + 8], udp_packet->data_length);

	return udp_packet;
}

udp_packet_t* udp_packet_duplicate(udp_packet_t* udp_packet) {
	udp_packet_t* udp_packet_new = calloc(1, sizeof(udp_packet_t));
	if(udp_packet->data_length && udp_packet->data) {

		udp_packet_new->data = calloc(udp_packet->data_length, sizeof(u_char));
		udp_packet_new->data_length = udp_packet->data_length;
		memcpy(udp_packet_new->data, udp_packet->data, udp_packet->data_length);
		udp_packet_new->data_position = 0;

		udp_packet_new->udp_flow.src_ip_addr = udp_packet->udp_flow.src_ip_addr;
		udp_packet_new->udp_flow.src_port	 = udp_packet->udp_flow.src_port;
		udp_packet_new->udp_flow.dst_ip_addr = udp_packet->udp_flow.dst_ip_addr;
		udp_packet_new->udp_flow.dst_port 	 = udp_packet->udp_flow.dst_port;

		return udp_packet_new;

	} else {
		return NULL;
	}
}

udp_packet_t* udp_packet_prepend_if_not_null(udp_packet_t* from_packet, udp_packet_t* to_packet) {
	if(!from_packet && !to_packet) {
		return NULL;
	}

	udp_packet_t* udp_packet_new = calloc(1, sizeof(udp_packet_t));
	assert(udp_packet_new);
	udp_packet_new->data_length = 0;

	if(from_packet) {
		udp_packet_new->data_length += udp_packet_get_remaining_bytes(from_packet);
	}

	if(to_packet) {
		udp_packet_new->data_length += udp_packet_get_remaining_bytes(to_packet);
	}

	if(!udp_packet_new->data_length) {
		free(udp_packet_new);
		return NULL;
	}

	udp_packet_new->data = calloc(udp_packet_new->data_length, sizeof(u_char));

	if(from_packet) {
		memcpy(&udp_packet_new->data[udp_packet_new->data_position], udp_packet_get_ptr(from_packet), udp_packet_get_remaining_bytes(from_packet));
		udp_packet_new->data_position += udp_packet_get_remaining_bytes(from_packet);
	}

	if(to_packet) {
		memcpy(&udp_packet_new->data[udp_packet_new->data_position], udp_packet_get_ptr(to_packet), udp_packet_get_remaining_bytes(to_packet));
		udp_packet_new->data_position += udp_packet_get_remaining_bytes(to_packet);
	}

	udp_packet_new->data_position = 0;


	udp_packet_new->udp_flow.src_ip_addr = to_packet->udp_flow.src_ip_addr;
	udp_packet_new->udp_flow.src_port	 = to_packet->udp_flow.src_port;
	udp_packet_new->udp_flow.dst_ip_addr = to_packet->udp_flow.dst_ip_addr;
	udp_packet_new->udp_flow.dst_port 	 = to_packet->udp_flow.dst_port;


	return udp_packet_new;
}

void udp_packet_free(udp_packet_t** udp_packet_p) {
	udp_packet_t* udp_packet = *udp_packet_p;

	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
	*udp_packet_p = NULL;
}


void cleanup(udp_packet_t** udp_packet_p) {
	udp_packet_t* udp_packet = *udp_packet_p;

	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
	*udp_packet_p = NULL;
}
