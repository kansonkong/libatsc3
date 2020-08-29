/*
 *  atsc3_sl_tlv_demod_type.c
 *  Created on: Nov 22, 2019
 */

#include "atsc3_sl_tlv_demod_type.h"

int _SL_TLV_DEMOD_DEBUG_ENABLED = 0;
int _SL_TLV_DEMOD_TRACE_ENABLED = 0;

int __ATSC3_SL_TLV_USE_INLINE_ALP_PARSER_CALL__ = 0;

//impl for default metrics collection
atsc3_sl_tlv_payload_metrics_t __GLOBAL_DEFAULT_SL_TLV_PAYLOAD_METRICS;

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t(block_t* atsc3_sl_tlv_payload_unparsed_block) {
	return atsc3_sl_tlv_payload_parse_from_block_t_with_metrics(atsc3_sl_tlv_payload_unparsed_block, &__GLOBAL_DEFAULT_SL_TLV_PAYLOAD_METRICS);
}
/*
 * atsc3_sl_tlv_payload_parse_from_block_t
 *
 * 		caller is responsible for setting i_pos to the start of the SL_TLV magic string
 *
 * 		Completed: jjustman-2019-11-23: 	add seek for searching for magic string from block_t,
 * 									add support for carry-over of ALP re-constitution
 * 									add support for returing partial ALP frame payload if block_t is incomplete
 *
 */

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t_with_metrics(block_t* atsc3_sl_tlv_payload_unparsed_block, atsc3_sl_tlv_payload_metrics_t* atsc3_sl_tlv_payload_metrics) {
	
    uint8_t* buf_start = block_Get(atsc3_sl_tlv_payload_unparsed_block);
    uint8_t* buf_end = buf_start + (atsc3_sl_tlv_payload_unparsed_block->p_size - atsc3_sl_tlv_payload_unparsed_block->i_pos);
    uint8_t* buf = buf_start;

    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = calloc(1, sizeof(atsc3_sl_tlv_payload_t));

    //our TLV header must be at least 188 bytes to parse
    if(buf_end - buf_start < 188) {
    	__SL_TLV_DEMOD_WARN("atsc3_sl_tlv_payload_parse_from_block_t: remaining payload length is less than TLV 188 bytes: %ld", (buf_end - buf_start));
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
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_count++;
		
		if(atsc3_sl_tlv_payload->magic_number == 0x00000000) {
			atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_is_null_value_count++;
		}

        bool magic_number_found = false;
        //try and find our magic number for start of TLV packet
        while(++buf < (buf_end - 188) && !magic_number_found) {
            atsc3_sl_tlv_payload->magic_number = *(uint32_t*)(buf);
            magic_number_found = atsc3_sl_tlv_payload->magic_number == 0x24681357;
            if(magic_number_found) {
				atsc3_sl_tlv_payload_metrics->total_tlv_bytes_discarded_due_to_magic_mismatch_count += (buf - buf_start);
                atsc3_sl_tlv_payload_unparsed_block->i_pos += (buf - buf_start);

                __SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: position: %d, found magic number - parsed as: 0x%08x (expected: 0x24681357), buf start: %p, buf_found: %p, buf end: %p, offset: %d",
                                     atsc3_sl_tlv_payload_unparsed_block->i_pos,
                                     atsc3_sl_tlv_payload->magic_number,
                                     buf_start,
                                     buf,
                                     buf_end,
                                     (int)(buf - buf_start));
            }
        }

        if(!magic_number_found) {
			atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_recovered_in_block_count++;
			atsc3_sl_tlv_payload_metrics->total_tlv_bytes_discarded_without_matching_magic_recovered_in_block_count += (buf - buf_start);

            free(atsc3_sl_tlv_payload);
            __SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: magic number not found in remaining atsc3_sl_tlv_payload_unparsed_block: %p, returning",
                                 atsc3_sl_tlv_payload_unparsed_block);
                                 return NULL;
        }
    }
    //we need at least 28 (4 bytes of magic + 24 bytes of TLV) bytes here to process TLV header, if not, bail
    if((buf_end - buf) < 28) {
        free(atsc3_sl_tlv_payload);
        __SL_TLV_DEMOD_ERROR("atsc3_sl_tlv_payload_parse_from_block_t: remaining TLV payload too short: %d, need at least 28 bytes, block: %p, returning",
                             (int)(buf_end - buf),
                             atsc3_sl_tlv_payload_unparsed_block);
        return NULL;
    }
    buf+=4;



    __SL_TLV_DEMOD_TRACE("parsing SL TLV packet with magic: 0x%8x, position: %d", atsc3_sl_tlv_payload->magic_number, atsc3_sl_tlv_payload_unparsed_block->i_pos);

    //atsc3_sl_tlv_payload->alp_packet_size = ntohl(*((uint32_t*)(buf)));
    atsc3_sl_tlv_payload->alp_packet_size = *(uint32_t*)(buf);
	if(atsc3_sl_tlv_payload->alp_packet_size > MAX_ATSC3_PHY_IP_DATAGRAM_SIZE) {
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_TLV_header_ALP_size_greater_than_max_IP_UDP_datagram_size_count++;
		__SL_TLV_DEMOD_ERROR( "INVALID TLV: alp packet size: %d - (0x%08x), bailing", atsc3_sl_tlv_payload->alp_packet_size, atsc3_sl_tlv_payload->alp_packet_size);
		return NULL;
	} else {
		//don't add this value yet if our TLV payload size is incomplete in our block_t, add it in "TLV packet is in this block_t boundary"
		__SL_TLV_DEMOD_TRACE( "alp packet size: %d", atsc3_sl_tlv_payload->alp_packet_size);
	}

    buf+=4;
    
    atsc3_sl_tlv_payload->plp_number = *buf++;
	if(atsc3_sl_tlv_payload->plp_number > 63) {
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_PLP_value_count++;
		//jjustman-2020-04-16 - reduce noisy TLV packets due to possible re-sync or alp buffer continutations
        __SL_TLV_DEMOD_DEBUG("INVALID TLV: plp number (expected < 63): %d (0x%2x)", atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->plp_number);

        //__SL_TLV_DEMOD_ERROR("INVALID TLV: plp number (expected < 63): %d (0x%2x)", atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->plp_number);
	} else {
		__SL_TLV_DEMOD_TRACE(" plp number: %d", atsc3_sl_tlv_payload->plp_number);
	}
    
    atsc3_sl_tlv_payload->ts_size = *buf++;
	if(atsc3_sl_tlv_payload->ts_size != 188) {
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_TS_transfer_size_count++;
        //jjustman-2020-04-16 - reduce noisy TLV packets due to possible re-sync or alp buffer continutations
        __SL_TLV_DEMOD_DEBUG("INVALID: TS transfer size (ts_size) packet size (expected 188) : %d", atsc3_sl_tlv_payload->ts_size);

        //__SL_TLV_DEMOD_ERROR("INVALID: TS transfer size (ts_size) packet size (expected 188) : %d", atsc3_sl_tlv_payload->ts_size);
	} else {
		__SL_TLV_DEMOD_TRACE(" TS transfer size (ts_size) packet size: %d", atsc3_sl_tlv_payload->ts_size);

	}
    atsc3_sl_tlv_payload->tlv_size = *buf++;
	if(atsc3_sl_tlv_payload->tlv_size != 24) {
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_TLV_header_size_value++;
		__SL_TLV_DEMOD_ERROR("INVALID:  tlv_size packet size: %d", atsc3_sl_tlv_payload->tlv_size);
	} else {
		__SL_TLV_DEMOD_TRACE(" tlv_size packet size: %d", atsc3_sl_tlv_payload->tlv_size);
	}
	
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
	
	//split TLV payload over block_t boundary, so hold this partial frame
    if((atsc3_sl_tlv_payload->alp_packet_size + atsc3_sl_tlv_payload->alp_trailing_padding_size) > (buf_end - buf)) {
    	atsc3_sl_tlv_payload->alp_payload_complete = false;
    	atsc3_sl_tlv_payload->alp_payload = NULL;
        atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size = buf_end - buf;
    } else {
		//TLV packet is in this block_t boundary
		atsc3_sl_tlv_payload_metrics->total_tlv_header_alp_size_bytes_read += atsc3_sl_tlv_payload->alp_packet_size;
		//deferred calc for total_tlv_packets_with_matching_magic_count
		atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_matching_magic_count++;

    	atsc3_sl_tlv_payload->alp_payload_complete = true;
        atsc3_sl_tlv_payload->alp_payload = block_Alloc(atsc3_sl_tlv_payload->alp_packet_size);
        block_Write(atsc3_sl_tlv_payload->alp_payload, buf, atsc3_sl_tlv_payload->alp_packet_size);

		//TODO - refactor me?
		if(__ATSC3_SL_TLV_USE_INLINE_ALP_PARSER_CALL__) {
			//TODO - make sure this "looks" like an ALP packet header
			//total_tlv_packets_without_ALP_starting_at_TS_transfer_size_header_length_count
			
			//parse our ALP_packet inline
			block_Rewind(atsc3_sl_tlv_payload->alp_payload);
			atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->alp_payload);
			if(!atsc3_alp_packet) {
				//total_tlv_packets_with_failed_extracted_alp_count
				atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_failed_extracted_alp_count++;
				__SL_TLV_DEMOD_ERROR("FAILED ALP EXTRACTION: at buf: %p, tlv_alp_len: %d", buf, atsc3_sl_tlv_payload->alp_packet_size);
			} else {
				atsc3_sl_tlv_payload->atsc3_alp_packet = atsc3_alp_packet;
			
				//validate our TLV header ALP_packet_size matches what the ALP packet length in header is
				//IP  ~ 2 bytes ALP header
				//LMT ~ 7 bytes ALP header after parsing
				if((atsc3_alp_packet->alp_packet_header.packet_type == 0x0 && atsc3_sl_tlv_payload->alp_packet_size != atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length + 2) ||
				   (atsc3_alp_packet->alp_packet_header.packet_type == 0x4 && atsc3_sl_tlv_payload->alp_packet_size != atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length + 7)
				   ) {
					atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_TLV_header_ALP_size_mismatch_from_parsed_ALP_header_count++;
					__SL_TLV_DEMOD_ERROR("FAILED ALP SIZE MATCH: at buf: %p, packet_type: 0x%1x, atsc3_sl_tlv_payload->alp_packet_size (%d) != atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length (%d) + 2 ",
										 buf,
										 atsc3_alp_packet->alp_packet_header.packet_type,
										 atsc3_sl_tlv_payload->alp_packet_size,
										 atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length);
				} else {
					atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_successfully_extracted_alp_count++;
					//jjustman-2020-03-13 - ignore alp_segmentation_concatenation for now
					atsc3_sl_tlv_payload_metrics->total_alp_packets_actual_size_bytes += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length + 2;

					//ip packet
					if(atsc3_alp_packet->alp_packet_header.packet_type == 0x0) {
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_ip_packets_count++;
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_ip_packets_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
					} else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
						//lmt packet
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_link_layer_signalling_packets_count++;
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_link_layer_signalling_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
					} else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x1 || atsc3_alp_packet->alp_packet_header.packet_type == 0x3 || atsc3_alp_packet->alp_packet_header.packet_type == 0x5) {
						//RESERVED packet type
						__SL_TLV_DEMOD_ERROR("RESERVED alp packet type! packet: %p", atsc3_sl_tlv_payload);
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_reserved_count++;
						atsc3_sl_tlv_payload_metrics->total_alp_packet_type_reserved_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
					} else {
						//other non-common packet types
						if(atsc3_alp_packet->alp_packet_header.packet_type == 0x2) {
							//alp_packet_type_packet_compressed_ip_packet
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_compressed_ip_packet_count++;
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_compressed_ip_packet_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
						} else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x6) {
							//alp_packet_type_packet_type_extension
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_type_extension_count++;
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_type_extension_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
						} else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x7) {
							//alp_packet_type_packet_mpeg2_transport_stream
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_mpeg2_transport_stream_count++;
							atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_mpeg2_transport_stream_bytes_read += atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length;
						}
					}
				}
			}
		}
		
        buf += atsc3_sl_tlv_payload->alp_packet_size;
		
		if((buf_end - buf) < atsc3_sl_tlv_payload->alp_trailing_padding_size) {
			//TODO: FIX ME for holdver trailing padding case so we don't lose the next TLV packet due to a missing magic
			//don't walk over end of block_t,
			__SL_TLV_DEMOD_WARN("atsc3_sl_tlv_payload->alp_trailing_padding_size: %d, not enough space remaining in block_t, truncating to len: %d",
								atsc3_sl_tlv_payload->alp_trailing_padding_size,
								(int)(buf_end - buf));
			buf = buf_end;
			
		} else {
			//discard alp_trailing_padding size - garbage data?
			atsc3_sl_tlv_payload_metrics->total_tlv_header_alp_trailing_padding_size_bytes += atsc3_sl_tlv_payload->alp_trailing_padding_size;
			buf += atsc3_sl_tlv_payload->alp_trailing_padding_size;
			
			if(buf_end - buf < 4) {
				//not enough space to do a peek for the next magic
				__SL_TLV_DEMOD_TRACE("after alp_trailing_padding_size, not enough space to peek for magic: buf_end: %p, buf: %p, len: %d",
									 buf_end,
									 buf,
									 (int)(buf_end - buf));
			} else {
				uint32_t peek_magic_number = *(uint32_t*)(buf);
				if(atsc3_sl_tlv_payload->magic_number != 0x24681357) {
					   __SL_TLV_DEMOD_ERROR("after alp_trailing_padding_size: buf: %p, position: %d, magic number is not 0x24681357, parsed as: 0x%08x",
							   buf,
							  (int)(buf - buf_start),
							   atsc3_sl_tlv_payload->magic_number);
					atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_magic_number_after_alp_size_data_bytes_consumed_count++;
					
				} else {
						__SL_TLV_DEMOD_TRACE("after alp_trailing_padding_size: buf: %p, position: %d, magic number is: 0x%08x",
												  buf,
												 (int)(buf - buf_start),
												  atsc3_sl_tlv_payload->magic_number);
				}
			}
		}
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
            if(atsc3_sl_tlv_payload->atsc3_alp_packet) {
                atsc3_alp_packet_free(&atsc3_sl_tlv_payload->atsc3_alp_packet);
            }
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

void atsc3_sl_tlv_payload_metrics_dump(atsc3_sl_tlv_payload_metrics_t* atsc3_sl_tlv_payload_metrics) {
	
	__SL_TLV_DEMOD_INFO("---TLV Dump Metrics---");

	__SL_TLV_DEMOD_INFO(" total_tlv_bytes_read:	%llu", atsc3_sl_tlv_payload_metrics->total_tlv_bytes_read);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_parsed_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_parsed_count);

	__SL_TLV_DEMOD_INFO(" total_tlv_header_alp_size_bytes_read:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_header_alp_size_bytes_read);
	__SL_TLV_DEMOD_INFO(" total_alp_packets_actual_size_bytes:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packets_actual_size_bytes);

	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO("---TLV magic Metrics---");
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_matching_magic_count: 	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_matching_magic_count);

	__SL_TLV_DEMOD_INFO(" total_tlv_packets_without_matching_magic_count: %d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_count);

	__SL_TLV_DEMOD_INFO(" total_tlv_packets_without_matching_magic_is_null_value_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_is_null_value_count);

	__SL_TLV_DEMOD_INFO(" total_tlv_bytes_discarded_due_to_magic_mismatch_count:	%llu", atsc3_sl_tlv_payload_metrics->total_tlv_bytes_discarded_due_to_magic_mismatch_count);
	
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_without_matching_magic_recovered_in_block_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_matching_magic_recovered_in_block_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_bytes_discarded_without_matching_magic_recovered_in_block_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_bytes_discarded_without_matching_magic_recovered_in_block_count);

	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO("---TLV header heuristics Metrics---");
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_invalid_PLP_value_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_PLP_value_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_invalid_TS_transfer_size_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_TS_transfer_size_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_invalid_TLV_header_size_value:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_invalid_TLV_header_size_value);
	
	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO("---TLV header ALP heuristic Metrics---");
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_successfully_extracted_alp_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_successfully_extracted_alp_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_failed_extracted_alp_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_failed_extracted_alp_count);
	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_TLV_header_ALP_size_greater_than_max_IP_UDP_datagram_size_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_TLV_header_ALP_size_greater_than_max_IP_UDP_datagram_size_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_with_TLV_header_ALP_size_mismatch_from_parsed_ALP_header_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_with_TLV_header_ALP_size_mismatch_from_parsed_ALP_header_count);
	//TODO	__SL_TLV_DEMOD_INFO(" total_tlv_packets_without_ALP_starting_at_TS_transfer_size_header_length_count:\t%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_ALP_starting_at_TS_transfer_size_header_length_count);
	__SL_TLV_DEMOD_INFO(" total_tlv_packets_without_magic_number_after_alp_size_data_bytes_consumed_count:	%d", atsc3_sl_tlv_payload_metrics->total_tlv_packets_without_magic_number_after_alp_size_data_bytes_consumed_count);

	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO("---ALP Payload Metrics---");
	
	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_ip_packets_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_ip_packets_count);
	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_ip_packets_bytes_read:	%llu", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_ip_packets_bytes_read);
	
	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_link_layer_signalling_packets_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_link_layer_signalling_packets_count);
	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_link_layer_signalling_bytes_read:	%llu", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_link_layer_signalling_bytes_read);
//	
//	//USUALLY not present in most ATSC 3.0 use-cases
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_compressed_ip_packet_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_compressed_ip_packet_count);
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_compressed_ip_packet_bytes_read:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_compressed_ip_packet_bytes_read);
//
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_type_extension_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_type_extension_count);
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_type_extension_bytes_read:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_type_extension_bytes_read);
//	
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_mpeg2_transport_stream_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_mpeg2_transport_stream_count);
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_packet_mpeg2_transport_stream_bytes_read:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_packet_mpeg2_transport_stream_bytes_read);
//	
//	//SHOULD NEVER be present according to A330:2019
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_reserved_count:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_reserved_count);
//	__SL_TLV_DEMOD_INFO(" total_alp_packet_type_reserved_bytes_read:	%d", atsc3_sl_tlv_payload_metrics->total_alp_packet_type_reserved_bytes_read);
//	
	__SL_TLV_DEMOD_INFO("\n");
	__SL_TLV_DEMOD_INFO("---End of TLV---");


}



