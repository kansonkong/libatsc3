//
//  atsc3_ip_udp_rtp_parser.h
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef ATSC3_IP_UDP_RTP_PARSER_H_
#define ATSC3_IP_UDP_RTP_PARSER_H_

#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTP_HEADER_LENGTH 12
    
ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);
block_t* atsc3_pcap_parse_ethernet_frame(const struct pcap_pkthdr *pkthdr, const u_char *packet);
ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_process_from_blockt_pos(block_t* from);
    
atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header_block_t(block_t* data);
atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header_udp_packet_t(udp_packet_t* udp_packet);
atsc3_rtp_fixed_header_t* atsc3_ip_udp_rtp_parse_header(uint8_t* data, uint32_t size);

ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate(ip_udp_rtp_packet_t* ip_udp_rtp_packet);
ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_prepend_if_not_null(ip_udp_rtp_packet_t* from_packet, ip_udp_rtp_packet_t* to_packet);
    
    

#define __IP_UDP_RTP_PARSER_ERROR(...)          printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")
#define __IP_UDP_RTP_PARSER_WARN(...)          printf("%s:%d:WARN : ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")
    
#define __IP_UDP_RTP_PARSER_DEBUG(...)          if(_IP_UDP_RTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n") }

#ifdef __cplusplus
}
#endif
#endif /* ATSC3_IP_UDP_RTP_PARSER_H_ */
