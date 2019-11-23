/*
 *  atsc3_sl_tlv_demod_type.c
 *  Created on: Nov 22, 2019
 */

#include "atsc3_sl_tlv_demod_type.h"

int _SL_TLV_DEMOD_DEBUG_ENABLED = 1;
int _SL_TLV_DEMOD_TRACE_ENABLED = 1;

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
    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = calloc(1, sizeof(atsc3_sl_tlv_payload_t));

    uint8_t* buf_start = block_Get(atsc3_sl_tlv_payload_unparsed_block);
    uint8_t* buf_end = buf_start + atsc3_sl_tlv_payload_unparsed_block->p_size;
    uint8_t* buf = buf_start;

    //read our magic number first
    
    //looks like we are already in host order?
    //atsc3_sl_tlv_payload->magic_number = ntohl(*((uint32_t*)(buf)));
    __SL_TLV_DEMOD_TRACE("atsc3_sl_tlv_payload_parse_from_block_t: magic number buf is: 0x%08x", (uint32_t)(*buf));

    atsc3_sl_tlv_payload->magic_number = (uint32_t)(*buf);
    if(atsc3_sl_tlv_payload->magic_number != 0x24681357) {
        __SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: magic number is not 0x24681357, parsed as: 0x%08x", atsc3_sl_tlv_payload->magic_number);
        free(atsc3_sl_tlv_payload);
        return NULL;
    }
    buf+=4;
    
    __SL_TLV_DEMOD_TRACE("parsing SL TLV packet with magic: 0x%8x", atsc3_sl_tlv_payload->magic_number);

    atsc3_sl_tlv_payload->alp_packet_size = ntohl(*((uint32_t*)(buf)));
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
    __SL_TLV_DEMOD_TRACE( "reserved_b12_b15: %d", atsc3_sl_tlv_payload->reserved_b12_b15);
    buf+=3;
    
    atsc3_sl_tlv_payload->reserved_b16 = *buf++;
    __SL_TLV_DEMOD_TRACE(" reserved_b16 packet size: %d", atsc3_sl_tlv_payload->reserved_b16);


    atsc3_sl_tlv_payload->reserved_b17_b19 = ntohl((*((uint32_t*)(buf))>>8 & 0x00FFFFFF));
    __SL_TLV_DEMOD_TRACE( "reserved_b17_b19: %d", atsc3_sl_tlv_payload->reserved_b17_b19);
    buf+=3;

    atsc3_sl_tlv_payload->reserved_b20_b23 = ntohl(*((uint32_t*)(buf)));
    __SL_TLV_DEMOD_TRACE( "reserved_b20_b23: %d", atsc3_sl_tlv_payload->reserved_b20_b23);
    buf+=4;
    
    if(atsc3_sl_tlv_payload->alp_packet_size > (buf_end - buf_start)) {
    	atsc3_sl_tlv_payload->alp_payload_complete = false;
    	atsc3_sl_tlv_payload->alp_payload = NULL;
        atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size = buf_end - buf;

    } else {
    	atsc3_sl_tlv_payload->alp_payload_complete = true;
        atsc3_sl_tlv_payload->alp_payload = block_Alloc(atsc3_sl_tlv_payload->alp_packet_size);
        block_Write(atsc3_sl_tlv_payload->alp_payload, buf, atsc3_sl_tlv_payload->alp_packet_size);
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
        __SL_TLV_DEMOD_DEBUG("SL_TLV: magic_number valid: %d, plp: %d, alp_packet_size: %d, alp_trailing_padding: %d, alp is_complete: %d, alp first two bytes: 0x%02x 0x%02x",
                             atsc3_sl_tlv_payload->magic_number == 0x24681357,
                             atsc3_sl_tlv_payload->plp_number,
                             atsc3_sl_tlv_payload->alp_packet_size,
                             atsc3_sl_tlv_payload->alp_trailing_padding_size,
							 atsc3_sl_tlv_payload->alp_payload_complete,
                             atsc3_sl_tlv_payload->alp_payload->p_buffer[0],
                             atsc3_sl_tlv_payload->alp_payload->p_buffer[1]
                             );
    }
}

