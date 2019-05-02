/*
 * atsc3_alp_parser.c
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#include "atsc3_alp_parser.h"

int _ALP_PARSER_INFO_ENABLED = 1;
int _ALP_PARSER_DEBUG_ENABLED = 1;


void atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet) {

	uint8_t *binary_payload = atsc3_stltp_baseband_packet->payload;

	uint8_t alp_packet_header_byte_1 = *binary_payload++;
	uint8_t alp_packet_header_byte_2 = *binary_payload++;

	alp_packet_header_t alp_packet_header;
	alp_packet_header.packet_type = (alp_packet_header_byte_1 >> 5) & 0x7;
	alp_packet_header.payload_configuration = (alp_packet_header_byte_1 >> 4) & 0x1;
	__ALP_PARSER_INFO("		ALP packet type: : 0x%x", alp_packet_header.packet_type);
	__ALP_PARSER_INFO("		payload config   : %d", alp_packet_header.payload_configuration);

	if(alp_packet_header.payload_configuration == 0) {
		alp_packet_header.alp_packet_header_mode.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_header_mode.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		__ALP_PARSER_INFO("header mode      : %d", alp_packet_header.alp_packet_header_mode.header_mode);
		__ALP_PARSER_INFO("ALP header length: %d", alp_packet_header.alp_packet_header_mode.length);
		__ALP_PARSER_INFO("-----------------------------");

		if(alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 0) {
				//no additional header size
			__ALP_PARSER_INFO(" no additional ALP header bytes");
		} else if (alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 1) {
			//one byte additional header
			uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
			__ALP_PARSER_INFO(" one additional ALP header byte: 0x%x (header_mode==1)", alp_additional_header_byte_1);
		} else if (alp_packet_header.payload_configuration == 1) {
			uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
			__ALP_PARSER_INFO(" one additional header byte -  0x%x (header_mode==0)", alp_additional_header_byte_1);
		}
		__ALP_PARSER_INFO("-----------------------------");

	} else if(alp_packet_header.payload_configuration == 1) {
		alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_segmentation_concatenation.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		__ALP_PARSER_INFO("segmentation_concatenation: %d", alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation);
		__ALP_PARSER_INFO("ALP header length	     : %d", alp_packet_header.alp_packet_segmentation_concatenation.length);
		__ALP_PARSER_INFO("-----------------------------");


	}

}



/**
 *

	if(alp_packet_header.payload_configuration == 0) {
		alp_packet_header.alp_packet_header_mode.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_header_mode.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		printf("header mode      : %d\n", alp_packet_header.alp_packet_header_mode.header_mode);
		printf("ALP header length: %d\n", alp_packet_header.alp_packet_header_mode.length);
		printf("-----------------------------\n");

		if(alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 0) {
				//no additional header size
				printf(" no additional ALP header bytes\n");
		} else if (alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 1) {
			//one byte additional header
			uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
			printf(" one additional ALP header byte: 0x%x\n", alp_additional_header_byte_1);
		} else if (alp_packet_header.payload_configuration == 1) {
			uint8_t alp_additional_header_byte_1 = *binary_payload+=1;
			printf(" one additional header byte -  0x%x\n", alp_additional_header_byte_1);
		}
		printf("-----------------------------\n");

	}

	//a/330 table 5.3


	/**
	 * 5.10 additional header for signaling:

signaling_information_hdr() {
	signaling_type 				8 uimsbf
	signaling_type_extension 	16 bslbf
	signaling_version 			8 uimsbf
	signaling_format 			2 uimsbf
	signaling_encoding 			2 uimsbf
	reserved 					4 ‘1111’
}

	---40 bits total = 5 bytes
	 */
//
//	uint8_t *signaling_information_hdr_bytes = binary_payload;
//	binary_payload+=5;
//	printf("signaling information header:\n");
//	printf("signaling type           : %d (should be 0x1)\n", signaling_information_hdr_bytes[0]);
//	printf("signaling type extension : 0x%x 0x%x (should be 0xFF 0xFF)\n", signaling_information_hdr_bytes[1], signaling_information_hdr_bytes[2]);
//	printf("signaling version        : %d\n", signaling_information_hdr_bytes[3]);
//	printf("signaling format         : 0x%x (should be 0)\n", (signaling_information_hdr_bytes[4] >> 6) &0x3);
//	printf("signaling extension      : 0x%x (should be 0)\n", (signaling_information_hdr_bytes[4] >> 4) &0x3);
//	printf("reserved                 : 0x%x (should be 0xF - 1111)\n", signaling_information_hdr_bytes[4] &0xF);
//	printf("-----------------------------\n");
// *
// */
