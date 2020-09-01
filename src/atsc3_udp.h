/*
 * atsc3_udp.h
 *
 *  Created on: Aug 31, 2020
 *      Author: jjustman
 */


#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "atsc3_utils.h"

#ifndef ATSC3_UDP_H_
#define ATSC3_UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

//jjustman-2019-09-24 - increase to phy maximum of 65535 - to support ATSC A/331:2019 PHY LLS size of 65535

#define MAX_PCAP_LEN MAX_ATSC3_PHY_IP_DATAGRAM_SIZE

typedef struct udp_flow {
	uint32_t		src_ip_addr;
	uint32_t		dst_ip_addr;
	uint16_t		src_port;
	uint16_t		dst_port;
} udp_flow_t;


typedef struct udp_packet {
	udp_flow_t		udp_flow;

    //note - data will be the payload after the following packet headers removed:
    //[ethernet, ip, udp]
    block_t*        data;

	//internals
	int				raw_packet_length;

} udp_packet_t;

typedef struct udp_packet atsc3_udp_packet_t;

//jjustman-2019-09-24 - increase to phy maximum of 65535 - to support ATSC A/331:2019 PHY LLS size of 65535

#define MAX_PCAP_LEN MAX_ATSC3_PHY_IP_DATAGRAM_SIZE

atsc3_udp_packet_t* atsc3_udp_packet_from_block_t(block_t* block_udp_packet);

void udp_packet_free(udp_packet_t** udp_packet_p);
void atsc3_udp_packet_free(atsc3_udp_packet_t** udp_packet_p);

#define _ATSC3_UDP_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_UDP_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);;
#define _ATSC3_UDP_INFO(...)    if(_ATSC3_UDP_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_UDP_DEBUG(...)   if(_ATSC3_UDP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_UDP_TRACE(...)   if(_ATSC3_UDP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif
#endif /* ATSC3_UDP_H_ */

