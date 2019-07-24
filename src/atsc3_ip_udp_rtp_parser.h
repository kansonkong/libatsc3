//
//  atsc3_ip_udp_rtp_parser.h
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef ATSC3_IP_UDP_RTP_PARSER_H_
#define ATSC3_IP_UDP_RTP_PARSER_H_

#include "atsc3_logging_externs.h"
#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTP_HEADER_LENGTH 12
#define ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE 40

    
block_t* atsc3_pcap_parse_ethernet_frame(const struct pcap_pkthdr *pkthdr, const u_char *packet);
atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);
atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_process_from_blockt_pos(block_t* from);
    
/** jjustman-2019-07-21 todo: remove these for context of outer/inner parsing block, e.g. 1. outer (use Marker bit for identification) and inner(s) based upon size **/
    
atsc3_rtp_header_t* atsc3_stltp_parse_rtp_header_block(block_t* data);
atsc3_rtp_header_t* atsc3_ip_udp_rtp_parse_header(uint8_t* data, uint32_t size);

    
atsc3_rtp_header_t* atsc3_rtp_header_duplicate(atsc3_rtp_header_t* atsc3_rtp_header_from);
void atsc3_rtp_header_free(atsc3_rtp_header_t** atsc3_rtp_header_p);

        
atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet);
atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate_no_data_block_t(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet);

atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_prepend_if_not_null(atsc3_ip_udp_rtp_packet_t* from_packet, atsc3_ip_udp_rtp_packet_t* to_packet);
    
//will use refcounting for block_t free determination
void atsc3_ip_udp_rtp_packet_free(atsc3_ip_udp_rtp_packet_t** ip_udp_rtp_packet_p);

#define __IP_UDP_RTP_PARSER_ERROR(...)         printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")
#define __IP_UDP_RTP_PARSER_WARN(...)          printf("%s:%d:WARN : ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")
#define __IP_UDP_RTP_PARSER_INFO(...)          printf("%s:%d:INFO : ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")

#define __IP_UDP_RTP_PARSER_DEBUG(...)         if(_IP_UDP_RTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n"); }
    
#define __IP_UDP_RTP_PARSER_TRACE(...)         if(_IP_UDP_RTP_PARSER_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n"); }


#ifdef __cplusplus
}
#endif
#endif /* ATSC3_IP_UDP_RTP_PARSER_H_ */
