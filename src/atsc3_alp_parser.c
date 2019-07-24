/*
 * atsc3_alp_parser.c
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#include "atsc3_alp_parser.h"

int _ALP_PARSER_INFO_ENABLED = 1;
int _ALP_PARSER_DEBUG_ENABLED = 1;

pcap_t* descrInject = NULL;


/**
 A/322-2018 - Section 5.2 Baseband Formatting:
 ..The baseband formatting block creates one or more PLPs as directed by the Scheduler. At the output of the baseband formatting block, each PLP consists of a stream of Baseband Packets and there is exactly one Baseband Packet per defined FEC Frame...
 
 5.2.1 Mapping ALP Packets to Baseband Packets
 A Baseband Packet shall consist of a header, described in Section 5.2.2, and a payload containing ALP packets, shown in Figure 5.3. Padding, if present, shall be added to the Baseband Packet Header. Baseband Packets have fixed length Kpayload, with the length determined by the outer code type, inner code rate and code length chosen for the target PLP. For specific values of Kpayload , see Table 6.1 and Table 6.2.
 ALP packets shall be mapped to the payload part in the same order they are received. The reordering of ALP packets in the Baseband Packet is not permitted. When the received ALP packets are not sufficient to create a Baseband Packet of size Kpayload, padding shall be added to the Baseband Packet Header to complete the Baseband Packet. See Section 5.2.2.3.2 for details.
 When the received ALP packets are enough to fill the Baseband Packet but the last ALP packet does not fit perfectly within the Baseband Packet, that ALP packet may be split between the current Baseband Packet with the remainder of the ALP packet transmitted at the start of the next Baseband Packet. When splitting is used, ALP packets shall be split in byte units only. When the final ALP packet in the Baseband Packet is not split, padding shall be used in the extension field of the Baseband Packet Header to completely fill the Baseband Packet. In Figure 5.3 the final ALP packet is split between the current Baseband Packet and the next Baseband Packet.
 
                                  \| ALP PACKET |    | ALP |    |   ALP PACKET   |  | ALP |  |   ALP P|/ACKET   |
+----------------------------------\------------------------------------------------------------------+
| Baseband Packet                   \                                                                 |
+------------------------------------+----------------------------------------------------------------+
|                  Header            |                    Payload                                     |
+-----------------------------------------------------------------------------------------------------+
 +--------------/------------\-------------------+
 | Base Field | Optional Field | Extension Field |
 +-----------------------------------------------+
 
 base_field {
    mode:                     1 bit
        if(mode==0) {
            pointer (LSB)         7 bits
        } else {                  ------
            pointer (LSB)         7 bits
 
            pointer (MSB)         6 bits
            optional field        2 bits
 
             =00 No    Ext. Mode     NO optional field present
             =01 Short Ext. Mode     EXT_TYPE  3 bits  | EXT_LEN       5 bits | Extension (0-31 bytes)
             =10 Long  Ext. Mode     EXT_TYPE  3 bits  | EXT_LEN (lsb) 5 bits | EXT_LEN (msb) 8 bits   |   Extension (0-full bbp)
             =11 Mixed Ext. Mode     EXT_TYPE  3 bits  | EXT_LEN (lsb) 5 bits | EXT_LEN (msb) 8 bits   |   Extension (0-full bbp)
 
 **/
 
void atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet) {

	uint8_t *binary_payload = atsc3_stltp_baseband_packet->payload;
    
    atsc3_baseband_packet_header_t* atsc3_baseband_packet_header = calloc(1, sizeof(atsc3_baseband_packet_header_t));
    
    atsc3_baseband_packet_header->base_field_mode = (*binary_payload >> 7) &0x1;
    atsc3_baseband_packet_header->base_field_pointer = (*binary_payload++) &0x7F;
    
    __ALP_PARSER_INFO("-----------------------------");
    __ALP_PARSER_INFO("Baseband Packet Header");
    __ALP_PARSER_INFO("base field mode   : %x",    atsc3_baseband_packet_header->base_field_mode);
    __ALP_PARSER_INFO("base field pointer: 0x%x",  atsc3_baseband_packet_header->base_field_pointer);
    int bbp_pointer_count = 0;
    
    if(!atsc3_baseband_packet_header->base_field_mode) {
        for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->base_field_pointer; bbp_pointer_count++) {
            binary_payload++;
        }
        __ALP_PARSER_INFO(" -> seeking ptr: %d", bbp_pointer_count);
      
        
    } else {
        atsc3_baseband_packet_header->base_field_pointer |= (((*binary_payload >>2) &0x3F) << 7);
        atsc3_baseband_packet_header->option_field_mode = (*binary_payload++) & 0x02;
        
        //no option field, no extension field, resolve from base_field_pointer
        for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->base_field_pointer; bbp_pointer_count++) {
            binary_payload++;
        }
        __ALP_PARSER_INFO(" -> seeking ptr: %d", bbp_pointer_count);
        
        
        if(atsc3_baseband_packet_header->option_field_mode == 0x00) {
            //noop
            __ALP_PARSER_INFO(" -> no extension");
      
        } else {
            
            atsc3_baseband_packet_header->ext_type = (*binary_payload >> 5) & 0x7;
            atsc3_baseband_packet_header->ext_len = (*binary_payload++) & 0x1F;
            
            if(atsc3_baseband_packet_header->option_field_mode == 0x01) {
                //short extension mode - 1 byte
                for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->ext_len; bbp_pointer_count++) {
                    binary_payload++;
                }
                __ALP_PARSER_INFO(" -> ext 0x01: %d", bbp_pointer_count);

            } else if(atsc3_baseband_packet_header->option_field_mode == 0x02) {
                //long extension mode - 2 bytes
                atsc3_baseband_packet_header->ext_len |= (((*binary_payload++) & 0xFF) << 5);

                for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->ext_len; bbp_pointer_count++) {
                    binary_payload++;
                }
                
                __ALP_PARSER_INFO(" -> ext 0x02: %d", bbp_pointer_count);

                
            } if(atsc3_baseband_packet_header->option_field_mode == 0x03) {
                //mixed extension bode - 2 bytes
                atsc3_baseband_packet_header->ext_len |= (((*binary_payload++) & 0xFF) << 5);

                for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->ext_len; bbp_pointer_count++) {
                    binary_payload++;
                }
                
                __ALP_PARSER_INFO(" -> ext 0x03: %d", bbp_pointer_count);

            }
        }
    }
  
    __ALP_PARSER_INFO("option field mode : 0x%x",  atsc3_baseband_packet_header->option_field_mode);
    __ALP_PARSER_INFO("ext type          : 0x%x",  atsc3_baseband_packet_header->ext_type);
    __ALP_PARSER_INFO("ext len           : 0x%x",  atsc3_baseband_packet_header->ext_len);
    __ALP_PARSER_INFO("-----------------------------");

	uint8_t alp_packet_header_byte_1 = *binary_payload++;
	uint8_t alp_packet_header_byte_2 = *binary_payload++;

	alp_packet_header_t alp_packet_header;
	alp_packet_header.packet_type = (alp_packet_header_byte_1 >> 5) & 0x7;
	alp_packet_header.payload_configuration = (alp_packet_header_byte_1 >> 4) & 0x1;
    __ALP_PARSER_INFO("-----------------------------");
    __ALP_PARSER_INFO("ALP packet type     : 0x%x", alp_packet_header.packet_type);
	__ALP_PARSER_INFO("payload config      : %d", alp_packet_header.payload_configuration);

    uint32_t alp_payload_length = 0;
    
	if(alp_packet_header.payload_configuration == 0) {
		alp_packet_header.alp_packet_header_mode.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_header_mode.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		__ALP_PARSER_INFO("header mode        : %d", alp_packet_header.alp_packet_header_mode.header_mode);
		__ALP_PARSER_INFO("length             : %d", alp_packet_header.alp_packet_header_mode.length);
		__ALP_PARSER_INFO("-----------------------------");
        alp_payload_length = alp_packet_header.alp_packet_header_mode.length;

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
        
		__ALP_PARSER_INFO("segmentation_concatenation : %d", alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation);
		__ALP_PARSER_INFO("length	                  : %d", alp_packet_header.alp_packet_segmentation_concatenation.length);
		__ALP_PARSER_INFO("-----------------------------");
        alp_payload_length = alp_packet_header.alp_packet_segmentation_concatenation.length;

        if(alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation == 0) {
            //segmentation_hdr
            __ALP_PARSER_INFO("segmentation_hdr()");
            uint8_t additional_header_byte_1 = *binary_payload++;
            alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.segment_sequence_number = (additional_header_byte_1 >> 3) & 0x1F;
            alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.last_segment_indicator = (additional_header_byte_1 >> 2) & 0x1;
            alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.SIF = (additional_header_byte_1 >> 1) & 0x1;
            alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.HEF = (additional_header_byte_1) & 0x1;
            __ALP_PARSER_INFO("segment_sequence_number     : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.segment_sequence_number);
            __ALP_PARSER_INFO("last_segment_indicator      : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.last_segment_indicator);
            __ALP_PARSER_INFO("SIF                         : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.SIF);
            __ALP_PARSER_INFO("HEF                         : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.HEF);
            __ALP_PARSER_INFO("-----------------------------");
            
        } else {
            //concatenation_hdr
            __ALP_PARSER_INFO("concatenation_hdr()");
        }
	}
    
    if(alp_payload_length && descrInject) {
        uint32_t eth_frame_size = alp_payload_length + 14;
        uint8_t* eth_frame = calloc(eth_frame_size, sizeof(uint8_t));
        
        eth_frame[0]=1;
        eth_frame[1]=1;
        eth_frame[2]=1;
        eth_frame[3]=1;
        eth_frame[4]=1;
        eth_frame[5]=1;
        
        /* set mac source to 2:2:2:2:2:2 */
        eth_frame[6]=2;
        eth_frame[7]=2;
        eth_frame[8]=2;
        eth_frame[9]=2;
        eth_frame[10]=2;
        eth_frame[11]=2;
        eth_frame[12]=0x08;
        eth_frame[13]=0x00;

        memcpy(&eth_frame[14], binary_payload, alp_payload_length);
        
        if (pcap_sendpacket(descrInject, eth_frame, eth_frame_size) != 0) {
            __ALP_PARSER_ERROR("error sending the packet: %s", pcap_geterr(descrInject));
            return;
        }
    }
    
//
//    __ALP_PARSER_INFO("ALP: 0x%x 0x%x 0x%x 0x%x   0x%x 0x%x 0x%x 0x%x   0x%x 0x%x 0x%x 0x%x   0x%x 0x%x 0x%x 0x%x",
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++,
//                      *binary_payload++);

    //cleanup
    if(atsc3_baseband_packet_header) {
        free(atsc3_baseband_packet_header);
        atsc3_baseband_packet_header = NULL;
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
