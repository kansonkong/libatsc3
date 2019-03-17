/*
 * atsc3_listener_udp.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */
#include <pcap.h>

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "atsc3_utils.h"

#ifndef ATSC3_LISTENER_UDP_H_
#define ATSC3_LISTENER_UDP_H_

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_PCAP_LEN 1514

typedef struct udp_flow {
	uint32_t		src_ip_addr;
	uint32_t		dst_ip_addr;
	uint16_t		src_port;
	uint16_t		dst_port;
} udp_flow_t;

typedef struct udp_packet {
	udp_flow_t		udp_flow;

	//inherit from libpcap type usage
	u_char* 		data;
	int 			data_length;
	int				data_position;

	//internals
	int				raw_packet_length;

} udp_packet_t;

#define udp_packet_get_remaining_bytes(udp_packet) (__MAX(0, udp_packet->data_length - udp_packet->data_position ))
#define udp_packet_get_ptr(udp_packet) (udp_packet->data_position < udp_packet->data_length ? &udp_packet->data[udp_packet->data_position] : NULL)
#define udp_packet_get_ptr_offset(udp_packet, offset) (udp_packet->data_position + offset < udp_packet->data_length ? &udp_packet->data[udp_packet->data_position + offset] : NULL)
#define udp_packet_seek_offset(udp_packet, offset) (udp_packet->data_position + offset < udp_packet->data_length ? udp_packet->data_position += offset : -1)
#define udp_packet_seek(udp_packet, position) (position < udp_packet->data_length ? udp_packet->data_position = position : -1)

//dump packet example
//__STLTP_PARSER_DEBUG("dst ip:port : %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));


udp_packet_t* process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);
udp_packet_t* udp_packet_process_from_ptr_raw_ethernet_packet(uint8_t* packet, uint32_t packet_length);

udp_packet_t* udp_packet_process_from_ptr(uint8_t* packet, uint32_t packet_length);
udp_packet_t* udp_packet_duplicate(udp_packet_t* udp_packet);
udp_packet_t* udp_packet_prepend_if_not_null(udp_packet_t* from_packet, udp_packet_t* to_packet);

void udp_packet_free(udp_packet_t** udp_packet_p);
void cleanup(udp_packet_t** udp_packet_p);


#define __LISTENER_UDP_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#ifdef __cplusplus
}
#endif
#endif /* ATSC3_LISTENER_UDP_H_ */
