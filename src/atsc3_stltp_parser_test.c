/*
 *
 * atsc3_stltp_parser_test.c
 * test driver for STLTP parsing
 *
 *
 */

#include "atsc3_utils.h"
#include "atsc3_stltp_parser.h"


void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, uint32_t * binary_payload_size) {
	int test_payload_base64_length = strlen(test_payload_base64);
	int test_payload_binary_size = test_payload_base64_length/2;

	uint8_t *test_payload_binary = calloc(test_payload_binary_size, sizeof(uint8_t));

	for (size_t count = 0; count < test_payload_binary_size; count++) {
	        sscanf(test_payload_base64, "%2hhx", &test_payload_binary[count]);
	        test_payload_base64 += 2;
	}

	*binary_payload = test_payload_binary;
	*binary_payload_size = test_payload_binary_size;
}


static char *__get_test_stltp() { return "80e1d0248a90f4e30000004c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000045001744d61400000011ab6400000000ef003330000075301730000080ced6148a90f4e30000171cfffef8b80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; }
  
int main() {

	uint8_t *binary_payload;
	uint32_t binary_payload_length;

	__create_binary_payload(__get_test_stltp(), &binary_payload, &binary_payload_length);
	uint8_t *binary_payload_start = binary_payload;

	__STLTP_PARSER_DEBUG("STLTP (RTP) dump");
	__STLTP_PARSER_DEBUG("-----------------------------");
	__STLTP_PARSER_DEBUG("base64 STLTP: %s\n", __get_test_stltp());
	__STLTP_PARSER_DEBUG("-----------------------------");

	atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header = atsc3_stltp_parse_rtp_fixed_header(binary_payload, binary_payload_length);
	assert(atsc3_rtp_fixed_header);
	atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header);

	//97 - tunnel packet
	if(atsc3_rtp_fixed_header->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {
		atsc3_stltp_tunnel_packet_t * atsc3_stltp_tunnel_packet = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));
		atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header;
		if(atsc3_rtp_fixed_header->marker) {

			uint32_t ip_header_packet_offset = atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header->packet_offset + 12;
			assert(ip_header_packet_offset  < binary_payload_length);
			atsc3_stltp_tunnel_packet->first_ip_header = &binary_payload[ip_header_packet_offset];
			atsc3_stltp_tunnel_packet->first_ip_header_length = binary_payload_length - ip_header_packet_offset;

			__STLTP_PARSER_DEBUG("--tunnel packet --");
			__STLTP_PARSER_DEBUG("  ip header:     0x%x", atsc3_stltp_tunnel_packet->first_ip_header[0]);

			atsc3_stltp_tunnel_packet->udp_packet = process_ip_udp_header(atsc3_stltp_tunnel_packet->first_ip_header, atsc3_stltp_tunnel_packet->first_ip_header_length);
			__STLTP_PARSER_DEBUG("  dst ip:port :  %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));
			__STLTP_PARSER_DEBUG("  packet length: %u ", atsc3_stltp_tunnel_packet->udp_packet->data_length);
			__STLTP_PARSER_DEBUG("  packet first byte: 0x%x ", atsc3_stltp_tunnel_packet->udp_packet->data[0]);

			//parse rtp again....
			atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel = atsc3_stltp_parse_rtp_fixed_header(atsc3_stltp_tunnel_packet->udp_packet->data, atsc3_stltp_tunnel_packet->udp_packet->data_length);
			atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_tunnel);

			if(atsc3_rtp_fixed_header_tunnel->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET) {
				atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = calloc(1, sizeof(atsc3_stltp_baseband_packet_t));
				atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_tunnel;

				if(atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->marker) {
					/**
ATSC A/324:2018 - Section 8.3
When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
field will contain the total length of the Baseband Packet data structure in bytes. This allows
the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
					 */

					uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet->atsc3_rtp_fixed_header->packet_offset;
					assert(baseband_header_packet_length < 65535);
					atsc3_stltp_baseband_packet->baseband_packet_length = baseband_header_packet_length;
					atsc3_stltp_baseband_packet->baseband_packet = calloc(baseband_header_packet_length, sizeof(uint8_t));

					uint32_t rtp_baseband_header_remaining_length = atsc3_stltp_tunnel_packet->udp_packet->data_length - 12;

					__STLTP_PARSER_DEBUG(" ----baseband packet-----");
					__STLTP_PARSER_DEBUG("     total packet length: %u",  atsc3_stltp_baseband_packet->baseband_packet_length);
					__STLTP_PARSER_DEBUG("     fragment 0 length:   %u",  rtp_baseband_header_remaining_length);
					memcpy(atsc3_stltp_baseband_packet->baseband_packet, &atsc3_stltp_tunnel_packet->udp_packet->data[12], rtp_baseband_header_remaining_length);
					__STLTP_PARSER_DEBUG("     first bytes:         0x%2x 0x%2x", atsc3_stltp_baseband_packet->baseband_packet[0], atsc3_stltp_baseband_packet->baseband_packet[1]);



				}

			}

		} else {
			//append...
		}


	}
//
//	uint8_t alp_packet_header_byte_1 = *binary_payload++;
//	uint8_t alp_packet_header_byte_2 = *binary_payload++;
//
//	alp_packet_header_t alp_packet_header;
//	alp_packet_header.packet_type = (alp_packet_header_byte_1 >> 5) & 0x7;
//	alp_packet_header.payload_configuration = (alp_packet_header_byte_1 >> 4) & 0x1;
//	alp_packet_header.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
//	alp_packet_header.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
//
//	__STLTP_PARSER_DEBUG("ALP packet type: : 0x%x (should be 0x4 - x100 - LLP signaling packet)\n", alp_packet_header.packet_type);
//	__STLTP_PARSER_DEBUG("payload config   : %d\n", alp_packet_header.payload_configuration);
//	__STLTP_PARSER_DEBUG("header mode      : %d\n", alp_packet_header.header_mode);
//	__STLTP_PARSER_DEBUG("ALP header length: %d\n", alp_packet_header.length);
//	__STLTP_PARSER_DEBUG("-----------------------------\n");
//
//	//a/330 table 5.3
//	if(alp_packet_header.payload_configuration == 0 && alp_packet_header.header_mode == 0) {
//		//no additional header size
//		__STLTP_PARSER_DEBUG(" no additional ALP header bytes\n");
//
//	} else if (alp_packet_header.payload_configuration == 0 && alp_packet_header.header_mode == 1) {
//		//one byte additional header
//		uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
//		__STLTP_PARSER_DEBUG(" one additional ALP header byte: 0x%x\n", alp_additional_header_byte_1);
//	} else if (alp_packet_header.payload_configuration == 1) {
//		uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
//		__STLTP_PARSER_DEBUG(" one additional header byte -  0x%x\n", alp_additional_header_byte_1);
//	}
//	__STLTP_PARSER_DEBUG("-----------------------------\n");
//
//	/**
//	 * 5.10 additional header for signaling:
//signaling_information_hdr() {
//	signaling_type 				8 uimsbf
//	signaling_type_extension 	16 bslbf
//	signaling_version 			8 uimsbf
//	signaling_format 			2 uimsbf
//	signaling_encoding 			2 uimsbf
//	reserved 					4 ‘1111’
//
//	---40 bits total = 5 bytes
//}
//	 */
//
//	uint8_t *signaling_information_hdr_bytes = binary_payload;
//	binary_payload+=5;
//	__STLTP_PARSER_DEBUG("signaling information header:\n");
//	__STLTP_PARSER_DEBUG("signaling type           : %d (should be 0x1)\n", signaling_information_hdr_bytes[0]);
//	__STLTP_PARSER_DEBUG("signaling type extension : 0x%x 0x%x (should be 0xFF 0xFF)\n", signaling_information_hdr_bytes[1], signaling_information_hdr_bytes[2]);
//	__STLTP_PARSER_DEBUG("signaling version        : %d\n", signaling_information_hdr_bytes[3]);
//	__STLTP_PARSER_DEBUG("signaling format         : 0x%x (should be 0)\n", (signaling_information_hdr_bytes[4] >> 6) &0x3);
//	__STLTP_PARSER_DEBUG("signaling extension      : 0x%x (should be 0)\n", (signaling_information_hdr_bytes[4] >> 4) &0x3);
//	__STLTP_PARSER_DEBUG("reserved                 : 0x%x (should be 0xF - 1111)\n", signaling_information_hdr_bytes[4] &0xF);
//	__STLTP_PARSER_DEBUG("-----------------------------\n");
//
//	uint8_t lmt_table_byte = *binary_payload++;
//	//__STLTP_PARSER_DEBUG("lmt_table_byte: 0x%x\n", lmt_table_byte);
//	lmt_table_header_t lmt_table_header;
//	lmt_table_header.num_PLPs_minus1 = (lmt_table_byte >> 2) & 0x3F;
//	lmt_table_header.reserved = (lmt_table_byte) & 0x3;
//	__STLTP_PARSER_DEBUG("lmt table:\n");
//	__STLTP_PARSER_DEBUG("num_PLPs_minus1: 0x%x (%d - should be 0)\n", lmt_table_header.num_PLPs_minus1, lmt_table_header.num_PLPs_minus1);
//	__STLTP_PARSER_DEBUG("reserved bits  : 0x%x (should be hex:0x3 - xxxx xx11)\n", lmt_table_header.reserved);
//	__STLTP_PARSER_DEBUG("-----------------------------\n");
//
//	; //move one byte
//
//	for(int i=0; i <= lmt_table_header.num_PLPs_minus1; i++) {
//		uint8_t lmt_table_plp_byte = *binary_payload++;
//		uint8_t lmt_table_plp_byte_2 = *binary_payload++;
//
//		//__STLTP_PARSER_DEBUG("lmt bytes: 0x%x 0x%x", lmt_table_plp_byte, lmt_table_plp_byte_2);
//		lmt_table_plp_t lmt_table_plp;
//		lmt_table_plp.PLP_ID = (lmt_table_plp_byte >> 2) & 0x3F;
//		lmt_table_plp.reserved = lmt_table_plp_byte & 0x3;
//		lmt_table_plp.num_multicasts = lmt_table_plp_byte_2;
//
//		__STLTP_PARSER_DEBUG("plp row:\n");
//		__STLTP_PARSER_DEBUG("plp id        : 0x%x\n", lmt_table_plp.PLP_ID);
//		__STLTP_PARSER_DEBUG("reserved bits : 0x%x (should be hex:0x3 - xxxx xx11)\n", lmt_table_plp.reserved);
//		__STLTP_PARSER_DEBUG("num_multicasts: 0x%x (%d)\n", lmt_table_plp.num_multicasts, lmt_table_plp.num_multicasts);
//		__STLTP_PARSER_DEBUG("-------------------------\n");
//
//		for(int j=0; j < lmt_table_plp.num_multicasts; j++) {
//			if(binary_payload - binary_payload_start >= binary_payload_size) {
//				__STLTP_PARSER_DEBUG("----LMT PLP underflow!\n\n");
//				exit(1);
//			}
//			lmt_table_multicast_t lmt_table_multicast;
//
//			uint8_t *lmt_table_plp_byte = binary_payload;
//
//			lmt_table_multicast.src_ip_add = (lmt_table_plp_byte[0] << 24) | (lmt_table_plp_byte[1] << 16) | (lmt_table_plp_byte[2] << 8) | lmt_table_plp_byte[3];
//			lmt_table_multicast.dst_ip_add = (lmt_table_plp_byte[4] << 24) | (lmt_table_plp_byte[5] << 16) | (lmt_table_plp_byte[6] << 8) | lmt_table_plp_byte[7];
//			lmt_table_multicast.src_udp_port = (lmt_table_plp_byte[8] << 8) | lmt_table_plp_byte[9];
//			lmt_table_multicast.dst_udp_port = (lmt_table_plp_byte[10] << 8) | lmt_table_plp_byte[11];
//			lmt_table_multicast.sid_flag = (lmt_table_plp_byte[12] >> 7) & 0x1;
//			lmt_table_multicast.compressed_flag = (lmt_table_plp_byte[12] >> 6) & 0x1;
//			lmt_table_multicast.reserved = lmt_table_plp_byte[12] & 0x3f;
//
//			__STLTP_PARSER_DEBUG("Multicast Entry #%d\n", j);
//			__STLTP_PARSER_DEBUG("src_ip_add     : %d.%d.%d.%d\n", (lmt_table_multicast.src_ip_add >> 24) & 0xFF, (lmt_table_multicast.src_ip_add >> 16) & 0xFF, (lmt_table_multicast.src_ip_add >> 8) & 0xFF, lmt_table_multicast.src_ip_add & 0xFF);
//			__STLTP_PARSER_DEBUG("dst_ip_add     : %d.%d.%d.%d\n", (lmt_table_multicast.dst_ip_add >> 24) & 0xFF, (lmt_table_multicast.dst_ip_add >> 16) & 0xFF, (lmt_table_multicast.dst_ip_add >> 8) & 0xFF, lmt_table_multicast.dst_ip_add & 0xFF);
//			__STLTP_PARSER_DEBUG("src_udp_port   : %d (0x%x 0x%x)\n", lmt_table_multicast.src_udp_port, (lmt_table_multicast.src_udp_port >> 8) & 0xFF, lmt_table_multicast.src_udp_port & 0xFF);
//			__STLTP_PARSER_DEBUG("dst_udp_port   : %d (0x%x 0x%x)\n", lmt_table_multicast.dst_udp_port, (lmt_table_multicast.dst_udp_port >> 8) & 0xFF, lmt_table_multicast.dst_udp_port & 0xFF);
//			__STLTP_PARSER_DEBUG("sid_flag       : 0x%x\n", lmt_table_multicast.sid_flag);
//			__STLTP_PARSER_DEBUG("compressed_flag: 0x%x\n", lmt_table_multicast.compressed_flag);
//			__STLTP_PARSER_DEBUG("reserved bits  : 0x%x (should be 0x3f - xx111111) \n", lmt_table_multicast.reserved);
//			binary_payload+=13; 	//move 13bytes=104bits=32+32+16+16+1+1+6
//
//			if(lmt_table_multicast.sid_flag == 1) {
//				uint8_t sid = *binary_payload++;
//				__STLTP_PARSER_DEBUG("sid            : 0x%x\n", sid);
//			}
//
//			if(lmt_table_multicast.compressed_flag == 1) {
//				uint8_t context_id = *binary_payload++;
//				__STLTP_PARSER_DEBUG("context_id     : 0x%x\n", context_id);
//			}
//
//			__STLTP_PARSER_DEBUG("-------------------------\n");
//
//		}
//	}

	return 0;
}
