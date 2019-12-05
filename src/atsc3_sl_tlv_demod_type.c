/*
 *  atsc3_sl_tlv_demod_type.c
 *  Created on: Nov 22, 2019
 */

#include "atsc3_sl_tlv_demod_type.h"

int _SL_TLV_DEMOD_DEBUG_ENABLED = 0;
int _SL_TLV_DEMOD_TRACE_ENABLED = 0;

//impl

/*
 * atsc3_sl_tlv_payload_parse_from_block_t
 *
 * 		caller is responsible for setting i_pos to the start of the SL_TLV magic string
 *
 * 		TODO: jjustman-2019-11-23: 	add seek for searching for magic string from block_t,
 * 									add support for carry-over of ALP re-constitution
 * 									add support for returing partial ALP frame payload if block_t is incomplete
 *
 */

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t(block_t* atsc3_sl_tlv_payload_unparsed_block) {

    uint8_t* buf_start = block_Get(atsc3_sl_tlv_payload_unparsed_block);
    uint8_t* buf_end = buf_start + (atsc3_sl_tlv_payload_unparsed_block->p_size - atsc3_sl_tlv_payload_unparsed_block->i_pos);
    uint8_t* buf = buf_start;

    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = calloc(1, sizeof(atsc3_sl_tlv_payload_t));

    //our TLV header must be at least 188 bytes to parse
    if(buf_end - buf_start < 188) {
    	__SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: remaining payload length is less than TLV 188 bytes: %ld", (buf_end - buf_start));
    	free(atsc3_sl_tlv_payload);
    	return NULL;
    }

    //read our magic number
    //looks like we are already in host order?
    //atsc3_sl_tlv_payload->magic_number = ntohl(*((uint32_t*)(buf)));
    __SL_TLV_DEMOD_TRACE("atsc3_sl_tlv_payload_parse_from_block_t: magic number buf is: 0x%08x", *(uint32_t*)buf);

    atsc3_sl_tlv_payload->magic_number = *(uint32_t*)(buf);
    if(atsc3_sl_tlv_payload->magic_number != 0x24681357) {
        __SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: position: %d, magic number is not 0x24681357, parsed as: 0x%08x",
        		atsc3_sl_tlv_payload_unparsed_block->i_pos,
        		atsc3_sl_tlv_payload->magic_number);
        free(atsc3_sl_tlv_payload);
        return NULL;
    }
    buf+=4;
    
    __SL_TLV_DEMOD_TRACE("parsing SL TLV packet with magic: 0x%8x, position: %d", atsc3_sl_tlv_payload->magic_number, atsc3_sl_tlv_payload_unparsed_block->i_pos);

    //atsc3_sl_tlv_payload->alp_packet_size = ntohl(*((uint32_t*)(buf)));
    atsc3_sl_tlv_payload->alp_packet_size = *(uint32_t*)(buf);

    __SL_TLV_DEMOD_TRACE( "alp packet size: %d", atsc3_sl_tlv_payload->alp_packet_size);
    buf+=4;
    
    atsc3_sl_tlv_payload->plp_number = *buf++;
    __SL_TLV_DEMOD_TRACE(" plp number: %d", atsc3_sl_tlv_payload->plp_number);
    
    atsc3_sl_tlv_payload->ts_size = *buf++;
    __SL_TLV_DEMOD_TRACE(" ts_size packet size: %d", atsc3_sl_tlv_payload->ts_size);

    atsc3_sl_tlv_payload->tlv_size = *buf++;
    __SL_TLV_DEMOD_TRACE(" tlv_size packet size: %d", atsc3_sl_tlv_payload->tlv_size);

    atsc3_sl_tlv_payload->alp_trailing_padding_size = *buf++;
    __SL_TLV_DEMOD_TRACE(" alp_trailing_padding_size packet size: %d", atsc3_sl_tlv_payload->alp_trailing_padding_size);

    atsc3_sl_tlv_payload->reserved_b12_b15 = ntohl((*((uint32_t*)(buf))>>8 & 0x00FFFFFF));
    __SL_TLV_DEMOD_TRACE("  reserved_b12_b15: 0x%06x", atsc3_sl_tlv_payload->reserved_b12_b15);
    buf+=3;
    
    atsc3_sl_tlv_payload->reserved_b16 = *buf++;
    __SL_TLV_DEMOD_TRACE("  reserved_b16 packet size: 0x%02x", atsc3_sl_tlv_payload->reserved_b16);

    atsc3_sl_tlv_payload->reserved_b17_b19 = ntohl((*((uint32_t*)(buf))>>8 & 0x00FFFFFF));
    __SL_TLV_DEMOD_TRACE("  reserved_b17_b19: 0x%06x", atsc3_sl_tlv_payload->reserved_b17_b19);
    buf+=3;

    atsc3_sl_tlv_payload->reserved_b20_b23 = ntohl(*((uint32_t*)(buf)));
    __SL_TLV_DEMOD_TRACE("  reserved_b20_b23: 0x%08x", atsc3_sl_tlv_payload->reserved_b20_b23);
    buf+=4;
    
    //increment past remaining TLV payload (23 bytes)
    buf += (188 - 23);

    uint32_t remaining_block_t_size = buf_end - buf;
    __SL_TLV_DEMOD_TRACE("  remaining block_t size: %d, alp_packet size + trailing size: %d",
    		remaining_block_t_size,
			atsc3_sl_tlv_payload->alp_packet_size + atsc3_sl_tlv_payload->alp_trailing_padding_size);


    if((atsc3_sl_tlv_payload->alp_packet_size + atsc3_sl_tlv_payload->alp_trailing_padding_size) > (buf_end - buf)) {
    	atsc3_sl_tlv_payload->alp_payload_complete = false;
    	atsc3_sl_tlv_payload->alp_payload = NULL;
        atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size = buf_end - buf;
    } else {
    	atsc3_sl_tlv_payload->alp_payload_complete = true;
        atsc3_sl_tlv_payload->alp_payload = block_Alloc(atsc3_sl_tlv_payload->alp_packet_size);
        block_Write(atsc3_sl_tlv_payload->alp_payload, buf, atsc3_sl_tlv_payload->alp_packet_size);
        buf += atsc3_sl_tlv_payload->alp_packet_size;
        //discard alp_trailing_padding size - garbage data?
        buf += atsc3_sl_tlv_payload->alp_trailing_padding_size;
        //properly parse out ALP packet header size here
        atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size = buf - buf_start;
    }

    atsc3_sl_tlv_payload_unparsed_block->i_pos += atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size;


    return atsc3_sl_tlv_payload;
}

void atsc3_sl_tlv_payload_free(atsc3_sl_tlv_payload_t** atsc3_sl_tlv_payload_p) {
    if(atsc3_sl_tlv_payload_p) {
        atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = *atsc3_sl_tlv_payload_p;
        if(atsc3_sl_tlv_payload) {
            block_Destroy(&atsc3_sl_tlv_payload->alp_payload);
            free(atsc3_sl_tlv_payload);
            atsc3_sl_tlv_payload = NULL;
        }
        *atsc3_sl_tlv_payload_p = NULL;
    }
}

void atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload) {
    if(atsc3_sl_tlv_payload) {
    	if(atsc3_sl_tlv_payload->alp_payload_complete) {
    		__SL_TLV_DEMOD_DEBUG("SL_TLV: magic_number valid: %d, ALP payload complete, total parsed payload size: %d, PLP: %d, alp_packet_size: %d (block_t->p_size: %d), alp_trailing_padding: %d, alp first two bytes: 0x%02x 0x%02x, alp last two bytes: 0x%02x 0x%02x",
                             atsc3_sl_tlv_payload->magic_number == 0x24681357,
							 atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size,
                             atsc3_sl_tlv_payload->plp_number,
                             atsc3_sl_tlv_payload->alp_packet_size,
							 atsc3_sl_tlv_payload->alp_payload->p_size,
                             atsc3_sl_tlv_payload->alp_trailing_padding_size,
                             atsc3_sl_tlv_payload->alp_payload->p_buffer[0],
                             atsc3_sl_tlv_payload->alp_payload->p_buffer[1],
							 atsc3_sl_tlv_payload->alp_payload->p_buffer[atsc3_sl_tlv_payload->alp_payload->p_size-2],
							 atsc3_sl_tlv_payload->alp_payload->p_buffer[atsc3_sl_tlv_payload->alp_payload->p_size-1]);
    	} else {
    		__SL_TLV_DEMOD_DEBUG("SL_TLV: magic_number valid: %d, ALP payload NOT complete, total incomplete parsed payload size: %d, PLP: %d, alp_packet_size: %d, alp_trailing_padding: %d",
    		                             atsc3_sl_tlv_payload->magic_number == 0x24681357,
    									 atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size,
    		                             atsc3_sl_tlv_payload->plp_number,
    		                             atsc3_sl_tlv_payload->alp_packet_size,
    		                             atsc3_sl_tlv_payload->alp_trailing_padding_size);
    	}
    }
}

