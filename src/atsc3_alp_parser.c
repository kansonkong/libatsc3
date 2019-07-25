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
    uint8_t *binary_payload_start = binary_payload;
    
    __ALP_PARSER_INFO("---------------------------------------");
    __ALP_PARSER_INFO("Baseband Packet Header: pointer: %p, length: %u", binary_payload, atsc3_stltp_baseband_packet->payload_length);
    __ALP_PARSER_INFO("Raw hex: 0x%02hhX 0x%02hhX 0x%02hhX 0x%02hhX", binary_payload[0], binary_payload[1], binary_payload[2], binary_payload[3]);
    __ALP_PARSER_INFO("---------------------------------------");

    atsc3_baseband_packet_header_t* atsc3_baseband_packet_header = calloc(1, sizeof(atsc3_baseband_packet_header_t));
    //base field byte 1
    atsc3_baseband_packet_header->base_field_mode = (*binary_payload >> 7) & 0x1;
    atsc3_baseband_packet_header->base_field_pointer = (*binary_payload)   & 0x7F;
    binary_payload++;
    __ALP_PARSER_INFO("base field mode          : %x",    atsc3_baseband_packet_header->base_field_mode);
    int bbp_pointer_count = 0;
    
    if(atsc3_baseband_packet_header->base_field_mode == 0) {
        __ALP_PARSER_INFO("base field pointer (7b)  : 0x%02X (%d bytes)",  atsc3_baseband_packet_header->base_field_pointer, atsc3_baseband_packet_header->base_field_pointer);
        //read our pointer here
        uint8_t* baseband_payload_start = binary_payload;
        __ALP_PARSER_INFO(" -> before seeking ptr, payload position: %p", binary_payload);
        //no option field, no extension field, resolve from base_field_pointer
        for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->base_field_pointer; bbp_pointer_count++) {
            binary_payload++;
        }
        __ALP_PARSER_INFO(" -> after seeking ptr : %d, payload: %p (diff: %ld)", bbp_pointer_count, binary_payload, (binary_payload - baseband_payload_start));
        
    } else { //base field mode == 1
        
        //A322 - 5.2.2.1 - if we have a pointer value of 8191 (2 byte base heder offset)
        //->no ALP packet starting within that baseband packet

        atsc3_baseband_packet_header->base_field_pointer |= (((*binary_payload >>2) &0x3F) << 7);
        __ALP_PARSER_INFO("base field pointer (13b) : 0x%04X (%d bytes)",  atsc3_baseband_packet_header->base_field_pointer, atsc3_baseband_packet_header->base_field_pointer);

        if(atsc3_baseband_packet_header->base_field_pointer  == 8191) {
            __ALP_PARSER_INFO(" -> squelching padding, ptr: %d", bbp_pointer_count);
            __ALP_PARSER_INFO("---------------------------------------");
            goto cleanup;
        }
        
        atsc3_baseband_packet_header->option_field_mode = (*binary_payload) & 0x02;
        //base field byte 2
        binary_payload++;

        /**
         OFI
         (bit)   (hex)
         -----   -----
         00      0x0   No extension mode
         01      0x1   Short extension mode
         10      0x2   Long Extension mode
         11      0x3   Mixed Extension mode
         **/
        
        if(atsc3_baseband_packet_header->option_field_mode == 0x00) {
            __ALP_PARSER_INFO("option field mode        : 0x%x (No extension mode)",  atsc3_baseband_packet_header->option_field_mode);
            __ALP_PARSER_INFO("extension mode           : n/a");
            __ALP_PARSER_INFO("extension len            : n/a");
        } else {
            atsc3_baseband_packet_header->ext_type = (*binary_payload >> 5) & 0x7;
            atsc3_baseband_packet_header->ext_len = (*binary_payload) & 0x1F;
            //option field byte 1
            binary_payload++;

            if(atsc3_baseband_packet_header->option_field_mode == 0x01) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Short extension mode)",  atsc3_baseband_packet_header->option_field_mode);
                __ALP_PARSER_INFO("extension type           : 0x%x", atsc3_baseband_packet_header->ext_type);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet_header->ext_len);
            } else if(atsc3_baseband_packet_header->option_field_mode == 0x02) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Long extension mode)",  atsc3_baseband_packet_header->option_field_mode);
                __ALP_PARSER_INFO("extension type           : 0x%x", atsc3_baseband_packet_header->ext_type);
                atsc3_baseband_packet_header->ext_len |= (((*binary_payload) & 0xFF) << 5);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet_header->ext_len);
                //option field byte 2
                binary_payload++;

            } if(atsc3_baseband_packet_header->option_field_mode == 0x03) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Mixed extension mode)",  atsc3_baseband_packet_header->option_field_mode);
                __ALP_PARSER_INFO("num_ext                  : 0x%x", atsc3_baseband_packet_header->ext_type);
                atsc3_baseband_packet_header->ext_len |= (((*binary_payload) & 0xFF) << 5);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet_header->ext_len);
                //option field byte 2
                binary_payload++;

            }
            
            //read our extension fields here
            uint8_t* extension_block_start = binary_payload;
            __ALP_PARSER_INFO(" -> before reading extension block, payload position: %p, extension len: %hu", binary_payload, atsc3_baseband_packet_header->ext_len);
            for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->ext_len; bbp_pointer_count++) {
                binary_payload++;
            }
            __ALP_PARSER_INFO(" -> after reading extension block, payload position : %p, diff: %lu", binary_payload, (binary_payload - extension_block_start));
        }
        
        uint8_t* baseband_payload_start = binary_payload;
        __ALP_PARSER_INFO(" -> before seeking ptr, base field ptr: %d, payload: %p", atsc3_baseband_packet_header->base_field_pointer, binary_payload);
        for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet_header->base_field_pointer; bbp_pointer_count++) {
            binary_payload++;
        }
        __ALP_PARSER_INFO(" -> after seeking ptr : %d, payload: %p (bytes: %lu)", bbp_pointer_count, binary_payload, (binary_payload - baseband_payload_start));
    }
    uint8_t* alp_binary_payload_start = binary_payload;

    uint8_t alp_packet_header_byte_1 = *binary_payload++;
	uint8_t alp_packet_header_byte_2 = *binary_payload++;
    
    __ALP_PARSER_INFO("------------------------------------------------");
    __ALP_PARSER_INFO("ALP Packet: pointer: %p, first 2 bytes: 0x%02X, 0x%02X",
                      alp_binary_payload_start, alp_packet_header_byte_1, alp_packet_header_byte_2);

	alp_packet_header_t alp_packet_header;
	alp_packet_header.packet_type = (alp_packet_header_byte_1 >> 5) & 0x7;
    __ALP_PARSER_INFO("-----------------------------------------------");
    if(alp_packet_header.packet_type == 0x0) {

        __ALP_PARSER_INFO("ALP packet type            : 0x%x (IPv4)", alp_packet_header.packet_type);
    } else if(alp_packet_header.packet_type == 0x2) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (compressed IP packet)", alp_packet_header.packet_type);

    } else if(alp_packet_header.packet_type == 0x4) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (LLS packet)", alp_packet_header.packet_type);
        
    } else if(alp_packet_header.packet_type == 0x6) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (packet type extension)", alp_packet_header.packet_type);
        
    } else if(alp_packet_header.packet_type == 0x7) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (MPEG-2 TS)", alp_packet_header.packet_type);
        
    } else {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (reserved/other)", alp_packet_header.packet_type);
    }
    
    alp_packet_header.payload_configuration = (alp_packet_header_byte_1 >> 4) & 0x1;

    __ALP_PARSER_INFO("payload config             : %d", alp_packet_header.payload_configuration);

    uint32_t alp_payload_length = 0;
    
	if(alp_packet_header.payload_configuration == 0) {
		alp_packet_header.alp_packet_header_mode.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_header_mode.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		__ALP_PARSER_INFO("header mode                : %d", alp_packet_header.alp_packet_header_mode.header_mode);
		__ALP_PARSER_INFO("length                     : %d", alp_packet_header.alp_packet_header_mode.length);
		__ALP_PARSER_INFO("-----------------------------");
        alp_payload_length = alp_packet_header.alp_packet_header_mode.length;

		if(alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 0) {
				//no additional header size
			__ALP_PARSER_INFO(" no additional ALP header bytes");
		} else if (alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 1) {
			//one byte additional header
            uint8_t alp_additional_header_byte_1 = *binary_payload;
			__ALP_PARSER_INFO(" one additional ALP header byte: 0x%x (header_mode==1)", alp_additional_header_byte_1);
		} else if (alp_packet_header.payload_configuration == 1) {
            uint8_t alp_additional_header_byte_1 = *binary_payload;
			__ALP_PARSER_INFO(" one additional header byte -  0x%x (header_mode==0)", alp_additional_header_byte_1);
		}
		__ALP_PARSER_INFO("-----------------------------");

	} else if(alp_packet_header.payload_configuration == 1) {
		alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header.alp_packet_segmentation_concatenation.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
        
		__ALP_PARSER_INFO("segmentation_concatenation: %d", alp_packet_header.alp_packet_segmentation_concatenation.segmentation_concatenation);
		__ALP_PARSER_INFO("length	                : %d", alp_packet_header.alp_packet_segmentation_concatenation.length);
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
            __ALP_PARSER_INFO("segment_sequence_number: 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.segment_sequence_number);
            __ALP_PARSER_INFO("last_segment_indicator : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.last_segment_indicator);
            __ALP_PARSER_INFO("SIF                    : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.SIF);
            __ALP_PARSER_INFO("HEF                    : 0x%x", alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.HEF);
            __ALP_PARSER_INFO("---------------------------------------");
            
            if(alp_packet_header.alp_packet_segmentation_concatenation.alp_segmentation_header.HEF) {
                uint8_t hef_type = *binary_payload++;
                uint8_t hef_length_minus1 = *binary_payload++;
                __ALP_PARSER_INFO("extension type: 0x%x, extension length: %u", hef_type, hef_length_minus1);
                for(int i=0; i < hef_length_minus1; i++) {
                    binary_payload++;
                }
            }
        } else {
            //concatenation_hdr
            __ALP_PARSER_INFO("concatenation_hdr()");
        }
	}
    
    if(alp_packet_header.packet_type == 0x4) {
        //See A/330 - Figure 5.6 Structure of ALP signaling packets (Base Header and Additional Header).
        
        if(alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 1) {
            alp_packet_header.alp_packet_header_mode.alp_single_packet_header.length_MSB = (*binary_payload >> 3) & 0x1F;
            alp_packet_header.alp_packet_header_mode.alp_single_packet_header.reserved = 1;
            alp_packet_header.alp_packet_header_mode.alp_single_packet_header.SIF = (*binary_payload >> 1) & 0x1;
            alp_packet_header.alp_packet_header_mode.alp_single_packet_header.HEF = (*binary_payload) & 0x1;
            __ALP_PARSER_INFO("Additional header for single packet:");
            __ALP_PARSER_INFO("length MSB: %d", alp_packet_header.alp_packet_header_mode.alp_single_packet_header.length_MSB);
            __ALP_PARSER_INFO("SIF: %d", alp_packet_header.alp_packet_header_mode.alp_single_packet_header.SIF );
            __ALP_PARSER_INFO("HEF: %d", alp_packet_header.alp_packet_header_mode.alp_single_packet_header.HEF);

        }
        
        if(alp_packet_header.payload_configuration == 0 && alp_packet_header.alp_packet_header_mode.header_mode == 0) {
            //read 5.2.1 Additional Header for Signaling Information - 40 bits
            uint8_t si_header[5];
            memcpy(&si_header, binary_payload, 5);
            binary_payload+=5;
            
            alp_packet_header.alp_additional_header_for_signaling_information.signaling_type = si_header[0];
            alp_packet_header.alp_additional_header_for_signaling_information.signaling_type_extension = (si_header[1] << 8) | si_header[2];
            alp_packet_header.alp_additional_header_for_signaling_information.signaling_version = si_header[3];
            alp_packet_header.alp_additional_header_for_signaling_information.signaling_format = (si_header[4] >> 6) & 0x3;
            alp_packet_header.alp_additional_header_for_signaling_information.signaling_encoding = (si_header[4] >> 4) & 0x3;
            alp_packet_header.alp_additional_header_for_signaling_information.reserved = (si_header[4]) & 0xF;
            __ALP_PARSER_INFO("---------------------------------------");
            __ALP_PARSER_INFO("LLS Packet");
            if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_type == 0x1) {
                __ALP_PARSER_INFO("Signaling type: 0x%0x (LMT)", alp_packet_header.alp_additional_header_for_signaling_information.signaling_type);
            } else {
                __ALP_PARSER_INFO("Signaling type: 0x%0x (other)", alp_packet_header.alp_additional_header_for_signaling_information.signaling_type);
            }
            
            __ALP_PARSER_INFO("Signaling type extension: 0x%04x", alp_packet_header.alp_additional_header_for_signaling_information.signaling_type_extension);
            __ALP_PARSER_INFO("Signaling version       : 0x%02x", alp_packet_header.alp_additional_header_for_signaling_information.signaling_version);
            if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_format == 0) {
                __ALP_PARSER_INFO("Signaling format      : 0x0 (binary)");
            } else if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_format == 1) {
                __ALP_PARSER_INFO("Signaling format      : 0x1 (xml)");
            } else if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_format == 2) {
                __ALP_PARSER_INFO("Signaling format      : 0x2 (json)");
            } else {
                __ALP_PARSER_INFO("Signaling format      : 0x3 (reserved)");
            }
            
            if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_encoding == 0) {
                __ALP_PARSER_INFO("Signaling encoding      : 0x0 (no compression)");
            } else if(alp_packet_header.alp_additional_header_for_signaling_information.signaling_encoding == 1) {
                __ALP_PARSER_INFO("Signaling encoding      : 0x01 (deflate)");
            } else {
                __ALP_PARSER_INFO("Signaling encoding      : 0x%x (reserved)", alp_packet_header.alp_additional_header_for_signaling_information.signaling_encoding);
            }
            __ALP_PARSER_INFO("Signaling reserved bits     : %d%d%d%d", alp_packet_header.alp_additional_header_for_signaling_information.reserved >> 3 & 0x1,
                                                                        alp_packet_header.alp_additional_header_for_signaling_information.reserved >> 2 & 0x1,
                                                                        alp_packet_header.alp_additional_header_for_signaling_information.reserved >> 1 & 0x1,
                                                                        alp_packet_header.alp_additional_header_for_signaling_information.reserved & 0x1);
            

            
        } else {
            //read:
            //pc=0 ^ hc = 1: additional header for single packets
            //pc=1 ^ hc = 0: additional header for segmentation
            //pc=1 ^ hc = 1: additional header for concatenation
            
            //then
            //read 5.2.1 Additional Header for Signaling Information
        }
    }
    
    //if we are an IP packet, push this via pcap
    if(alp_packet_header.packet_type == 0x0 && alp_payload_length && descrInject) {
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
        __ALP_PARSER_INFO("STLTP reflector: sending payload size: %u", eth_frame_size);
       
        if (pcap_sendpacket(descrInject, eth_frame, eth_frame_size) != 0) {
            __ALP_PARSER_ERROR("error sending the packet: %s", pcap_geterr(descrInject));
        }
        free(eth_frame);
        eth_frame = NULL;
    }
    
cleanup:
    //cleanup
    if(atsc3_baseband_packet_header) {
        if(atsc3_baseband_packet_header->extension) {
            free(atsc3_baseband_packet_header->extension);
            atsc3_baseband_packet_header = NULL;
        }
        
        free(atsc3_baseband_packet_header);
        atsc3_baseband_packet_header = NULL;
    }
    
    //cleanup of the atsc3_stltp_baseband_packet->payload occurs in
    //atsc3_stltp_baseband_packet_free_v, which is called from
    //atsc3_stltp_tunnel_packet_clear_completed_inner_packets
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
