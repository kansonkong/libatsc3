//
//  atsc3_ip_udp_rtp_parser.c
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#include "atsc3_ip_udp_rtp_parser.h"

int _IP_UDP_RTP_PARSER_INFO_ENABLED = 0;
int _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 0;
int _IP_UDP_RTP_PARSER_TRACE_ENABLED = 0;

//from a pcap packet, extract ip/udp and rtp header, data[0] will be start of inner payload
atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_process_packet_from_pcap(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
   atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = NULL;
    
    block_t* ip_udp_rtp_raw_block = atsc3_pcap_parse_ethernet_frame(pkthdr, packet);
    if(ip_udp_rtp_raw_block) {
        block_Rewind(ip_udp_rtp_raw_block);
        ip_udp_rtp_packet = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(ip_udp_rtp_raw_block);
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
    block_RefZero(ip_udp_rtp);
    return ip_udp_rtp;
}

//from will be promoted to pointer of ->data
//if you need an independent data block, do: ip_udp_rtp_packet->data = block_Duplicate(ip_udp_rtp_packet->data)

//#define __ATSC3_IP_UDP_RTP_PENDANTIC_DEBUGGING__

atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_process_from_blockt_pos(block_t* from) {
    int i = 0;
    int k = 0;
    u_char ip_header[24];
    u_char udp_header[8];
    int udp_header_start = 20;
    int rtp_header_start = udp_header_start + 8;
    int outer_payload_start = rtp_header_start + RTP_HEADER_LENGTH;
    
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = NULL;
    
    uint8_t* packet = block_Get(from);
    uint32_t packet_length = block_Remaining_size(from);
    /*
     * jjustman-2020-08-17 - guard for too short inner packet:
     *
     *
    frame #5: 0x00000001000d6086 srt_stltp_virtual_phy_test`atsc3_ip_udp_rtp_packet_process_from_blockt_pos(from=0x0000603000048a00) at atsc3_ip_udp_rtp_parser.c:73:22 [opt]
    frame #6: 0x00000001000e28c0 srt_stltp_virtual_phy_test`atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet=0x000060b00003ca50) at atsc3_stltp_parser.c:128:62 [opt]
    frame #7: 0x00000001000e4562 srt_stltp_virtual_phy_test`atsc3_stltp_raw_packet_extract_inner_from_outer_packet(ip_udp_rtp_packet=<unavailable>, atsc3_stltp_tunnel_packet_last=<unavailable>) at atsc3_stltp_parser.c:395:55 [opt]
    frame #8: 0x000000010008e807 srt_stltp_virtual_phy_test`atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(ip_udp_rtp_packet=<unavailable>, atsc3_stltp_depacketizer_context=<unavailable>) at atsc3_stltp_depacketizer.c:68:78 [opt]
    frame #9: 0x0000000100091171 srt_stltp_virtual_phy_test`atsc3_stltp_depacketizer_from_blockt(packet_p=0x00007000018aed60, atsc3_stltp_depacketizer_context=0x0000607000000330) at atsc3_stltp_depacketizer.c:381:2 [opt]
     *
     */
    if(packet_length < outer_payload_start) {
        __LISTENER_UDP_ERROR("udp_packet_process_from_ptr: packet too short for parsing IP/UDP/RTP, needed %d bytes, packet_len is only: %d bytes", outer_payload_start, packet_length);
    	return NULL;
    }
    
    for (i = 0; i < udp_header_start; i++) {
         ip_header[i] = packet[i];
    }
    
    //check if we are a UDP packet, otherwise bail
    if (ip_header[9] != 0x11) {
        __LISTENER_UDP_ERROR("udp_packet_process_from_ptr: not a UDP packet! ip_header[9]: 0x%02x", ip_header[9]);

#ifdef __ATSC3_IP_UDP_RTP_PENDANTIC_DEBUGGING__
        printf("block_t pos: %d, size: %d\t", from->i_pos, from->p_size);
        for(int i=0; i < 40; i++) {
        	if(i>0 && ((i % 8) == 0)) {
        		printf(" ");
        	}
        	printf("%02x ", packet[i]);
        }
        printf("\n");
#endif
        return NULL;
    }
    
    if ((ip_header[0] & 0x0F) > 5) {
        udp_header_start = 28;
    }
    
    //malloc our udp_packet_header:
    ip_udp_rtp_packet_new = (atsc3_ip_udp_rtp_packet_t*)calloc(1, sizeof(atsc3_ip_udp_rtp_packet_t));
    ip_udp_rtp_packet_new->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
    ip_udp_rtp_packet_new->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);
    
    for (i = 0; i < 8; i++) {
        udp_header[i] = packet[udp_header_start + i];
    }
    
    ip_udp_rtp_packet_new->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
    ip_udp_rtp_packet_new->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];
    
    uint32_t data_length = packet_length - outer_payload_start;
    
    if(data_length <= 0 || data_length > MAX_ATSC3_PHY_ALP_DATA_PAYLOAD_SIZE) {
        __IP_UDP_RTP_PARSER_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", data_length);
        return NULL;
    }
    
    atsc3_rtp_header_t* rtp_header = atsc3_ip_udp_rtp_parse_header(&packet[rtp_header_start], packet_length);
    ip_udp_rtp_packet_new->rtp_header = rtp_header;
    
    block_Seek_Relative(from, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
    ip_udp_rtp_packet_new->data = block_Duplicate(from); //block_Refcount(from);
    return ip_udp_rtp_packet_new;
}


atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_process_header_only_no_data_block_from_blockt_pos(block_t* from) {
	 int i = 0;
	int k = 0;
	u_char ip_header[24];
	u_char udp_header[8];
	int udp_header_start = 20;
	int rtp_header_start = udp_header_start + 8;
	int outer_payload_start = rtp_header_start + RTP_HEADER_LENGTH;

	atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = NULL;

	uint8_t* packet = block_Get(from);
	uint32_t packet_length = block_Remaining_size(from);
	/*
	 * jjustman-2020-08-17 - guard for too short inner packet:
	 *
	 *
	frame #5: 0x00000001000d6086 srt_stltp_virtual_phy_test`atsc3_ip_udp_rtp_packet_process_from_blockt_pos(from=0x0000603000048a00) at atsc3_ip_udp_rtp_parser.c:73:22 [opt]
	frame #6: 0x00000001000e28c0 srt_stltp_virtual_phy_test`atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet=0x000060b00003ca50) at atsc3_stltp_parser.c:128:62 [opt]
	frame #7: 0x00000001000e4562 srt_stltp_virtual_phy_test`atsc3_stltp_raw_packet_extract_inner_from_outer_packet(ip_udp_rtp_packet=<unavailable>, atsc3_stltp_tunnel_packet_last=<unavailable>) at atsc3_stltp_parser.c:395:55 [opt]
	frame #8: 0x000000010008e807 srt_stltp_virtual_phy_test`atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(ip_udp_rtp_packet=<unavailable>, atsc3_stltp_depacketizer_context=<unavailable>) at atsc3_stltp_depacketizer.c:68:78 [opt]
	frame #9: 0x0000000100091171 srt_stltp_virtual_phy_test`atsc3_stltp_depacketizer_from_blockt(packet_p=0x00007000018aed60, atsc3_stltp_depacketizer_context=0x0000607000000330) at atsc3_stltp_depacketizer.c:381:2 [opt]
	 *
	 */
	if(packet_length < outer_payload_start) {
		__LISTENER_UDP_ERROR("atsc3_ip_udp_rtp_packet_process_header_only_no_data_block_from_blockt_pos: packet too short for parsing IP/UDP/RTP, needed %d bytes, packet_len is only: %d bytes", outer_payload_start, packet_length);
		return NULL;
	}

	for (i = 0; i < udp_header_start; i++) {
		 ip_header[i] = packet[i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__LISTENER_UDP_ERROR("atsc3_ip_udp_rtp_packet_process_header_only_no_data_block_from_blockt_pos: not a UDP packet! ip_header[9]: 0x%02x", ip_header[9]);

#ifdef __ATSC3_IP_UDP_RTP_PENDANTIC_DEBUGGING__
		printf("block_t pos: %d, size: %d\t", from->i_pos, from->p_size);
		for(int i=0; i < 40; i++) {
			if(i>0 && ((i % 8) == 0)) {
				printf(" ");
			}
			printf("%02x ", packet[i]);
		}
		printf("\n");
#endif
		return NULL;
	}

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 28;
	}

	//malloc our udp_packet_header:
	ip_udp_rtp_packet_new = (atsc3_ip_udp_rtp_packet_t*)calloc(1, sizeof(atsc3_ip_udp_rtp_packet_t));
	ip_udp_rtp_packet_new->udp_flow.src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	ip_udp_rtp_packet_new->udp_flow.dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = packet[udp_header_start + i];
	}

	ip_udp_rtp_packet_new->udp_flow.src_port = (udp_header[0] << 8) + udp_header[1];
	ip_udp_rtp_packet_new->udp_flow.dst_port = (udp_header[2] << 8) + udp_header[3];

	uint32_t data_length = packet_length - outer_payload_start;

	if(data_length <= 0 || data_length > MAX_ATSC3_PHY_ALP_DATA_PAYLOAD_SIZE) {
		__IP_UDP_RTP_PARSER_ERROR("udp_packet_process_from_ptr: invalid data length of udp packet: %d", data_length);
		return NULL;
	}

	atsc3_rtp_header_t* rtp_header = atsc3_ip_udp_rtp_parse_header(&packet[rtp_header_start], packet_length);
	ip_udp_rtp_packet_new->rtp_header = rtp_header;

	block_Seek_Relative(from, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);

	//dont duplicate data here, just return the ip_udp_rtp_packet header
	ip_udp_rtp_packet_new->data = NULL;
	return ip_udp_rtp_packet_new;
}

atsc3_rtp_header_t* atsc3_stltp_parse_rtp_header_block(block_t* data) {
    if(!block_Valid(data)) {
        __IP_UDP_RTP_PARSER_ERROR("fragment: atsc3_stltp_parse_rtp_header_block, !block_Valid(data)");
        return NULL;
    }
    uint8_t* block_data = block_Get(data);
    uint32_t block_size = block_Remaining_size(data);
    //total bits needed for rtp_fixed_header == 96, so this is a fragment
    
    if(block_Remaining_size(data)  < 12) {
        __IP_UDP_RTP_PARSER_WARN("fragment: atsc3_stltp_parse_rtp_header_block, raw position: %u, size is: %u, data: %p, data_offset:%p, first four bytes: 0x%x 0x%x 0x%x 0x%x",
                                 data->i_pos,
                                 block_size,
                                 data,
                                 block_data,
                                 block_data[0], block_data[1], block_data[2], block_data[3]);
        return NULL;
    }
    
    return atsc3_ip_udp_rtp_parse_header(block_data, block_size);
}

atsc3_rtp_header_t* atsc3_ip_udp_rtp_parse_header(uint8_t* data, uint32_t size) {
    assert(size > RTP_HEADER_LENGTH);
        
    atsc3_rtp_header_t* atsc3_rtp_header = calloc(1, sizeof(atsc3_rtp_header_t));
    
    uint8_t flags[2];
    
    flags[0] = data[0];
    flags[1] = data[1];
    
    atsc3_rtp_header->version = (flags[0] >> 6) & 0x3;
    atsc3_rtp_header->padding = (flags[0] >> 5) & 0x1;
    atsc3_rtp_header->extension = (flags[0] >> 4) & 0x1;
    atsc3_rtp_header->csrc_count = (flags[0]) & 0xF;
    
    atsc3_rtp_header->marker = (flags[1] >> 7 ) & 0x1;
    atsc3_rtp_header->payload_type = (flags[1]) & 0x7F;
    
    atsc3_rtp_header->sequence_number = ntohs(*((uint16_t*)(&data[2])));
    atsc3_rtp_header->timestamp = ntohl(*((uint32_t*)(&data[4])));
    atsc3_rtp_header->packet_offset = ntohl(*((uint32_t*)(&data[8])));
    
    return atsc3_rtp_header;
}

atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet) {
    if(ip_udp_rtp_packet && block_Valid(ip_udp_rtp_packet->data)) {
       atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = calloc(1, sizeof(atsc3_ip_udp_rtp_packet_t));
        if(!ip_udp_rtp_packet_new) return NULL;
        
        ip_udp_rtp_packet_new->rtp_header = atsc3_rtp_header_duplicate(ip_udp_rtp_packet->rtp_header);
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


atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_duplicate_no_data_block_t(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet) {
    if(ip_udp_rtp_packet) {
        atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = calloc(1, sizeof(atsc3_ip_udp_rtp_packet_t));
        if(!ip_udp_rtp_packet_new) return NULL;

        ip_udp_rtp_packet_new->rtp_header = atsc3_rtp_header_duplicate(ip_udp_rtp_packet->rtp_header);

        ip_udp_rtp_packet_new->udp_flow.src_ip_addr   = ip_udp_rtp_packet->udp_flow.src_ip_addr;
        ip_udp_rtp_packet_new->udp_flow.src_port      = ip_udp_rtp_packet->udp_flow.src_port;
        ip_udp_rtp_packet_new->udp_flow.dst_ip_addr   = ip_udp_rtp_packet->udp_flow.dst_ip_addr;
        ip_udp_rtp_packet_new->udp_flow.dst_port      = ip_udp_rtp_packet->udp_flow.dst_port;
        
        return ip_udp_rtp_packet_new;
    } else {
        return NULL;
    }
}


atsc3_ip_udp_rtp_packet_t* atsc3_ip_udp_rtp_packet_prepend_if_not_null(atsc3_ip_udp_rtp_packet_t* from_packet,atsc3_ip_udp_rtp_packet_t* to_packet) {
    
    if(!from_packet || !block_Valid(from_packet->data) || !to_packet || !block_Valid(to_packet->data)) {
        return NULL;
    }
    
   atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = calloc(1, sizeof(atsc3_ip_udp_rtp_packet_t));
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
    __IP_UDP_RTP_PARSER_INFO("atsc3_ip_udp_rtp_packet_prepend_if_not_null: block_size: %u", block_size);
      
    if(block_Valid(from_packet->data)) {
        __IP_UDP_RTP_PARSER_INFO("atsc3_ip_udp_rtp_packet_prepend_if_not_null: appending from_packet %p, size: %u", from_packet->data, block_Remaining_size(from_packet->data));

        block_Append(ip_udp_rtp_packet_new->data, from_packet->data);
    }

    /**
     TODO: limit this to min(inner->offset, data->p_size)
     */
    if(block_Valid(to_packet->data)) {
          __IP_UDP_RTP_PARSER_INFO("atsc3_ip_udp_rtp_packet_prepend_if_not_null: appending to_packet %p, size: %u", to_packet->data, block_Remaining_size(to_packet->data));
        block_Append(ip_udp_rtp_packet_new->data, to_packet->data);
    }
    
    ip_udp_rtp_packet_new->udp_flow.src_ip_addr  = to_packet->udp_flow.src_ip_addr;
    ip_udp_rtp_packet_new->udp_flow.src_port     = to_packet->udp_flow.src_port;
    ip_udp_rtp_packet_new->udp_flow.dst_ip_addr  = to_packet->udp_flow.dst_ip_addr;
    ip_udp_rtp_packet_new->udp_flow.dst_port     = to_packet->udp_flow.dst_port;
    
    return ip_udp_rtp_packet_new;
}


atsc3_rtp_header_t* atsc3_rtp_header_duplicate(atsc3_rtp_header_t* atsc3_rtp_header_from) {
    atsc3_rtp_header_t* atsc3_rtp_header_new = calloc(1, sizeof(atsc3_rtp_header_t));
    memcpy(atsc3_rtp_header_new, atsc3_rtp_header_from, sizeof(atsc3_rtp_header_t));
    
    return atsc3_rtp_header_new;
}

void atsc3_rtp_header_free(atsc3_rtp_header_t** atsc3_rtp_header_p) {
    atsc3_rtp_header_t* atsc3_rtp_header = *atsc3_rtp_header_p;
    if(atsc3_rtp_header) {
        __IP_UDP_RTP_PARSER_TRACE("atsc3_rtp_header_free: freeing: %p", atsc3_rtp_header);
        memset(atsc3_rtp_header, 0, sizeof(atsc3_rtp_header_p));
        free(atsc3_rtp_header);
        *atsc3_rtp_header_p = NULL;
    }
}

void atsc3_ip_udp_rtp_packet_free(atsc3_ip_udp_rtp_packet_t** ip_udp_rtp_packet_p) {
    if(ip_udp_rtp_packet_p) {
        atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = *ip_udp_rtp_packet_p;
        if(ip_udp_rtp_packet) {
            //rely on reference counting for outer/inner pointer sharing
            if(ip_udp_rtp_packet->data) {
                __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_free: freeing ip_udp_rtp_packet->data: %p", ip_udp_rtp_packet->data);
                block_Destroy(&ip_udp_rtp_packet->data);
            }
            atsc3_rtp_header_free(&ip_udp_rtp_packet->rtp_header);
            free(ip_udp_rtp_packet);
            ip_udp_rtp_packet = NULL;
        }
        *ip_udp_rtp_packet_p = NULL;
    }        
}


//destroy: hard free at the end of the main pcap loop
void atsc3_ip_udp_rtp_packet_destroy(atsc3_ip_udp_rtp_packet_t** ip_udp_rtp_packet_p) {
    if(ip_udp_rtp_packet_p) {
        atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = *ip_udp_rtp_packet_p;
        if(ip_udp_rtp_packet) {
            
            if(ip_udp_rtp_packet->data) {
                __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet->data: %p", ip_udp_rtp_packet->data);
                block_Destroy(&ip_udp_rtp_packet->data);
            }
            
            if(ip_udp_rtp_packet->rtp_header) {
                __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet->rtp_header: %p", ip_udp_rtp_packet->rtp_header);
                freesafe(ip_udp_rtp_packet->rtp_header);
            }
            ip_udp_rtp_packet->rtp_header = NULL;
            free(ip_udp_rtp_packet);
            ip_udp_rtp_packet = NULL;
            
        }
        *ip_udp_rtp_packet_p = NULL;
    }
}



//destroy: hard free at the end of the main pcap loop for outer_inner which may have shared block_t*
void atsc3_ip_udp_rtp_packet_destroy_outer_inner(atsc3_ip_udp_rtp_packet_t** ip_udp_rtp_packet_outer_p, atsc3_ip_udp_rtp_packet_t** ip_udp_rtp_packet_inner_p) {
    
    block_t* shared_outer_inner_block_t_to_check = NULL;
    atsc3_rtp_header_t* shared_outer_inner_rtp_header_t_to_check = NULL;
    
    if(ip_udp_rtp_packet_outer_p) {
        atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_outer = *ip_udp_rtp_packet_outer_p;
        if(ip_udp_rtp_packet_outer) {
            
            if(ip_udp_rtp_packet_outer->data) {
                __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet_outer->data: %p", ip_udp_rtp_packet_outer->data);
                shared_outer_inner_block_t_to_check = ip_udp_rtp_packet_outer->data;
                block_Destroy(&ip_udp_rtp_packet_outer->data);
            }
            
            if(ip_udp_rtp_packet_outer->rtp_header) {
                __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet_outer->rtp_header: %p", ip_udp_rtp_packet_outer->rtp_header);
                shared_outer_inner_rtp_header_t_to_check = ip_udp_rtp_packet_outer->rtp_header;
                freesafe(ip_udp_rtp_packet_outer->rtp_header);

            }
            ip_udp_rtp_packet_outer->rtp_header = NULL;
            free(ip_udp_rtp_packet_outer);
            ip_udp_rtp_packet_outer = NULL;
            
        }
        *ip_udp_rtp_packet_outer_p = NULL;
    }
    
    if(ip_udp_rtp_packet_inner_p) {
        atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner = *ip_udp_rtp_packet_inner_p;
        if(ip_udp_rtp_packet_inner) {
            __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet_inner->rtp_header: %p", ip_udp_rtp_packet_inner->rtp_header);
            
            if(ip_udp_rtp_packet_inner->data) {
                if(shared_outer_inner_block_t_to_check && shared_outer_inner_block_t_to_check != ip_udp_rtp_packet_inner->data) {
                    __IP_UDP_RTP_PARSER_TRACE("atsc3_ip_udp_rtp_packet_destroy: freeing ip_udp_rtp_packet_inner->data: %p", ip_udp_rtp_packet_inner->data);
                    block_Destroy(&ip_udp_rtp_packet_inner->data);
                }
            }
            
            if(shared_outer_inner_rtp_header_t_to_check && shared_outer_inner_rtp_header_t_to_check != ip_udp_rtp_packet_inner->rtp_header) {
                freesafe(ip_udp_rtp_packet_inner->rtp_header);
                ip_udp_rtp_packet_inner->rtp_header = NULL;
            }
            free(ip_udp_rtp_packet_inner);
            ip_udp_rtp_packet_inner = NULL;
            
        }
        *ip_udp_rtp_packet_inner_p = NULL;
    }
}
