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

#ifndef ATSC3_LISTENER_UDP_H_
#define ATSC3_LISTENER_UDP_H_

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
	int 			data_length;
	u_char* 		data;
	int				total_packet_length;

} udp_packet_t;

udp_packet_t* process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);


#define __LISTENER_UDP_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");

#endif /* ATSC3_LISTENER_UDP_H_ */
