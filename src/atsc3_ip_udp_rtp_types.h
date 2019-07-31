//
//  atsc3_ip_udp_rtp_parser_types.h
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#ifndef ATSC3_IP_UDP_RTP_PARSER_TYPES_H
#define ATSC3_IP_UDP_RTP_PARSER_TYPES_H


typedef struct atsc3_rtp_header {
    uint8_t version:2;
    uint8_t padding:1;
    uint8_t extension:1;
    uint8_t csrc_count:4;
    uint8_t marker:1;
    uint8_t payload_type:7;
    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t packet_offset;
} atsc3_rtp_header_t;

typedef struct atsc3_ip_udp_rtp_packet {
    udp_flow_t              udp_flow;
    atsc3_rtp_header_t*     rtp_header;
    bool                    is_in_marker;

    //note - data will be the payload after the following packet headers removed:
    //[ethernet, ip, udp, rtp]
    block_t*                data;
    
} atsc3_ip_udp_rtp_packet_t;

#endif /* ATSC3_IP_UDP_RTP_PARSER_TYPES_H */
