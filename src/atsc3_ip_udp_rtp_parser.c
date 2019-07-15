//
//  atsc3_ip_udp_rtp_parser.c
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#include "atsc3_ip_udp_rtp_parser.h"

//from a pcap packet, extract ip/udp and rtp header, data[0] will be start of inner payload
ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    ip_udp_rtp_packet_t* ip_udp_rtp_packet = NULL;
    
    block_t* ip_udp_rtp_raw_block = atsc3_pcap_parse_ethernet_frame(pkthdr, packet);
    if(ip_udp_rtp_raw_block) {
        block_Rewind(ip_udp_rtp_raw_block);
        ip_udp_rtp_packet = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(ip_udp_rtp_raw_block);
//
//        atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = atsc3_stltp_parse_rtp_fixed_header_udp_packet_t(udp_packet);
//        if(atsc3_rtp_fixed_header) {
//            //build our final packet
//            ip_udp_rtp_packet = (ip_udp_rtp_packet_t*)calloc(1, sizeof(ip_udp_rtp_packet_t));
//            ip_udp_rtp_packet->udp_flow.src_ip_addr   = udp_packet->udp_flow.src_ip_addr;
//            ip_udp_rtp_packet->udp_flow.src_port      = udp_packet->udp_flow.src_port;
//            ip_udp_rtp_packet->udp_flow.dst_ip_addr   = udp_packet->udp_flow.dst_ip_addr;
//            ip_udp_rtp_packet->udp_flow.dst_port      = udp_packet->udp_flow.dst_port;
//
//            ip_udp_rtp_packet->atsc3_rtp_fixed_header     = atsc3_rtp_fixed_header;
//            ip_udp_rtp_packet->raw_packet_length          = udp_packet->raw_packet_length;
//
//            ip_udp_rtp_packet->data = block_Duplicate_from_ptr(&udp_packet->data[RTP_HEADER_LENGTH], udp_packet->data_length - RTP_HEADER_LENGTH);
//
//            cleanup(&udp_packet);
//        }
    }
    return ip_udp_rtp_packet;
}

block_t* atsc3_pcap_parse_ethernet_frame(const struct pcap_pkthdr *pkthdr, const u_char *packet) {
 
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
    if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {        __LISTENER_UDP_ERROR("udp_packet_process_from_ptr: invalid ethernet frame");
        return NULL;
    }
    
    uint32_t ip_udp_rtp_size = pkthdr->len - 14; //ethernet frame header size
    
    block_t* ip_udp_rtp = block_Alloc(ip_udp_rtp_size);
    block_Write(ip_udp_rtp, (uint8_t*)&packet[14], ip_udp_rtp_size);
    
    return ip_udp_rtp;
}


ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_process_from_blockt_pos(block_t* from) {
    int i = 0;
    int k = 0;
    u_char ip_header[24];
    u_char udp_header[8];
    int udp_header_start = 20;
    int rtp_header_start = udp_header_start + 8;
    int outer_payload_start = rtp_header_start + RTP_HEADER_LENGTH;
    
    ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = NULL;
    
    uint8_t* packet = block_Get(from);
    uint32_t packet_length = block_Remaining_size(from);
    
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
    ip_udp_rtp_packet_new = (ip_udp_rtp_packet_t*)calloc(1, sizeof(ip_udp_rtp_packet_t));
    ip_udp_rtp_packet_new->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
    ip_udp_rtp_packet_new->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);
    
    for (i = 0; i < 8; i++) {
        udp_header[i] = packet[udp_header_start + i];
    }
    
    ip_udp_rtp_packet_new->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
    ip_udp_rtp_packet_new->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];
    
    uint32_t data_length = packet_length - outer_payload_start;
    
    if(data_length <=0 || data_length > 1514) {
        __IP_UDP_RTP_PARSER_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", data_length);
        return NULL;
    }
    
    atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = atsc3_ip_udp_rtp_parse_header(&packet[rtp_header_start], packet_length);
    ip_udp_rtp_packet_new->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header;
    
    ip_udp_rtp_packet_new->data = block_Alloc(data_length);
    block_Write(ip_udp_rtp_packet_new->data, packet, packet_length);
    return ip_udp_rtp_packet_new;
}

atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header_udp_packet_t(udp_packet_t* udp_packet) {
    
    //total bits needed for rtp_fixed_header == 96, so this is a fragment
    if(!udp_packet) {
        __IP_UDP_RTP_PARSER_ERROR("fragment: atsc3_stltp_parse_rtp_fixed_header_udp_packet_t, udp_packet is null!");
        return NULL;
    }
    if(udp_packet_get_remaining_bytes(udp_packet) < 12) {
        __IP_UDP_RTP_PARSER_WARN("fragment: atsc3_stltp_parse_rtp_fixed_header_udp_packet_t, position: %u, size is: %u, data: %p", udp_packet->data_position, udp_packet->data_length, udp_packet->data);
        return NULL;
    }

    return atsc3_ip_udp_rtp_parse_header(udp_packet_get_ptr(udp_packet), udp_packet_get_remaining_bytes(udp_packet));
}


atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header_block_t(block_t* data) {
    if(block_Valid(data)) {
        __IP_UDP_RTP_PARSER_ERROR("fragment: atsc3_stltp_parse_rtp_fixed_header_block_t, udp_packet is null!");
        return NULL;
    }
    uint8_t* block_data = block_Get(data);
    uint32_t block_size = block_Remaining_size(data);
    //total bits needed for rtp_fixed_header == 96, so this is a fragment
    
    if(block_Remaining_size(data)  < 12) {
        __IP_UDP_RTP_PARSER_WARN("fragment: atsc3_stltp_parse_rtp_fixed_header_block_t, raw position: %u, size is: %u, data: %p, data_offset:%p, first four bytes: 0x%x 0x%x 0x%x 0x%x",
                                 data->i_pos,
                                 block_size,
                                 data,
                                 block_data,
                                 block_data[0], block_data[1], block_data[2], block_data[3]);
        return NULL;
    }
    
    return atsc3_ip_udp_rtp_parse_header(block_data, block_size);
}

atsc3_rtp_fixed_header_t* atsc3_ip_udp_rtp_parse_header(uint8_t* data, uint32_t size) {
        
    atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = calloc(1, sizeof(atsc3_rtp_fixed_header_t));
    
    uint8_t flags[2];
    
    flags[0] = data[0];
    flags[1] = data[1];
    
    atsc3_rtp_fixed_header->version = (flags[0] >> 6) & 0x3;
    atsc3_rtp_fixed_header->padding = (flags[0] >> 5) & 0x1;
    atsc3_rtp_fixed_header->extension = (flags[0] >> 4) & 0x1;
    atsc3_rtp_fixed_header->csrc_count = (flags[0]) & 0xF;
    
    atsc3_rtp_fixed_header->marker = (flags[1] >> 7 ) & 0x1;
    atsc3_rtp_fixed_header->payload_type = (flags[1]) & 0x7F;
    
    atsc3_rtp_fixed_header->sequence_number = ntohs(*((uint16_t*)(&data[2])));
    atsc3_rtp_fixed_header->timestamp = ntohl(*((uint32_t*)(&data[4])));
    atsc3_rtp_fixed_header->packet_offset = ntohl(*((uint32_t*)(&data[8])));
    
    return atsc3_rtp_fixed_header;
}

ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate(ip_udp_rtp_packet_t* ip_udp_rtp_packet) {
    if(ip_udp_rtp_packet && block_Valid(ip_udp_rtp_packet->data)) {
        ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = calloc(1, sizeof(ip_udp_rtp_packet_t));
        if(!ip_udp_rtp_packet_new) return NULL;
        ip_udp_rtp_packet_new->atsc3_rtp_fixed_header = calloc(1, sizeof(atsc3_rtp_fixed_header_t));
        memcpy(&ip_udp_rtp_packet_new->atsc3_rtp_fixed_header, &ip_udp_rtp_packet->atsc3_rtp_fixed_header, sizeof(atsc3_rtp_fixed_header_t));
        ip_udp_rtp_packet_new->data = block_Duplicate_from_position(ip_udp_rtp_packet->data);
        
        ip_udp_rtp_packet_new->udp_flow.src_ip_addr   = ip_udp_rtp_packet->udp_flow.src_ip_addr;
        ip_udp_rtp_packet_new->udp_flow.src_port      = ip_udp_rtp_packet->udp_flow.src_port;
        ip_udp_rtp_packet_new->udp_flow.dst_ip_addr   = ip_udp_rtp_packet->udp_flow.dst_ip_addr;
        ip_udp_rtp_packet_new->udp_flow.dst_port      = ip_udp_rtp_packet->udp_flow.dst_port;
        
        return ip_udp_rtp_packet_new;
    } else {
        return NULL;
    }
}


ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_prepend_if_not_null(ip_udp_rtp_packet_t* from_packet, ip_udp_rtp_packet_t* to_packet) {
    
    if(!from_packet || !block_Valid(from_packet->data) || !to_packet || !block_Valid(to_packet->data)) {
        return NULL;
    }
    
    ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = calloc(1, sizeof(ip_udp_rtp_packet_t));
    assert(ip_udp_rtp_packet_new);
    int block_size = 0;
    
    if(block_Valid(from_packet->data)) {
        block_size += block_Remaining_size(from_packet->data);
    }
    
    if(block_Valid(to_packet->data)) {
        block_size += block_Remaining_size(to_packet->data);
    }
    
    if(!block_size) {
        free(ip_udp_rtp_packet_new);
        return NULL;
    }
    
    ip_udp_rtp_packet_new->data = block_Alloc(block_size);
      
    if(block_Valid(from_packet->data)) {
        block_Append(ip_udp_rtp_packet_new->data, from_packet->data);
    }

    /**
     TODO: limit this to min(inner->offset, data->p_size)
     */
    if(block_Valid(to_packet->data)) {
        block_Append(ip_udp_rtp_packet_new->data, to_packet->data);
    }
    
    ip_udp_rtp_packet_new->udp_flow.src_ip_addr  = to_packet->udp_flow.src_ip_addr;
    ip_udp_rtp_packet_new->udp_flow.src_port     = to_packet->udp_flow.src_port;
    ip_udp_rtp_packet_new->udp_flow.dst_ip_addr  = to_packet->udp_flow.dst_ip_addr;
    ip_udp_rtp_packet_new->udp_flow.dst_port     = to_packet->udp_flow.dst_port;
    
    return ip_udp_rtp_packet_new;
}

