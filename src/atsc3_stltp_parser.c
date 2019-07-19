/*
 * atsc3_stltp_parser.c
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 *
 *      https://tools.ietf.org/html/rfc3550
 *
 *
 */


#include "atsc3_stltp_parser.h"

int _STLTP_PARSER_DEBUG_ENABLED = 1;


ip_udp_rtp_packet_t* atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    block_Seek(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);

	__STLTP_PARSER_DEBUG("atsc3_stltp_udp_packet_inner_prepend_fragment: seeking udp_packet_outer: %p, to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->i_pos);

	ip_udp_rtp_packet_t* ip_udp_rtp_packet_new = atsc3_ip_udp_rtp_packet_prepend_if_not_null(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer);
	return ip_udp_rtp_packet_new;
}

//when marker == 1, The packet_offset is the number of bytes after the RTP header where the first byte of the first tunneled IP header resides within the Tunnel Packet
//base of IP/UDP/RTP = 40 bytes, not 12 (RTP only) ???
ip_udp_rtp_packet_t* atsc3_stltp_udp_packet_outer_fragment_check_marker(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    if(block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data) <= ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE) {
        __STLTP_PARSER_ERROR("atsc3_stltp_udp_packet_outer_fragment_check_marker: short packet: %u", block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data) );

        return NULL;
    }
    if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->marker) {
        
        uint32_t outer_data_pos = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->i_pos;
    
        
    	block_Seek(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data, atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->packet_offset + ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
        
        if(block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data) <= ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE) {
            __STLTP_PARSER_ERROR("atsc3_stltp_udp_packet_outer_fragment_check_marker: short packet: %u", block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data) );
            
            return NULL;
        }

		__STLTP_PARSER_DEBUG("seeking udp_packet_outer: %p, from %u to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data, outer_data_pos, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->i_pos);

        atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);
        
		if(!atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
			__STLTP_PARSER_ERROR("unable to parse inner packet packet");
			return NULL;
		} else {
			return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
		}
	} else {
		__STLTP_PARSER_WARN("atsc3_stltp_udp_packet_fragment_check_marker, no marker set using outer packet as inner fragment!");

		atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer);
	}
    return NULL;
}



ip_udp_rtp_packet_t* atsc3_stltp_udp_packet_inner_parse_ip_udp_header(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {

	__STLTP_PARSER_DEBUG("atsc3_stltp_udp_packet_inner_parse_ip_udp_header: parsing udp_packet_inner: %p, to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data->i_pos);

    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data);
   
    if(!atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
		__STLTP_PARSER_ERROR("unable to parse inner packet packet");
		return NULL;
	}

	return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
}




atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet(ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {
	__STLTP_PARSER_DEBUG(" ----atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet, size: %u, pkt: %p-----",
                         ip_udp_rtp_packet->data->p_size,
                         ip_udp_rtp_packet->data);

    atsc3_stltp_tunnel_packet_clear_completed_packets(atsc3_stltp_tunnel_packet_fragment);
    
	atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_fragment;

	if(!atsc3_stltp_tunnel_packet) {
		atsc3_stltp_tunnel_packet = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));
	}
    block_Rewind(ip_udp_rtp_packet->data);
	atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer = atsc3_ip_udp_rtp_packet_duplicate(ip_udp_rtp_packet);

	if(!atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet: atsc3_stltp_tunnel_packet->udp_packet_outer is null");
		return NULL;
	}
    atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->atsc3_rtp_fixed_header;

    if(!atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel) {
        __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet: atsc3_rtp_fixed_header_tunnel- is null");
        return NULL;
    }
    
 	atsc3_rtp_fixed_header_dump_outer(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel);

	//97 - tunnel packet
	if(!atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel || atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->payload_type != ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_fragment_from_udp_packet: unknown outer tunnel packet: %u", (atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel ? atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->payload_type : -1));
		return NULL;
	}

    //if we have a pending fragment, append with previous inner rtp header
//	atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented = atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet);

//    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented &&
//       block_Valid(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data) &&
//       block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data)  && atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last) {

    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment && block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment->data) > 0 && block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment->data) < ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE) {
  
            atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented = atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet);
		//re-fragment
		__STLTP_PARSER_WARN("--tunnel packet: processing short fragment payload type: %u, fragment length is: %u",
                            atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last->payload_type,
                            block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data));

        atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->atsc3_rtp_fixed_header = atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last;
		atsc3_rtp_fixed_header_dump_inner(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->atsc3_rtp_fixed_header);
        
		atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented);
        
		//out of data for refragmenting
		if(block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data) <= 40) {
            if(block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data) > 0) {
                atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented;
            }
            return atsc3_stltp_tunnel_packet;
		}
    } else if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last) {
        ip_udp_rtp_packet->atsc3_rtp_fixed_header = atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last;
        //process this full frame
        atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_result = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, ip_udp_rtp_packet);

    }
    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment = NULL;
    
    if(!atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->marker) {
        __STLTP_PARSER_WARN("--tunnel packet: completed refragmentation but no new marker");

        return atsc3_stltp_tunnel_packet;
    }

    //build a inner packet from a marker if present
	ip_udp_rtp_packet_t* ip_udp_inner_from_outer_packet_marker = atsc3_stltp_udp_packet_outer_fragment_check_marker(atsc3_stltp_tunnel_packet);
    atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner = NULL;

    if(ip_udp_inner_from_outer_packet_marker) {
        atsc3_rtp_fixed_header_dump_inner(ip_udp_inner_from_outer_packet_marker->atsc3_rtp_fixed_header);
        atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_result = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, ip_udp_inner_from_outer_packet_marker);
        atsc3_rtp_fixed_header_inner = ip_udp_inner_from_outer_packet_marker->atsc3_rtp_fixed_header;
        atsc3_rtp_fixed_header_inner->marker = 0; //force trigger offset to 0
    } else {
        //if we have a marker, but we didnt process, then push back and loop
        if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_outer_tunnel->marker) {
            atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
            return atsc3_stltp_tunnel_packet;
        }
        //parse inner payload
        ip_udp_inner_from_outer_packet_marker = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
    }
    
    //parse headers for additional inner marker
	while(ip_udp_inner_from_outer_packet_marker && block_Remaining_size(ip_udp_inner_from_outer_packet_marker->data) > (40)) {

		atsc3_rtp_fixed_header_inner = atsc3_stltp_parse_rtp_fixed_header_block_t(ip_udp_inner_from_outer_packet_marker->data);

		if(atsc3_rtp_fixed_header_inner) {
			atsc3_rtp_fixed_header_dump_inner(atsc3_rtp_fixed_header_inner);

			__STLTP_PARSER_DEBUG("--tunnel packet: processing from fragment: %u, length is: %u", atsc3_rtp_fixed_header_inner->payload_type, block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data));

			atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_result = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet, ip_udp_inner_from_outer_packet_marker);
			if(!atsc3_stltp_tunnel_packet_result) {
				__STLTP_PARSER_ERROR("--tunnel packet: error processing atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload");
				break;
			}
		} else {
			__STLTP_PARSER_ERROR("--tunnel packet: error processing udp packet inner");
			break;
		}
	}
    
    //out of data for refragmenting        //hold over this payload

    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented && block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data) <= 40) {
        if(block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_refragmented->data) > 0) {
            atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
        }
    }
	if(atsc3_rtp_fixed_header_inner) {
		atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_inner_last = atsc3_rtp_fixed_header_inner;
	}
    
    //if we are out of data and not complete, then carry over this as inner_last_fragment
    //todo: does it matter what size we have?
//    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner  && block_Remaining_size(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data) <= 40) {
//        atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner_last_fragment = atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
//    }

	return atsc3_stltp_tunnel_packet;

//
//		//marker signifies that we may have a split payload
//		if(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header->marker) {
//			if(atsc3_stltp_tunnel_packet_fragment && !atsc3_stltp_tunnel_packet_fragment->is_complete) {
//
//				if(atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment) {
//					assert(header_packet_offset > atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data_length);
//
//					header_packet_offset -= atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data_length;
//
//					memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment->data, atsc3_stltp_tunnel_packet_fragment->udp_packet->data_length);
//					udp_packet_free(&atsc3_stltp_tunnel_packet_fragment->udp_packet_last_fragment);
//				}
//				//process remaining fragment
//
//				__STLTP_PARSER_DEBUG("--tunnel packet: fragment end, marker present : %u --", ++atsc3_stltp_tunnel_packet_fragment->fragment_count);
//
//				if(atsc3_stltp_tunnel_packet_fragment && !atsc3_stltp_tunnel_packet_fragment->is_complete) {
//					__STLTP_PARSER_DEBUG(" ----tunnel packet: fragment truncated from marker-----");
//
//					atsc3_stltp_tunnel_packet_fragment->is_truncated_from_marker = true;
//				}
//
//
//				__STLTP_PARSER_WARN("--tunnel packet: marker: remaining fragment length is: %u, position is: %u", atsc3_stltp_tunnel_packet->udp_packet->data_length, atsc3_stltp_tunnel_packet->udp_packet_last_position);
//
////			}
//
//			__STLTP_PARSER_DEBUG("--tunnel packet: marker --");
//
//			header_packet_offset += atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel->packet_offset;
//			assert(header_packet_offset  < raw_packet_length);
//
//			atsc3_stltp_tunnel_packet->outer_ip_header = &raw_packet_data[header_packet_offset];
//			atsc3_stltp_tunnel_packet->outer_ip_header_length = raw_packet_length - header_packet_offset;
//			__STLTP_PARSER_DEBUG("  header packet offset: %u, length: %u, first_ip_header: %p",  header_packet_offset, raw_packet_length, atsc3_stltp_tunnel_packet->outer_ip_header);
//			atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
//
//			__STLTP_PARSER_DEBUG("  ip header:     0x%x", atsc3_stltp_tunnel_packet->outer_ip_header[0]);
//
//			atsc3_stltp_tunnel_packet->udp_packet = udp_packet_process_from_ptr(atsc3_stltp_tunnel_packet->outer_ip_header, atsc3_stltp_tunnel_packet->outer_ip_header_length);
//			if(!atsc3_stltp_tunnel_packet->udp_packet) {
//				__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp packet from: %p, size: %u", atsc3_stltp_tunnel_packet->outer_ip_header, atsc3_stltp_tunnel_packet->outer_ip_header_length);
//				return NULL;
//			}
//			__STLTP_PARSER_DEBUG("  dst ip:port :  %u.%u.%u.%u:%u",__toipandportnonstruct(atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_ip_addr, atsc3_stltp_tunnel_packet->udp_packet->udp_flow.dst_port));
//			__STLTP_PARSER_DEBUG("  packet length: %u ", atsc3_stltp_tunnel_packet->udp_packet->data_length);
//			__STLTP_PARSER_DEBUG("  packet first byte: 0x%x ", atsc3_stltp_tunnel_packet->udp_packet->data[0]);
//
//
//		} else if(atsc3_stltp_tunnel_packet_fragment){
//			//fragment, so replace udp_packet->data
//			__STLTP_PARSER_DEBUG("--tunnel packet: fragment: %u --", ++atsc3_stltp_tunnel_packet->fragment_count);
//			//atsc3_rtp_fixed_header_dump(atsc3_stltp_tunnel_packet->atsc3_rtp_fixed_header_tunnel);
//			atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header);
//			if(atsc3_stltp_tunnel_packet->udp_packet_last_fragment) {
//				__STLTP_PARSER_WARN("--tunnel packet: patching udp_packet_short_fragment, length is: %u, copying short fragment", atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length);
//				assert(header_packet_offset > atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length);
//
//				header_packet_offset -= atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data_length;
//
//				memcpy(&raw_packet_data[header_packet_offset], atsc3_stltp_tunnel_packet->udp_packet_last_fragment->data, atsc3_stltp_tunnel_packet->udp_packet->data_length);
//				udp_packet_free(&atsc3_stltp_tunnel_packet->udp_packet_last_fragment);
//			}
//
//			atsc3_stltp_tunnel_packet->udp_packet->data = &raw_packet_data[header_packet_offset];
//			atsc3_stltp_tunnel_packet->udp_packet->data_length = raw_packet_length - header_packet_offset;
//		} else {
//			__STLTP_PARSER_ERROR("--tunnel packet: RESET --");
//			return NULL;
//		}
//
//		if(atsc3_stltp_tunnel_packet->udp_packet && atsc3_stltp_tunnel_packet->udp_packet->data_length >= 12) {
//			atsc3_stltp_tunnel_packet = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet);
//			__STLTP_PARSER_WARN("--tunnel packet: remaining fragment length is: %u, position is: %u", atsc3_stltp_tunnel_packet->udp_packet->data_length, atsc3_stltp_tunnel_packet->udp_packet_last_position);
//
//		} else if(atsc3_stltp_tunnel_packet->udp_packet) {
//			__STLTP_PARSER_WARN("--tunnel packet: remaining fragment length is: %u, copying to tunnel_packet_buffer", atsc3_stltp_tunnel_packet->udp_packet->data_length);
//			atsc3_stltp_tunnel_packet->udp_packet_last_fragment = udp_packet_duplicate(atsc3_stltp_tunnel_packet->udp_packet);
//		} else {
//			__STLTP_PARSER_ERROR("--tunnel packet: unable to parse udp_packet --");
//			return NULL;
//
//		}
//		return atsc3_stltp_tunnel_packet;
//	}
//
//	return NULL;
}

//atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment) {

//parse rtp again....

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner)  {

    atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner = ip_udp_rtp_packet_inner->atsc3_rtp_fixed_header;
    block_Rewind(ip_udp_rtp_packet_inner->data);
    block_Seek(ip_udp_rtp_packet_inner->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);

	if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET) {
		atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_inner, atsc3_rtp_fixed_header_inner);
	} else if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET){
		atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_inner,  atsc3_rtp_fixed_header_inner);
	} else if(atsc3_rtp_fixed_header_inner->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET) {
		atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_inner, atsc3_rtp_fixed_header_inner);
	} else {
		__STLTP_PARSER_ERROR("Unknown inner payload type of 0x%2x", atsc3_rtp_fixed_header_inner->payload_type);
		return NULL;
	}

	return atsc3_stltp_tunnel_packet;
}



//atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel) {
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {

//    if(ip_udp_rtp_packet_inner->atsc3_rtp_fixed_header->marker && atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending) {
//        //force clear out pending packet
//        free(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending);
//        atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending = NULL;
//    }

    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_pending = atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending;
    
	if(!atsc3_stltp_baseband_packet_pending) {
		atsc3_stltp_baseband_packet_pending = calloc(1, sizeof(atsc3_stltp_baseband_packet_t));
        atsc3_stltp_baseband_packet_pending->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
        atsc3_stltp_baseband_packet_pending->payload_length = atsc3_rtp_fixed_header_inner->packet_offset;
        
        atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending = atsc3_stltp_baseband_packet_pending;
	}

    block_t* packet = ip_udp_rtp_packet_inner->data;
    uint32_t block_remaining_length = block_Remaining_size(packet);
    
    if(block_remaining_length + atsc3_stltp_baseband_packet_pending->payload_offset > atsc3_stltp_baseband_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_baseband_packet_pending->payload_length - atsc3_stltp_baseband_packet_pending->payload_offset);
    }
   
	if(atsc3_stltp_baseband_packet_pending->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending->fragment_count) {
		/**
		ATSC A/324:2018 - Section 8.3
		When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
		‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
		field will contain the total length of the Baseband Packet data structure in bytes. This allows
		the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
		 */

		uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet_pending->atsc3_rtp_fixed_header->packet_offset; //SSRC packet length
		assert(baseband_header_packet_length < 65535);
		atsc3_stltp_baseband_packet_pending->payload_length = baseband_header_packet_length;
		atsc3_stltp_baseband_packet_pending->payload = calloc(baseband_header_packet_length, sizeof(uint8_t));
        assert(atsc3_stltp_baseband_packet_pending->payload);

		__STLTP_PARSER_DEBUG(" ----baseband packet: new -----");
		__STLTP_PARSER_DEBUG("     total packet length:  %u",  atsc3_stltp_baseband_packet_pending->payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:    %u",  block_remaining_length);

	} else {
		__STLTP_PARSER_DEBUG(" ----baseband packet: fragment-----");
        __STLTP_PARSER_DEBUG("     fragment %u, offset: %u, length:   %u",   atsc3_stltp_baseband_packet_pending->fragment_count, atsc3_stltp_baseband_packet_pending->payload_offset, block_remaining_length);
	}
    
	memcpy(&atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset], block_Get(packet), block_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x 0x%02x 0x%02x", atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset], atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+1],
        atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+2],
        atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+3]);
    
    atsc3_stltp_baseband_packet_pending->fragment_count++;
	atsc3_stltp_baseband_packet_pending->payload_offset += block_remaining_length;
	__STLTP_PARSER_DEBUG("     payload_offset now: %u, baseband_packet_length: %u", atsc3_stltp_baseband_packet_pending->payload_offset, atsc3_stltp_baseband_packet_pending->payload_length);

	if(atsc3_stltp_baseband_packet_pending->payload_offset >= atsc3_stltp_baseband_packet_pending->payload_length) {
		//todo - parse ALP payloads
		__STLTP_PARSER_DEBUG(" ----baseband packet: complete-----");
		atsc3_stltp_baseband_packet_pending->is_complete = true;
        atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet = atsc3_stltp_baseband_packet_pending;
        atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_pending = NULL;
	}
           
	block_Seek(packet, packet->i_pos + block_remaining_length);

	return atsc3_stltp_baseband_packet_pending;
}



atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {
    
    if(ip_udp_rtp_packet_inner->atsc3_rtp_fixed_header->marker && atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending) {
        //force clear out pending packet
        free(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending);
        atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending = NULL;
    }
    
	atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_pending = atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending;
	if(!atsc3_stltp_preamble_packet_pending) {
		atsc3_stltp_preamble_packet_pending = calloc(1, sizeof(atsc3_stltp_preamble_packet_t));
        atsc3_stltp_preamble_packet_pending->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;

		atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending = atsc3_stltp_preamble_packet_pending;
	}

	atsc3_stltp_preamble_packet_pending->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
    block_t* packet = ip_udp_rtp_packet_inner->data;

	if(atsc3_stltp_preamble_packet_pending->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_pending->fragment_count) {
		//read the first uint16_t for our preamble length

        //The length field shall contain the number of bytes in the Preamble Payload data structure following the length field excluding the crc16 bytes
		atsc3_stltp_preamble_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet)))) +2;
		__STLTP_PARSER_DEBUG(" ----preamble packet: new -----");
		__STLTP_PARSER_DEBUG("     preamble length:    %u",  atsc3_stltp_preamble_packet_pending->payload_length);
		__STLTP_PARSER_DEBUG("     fragment 0 length:  %u",  block_Remaining_size(packet));
        
		atsc3_stltp_preamble_packet_pending->payload = calloc(atsc3_stltp_preamble_packet_pending->payload_length, sizeof(uint8_t));

	}
    
    memcpy(&atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], block_Get(packet), atsc3_stltp_preamble_packet_pending->payload_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset+1]);

    __STLTP_PARSER_DEBUG("      preamble_payload_length: %u", atsc3_stltp_preamble_packet_pending->payload_length);

	if(atsc3_stltp_preamble_packet_pending->payload_offset >= atsc3_stltp_preamble_packet_pending->payload_length) {

		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----preamble packet: complete-----");
		atsc3_stltp_preamble_packet_pending->is_complete = true;
        atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet_pending;
        block_Seek(packet, packet->i_pos + atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet->payload_length);
	}

	return atsc3_stltp_preamble_packet_pending;
}

atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_inner) {
    
    if(ip_udp_rtp_packet_inner->atsc3_rtp_fixed_header->marker && atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending) {
        //force clear out pending packet
        free(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending);
        atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending = NULL;
    }
    
	atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_pending = atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending;
	if(!atsc3_stltp_timing_management_packet_pending) {
		atsc3_stltp_timing_management_packet_pending = calloc(1, sizeof(atsc3_stltp_timing_management_packet_t));
        atsc3_stltp_timing_management_packet_pending->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
		atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending = atsc3_stltp_timing_management_packet_pending;
	}

	atsc3_stltp_timing_management_packet_pending->atsc3_rtp_fixed_header = atsc3_rtp_fixed_header_inner;
	uint32_t header_packet_offset = 0;
    block_t* packet = ip_udp_rtp_packet_inner->data;

    if(atsc3_stltp_timing_management_packet_pending->atsc3_rtp_fixed_header->marker && !atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_pending->fragment_count) {
		//read the first uint16_t for our preamble length

		atsc3_stltp_timing_management_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet))));
		__STLTP_PARSER_DEBUG(" ----timing_management packet: new -----");
		__STLTP_PARSER_DEBUG("     preamble length:    %u",  atsc3_stltp_timing_management_packet_pending->payload_length);
        __STLTP_PARSER_DEBUG("     fragment 0 length:  %u",  block_Remaining_size(packet));
		atsc3_stltp_timing_management_packet_pending->payload = calloc(atsc3_stltp_timing_management_packet_pending->payload_length, sizeof(uint8_t));
	}
    

	atsc3_stltp_timing_management_packet_pending->fragment_count++;
	memcpy(&atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], block_Get(packet), atsc3_stltp_timing_management_packet_pending->payload_length );
    if(atsc3_stltp_timing_management_packet_pending->payload_length  > 1) {
        __STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset+1]);
    }
	__STLTP_PARSER_DEBUG("     payload_offset now: %u, preamble_payload_length: %u", atsc3_stltp_timing_management_packet_pending->payload_offset, atsc3_stltp_timing_management_packet_pending->payload_length);

	if(atsc3_stltp_timing_management_packet_pending->payload_offset >= atsc3_stltp_timing_management_packet_pending->payload_length) {

		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----timing_management packet: complete-----");
		atsc3_stltp_timing_management_packet_pending->is_complete = true;
        atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet = atsc3_stltp_timing_management_packet_pending;
        block_Seek(packet, packet->i_pos + atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet->payload_length );

	}

	return atsc3_stltp_timing_management_packet_pending;
}

/**
 
 todo: 2019-03-16
 
 free any udp_packet reassembly payloads
 
 **/

void atsc3_stltp_tunnel_packet_clear_completed_packets(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    if(atsc3_stltp_tunnel_packet) {
        if(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet && atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet->is_complete) {
            free(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet);
            atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet = NULL;
        }
        if(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet && atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet->is_complete) {
            free(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet);
            atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet = NULL;
        }
        if(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet && atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet->is_complete) {
            free(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet);
            atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet = NULL;
        }
    }
}


void atsc3_rtp_fixed_header_dump_outer(atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header) {
	__STLTP_PARSER_DEBUG(" ---outer---");
	atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header, 1);

}

void atsc3_rtp_fixed_header_dump_inner(atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header) {
	__STLTP_PARSER_DEBUG("   ---inner---");
	atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header, 3);
}
void atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header, int spaces) {

	__STLTP_PARSER_DEBUG("%*sversion:         %x", spaces, "", atsc3_rtp_fixed_header->version);
	__STLTP_PARSER_DEBUG("%*spadding:         %x", spaces, "", atsc3_rtp_fixed_header->padding);
	__STLTP_PARSER_DEBUG("%*sextension:       %x", spaces, "", atsc3_rtp_fixed_header->extension);
	__STLTP_PARSER_DEBUG("%*scsrc_count:      %x", spaces, "", atsc3_rtp_fixed_header->csrc_count);
	__STLTP_PARSER_DEBUG("%*smarker:          %x", spaces, "", atsc3_rtp_fixed_header->marker);
	__STLTP_PARSER_DEBUG("%*spayload_type:    0x%x (%hhu)", spaces, "", atsc3_rtp_fixed_header->payload_type, 	atsc3_rtp_fixed_header->payload_type);
	__STLTP_PARSER_DEBUG("%*ssequence_number: 0x%x (%u)", spaces, "", atsc3_rtp_fixed_header->sequence_number, atsc3_rtp_fixed_header->sequence_number);
	__STLTP_PARSER_DEBUG("%*stimestamp:       0x%x (%u)", spaces, "", atsc3_rtp_fixed_header->timestamp, 		atsc3_rtp_fixed_header->timestamp);
    if(atsc3_rtp_fixed_header->payload_type == 0x61) {
        __STLTP_PARSER_DEBUG("%*spacket_offset:   0x%x (%u)", spaces, "", atsc3_rtp_fixed_header->packet_offset, 	atsc3_rtp_fixed_header->packet_offset);
    } else {
        __STLTP_PARSER_DEBUG("%*spacket length:   0x%x (%u)", spaces, "", atsc3_rtp_fixed_header->packet_offset,     atsc3_rtp_fixed_header->packet_offset);
    }
}


