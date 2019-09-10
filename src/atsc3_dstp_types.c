/*
 * atsc3_dstp_types.c
 *
 *  Created on: Sep 10, 2019
 *      Author: jjustman
 */

#include "atsc3_dstp_types.h"

int _DSTP_TYPES_DEBUG_ENABLED = 0;
int _DSTP_TYPES_TRACE_ENABLED = 0;

//cctor's
atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header_new() {
	atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header = calloc(1, sizeof(atsc3_rtp_dstp_header_t));

	atsc3_rtp_dstp_header->version = 0x2;				//'10'
	atsc3_rtp_dstp_header->padding = 0x0;				//'0'
	atsc3_rtp_dstp_header->extension = 0x0;				//'0'
	atsc3_rtp_dstp_header->csrc_count = 0x0;			//'0000'

	atsc3_rtp_dstp_header->marker = 0;					//'0'

	atsc3_rtp_dstp_header->payload_type.prefix = 0x5; 	//'101'
	atsc3_rtp_dstp_header->payload_type.reserved = 0x3; //'11'

	return atsc3_rtp_dstp_header;
}

atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet_new() {
	atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet = calloc(1, sizeof(atsc3_ip_udp_rtp_dstp_packet_t));

    atsc3_ip_udp_rtp_dstp_packet->rtp_header = atsc3_rtp_dstp_header_new();
	atsc3_ip_udp_rtp_dstp_packet->data = block_Alloc(0);

	return atsc3_ip_udp_rtp_dstp_packet;
}

atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet_new_from_flow(udp_flow_t* udp_flow) {

	atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet = atsc3_ip_udp_rtp_dstp_packet_new();
	memcpy(&atsc3_ip_udp_rtp_dstp_packet->udp_flow, udp_flow, sizeof(udp_flow_t));
    atsc3_ip_udp_rtp_dstp_packet->data = block_Alloc(0);
	return atsc3_ip_udp_rtp_dstp_packet;
}

//writer methods
block_t* atsc3_rtp_dstp_header_write_to_block_t(atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header) {
	block_t* rtp_dstp_header = block_Alloc(12); //sum(RTP_fixed_header): 96 bits -> 12 bytes;
	uint8_t payload[12] = { 0 };

	payload[0] |= (atsc3_rtp_dstp_header->version & 0x3) << 6;
	payload[0] |= (atsc3_rtp_dstp_header->padding & 0x1) << 5;
	payload[0] |= (atsc3_rtp_dstp_header->extension & 0x1) << 4;
	payload[0] |= (atsc3_rtp_dstp_header->csrc_count & 0xF);

	payload[1] |= (atsc3_rtp_dstp_header->marker & 0x1) << 7;
	payload[1] |= (atsc3_rtp_dstp_header->payload_type.prefix & 0x7) << 4;
	payload[1] |= (atsc3_rtp_dstp_header->payload_type.reserved & 0x3) << 2;
	payload[1] |= (atsc3_rtp_dstp_header->payload_type.wakeup_control.wakeup_active & 0x1) << 1;
	payload[1] |= (atsc3_rtp_dstp_header->payload_type.wakeup_control.aeat_wakeup_alert & 0x1);

	*((uint16_t*)&payload[2]) = htons(atsc3_rtp_dstp_header->sequence_number);

	*((uint32_t*)&payload[4]) = htonl(atsc3_rtp_dstp_header->timestamp_min.seconds << 16 | atsc3_rtp_dstp_header->timestamp_min.fraction);

	*((uint32_t*)&payload[8]) = htonl(atsc3_rtp_dstp_header->timestamp_max.seconds << 16 | atsc3_rtp_dstp_header->timestamp_max.fraction);

	block_Write(rtp_dstp_header, &payload[0], 12);
	return rtp_dstp_header;
}

uint16_t atsc3_ip_compute_checksum(const void *buf, size_t hdr_len)
{
         unsigned long sum = 0;
         const uint16_t *ip1;

         ip1 = buf;
         while (hdr_len > 1)
         {
                 sum += *ip1++;
                 if (sum & 0x80000000)
                         sum = (sum & 0xFFFF) + (sum >> 16);
                 hdr_len -= 2;
         }

         while (sum >> 16)
                 sum = (sum & 0xFFFF) + (sum >> 16);

         return(~sum);
}

block_t* atsc3_ip_udp_rtp_dstp_write_to_eth_phy_packet_block_t(atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet) {
	/**
	 *
	 * eth + ip + udp
	 * 14  + 20 + 8 = 42 bytes
	 *
	 * As defined by IANA, the most significant 24 bits of an IPv4 multicast MAC address are 0x01005E.
	 * Bit 25 is 0, and the other 23 bits are the least significant 23 bits of an IPv4 multicast address.
	 *
	 */
	block_t* ip_udp_dtp_dstp_eth_phy_packet = block_Alloc(42);
	uint8_t eth_frame[42] = { 0 };

	//dest eth frame
	eth_frame[0] = 0x01;
	eth_frame[1] = 0x00;
	eth_frame[2] = 0x5E;
	eth_frame[3] = (atsc3_ip_udp_rtp_dstp_packet->udp_flow.dst_ip_addr >> 16) & 0xEF;
	eth_frame[4] = (atsc3_ip_udp_rtp_dstp_packet->udp_flow.dst_ip_addr >> 8) & 0xFF;
	eth_frame[5] = (atsc3_ip_udp_rtp_dstp_packet->udp_flow.dst_ip_addr) & 0xFF;

	/** src eth addr alternative
	 * struct ethhdr *eth = (struct ethhdr *)(sendbuff);
eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]); 
	 *
	 */
	long replay_timestamp = gtl();

	eth_frame[6]  = (replay_timestamp >> 10) & 0xFF;
	eth_frame[7]  = (replay_timestamp >> 8) & 0xFF;
	eth_frame[8]  = (replay_timestamp >> 6) & 0xFF;
	eth_frame[9]  = (replay_timestamp >> 4) & 0xFF;
	eth_frame[10] = (replay_timestamp >> 2) & 0xFF;
	eth_frame[11] = replay_timestamp & 0xFF;

	//ipv4 type
	eth_frame[12]=0x08;
	eth_frame[13]=0x00;

	//ipv4 header
	eth_frame[14] = 0x45;
	eth_frame[15] = 0x00;

	//payload 16-17: ip.len
	eth_frame[16] = 0x00;
	eth_frame[17] = 0x00;

	//ip.id
	//was arc4random
	srandom(time(NULL));
	uint32_t ip_id = random();
	eth_frame[18] = (ip_id >> 8) & 0xFF;
	eth_frame[19] = (ip_id) & 0xFF;

	//ip.flags
	eth_frame[20] = 0x40;
	eth_frame[21] = 0x00;

	//ip.ttl
	eth_frame[22] = 0x00;
	//ip.proto
	eth_frame[23] = 0x11; //17 == udp

	//header checksum
	eth_frame[24] = 0x00;
	eth_frame[25] = 0x00;

	//ip.src addr
	*((uint32_t*)&eth_frame[26]) = htonl(atsc3_ip_udp_rtp_dstp_packet->udp_flow.src_ip_addr); //bytes 26-29
	//ip.dst addr
	*((uint32_t*)&eth_frame[30]) = htonl(atsc3_ip_udp_rtp_dstp_packet->udp_flow.dst_ip_addr); //bytes 30-33

	//udp

	*((uint16_t*)&eth_frame[34]) = htons(atsc3_ip_udp_rtp_dstp_packet->udp_flow.src_port); //bytes 34-35
	*((uint16_t*)&eth_frame[36]) = htons(atsc3_ip_udp_rtp_dstp_packet->udp_flow.dst_port); //bytes 36-37

	//bytes 38-39 are udp.len
	eth_frame[38] = 0x00;
	eth_frame[39] = 0x00;

	//udp checksum
	eth_frame[40] = 0x00;
	eth_frame[41] = 0x00;

	block_t* rtp_header_block = atsc3_rtp_dstp_header_write_to_block_t(atsc3_ip_udp_rtp_dstp_packet->rtp_header);

	uint16_t udp_payload_size = rtp_header_block->p_size + atsc3_ip_udp_rtp_dstp_packet->data->p_size + 8;
	*((uint16_t*)&eth_frame[38]) = htons(udp_payload_size);

	uint16_t ip_payload_size = udp_payload_size + 20;
	*((uint16_t*)&eth_frame[16]) = htons(ip_payload_size);

	//manually flip endianess

	uint16_t checksum = atsc3_ip_compute_checksum(&eth_frame[14], 20);
	eth_frame[24] = (checksum) & 0xFF;
	eth_frame[25] = (checksum >> 8) & 0xFF;



	block_Write(ip_udp_dtp_dstp_eth_phy_packet, &eth_frame[0], 42);
	block_Merge(ip_udp_dtp_dstp_eth_phy_packet, rtp_header_block);
	block_Merge(ip_udp_dtp_dstp_eth_phy_packet, atsc3_ip_udp_rtp_dstp_packet->data);

	return ip_udp_dtp_dstp_eth_phy_packet;
}


//destructors

void atsc3_rtp_dstp_header_free(atsc3_rtp_dstp_header_t** atsc3_rtp_dstp_header_p) {
	if(atsc3_rtp_dstp_header_p) {
		atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header = *atsc3_rtp_dstp_header_p;

		if(atsc3_rtp_dstp_header) {

			free(atsc3_rtp_dstp_header);
			atsc3_rtp_dstp_header = NULL;
		}
		*atsc3_rtp_dstp_header_p = NULL;
	}
}

void atsc3_ip_udp_rtp_dstp_packet_free(atsc3_ip_udp_rtp_dstp_packet_t** atsc3_ip_udp_rtp_dstp_packet_p) {
	if(atsc3_ip_udp_rtp_dstp_packet_p) {
		atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet = *atsc3_ip_udp_rtp_dstp_packet_p;

		if(atsc3_ip_udp_rtp_dstp_packet) {

			if(atsc3_ip_udp_rtp_dstp_packet->rtp_header) {
				atsc3_rtp_dstp_header_free(&atsc3_ip_udp_rtp_dstp_packet->rtp_header);
			}
			if(atsc3_ip_udp_rtp_dstp_packet->data) {
				block_Destroy(&atsc3_ip_udp_rtp_dstp_packet->data);
			}

			free(atsc3_ip_udp_rtp_dstp_packet);
			atsc3_ip_udp_rtp_dstp_packet = NULL;
		}
		*atsc3_ip_udp_rtp_dstp_packet_p = NULL;
	}
}
