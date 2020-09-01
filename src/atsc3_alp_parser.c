/*
 * atsc3_alp_parser.c
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#include "atsc3_alp_parser.h"

#include "atsc3_baseband_packet_types.h"

int _ALP_PARSER_INFO_ENABLED = 0;
int _ALP_PARSER_DEBUG_ENABLED = 0;

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
 
 
 Baseband payload size:
 
 Table 6.1 Length of Kpayload (bits) for Ninner = 64800
 
 Code Kpayload Mouter Kpayload Mouter Kpayload Mouter Minner Nouter Rate (BCH) (BCH) (CRC) (CRC) (no outer) (no outer)
 2/15 8448  192 8608 32 8640 0 56160 8640
 3/15 12768  192 12928 32 12960 0 51840 12960
 4/15 17088 192 17248 32 17280 0 47520 17280
 5/15 21408 192 21568 32 21600 0 43200 21600
 6/15 25728 192 25888 32 25920 0 38880 25920
 7/15 30048 192 30208 32 30240 0 34560 30240
 8/15 34368 192 34528 32 34560 0 30240 34560
 9/15 38688 192 38848 32 38880 0 25920 38880
 10/15 43008 192 43168 32 43200 0 21600 43200
 11/15 47328 192 47488 32 47520 0 17280 47520
 12/15 51648 192 51808 32 51840 0 12960 51840
 13/15 55968 192 56128 32  56160 0 8640 56160
 
 Table 6.2 Length of Kpayload (bits) for Ninner = 16200
 
 Code Kpayload Mouter Kpayload Mouter Kpayload Mouter Minner Nouter Rate (BCH) (BCH) (CRC) (CRC) (no outer) (no outer)
 2/15 1992 168 2128 32  2160 0 14040 2160
 3/15 3072 168 3208 32 3240 0 12960 3240
 4/15 4152 168 4288 32 4320 0 11880 4320
 5/15 5232 168 5368 32 5400 0 10800 5400
 6/15 6312 168 6448 32 6480 0 9720 6480
 7/15 7392 168 7528 32 7560 0 8640 7560
 8/15 8472 168 8608 32 8640 0 7560 8640
 9/15 9552 168 9688 32 9720 0 6480 9720
 10/15 10632 168 10768 32 10800 0 5400 10800
 11/15 11712 168 11848 32 11880 0 4320 11880
 12/15 12792 168 12928 32 12960 0 3240 12960
 13/15 13872 168 14008 32 14040 0 2160 14040
 
 
 Since ALP packets may be split across Baseband Packets, the start of the payload of a Baseband
 Packet does not necessarily signify the start of an ALP packet.
 
 The Base Field of a Baseband
 Packet shall provide the start position of the first ALP packet that begins in the Baseband Packet
 through a pointer.
 
 The value of the pointer shall be the offset (in bytes) from the beginning of the payload to the
 start of the first ALP packet that begins in that Baseband Packet.
 
 When an ALP packet begins at
 the start of the payload portion of a Baseband Packet, the value of the pointer shall be 0. When
 there is no ALP packet starting within that Baseband Packet, the value of the pointer shall be 8191
 and a 2 byte Base Field shall be used.
 
 When there are no ALP packets and only padding is present,
 the value of the pointer shall also be 8191 and a 2 byte Base Field shall be used, together with any
 necessary Optional Fields and Extension Fields as signaled by the OFI (Optional Field Indicator)
 field.
 **/
 
atsc3_baseband_packet_t* atsc3_stltp_parse_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet) {

	uint8_t *binary_payload = atsc3_stltp_baseband_packet->payload;
    uint8_t *binary_payload_start = binary_payload;

    uint32_t binary_payload_length = atsc3_stltp_baseband_packet->payload_length;
    
    __ALP_PARSER_INFO("---------------------------------------");
    __ALP_PARSER_INFO("Baseband Packet Header: pointer: %p, sequence_number: %d, port: %d, length: %u",
                      binary_payload,
                      atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                      atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port,
                      atsc3_stltp_baseband_packet->payload_length);
    __ALP_PARSER_INFO("Raw hex: 0x%02hhX 0x%02hhX 0x%02hhX 0x%02hhX", binary_payload[0], binary_payload[1], binary_payload[2], binary_payload[3]);
    __ALP_PARSER_INFO("---------------------------------------");

    atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_baseband_packet_new();
    atsc3_baseband_packet_set_plp_from_stltp_baseband_packet(atsc3_baseband_packet, atsc3_stltp_baseband_packet);
    atsc3_baseband_packet_set_bootstrap_timing_ref_from_stltp_baseband_packet(atsc3_baseband_packet, atsc3_stltp_baseband_packet);

    //base field byte 1
    atsc3_baseband_packet->base_field_mode = (*binary_payload >> 7) & 0x1;
    atsc3_baseband_packet->base_field_pointer = (*binary_payload)   & 0x7F;
    binary_payload++;
    __ALP_PARSER_INFO("base field mode          : %x",    atsc3_baseband_packet->base_field_mode);
    int bbp_pointer_count = 0;
    
    uint8_t* baseband_pre_pointer_payload_start = NULL;

    if(atsc3_baseband_packet->base_field_mode == 0) {
        __ALP_PARSER_INFO("base field pointer (7b)  : 0x%02X (%d bytes)",  atsc3_baseband_packet->base_field_pointer, atsc3_baseband_packet->base_field_pointer);
        baseband_pre_pointer_payload_start = binary_payload;
        
    } else {
        //base field mode == 1
        //A322 - 5.2.2.1 - if we have a pointer value of 8191 (2 byte base heder offset)
        //->no ALP packet starting within that baseband packet

        atsc3_baseband_packet->base_field_pointer |= (((*binary_payload >>2) &0x3F) << 7);
        __ALP_PARSER_INFO("base field pointer (13b) : 0x%04X (%d bytes)",  atsc3_baseband_packet->base_field_pointer, atsc3_baseband_packet->base_field_pointer);

        /*  A/322:2018 - Section 5.2.2 - Baseband Packet Header
            Optional Field - OFI

         Mode: 1
         Pointer MSB: 2 LSB
         
          2LSB  Description             Fields
         -----  -----------             ----------
            00  No optional field
            01  Short extension mode    |EXT_TYPE: 3b, EXT_LEN: 5b|     |EXT..0-31bytes|
            10  Long Extension mode     |EXT_TYPE: 3b, EXT_LEN: 5b LSB| |EXT_LEN 8b MSB| |EXT..0-fullbb|
            11  Mixed Extension mode    |NUM_EXT:  3b, EXT_LEN: 5b LSB| |EXT_LEN 8b MSB| |EXT..0-fullbb|
         
         */
        atsc3_baseband_packet->option_field_mode = (*binary_payload) & 0x3;
        
        //base field byte 2
        binary_payload++;

        bool baseband_packet_is_padding = false;
        if(atsc3_baseband_packet->base_field_pointer == 8191) {
            if(atsc3_baseband_packet->option_field_mode == 0) {
                /**
                 The value of the pointer shall be the offset (in bytes) from the beginning of the payload to the start of the first ALP packet that begins in that Baseband Packet.
                 
                 When an ALP packet begins at the start of the payload portion of a Baseband Packet, the value of the pointer shall be 0. When there is no ALP packet starting within that Baseband Packet, the value of the pointer shall be 8191 and a 2 byte base header shall be used.
                 
                 When there are no ALP packets and only padding is present, the value of the pointer shall also be 8191 and a 2 byte base header shall be used, together with any necessary optional and extension fields as signaled by the OFI (Optional Field Indicator) field.
                 **/
                uint32_t baseband_packet_size_to_copy = binary_payload_length - (binary_payload - binary_payload_start);
                __ALP_PARSER_INFO(" base_field_pointer=8191, but option_field_mode=0, copying full packet into alp_payload_pre_pointer, size: %d, binary_payload_length: %d", baseband_packet_size_to_copy, binary_payload_length);

                atsc3_baseband_packet->alp_payload_pre_pointer = block_Alloc(baseband_packet_size_to_copy);
                block_Write(atsc3_baseband_packet->alp_payload_pre_pointer, binary_payload, baseband_packet_size_to_copy);
                block_Rewind(atsc3_baseband_packet->alp_payload_pre_pointer);
                return atsc3_baseband_packet;
            }
        }
        

        /**
         OFI
         (bit)   (hex)
         -----   -----
         00      0x0   No extension mode
         01      0x1   Short extension mode
         10      0x2   Long Extension mode
         11      0x3   Mixed Extension mode
         **/
        
        if(atsc3_baseband_packet->option_field_mode == 0x00) {
            __ALP_PARSER_INFO("option field mode        : 0x%x (No extension mode)",  atsc3_baseband_packet->option_field_mode);
            __ALP_PARSER_INFO("extension mode           : n/a");
            __ALP_PARSER_INFO("extension len            : n/a");
        } else {
            atsc3_baseband_packet->ext_type = (*binary_payload >> 5) & 0x7;
            atsc3_baseband_packet->ext_len = (*binary_payload) & 0x1F;
            //option field byte 1
            binary_payload++;

            if(atsc3_baseband_packet->option_field_mode == 0x01) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Short extension mode)",  atsc3_baseband_packet->option_field_mode);
                __ALP_PARSER_INFO("extension type           : 0x%x", atsc3_baseband_packet->ext_type);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet->ext_len);
            } else if(atsc3_baseband_packet->option_field_mode == 0x02) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Long extension mode)",  atsc3_baseband_packet->option_field_mode);
                __ALP_PARSER_INFO("extension type           : 0x%x", atsc3_baseband_packet->ext_type);
                atsc3_baseband_packet->ext_len |= (((*binary_payload) & 0xFF) << 5);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet->ext_len);
                //option field byte 2
                binary_payload++;

            } if(atsc3_baseband_packet->option_field_mode == 0x03) {
                __ALP_PARSER_INFO("option field mode        : 0x%x (Mixed extension mode)",  atsc3_baseband_packet->option_field_mode);
                __ALP_PARSER_INFO("num_ext                  : 0x%x", atsc3_baseband_packet->ext_type);
                atsc3_baseband_packet->ext_len |= (((*binary_payload) & 0xFF) << 5);
                __ALP_PARSER_INFO("extension len            : 0x%x", atsc3_baseband_packet->ext_len);
                //option field byte 2
                binary_payload++;
            }
            
            if(atsc3_baseband_packet->base_field_pointer == 8191 && atsc3_baseband_packet->ext_type == 0x7) { //ext_type = 111
                uint32_t baseband_packet_remaining_size = binary_payload_length - (binary_payload - binary_payload_start);

                if(atsc3_baseband_packet->ext_len == baseband_packet_remaining_size) {
                    __ALP_PARSER_INFO(" base_field_pointer=8191, only padding present, ext_len == baseband_packet_remaining_size: %d",  atsc3_baseband_packet->ext_len);
                    goto cleanup;
                    
                } else if(baseband_packet_remaining_size > atsc3_baseband_packet->ext_len) {
                    
                    binary_payload += atsc3_baseband_packet->ext_len;
                    baseband_packet_remaining_size -= atsc3_baseband_packet->ext_len;
                    __ALP_PARSER_INFO(" base_field_pointer=8191, copying trimmed extension packet into alp_payload_pre_pointer, size: %d, binary_payload_length: %d, ext_len: %d",
                                      baseband_packet_remaining_size,
                                      binary_payload_length,
                                      atsc3_baseband_packet->ext_len);

                } else {
                    __ALP_PARSER_INFO(" base_field_pointer=8191, copying full packet into alp_payload_pre_pointer, size: %d, binary_payload_length: %d, no ext_len",
                                      baseband_packet_remaining_size,
                                      binary_payload_length);
                }
                
                atsc3_baseband_packet->alp_payload_pre_pointer = block_Alloc(baseband_packet_remaining_size);
                block_Write(atsc3_baseband_packet->alp_payload_pre_pointer, binary_payload, baseband_packet_remaining_size);
                block_Rewind(atsc3_baseband_packet->alp_payload_pre_pointer);
                return atsc3_baseband_packet;
            }
            
            //else, other extension field, e.g. counter or reserved, read here for payload...
            if(atsc3_baseband_packet->ext_type == 0x7) {
                uint8_t* old_binary_payload = binary_payload;
                binary_payload += atsc3_baseband_packet->ext_len;
                
                __ALP_PARSER_INFO("seeking past padding (ext_type == 111), from: %p, bytes: %d, to: %p", old_binary_payload, atsc3_baseband_packet->ext_len, binary_payload);
            } else {
                uint8_t* extension_block_start = binary_payload;
                __ALP_PARSER_WARN(" -> UNKNOWN EXTENSION TYPE: 0x%x, before reading extension block, payload position: %p, extension len: %hu",
                                  atsc3_baseband_packet->ext_type,
                                  binary_payload, atsc3_baseband_packet->ext_len);
                for(bbp_pointer_count=0; bbp_pointer_count < atsc3_baseband_packet->ext_len; bbp_pointer_count++) {
                            binary_payload++;
                }
                __ALP_PARSER_INFO(" -> after reading extension block, payload position : %p, diff: %lu", binary_payload, (binary_payload - extension_block_start));
            }
        }
        baseband_pre_pointer_payload_start = binary_payload;
    }
    
    //build pre-pointer and post-pointer payloads
    
    int32_t bytes_pre_pointer_remaining_len = binary_payload_length - (baseband_pre_pointer_payload_start - binary_payload_start);
    
    if(!bytes_pre_pointer_remaining_len) {
        __ALP_PARSER_ERROR(" no bytes remaining for baseband pre_pointer packet_payload! binary_payload_length: %d, ptr: start: %p, baseband_pre_pointer_payload_start: %p",
                           binary_payload_length,
                           baseband_pre_pointer_payload_start,
                           binary_payload_start);
        
        goto cleanup;
    }
    
    __ALP_PARSER_INFO(" -> before ptr, base field ptr: %d, payload position : %p, pre_pointer bytes remaining: %d",
                      atsc3_baseband_packet->base_field_pointer,
                      baseband_pre_pointer_payload_start,
                      bytes_pre_pointer_remaining_len);
   
    
    if(atsc3_baseband_packet->base_field_pointer && bytes_pre_pointer_remaining_len) {
        __ALP_PARSER_INFO(" copying block_t into alp_payload_pre_pointer, from %p to %p (len: %d)",
                          baseband_pre_pointer_payload_start,
                          baseband_pre_pointer_payload_start + atsc3_baseband_packet->base_field_pointer,
                          atsc3_baseband_packet->base_field_pointer);
        
        atsc3_baseband_packet->alp_payload_pre_pointer = block_Alloc(atsc3_baseband_packet->base_field_pointer);
        block_Write(atsc3_baseband_packet->alp_payload_pre_pointer, baseband_pre_pointer_payload_start, atsc3_baseband_packet->base_field_pointer);
        block_Rewind(atsc3_baseband_packet->alp_payload_pre_pointer);        
    }
    //end our pre-pointer
    
    //build our post-pointer
    //start our pointer payload at pre_payload + base_field_pointer
    uint8_t* baseband_pointer_payload_start = baseband_pre_pointer_payload_start + atsc3_baseband_packet->base_field_pointer;
    
    int32_t bytes_remaining_len = binary_payload_length - (baseband_pointer_payload_start - binary_payload_start);
    if(!bytes_remaining_len) {
        __ALP_PARSER_ERROR(" no bytes remaining for baseband packet_payload! binary_payload_length: %d, ptr: start: %p, baseband_pre_pointer_payload_start: %p",
                           binary_payload_length,
                           baseband_pre_pointer_payload_start,
                           binary_payload_start);
        
        goto cleanup;
    }
    
    __ALP_PARSER_INFO(" -> post_pointer: start at: %p, size: %d, payload: 0x%02x 0x%02x 0x%02x 0x%02x",
                      baseband_pointer_payload_start,
                      bytes_remaining_len,
                      (bytes_remaining_len > 0 ? baseband_pointer_payload_start[0] : 0),
                      (bytes_remaining_len > 1 ? baseband_pointer_payload_start[1] : 0),
                      (bytes_remaining_len > 2 ? baseband_pointer_payload_start[2] : 0),
                      (bytes_remaining_len > 3 ? baseband_pointer_payload_start[3] : 0));
    
    atsc3_baseband_packet->alp_payload_post_pointer = block_Alloc(bytes_remaining_len);
    block_Write(atsc3_baseband_packet->alp_payload_post_pointer, baseband_pointer_payload_start, bytes_remaining_len);
    block_Rewind(atsc3_baseband_packet->alp_payload_post_pointer);
    __ALP_PARSER_INFO("---------------------------------------");
    return atsc3_baseband_packet;

cleanup:
    atsc3_baseband_packet_free(&atsc3_baseband_packet);
    return NULL;
}


//parse relative position of baseband_packet_payload,

/*
 * jjustman-2020-08-17 - TODO: wrap this with better block_t reads so we don't possibly over-read from our starting_block_size
 */
atsc3_alp_packet_t* atsc3_alp_packet_parse(uint8_t plp_num, block_t* baseband_packet_payload) {
    uint32_t starting_block_size = block_Remaining_size(baseband_packet_payload);
    
    if(starting_block_size == 0) {
    	__ALP_PARSER_DEBUG("atsc3_alp_packet_parse: remaining size is 0 bytes, returning NULL, ptr: %p, pos: %d, size: %d",
                           baseband_packet_payload,
                           baseband_packet_payload->i_pos,
                           baseband_packet_payload->p_size);
        return NULL;
    }
    
    //check for alp underrun which will need re-fragmenting
    if(starting_block_size < 8) {
        __ALP_PARSER_DEBUG("atsc3_alp_packet_parse: remaining size less than 8 bytes, ptr: %p, pos: %d, size: %d",
                           baseband_packet_payload,
                           baseband_packet_payload->i_pos,
                           baseband_packet_payload->p_size);
        
        return NULL;
    }
    
    uint8_t* alp_binary_payload_start =  block_Get(baseband_packet_payload);//binary_payload;
    uint8_t* binary_payload = alp_binary_payload_start;
    
    uint8_t alp_packet_header_byte_1 = *binary_payload++;
	uint8_t alp_packet_header_byte_2 = *binary_payload++;
    
    atsc3_alp_packet_t* alp_packet = calloc(1, sizeof(atsc3_alp_packet_t));
    alp_packet->plp_num = plp_num;

    __ALP_PARSER_INFO("------------------------------------------------");
    __ALP_PARSER_INFO("ALP Packet: recv on plp: %d, pointer: %p, payload start pointer: %p, first 2 bytes: 0x%02X, 0x%02X",
    				  alp_packet->plp_num,
    				  alp_packet,
                      alp_binary_payload_start,
                      alp_packet_header_byte_1,
                      alp_packet_header_byte_2);

	alp_packet_header_t* alp_packet_header = &alp_packet->alp_packet_header;
	alp_packet_header->packet_type = (alp_packet_header_byte_1 >> 5) & 0x7;
    __ALP_PARSER_INFO("-----------------------------------------------");
    if(alp_packet_header->packet_type == 0x0) {

        __ALP_PARSER_INFO("ALP packet type            : 0x%x (IPv4)", alp_packet_header->packet_type);
    } else if(alp_packet_header->packet_type == 0x2) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (compressed IP packet)", alp_packet_header->packet_type);

    } else if(alp_packet_header->packet_type == 0x4) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (Link Layer Signalling/LMT packet)", alp_packet_header->packet_type);

    } else if(alp_packet_header->packet_type == 0x6) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (packet type extension)", alp_packet_header->packet_type);
        
    } else if(alp_packet_header->packet_type == 0x7) {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (MPEG-2 TS)", alp_packet_header->packet_type);
        
    } else {
        __ALP_PARSER_INFO("ALP packet type            : 0x%x (reserved/other)", alp_packet_header->packet_type);
    }
    
    alp_packet_header->payload_configuration = (alp_packet_header_byte_1 >> 4) & 0x1;

    __ALP_PARSER_INFO("payload config             : %d", alp_packet_header->payload_configuration);

    uint32_t alp_payload_length = 0;
    
	if(alp_packet_header->payload_configuration == 0) {
		alp_packet_header->alp_packet_header_mode.header_mode = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header->alp_packet_header_mode.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
		__ALP_PARSER_INFO("header mode                : %d", alp_packet_header->alp_packet_header_mode.header_mode);
		__ALP_PARSER_INFO("length                     : %d", alp_packet_header->alp_packet_header_mode.length);
		__ALP_PARSER_INFO("-----------------------------");
        alp_payload_length = alp_packet_header->alp_packet_header_mode.length;

		if(alp_packet_header->payload_configuration == 0 && alp_packet_header->alp_packet_header_mode.header_mode == 0) {
				//no additional header size
			__ALP_PARSER_INFO(" no additional ALP header bytes");
		} else if (alp_packet_header->payload_configuration == 0 && alp_packet_header->alp_packet_header_mode.header_mode == 1) {
			//one byte additional header
            uint8_t alp_additional_header_byte_1 = *binary_payload;
			__ALP_PARSER_INFO(" one additional ALP header byte: 0x%x (header_mode==1)", alp_additional_header_byte_1);
		} else if (alp_packet_header->payload_configuration == 1) {
            uint8_t alp_additional_header_byte_1 = *binary_payload;
			__ALP_PARSER_INFO(" one additional header byte -  0x%x (header_mode==0)", alp_additional_header_byte_1);
		}
		__ALP_PARSER_INFO("-----------------------------");

	} else if(alp_packet_header->payload_configuration == 1) {
		alp_packet_header->alp_packet_segmentation_concatenation.segmentation_concatenation = (alp_packet_header_byte_1 >> 3) & 0x01;
		alp_packet_header->alp_packet_segmentation_concatenation.length = (alp_packet_header_byte_1 & 0x7) << 8 | alp_packet_header_byte_2;
        
		__ALP_PARSER_INFO("segmentation_concatenation: %d", alp_packet_header->alp_packet_segmentation_concatenation.segmentation_concatenation);
		__ALP_PARSER_INFO("length	                : %d", alp_packet_header->alp_packet_segmentation_concatenation.length);
		__ALP_PARSER_INFO("-----------------------------");
        alp_payload_length = alp_packet_header->alp_packet_segmentation_concatenation.length;

        
        if(alp_packet_header->alp_packet_segmentation_concatenation.segmentation_concatenation == 0) {
            //segmentation_hdr
            __ALP_PARSER_INFO("segmentation_hdr()");
            uint8_t additional_header_byte_1 = *binary_payload++;
            alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.segment_sequence_number = (additional_header_byte_1 >> 3) & 0x1F;
            alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.last_segment_indicator = (additional_header_byte_1 >> 2) & 0x1;
            alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.SIF = (additional_header_byte_1 >> 1) & 0x1;
            alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.HEF = (additional_header_byte_1) & 0x1;
            __ALP_PARSER_INFO("segment_sequence_number: 0x%x", alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.segment_sequence_number);
            __ALP_PARSER_INFO("last_segment_indicator : 0x%x", alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.last_segment_indicator);
            __ALP_PARSER_INFO("SIF                    : 0x%x", alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.SIF);
            __ALP_PARSER_INFO("HEF                    : 0x%x", alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.HEF);
            __ALP_PARSER_INFO("---------------------------------------");
            
            if(alp_packet_header->alp_packet_segmentation_concatenation.alp_segmentation_header.HEF) {
                uint8_t hef_type = *binary_payload++;
                uint8_t hef_length_minus1 = *binary_payload++;
                __ALP_PARSER_INFO("extension type: 0x%x, extension length: %u", hef_type, hef_length_minus1);
                for(int i=0; i < hef_length_minus1; i++) {
                	if((starting_block_size - (binary_payload - alp_binary_payload_start)) < 1) {
                		 __ALP_PARSER_WARN("atsc3_alp_packet_parse: remaining size is %lu bytes, returning NULL, ptr: %p, pos: %d, size: %d",
								   starting_block_size - (binary_payload - alp_binary_payload_start),
								   baseband_packet_payload,
								   baseband_packet_payload->i_pos,
								   baseband_packet_payload->p_size);
                		 return NULL;
                	}
                    binary_payload++;
                }
            }
        } else {
            //concatenation_hdr
            __ALP_PARSER_INFO("concatenation_hdr()");
        }
	}
    
    if(alp_packet_header->packet_type == 0x4) {
        //See A/330 - Figure 5.6 Structure of ALP signaling packets (Base Header and Additional Header).
        
        if(alp_packet_header->payload_configuration == 0 && alp_packet_header->alp_packet_header_mode.header_mode == 1) {
            alp_packet_header->alp_packet_header_mode.alp_single_packet_header.length_MSB = (*binary_payload >> 3) & 0x1F;
            alp_packet_header->alp_packet_header_mode.alp_single_packet_header.reserved = 1;
            alp_packet_header->alp_packet_header_mode.alp_single_packet_header.SIF = (*binary_payload >> 1) & 0x1;
            alp_packet_header->alp_packet_header_mode.alp_single_packet_header.HEF = (*binary_payload) & 0x1;
            __ALP_PARSER_INFO("Additional header for single packet:");
            __ALP_PARSER_INFO("length MSB: %d", alp_packet_header->alp_packet_header_mode.alp_single_packet_header.length_MSB);
            __ALP_PARSER_INFO("SIF: %d", alp_packet_header->alp_packet_header_mode.alp_single_packet_header.SIF );
            __ALP_PARSER_INFO("HEF: %d", alp_packet_header->alp_packet_header_mode.alp_single_packet_header.HEF);

        }
        
        if(alp_packet_header->payload_configuration == 0 && alp_packet_header->alp_packet_header_mode.header_mode == 0) {
            //read 5.2.1 Additional Header for Signaling Information - 40 bits
        	if((starting_block_size - (binary_payload - alp_binary_payload_start)) < 1) {
        		 __ALP_PARSER_WARN("atsc3_alp_packet_parse: remaining size is %lu bytes, returning NULL, ptr: %p, pos: %d, size: %d",
        									   starting_block_size - (binary_payload - alp_binary_payload_start),
        									   baseband_packet_payload,
        									   baseband_packet_payload->i_pos,
        									   baseband_packet_payload->p_size);
				 return NULL;
			}
            uint8_t si_header[5];
            memcpy(&si_header, binary_payload, 5);
            binary_payload+=5;
            
            alp_packet_header->alp_additional_header_for_signaling_information.signaling_type = si_header[0];
            alp_packet_header->alp_additional_header_for_signaling_information.signaling_type_extension = (si_header[1] << 8) | si_header[2];
            alp_packet_header->alp_additional_header_for_signaling_information.signaling_version = si_header[3];
            alp_packet_header->alp_additional_header_for_signaling_information.signaling_format = (si_header[4] >> 6) & 0x3;
            alp_packet_header->alp_additional_header_for_signaling_information.signaling_encoding = (si_header[4] >> 4) & 0x3;
            alp_packet_header->alp_additional_header_for_signaling_information.reserved = (si_header[4]) & 0xF;
            __ALP_PARSER_INFO("---------------------------------------");
            __ALP_PARSER_INFO("Link Layer Signalling Packet");
            if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_type == 0x1) {
                __ALP_PARSER_INFO("Signaling type: 0x%0x (LMT)", alp_packet_header->alp_additional_header_for_signaling_information.signaling_type);
            } else {
                __ALP_PARSER_INFO("Signaling type: 0x%0x (other)", alp_packet_header->alp_additional_header_for_signaling_information.signaling_type);
            }
            
            __ALP_PARSER_INFO("Signaling type extension: 0x%04x", alp_packet_header->alp_additional_header_for_signaling_information.signaling_type_extension);
            __ALP_PARSER_INFO("Signaling version       : 0x%02x", alp_packet_header->alp_additional_header_for_signaling_information.signaling_version);
            if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_format == 0) {
                __ALP_PARSER_INFO("Signaling format         : 0x0 (binary)");
            } else if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_format == 1) {
                __ALP_PARSER_INFO("Signaling format         : 0x1 (xml)");
            } else if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_format == 2) {
                __ALP_PARSER_INFO("Signaling format         : 0x2 (json)");
            } else {
                __ALP_PARSER_INFO("Signaling format         : 0x3 (reserved)");
            }
            
            if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_encoding == 0) {
                __ALP_PARSER_INFO("Signaling encoding      : 0x0 (no compression)");
            } else if(alp_packet_header->alp_additional_header_for_signaling_information.signaling_encoding == 1) {
                __ALP_PARSER_INFO("Signaling encoding      : 0x01 (deflate)");
            } else {
                __ALP_PARSER_INFO("Signaling encoding      : 0x%x (reserved)", alp_packet_header->alp_additional_header_for_signaling_information.signaling_encoding);
            }
            __ALP_PARSER_INFO("Signaling reserved bits : %d%d%d%d", alp_packet_header->alp_additional_header_for_signaling_information.reserved >> 3 & 0x1,
                                                                        alp_packet_header->alp_additional_header_for_signaling_information.reserved >> 2 & 0x1,
                                                                        alp_packet_header->alp_additional_header_for_signaling_information.reserved >> 1 & 0x1,
                                                                        alp_packet_header->alp_additional_header_for_signaling_information.reserved & 0x1);
            

			//parse out link_mapping_table (A/330 7.2)
            
        } else {
            //read:
            //pc=0 ^ hc = 1: additional header for single packets
            //pc=1 ^ hc = 0: additional header for segmentation
            //pc=1 ^ hc = 1: additional header for concatenation
            
            //then
            //read 5.2.1 Additional Header for Signaling Information
        }
    }
    //cleanup of the atsc3_stltp_baseband_packet->payload occurs in
    //atsc3_stltp_baseband_packet_free_v, which is called from
    //atsc3_stltp_tunnel_packet_clear_completed_inner_packets
    
    int32_t remaining_binary_payload_bytes = starting_block_size - (binary_payload - alp_binary_payload_start);

    if(remaining_binary_payload_bytes < 1) {
    	__ALP_PARSER_WARN("atsc3_alp_packet_parse: remaining size is %d bytes, returning NULL, ptr: %p, pos: %d, size: %d",
    		   remaining_binary_payload_bytes,
			   baseband_packet_payload,
			   baseband_packet_payload->i_pos,
			   baseband_packet_payload->p_size);
		 return NULL;
	}


    if(!alp_payload_length) {

        __ALP_PARSER_WARN("ALP payload length is: 0! packet_type is: 0x%x, seeking to end of payload, remaining: %d from pos: %lu, size: %d",
                          alp_packet_header->packet_type,
                          remaining_binary_payload_bytes,
                          baseband_packet_payload->i_pos + (binary_payload - alp_binary_payload_start),
                          baseband_packet_payload->p_size);
        
        //hack to eof
        baseband_packet_payload->i_pos = baseband_packet_payload->p_size;
        goto cleanup;
    }
                          

    alp_packet->alp_payload = block_Alloc(alp_payload_length);
    int32_t alp_payload_bytes_to_write = __MIN(remaining_binary_payload_bytes, alp_payload_length);
    if(alp_payload_bytes_to_write < alp_payload_length) {
        alp_packet->is_alp_payload_complete = false;
    } else {
        alp_packet->is_alp_payload_complete = true;
    }
    block_Write(alp_packet->alp_payload, binary_payload, alp_payload_bytes_to_write);
    block_Seek_Relative(baseband_packet_payload, alp_payload_bytes_to_write + (binary_payload - alp_binary_payload_start));
    
    __ALP_PARSER_INFO("alp_packet->alp_payload: building block_t: ALP payload to: %p, alp_payload_bytes_to_write: %d MIN(alp_payload_length: %d, baseband_packet_payload bytes: %d), remaining ALP bytes: %d, remaining baseband bytes: %d",
                      alp_packet,
                      alp_payload_bytes_to_write,
                      alp_payload_length,
                      remaining_binary_payload_bytes,
                      alp_payload_length - alp_payload_bytes_to_write,
                      block_Remaining_size(baseband_packet_payload));
    return alp_packet;
    
cleanup:
    if(alp_packet) {
        free(alp_packet);
        alp_packet = NULL;
    }
    
    return NULL;
}

void atsc3_alp_packet_packet_set_bootstrap_timing_ref_from_baseband_packet(atsc3_alp_packet_t* atsc3_alp_packet, atsc3_baseband_packet_t* atsc3_baseband_packet) {
	if(!atsc3_alp_packet || !atsc3_baseband_packet) {
		__ALP_PARSER_WARN("atsc3_alp_packet_packet_set_bootstrap_timing_ref_from_baseband_packet - atsc3_alp_packet: %p, atsc3_baseband_packet: %p is null!", atsc3_alp_packet, atsc3_baseband_packet);
		return;
	}
	atsc3_alp_packet->bootstrap_timing_data_timestamp_short_reference.seconds_pre = atsc3_baseband_packet->bootstrap_timing_data_timestamp_short_reference.seconds_pre;
	atsc3_alp_packet->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre = atsc3_baseband_packet->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre;

}


void atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, pcap_t* atsc3_baseband_alp_output_pcap_device_reference) {
    //iterate thru completd packets
    for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
        atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];

        //if we are an IP packet, push this via pcap
        if(atsc3_baseband_alp_output_pcap_device_reference && atsc3_alp_packet && atsc3_alp_packet->alp_packet_header.packet_type == 0x0) {
            
            block_Rewind(atsc3_alp_packet->alp_payload);
            uint8_t* alp_payload = block_Get(atsc3_alp_packet->alp_payload);
            uint32_t alp_payload_length = block_Remaining_size(atsc3_alp_packet->alp_payload);
            
            if(atsc3_alp_packet && alp_payload_length) {
                uint32_t eth_frame_size = alp_payload_length + 14;
                uint8_t* eth_frame = calloc(eth_frame_size, sizeof(uint8_t));
                
                eth_frame[0]=1;
                eth_frame[1]=1;
                eth_frame[2]=1;
                eth_frame[3]=1;
                eth_frame[4]=1;
                eth_frame[5]=1;
                
                /* set mac source to local timestamp */
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
                
                memcpy(&eth_frame[14], alp_payload, alp_payload_length);
                
                __ALP_PARSER_INFO("[%2x:%2x:%2x:%2x:%2x:%2x] STLTP reflector: sending packet_type: %d, payload size: %u",
                                  eth_frame[6],
                                  eth_frame[7],
                                  eth_frame[8],
                                  eth_frame[9],
                                  eth_frame[10],
                                  eth_frame[11],
                                  atsc3_alp_packet->alp_packet_header.packet_type, eth_frame_size);
                
                if (pcap_sendpacket(atsc3_baseband_alp_output_pcap_device_reference, eth_frame, eth_frame_size) != 0) {
                    __ALP_PARSER_ERROR("error sending the packet: %s", pcap_geterr(atsc3_baseband_alp_output_pcap_device_reference));
                }
                free(eth_frame);
                eth_frame = NULL;
            }
        }
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


void atsc3_alp_packet_collection_extract_lmt(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {
    //iterate thru completd packets
    for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
        atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];

        //if we are an link layer signalling packet
	    if(atsc3_alp_packet && atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
			//skip past our initial 5 bytes
			atsc3_alp_packet_extract_lmt(atsc3_alp_packet);
		}
	}
}



atsc3_link_mapping_table_t* atsc3_alp_packet_extract_lmt(atsc3_alp_packet_t* atsc3_alp_packet) {
	atsc3_link_mapping_table_t* atsc3_link_mapping_table = NULL;
	
	block_Rewind(atsc3_alp_packet->alp_payload);
	uint32_t alp_payload_length = block_Remaining_size(atsc3_alp_packet->alp_payload);
	if(alp_payload_length >=2) { //we need at least one PLP entry to parse out
		__ALP_PARSER_INFO("atsc3_alp_packet_collection_extract_lmt: alp_payload: %p, alp_payload_length after signalling header extension: %d", atsc3_alp_packet->alp_payload, alp_payload_length);
		
		atsc3_link_mapping_table = atsc3_link_mapping_table_new();
	    atsc3_link_mapping_table->alp_additional_header_for_signaling_information_signaling_version = atsc3_alp_packet->alp_packet_header.alp_additional_header_for_signaling_information.signaling_version;

		atsc3_link_mapping_table->num_PLPs_minus1 = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 6);
		atsc3_link_mapping_table->reserved = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 2);
		if(atsc3_link_mapping_table->reserved != 0x3) {
			__ALP_PARSER_WARN("atsc3_alp_packet_collection_extract_lmt: atsc3_link_mapping_table->reserved != 0x3");
		}
		
		__ALP_PARSER_INFO("atsc3_alp_packet_collection_extract_lmt: num_PLPs_minus1: %d", atsc3_link_mapping_table->num_PLPs_minus1);
		for(int i=0; i <= atsc3_link_mapping_table->num_PLPs_minus1; i++) {
			atsc3_link_mapping_table_plp_t* atsc3_link_mapping_table_plp = atsc3_link_mapping_table_plp_new();
			atsc3_link_mapping_table_plp->PLP_ID = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 6);
			atsc3_link_mapping_table_plp->reserved = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 2);
			if(atsc3_link_mapping_table_plp->reserved != 0x3) {
				__ALP_PARSER_WARN("atsc3_alp_packet_collection_extract_lmt: atsc3_link_mapping_table_plp->reserved != 0x3");
			}
			
			
			atsc3_link_mapping_table_plp->num_multicasts = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 8);
			for(int j=0; j < atsc3_link_mapping_table_plp->num_multicasts; j++) {
				atsc3_link_mapping_table_multicast_t* atsc3_link_mapping_table_multicast = atsc3_link_mapping_table_multicast_new();
				atsc3_link_mapping_table_multicast->src_ip_add = block_Read_uint32_ntohl(atsc3_alp_packet->alp_payload);
				atsc3_link_mapping_table_multicast->dst_ip_add = block_Read_uint32_ntohl(atsc3_alp_packet->alp_payload);
				
				atsc3_link_mapping_table_multicast->src_udp_port = block_Read_uint16_ntohs(atsc3_alp_packet->alp_payload);
				atsc3_link_mapping_table_multicast->dst_udp_port = block_Read_uint16_ntohs(atsc3_alp_packet->alp_payload);
				
				atsc3_link_mapping_table_multicast->sid_flag = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 1);
				atsc3_link_mapping_table_multicast->compressed_flag = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 1);
				atsc3_link_mapping_table_multicast->reserved = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 6);
				if(atsc3_link_mapping_table_multicast->reserved != 0x3F) {
					__ALP_PARSER_WARN("atsc3_alp_packet_collection_extract_lmt: atsc3_link_mapping_table_multicast->reserved != 0x3");
				}
				
				if(atsc3_link_mapping_table_multicast->sid_flag) {
					atsc3_link_mapping_table_multicast->sid_flag = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 8);
				}
				
				if(atsc3_link_mapping_table_multicast->compressed_flag) {
					atsc3_link_mapping_table_multicast->compressed_flag = block_Read_uint8_bitlen(atsc3_alp_packet->alp_payload, 8);
				}

				__ALP_PARSER_DEBUG("atsc3_alp_packet_collection_extract_lmt: plp_id: %d, adding mcast: src: %u.%u.%u.%u:%u, dest: %u.%u.%u.%u:%u, sid_flag: %d, compressed_flag: %d",
								   atsc3_link_mapping_table_plp->PLP_ID,
								   __toipandportnonstruct(atsc3_link_mapping_table_multicast->src_ip_add, atsc3_link_mapping_table_multicast->src_udp_port),
								   __toipandportnonstruct(atsc3_link_mapping_table_multicast->dst_ip_add, atsc3_link_mapping_table_multicast->dst_udp_port),
									atsc3_link_mapping_table_multicast->sid_flag,
								   atsc3_link_mapping_table_multicast->compressed_flag);
								   
				atsc3_link_mapping_table_plp_add_atsc3_link_mapping_table_multicast(atsc3_link_mapping_table_plp, atsc3_link_mapping_table_multicast);
			}
			atsc3_link_mapping_table_add_atsc3_link_mapping_table_plp(atsc3_link_mapping_table, atsc3_link_mapping_table_plp);
		}
	}
	return atsc3_link_mapping_table;
}


