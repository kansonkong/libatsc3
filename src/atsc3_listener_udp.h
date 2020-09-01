/*
 * atsc3_listener_udp.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#ifndef __DISABLE_LIBPCAP__
#include <pcap.h>
#else
#ifndef __JJ_PCAP_PKTHDR__
#define __JJ_PCAP_PKTHDR__
//hack-ish
#include <sys/time.h>

struct pcap_pkthdr {
	struct timeval ts;	/* time stamp */
	uint32_t caplen;	/* length of portion present */
	uint32_t len;	/* length this packet (off wire) */
};
#endif
#endif

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "atsc3_utils.h"

#ifndef ATSC3_LISTENER_UDP_H_
#define ATSC3_LISTENER_UDP_H_

#include "atsc3_udp.h"

#ifdef __cplusplus
extern "C" {
#endif


#define udp_packet_get_remaining_bytes(udp_packet) (__MAX(0, udp_packet->data_length - udp_packet->data_position ))
#define udp_packet_get_ptr(udp_packet) (udp_packet->data_position < udp_packet->data_length ? &udp_packet->data[udp_packet->data_position] : NULL)
#define udp_packet_get_ptr_offset(udp_packet, offset) (udp_packet->data_position + offset < udp_packet->data_length ? &udp_packet->data[udp_packet->data_position + offset] : NULL)
#define udp_packet_seek_offset(udp_packet, offset) (udp_packet->data_position + offset <= udp_packet->data_length ? udp_packet->data_position += offset : -1)
#define udp_packet_seek(udp_packet, position) (position <= udp_packet->data_length ? udp_packet->data_position = position : -1); __LISTENER_UDP_ERROR("udp packet seek to position: %u", udp_packet->data_position);

//dump packet example
//__STLTP_PARSER_DEBUG("dst ip:port : %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));


udp_packet_t* process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);
udp_packet_t* udp_packet_process_from_ptr_raw_ethernet_packet(uint8_t* packet, uint32_t packet_length);

udp_packet_t* udp_packet_process_from_ptr(uint8_t* packet, uint32_t packet_length);

udp_packet_t* udp_packet_duplicate(udp_packet_t* udp_packet);
udp_packet_t* udp_packet_prepend_if_not_null(udp_packet_t* from_packet, udp_packet_t* to_packet);

//WRITING:
//see: block_t* atsc3_ip_udp_rtp_dstp_write_to_block_t(atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet) {



#define __LISTENER_UDP_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("%s%s","\r","\n")
#ifdef __cplusplus
}
#endif
#endif /* ATSC3_LISTENER_UDP_H_ */
