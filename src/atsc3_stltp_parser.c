/*
 *
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 *
 *      https://tools.ietf.org/html/rfc3550
 *
 
 One of the principal functions of the Scheduler is to generate Preamble data for the Transmitter(s) that it controls. Conceptually, as shown in Figure 4.2, the Preamble generation function is assigned to a Preamble Generator, which is part of the Broadcast Gateway. The Preamble Generator outputs the data to be transmitted to receivers to allow their configurations to match the processes and parameters that will be used in Transmission. As the Transmitter(s) process the Preamble data for emission to receivers, the Preamble data also will be used by the Transmitter(s) to set up the Input Formatting, Coded Modulation, Framing/Structure, and Waveform Generation so that the emitted waveform will match what receivers will be instructed by the Preamble to receive. The exact format for the Preamble data is specified in [3].
 
 Similarly, the Scheduler must control the generation and emission of Bootstrap waveforms by the Transmitter(s). To accomplish this, a data structure, similar to the Preamble, is defined in this document to carry Timing and Management (T&M) data to the Transmitters. Conceptually, as shown in Figure 4.2, a Timing and Management Data Generator is included in the Broadcast Gateway and provides the function under control of the Scheduler.
 
 
 Port Assignments
 Each Multicast destination address has usable port numbers from 1 through 65535. Values of 30000 – 30366 inclusive shall be used for this standard. Ports 30000 through 30063 shall be used to identify inner Tunneled Streams having destinations of PLPs 0 through 63, respectively; port 30064 shall be used to identify the inner Tunneled Stream carrying the Preamble data; port 30065 shall be used to identify the inner Tunneled Stream carrying the Timing and Management Data; and port 30066 shall be used to identify the inner Tunneled Stream carrying the Security Data Stream. All the listed Tunneled Packet Streams (i.e., Preamble, Timing and Management, Baseband Packets, and Security Data) required to populate an RF channel comprise a group of inner Tunneled Packet Streams.
 The arrangement of UDP port numbers described above for an inner group of Tunneled Packet Streams can be offset in steps of 100 to permit addressing similar sets of Tunneled Packet groups intended for delivery to other Transmitters over the same STLTP. In such cases, an offset value for the port numbers for Transmitter P (where P is a value from 0 – 3) can obtained from Offset [P] = P × 100. Alternatively, the number P can be used as the middle digit of a 5-digit number, with the first two digits being ‘30’ and the last two digits ranging from ‘00’ through ’66’. (See Table 8.7 for the semantics of the number_of_channels field.) For example, in a case in which a Broadcast Gateway processes two channels, respectively A and B, the group of inner Tunneled Packet Streams of channel A would use the UDP port range from 30000 to 30066 (offset 0), while the group of inner Tunneled Packet Streams of channel B would use the UDP range 30100 to 30166 (offset 100). All these inner Tunneled Packet Streams then would be carried by the same Stream of Tunnel Packets. See Annex E for descriptions of the use of multichannel STLTP in Channel Bonding cases.
 
 The RTP header fields of the TMP packet set shall be as described below, configured with the marker (M) bit of the packet containing the beginning of a TMP () data structure set to ‘1’. The marker (M) bits of the remaining packets shall be set to ‘0’. This allows the transmission system on the consumer end of the STL to reconstruct the TMP () data structure after any resequencing takes place. The timestamps of the packets of a given TMP packet set shall have the same values. The timestamp values are derived from a subset of the Bootstrap_Timing_Data, providing a mechanism to uniquely associate each of the TMP packets with a specific Physical Layer frame. The RTP header fields shall follow the syntax defined in RFC 3550 [6] with the following additional constraints:
 
 
 BBP data is carried across the STL as an RTP/UDP/IP Multicast Stream for each PLP. These Streams are multiplexed into a single RTP/UDP/IP Stream for each broadcast emission to enable reliable delivery to the Transmitter(s) of correctly identified and ordered BBPs. Conceptually, the BBP data Streams, as well as the Preamble Stream, the Timing and Management Stream, and the Security Data Stream are encapsulated as inner Streams carried through the outer (or Tunneling) Stream formed by the STLTP. While the inner Streams are Multicast-only, the outer Stream can be Multicast or Unicast. (See Annex B for an example of Unicast use.) The inner Stream provides addressing of BBP Streams to their respective PLPs through use of UDP port numbers. The outer protocol, STLTP, provides maintenance of packet order through use of RTP header packet sequence numbering. The STLTP also enables use of (SMPTE 2022-1) ECC to maintain reliability of Stream delivery under conditions of imperfectly reliable STL Networks.
 At the Transmitter(s), an input buffer is used for each PLP to hold BBP data until it is needed for Transmission. There also are FIFO buffers for the Preamble Stream and the Timing and Management Stream. The Preamble Stream processing includes a Preamble Parser that collects all of the configuration information for the next and possibly several upcoming Physical Layer frames to use in configuring the Transmitter data processing for those frames.
 Preamble data is scheduled to arrive at the Transmitter input at least one full Physical Layer frame Period prior to the first byte of the associated payload to provide time for the Transmitter data processing to be configured properly. Preamble data also can be sent multiple times in advance to enable acquisition of the data with improved reliability. The same considerations also are applicable to the Timing and Management Data; i.e., it is scheduled to arrive at the Transmitter input at least one Physical Layer frame Period prior to the first byte of the associated payload (+processing delay) it describes, and it can be sent multiple times to enable improved reliability of its acquisition.
 
 
 maj_log_rep_cnt_pre shall indicate the number of repetitions of Preamble data in the Preamble Stream at UDP port 30064 prior to emission of the Preamble. Permitted values are 1, 3, 5, 7, and 9. Note that value of L1B_lls_flag may be correct only in the final copy of the Preamble data sent to Transmitters prior to emission. Consequently, majority logic error correction can be applied reliably to all portions of the Preamble Stream data except the flag value noted. See Section 9.2 for details of placement of the repeated data.
 maj_log_rep_cnt_tim shall indicate the number of repetitions of Timing & Management data in the Timing & Management Stream at UDP port 30065 prior to emission of the next Bootstrap. Permitted values are 1, 3, 5, 7, and 9. Note that values for the ea_wakeup bits may be correct only in the final copy of the Timing & Management data sent to Transmitters prior to emission. Consequently, majority logic error correction can be applied reliably to all portions of the Timing & Management Stream data except the ea_wakeup values noted. See Section 9.1 for details of placement of the repeated data.
 
 
 *
 */


#include "atsc3_stltp_parser.h"

int _STLTP_PARSER_INFO_ENABLED = 1;
int _STLTP_PARSER_DUMP_ENABLED = 0;
int _STLTP_PARSER_DEBUG_ENABLED = 0;
int _STLTP_PARSER_TRACE_ENABLED = 0;


/*
 read inner data from outer packet position
 
 todo - wrap this to produce the ip_udp_rtp_packet_inner payload
 */

block_t* atsc3_stltp_read_from_outer_packet(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_outer, bool after_refragment) {
    block_t* inner_packet_data = NULL;
    
    int outer_packet_bytes_remaining = block_Remaining_size(ip_udp_rtp_packet_outer->data);
    if(outer_packet_bytes_remaining < ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE) {
        if(outer_packet_bytes_remaining) {
            inner_packet_data = block_Duplicate_from_position(ip_udp_rtp_packet_outer->data);
        }
        __STLTP_PARSER_WARN("atsc3_stltp_read_from_outer_packet: returning short inner data packet, size: %u, block_t*: %p", outer_packet_bytes_remaining, inner_packet_data);
        return inner_packet_data;
    }
    //assume we are not in the outer marker
    ip_udp_rtp_packet_outer->is_in_marker = false;

    if(!after_refragment) {
        if(ip_udp_rtp_packet_outer->rtp_header->marker) {
            //return partial block_t
            block_Seek(ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
            inner_packet_data = block_Duplicate_from_position(ip_udp_rtp_packet_outer->data); //clone this block explicity
            block_Resize(inner_packet_data, ip_udp_rtp_packet_outer->rtp_header->packet_offset); //trim up to the marker boundary for inner packet
            //do not seek to marker position here, this will happen on next call with after_refragment=true
            
            //block_Seek(ip_udp_rtp_packet_outer->data, ip_udp_rtp_packet_outer->rtp_header->packet_offset); //move our outer to the marker position
            
            return inner_packet_data;
        } else {
            //return full block_t
            block_Seek(ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
            inner_packet_data = block_Duplicate_from_position(ip_udp_rtp_packet_outer->data); //clone this block explicity
            return inner_packet_data;
    
        }
    }
    
    if(ip_udp_rtp_packet_outer->rtp_header->marker) {
        //if our outer packet position is less than our marker's packet_offset, then skip to marker
        if(ip_udp_rtp_packet_outer->data->i_pos <= ip_udp_rtp_packet_outer->rtp_header->packet_offset) {
            block_Seek(ip_udp_rtp_packet_outer->data, ip_udp_rtp_packet_outer->rtp_header->packet_offset + ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
            ip_udp_rtp_packet_outer->is_in_marker = true;
        }
    }
    inner_packet_data = block_Duplicate_from_position(ip_udp_rtp_packet_outer->data); //don't return a reference here as transient
    
    return inner_packet_data;
}


/*
 only use this for re-fragmentation parsing, which block_t is duplicated (not original pointer)
 
 jjustman-2019-08-02 - remove me
 */
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {

	__STLTP_PARSER_DEBUG("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data: parsing udp_packet_inner: %p, to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data->i_pos);

    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner_new = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data);
    
    if(!ip_udp_rtp_packet_inner_new) {
        __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data: unable to parse inner packet");
        return NULL;
    }
    
    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
        __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data: atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner is: %p, freeing",
        		atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
       atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    }
    
    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = ip_udp_rtp_packet_inner_new;

	return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
}

//this method will update atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner with the new frame
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    
    __STLTP_PARSER_DEBUG("  atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data: parsing ip_udp_rtp_packet_outer: %p, outer position: %u, outer packet len: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->i_pos,
        atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->p_size);

//jjustman-2020-08-18 - leaking
//atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner_new = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);

    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner_new = atsc3_ip_udp_rtp_packet_process_header_only_no_data_block_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);
    
    if(!ip_udp_rtp_packet_inner_new) {
        __STLTP_PARSER_ERROR(" atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data: unable to parse inner packet");
        return NULL;
    }
    
    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
        __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data: atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner is: %p, freeing", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
       atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    }
    
    ip_udp_rtp_packet_inner_new->data = block_Duplicate_from_position(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);
    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = ip_udp_rtp_packet_inner_new;
    return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
}

/*
 invoked from main pcap listener
 */

/*
 
 jjustman-2019-08-20 - investigate inner port for multi-plp emissions...
 extracted in driver for baseband re-constitution/alp parsing
 
 if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->udp_flow.dst_port != inner_port) {
 __STLTP_PARSER_WARN("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: inner port mismatch: %d, expected: %d",
 atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->udp_flow.dst_port,
 inner_port);
 
 goto concatenation_check_for_outer_marker;
 }
**/
atsc3_stltp_tunnel_packet_t* atsc3_stltp_raw_packet_extract_inner_from_outer_packet(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_last) {
   
    __STLTP_PARSER_DEBUG("  atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: OUTER: packet: %p, sequence_number: %d, dst: %u.%u.%u.%u:%u, size: %u, pkt: %p",
                         ip_udp_rtp_packet,
                         ip_udp_rtp_packet->rtp_header->sequence_number,
                         __toipandportnonstruct(ip_udp_rtp_packet->udp_flow.dst_ip_addr, ip_udp_rtp_packet->udp_flow.dst_port),
                         ip_udp_rtp_packet->data->p_size,
                         ip_udp_rtp_packet->data);

    
    atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));
    
    if(atsc3_stltp_tunnel_packet_last) {
    	//jjustman-2020-08-18 - TODO - these must come from our atsc3_stltp_depacketizer_context when we resolve our ip_udp_rtp_packet_inner (e.g. PLP for BBP)
    	//atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp
    	//atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp

        //map any pending non baseband packets over to our packet_current
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending          	= atsc3_stltp_tunnel_packet_last->atsc3_stltp_preamble_packet_pending;
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending 	= atsc3_stltp_tunnel_packet_last->atsc3_stltp_timing_management_packet_pending;
        
        //null out any last packet references, otherwise we will doublefree()
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_tunnel_baseband_packet_pending_by_plp   = NULL;
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_preamble_packet_pending            		= NULL;
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_timing_management_packet_pending   		= NULL;
    }
    
    //rewind raw packet buffer to outer packet
    block_Rewind(ip_udp_rtp_packet->data);
    
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer) {
    	atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer);
    }
    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer = atsc3_ip_udp_rtp_packet_duplicate(ip_udp_rtp_packet);
    
	if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: atsc3_stltp_tunnel_packet_current->udp_packet_outer is null");
		return NULL;
	}


    //seek past the outer packet header data, as we have already parsed this data..
    block_Seek(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);

    atsc3_rtp_header_dump_outer(atsc3_stltp_tunnel_packet_current);

    //make sure our outer packet is type: 97 - tunnel packet
	if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header || atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->payload_type != ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: unknown outer tunnel packet: %u", (atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header ? atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->payload_type : -1));
        
        atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_current);
		return NULL;
	}
    
    bool processed_refragmentation_or_concatenation_tunnel_packet = false;
    
    if(atsc3_stltp_tunnel_packet_last) {
    	if(!atsc3_stltp_tunnel_packet_is_rtp_packet_outer_sequence_number_contiguous(atsc3_stltp_tunnel_packet_last, atsc3_stltp_tunnel_packet_current)) {

    		int32_t last_sequence_number = -1;
    		int32_t current_sequence_number = -2;

    		if(atsc3_stltp_tunnel_packet_last && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header) {
    			last_sequence_number = atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header->sequence_number;
    		}

    		if(atsc3_stltp_tunnel_packet_current && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header) {
    			current_sequence_number = atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number;
    		}

    		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: outer sequence number is not contiguous! outer tunnel packet last: %d, outer tunnel packet current: %d",
    					last_sequence_number,
						current_sequence_number);
    	}
    } else {
    	__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: no last atsc3_stltp_tunnel_packet_last, current sequence number: %u!",
    			atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number);
    }

    //last frame smaller than 40 bytes, for recovering refragmentation of prior frame due to being short for ip_udp_rtp header parsing
    if(atsc3_stltp_tunnel_packet_last && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_refragmentation_outer) {
        
        //first, try and parse any remaining bytes in atsc3_stltp_tunnel_packet_last that did not get an inner packet started
        int last_outer_packet_bytes_remaining_to_parse = block_Remaining_size(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_refragmentation_outer->data);
        if(last_outer_packet_bytes_remaining_to_parse > 0) {
            __STLTP_PARSER_DEBUG("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment of len: %u", last_outer_packet_bytes_remaining_to_parse);
            
            atsc3_ip_udp_rtp_packet_t* inner_packet = NULL;
            
            block_t* inner_payload_last_short_frame = block_Duplicate_from_position(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_refragmentation_outer->data);

            //read from outer packet data, either up to marker or the full payload
            block_t* inner_payload_current = atsc3_stltp_read_from_outer_packet(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, false);
            uint32_t inner_payload_current_size = inner_payload_current->p_size;
            
            if(inner_payload_last_short_frame) {
                block_Merge(inner_payload_last_short_frame, inner_payload_current);
                //todo: validate destroy is the propr action - block_Destroy(&inner_payload_current);
                block_Destroy(&inner_payload_current);
                inner_packet = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(inner_payload_last_short_frame);
                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner = inner_packet;
                block_Destroy(&inner_payload_current);
                block_Destroy(&inner_payload_last_short_frame);
            } else {
                //todo: figure out what to do here
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet from last outer position failed");
                //block_Destroy(&inner_payload_current);
                block_Destroy(&inner_payload_current);
                return NULL;
            }
        
            if(!inner_packet) {
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment failed, last len: %u, inner_packet is null",
                                    last_outer_packet_bytes_remaining_to_parse);
                return NULL;
            }
            
            __STLTP_PARSER_DEBUG("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment of len: %u, fragment payload type: %u, merged length is: %u",
                                last_outer_packet_bytes_remaining_to_parse,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);

            atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_current);

            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current);
            
            if(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header && atsc3_stltp_tunnel_packet_is_rtp_packet_outer_sequence_number_contiguous(atsc3_stltp_tunnel_packet_last, atsc3_stltp_tunnel_packet_current)) {

                atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
                processed_refragmentation_or_concatenation_tunnel_packet = true;
                
                //hack - seek to the proper location of bytes consumed from inner_payload_current_size for marker positioning on parsing next inner packet
                //jjustman-2020-08-19 - this seems to work only for BBP, not for preamble or t&m?

                block_Seek_Relative(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE - last_outer_packet_bytes_remaining_to_parse);
                //todo: end - refactor this common pattern out here
                
                if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos != atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size)  {
                    __STLTP_PARSER_WARN("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: final ip_udp_rtp_packet_outer does not match pos: %u, size: %u, sequence: %d, last_outer_packet_bytes_remaining_to_parse: %d",
                                    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                                    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size,
									atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number,
									last_outer_packet_bytes_remaining_to_parse);

                    int _LAST_STLTP_TYPES_DEBUG_ENABLED = _STLTP_TYPES_DEBUG_ENABLED;
                    _STLTP_TYPES_DEBUG_ENABLED = 1;
                    atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current);
                    _STLTP_TYPES_DEBUG_ENABLED = _LAST_STLTP_TYPES_DEBUG_ENABLED;
                }
            } else {
                //sequence gap
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation sequence gap, from: %u to %u",
                                     atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header->sequence_number,
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number);
            }
        }
    }
    
    //for concatenation of prior tunneled packet that is not yet complete
    //remember, inner fragments will not have a corresponding inner ip/udp/rtp header, so we have to use our last if last_outer->seq += current_outer->seq
    if(atsc3_stltp_tunnel_packet_last && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_concatenation_inner) {
        if(!atsc3_stltp_tunnel_packet_last || !atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer) {
            __STLTP_PARSER_WARN(" atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: checking outer: last.sequence_number: MISSING, current.sequence_number: %d",
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number);
            goto concatenation_check_for_outer_marker;
        }
   
        bool is_outer_sequence_number_congruent = atsc3_stltp_tunnel_packet_is_rtp_packet_outer_sequence_number_contiguous(atsc3_stltp_tunnel_packet_last, atsc3_stltp_tunnel_packet_current);
        
        __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: is_outer_sequence_number_congruent: %d, checking outer: last.sequence_number: %d, current.sequence_number: %d",
                            is_outer_sequence_number_congruent,
                            atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header->sequence_number,
                            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->sequence_number);
        
        if(!is_outer_sequence_number_congruent) {
            goto concatenation_check_for_outer_marker;
        }
        
        bool should_concatenate_from_no_marker_or_marker_and_packet_offset = (!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker ||
                                                                              (atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->packet_offset > 0));
                                                                              
        //only concatenate if outer.marker == 0, or outer.marker == 1 && outer.packet_offset > 0
        __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: should_concatenate_from_no_marker_or_offset: %d, current outer.marker: %d, current packet.offset: %d",
                            should_concatenate_from_no_marker_or_marker_and_packet_offset,
                            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker,
                            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->packet_offset);
        
        if(!should_concatenate_from_no_marker_or_marker_and_packet_offset) {
            goto concatenation_check_for_outer_marker;
        }
        
        //seek past the outer packet header data, as we are re-using our inner ip_udp_rtp header from our last inner packet
        //block_Seek(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);

        __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_inner present: concatenating, outer pos: %u, outer remaining len: %u",
                            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                            block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data));
        
        int current_outer_packet_bytes_remaining_to_parse = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data);
        if(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_concatenation_inner &&  current_outer_packet_bytes_remaining_to_parse > 0) {

            //copy header but not data block
            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate_no_data_block_t(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_pending_concatenation_inner); //this will create our new inner packet, without data fragment, from the remaining fragment from our previous packet
            if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner) {
            	//assert(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
                __STLTP_PARSER_WARN(" atsc3_ip_udp_rtp_packet_duplicate_no_data_block_t: ip_udp_rtp_packet_inner is null!");
                return NULL;
            }
            
            //persist our pending baseband frames from inner ip/udp/rtp for plp number
            atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_current);

            //clear our marker flag
            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker = 0;
            
            //read from outer packet data, either up to marker or the full payload
            block_t* inner_payload_current = atsc3_stltp_read_from_outer_packet(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, false);
            if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data) {
                block_Destroy(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);
            }
            
            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data = inner_payload_current;
            
            __STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: outer pos: %u, outer remaining len: %u",
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                                block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data));

            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current);
        
            atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
            uint32_t current_ip_udp_rtp_packet_data_size_remaining = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data);
            
            if(current_ip_udp_rtp_packet_data_size_remaining < ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE && current_ip_udp_rtp_packet_data_size_remaining > 0) {
                __STLTP_PARSER_DEBUG(" block outer remaining size: %p less than %d, actual: %d", atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE, block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data));
            }
            
            //or clear inner data?
            //jjustman-2020-08-18 - TODO - fix me!
            //atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);

            processed_refragmentation_or_concatenation_tunnel_packet = true;
        }
    }

concatenation_check_for_outer_marker: ;
    //check if we have an outer marker, skip to end if present

    //atsc3_stltp_tunnel_packet_destroy(&atsc3_stltp_tunnel_packet_last);
    
    uint32_t outer_remaining_payload = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data);
    
    //if successful, check if we have a remaining payload for processing, do _not_ rely on marker
    //atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker ||

    if(!outer_remaining_payload) {
    	__STLTP_PARSER_DEBUG(" atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: marker is: %d, outer_remaining_payload is: %d",
                            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker,
                            outer_remaining_payload);
        
        //don't clear this out here for sequence congruence
        //atsc3_stltp_tunnel_packet_outer_inner_destroy(atsc3_stltp_tunnel_packet_current);

        goto cleanup_last_packet_and_return;
    }

    //if we don't have an inner payload yet due to marker, then we still need to parse 2x headers
    while(block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data) >
          (atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->packet_offset > atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos ? ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE * 2 : ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE)) {
        
        //read from outer packet data to build an inner payload
        block_t* outer_reference_inner_payload_current = atsc3_stltp_read_from_outer_packet(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, true);
        if(outer_reference_inner_payload_current) {
            
            atsc3_ip_udp_rtp_packet_t* inner_packet = atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet_current);


            if(!inner_packet) {
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: processing inner loop packet fragment failed, pos: %u,  len: %u",
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);
                return NULL;
            }
            
            //persist our pending baseband frames from inner ip/udp/rtp for plp number
            atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_current);

            __STLTP_PARSER_TRACE("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: processing inner loop packet fragment of len: %u, fragment payload type: %u, packet length is: %u",
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->i_pos,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);
           
            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current);
            
            bool last_inner_packet_complete = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
            block_Destroy(&outer_reference_inner_payload_current);
            
        }
    }
    
    //if we have carry-over payload, keep our 'outer' packet present for intra-packet reconstiution
    if(block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data)  > 0) {
    	if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer) {
    		__STLTP_PARSER_WARN("refragmentation: atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer is already set: %p (pos: %d, size: %d), but we have a new outer: %p, len: %d",
    				atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer->data,
					atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer->data->i_pos,
					atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer->data->p_size,
					atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer,
					block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data));
    	}
        __STLTP_PARSER_TRACE("Refragmentation: block remaining size: %u, outer packet: %p, setting ip_udp_rtp_packet_pending_refragmentation_outer",
        		block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data),
                             atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer);
        atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_refragmentation_outer = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer);
    } else {
        __STLTP_PARSER_TRACE("End of outer/inner parse loop, returning atsc3_stltp_tunnel_packet_current: %p", atsc3_stltp_tunnel_packet_current);
    }
    
cleanup_last_packet_and_return:
    
    //free our "last" last payload
    if(atsc3_stltp_tunnel_packet_last) {
        atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_last);
    }
    
    return atsc3_stltp_tunnel_packet_current;
}

/*
 jjustman-2020-06-02: TODO, enforce IP and PORT requirements as defined in A/324:2018
 

 
 The collection of packet(s) representing a single Baseband Packet is referred to hereafter as the Baseband Packetizer Packet Set (BPPS).
	The resultant packet Stream shall have IP destination address 239.0.51.48 and IP destination ports 30000 through 30063,
	corresponding to the PLPs numbered from 0 through 63, respectively.
 
 The resultant Stream of Preamble Payload packets shall have destination address 239.0.51.48 and destination port 30064.
 The resultant stream of TMP() packets shall have IP destination address 239.0.51.48 and destination port 30065.
 */

bool atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current)  {

    uint32_t pre_extract_container_size = 0;
    bool	has_completed_packet = false;

    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET) {
    	pre_extract_container_size = atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_v.count;

    	atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_current);
        if(atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_v.count > pre_extract_container_size) {
            has_completed_packet = true;
        }
    } else if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type == ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET){
    	pre_extract_container_size = atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_v.count;

    	atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_current);
        if(atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_v.count > pre_extract_container_size) {
            has_completed_packet = true;
        }
    } else if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type  == ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET) {
    	pre_extract_container_size = atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_v.count;

    	atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_current);
        if(atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_v.count > pre_extract_container_size) {
            has_completed_packet = true;
        }
    } else {
        __STLTP_PARSER_ERROR("Unknown inner payload type of 0x%2x", atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type);
        return NULL;
    }
    
    return has_completed_packet;
}
                                            

/**
 When the marker (M) bit is ‘0’, the Synchronization Source (SSRC) Identifier shall be set to ‘0’. When the marker (M) bit is set to ‘1’, indicating the first packet of the BPPS, the SSRC field shall contain the total length of the Baseband Packet data structure in bytes. This allows the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
 
 
 dst_ip_addr    uint32_t    4009767728
 
the payload data for each RTP/UDP/IP multicast packet shall be either a fragment of or the entire Baseband Packet data structure as defined in [3]. The collection of packet(s) representing a single Baseband Packet is referred to hereafter as the Baseband Packetizer Packet Set (BPPS). The resultant packet Stream shall have IP destination address 239.0.51.48 and IP destination ports 30000 through 30063, corresponding to the PLPs numbered from 0 through 63, respectively.


 **/
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {

    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_pending = NULL;

    if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner) {
        __STLTP_PARSER_WARN("   --- baseband packet: atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner is NULL!");
        return NULL;
    }

    if(!atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp) {
        __STLTP_PARSER_WARN("   --- baseband packet: atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp is NULL!");
        return NULL;
    }

    atsc3_stltp_baseband_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending;

    //clear out if we have a miss of incomplete packet but we have a rtp inner marker
    if(atsc3_stltp_baseband_packet_pending && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
        __STLTP_PARSER_WARN("   --- baseband packet: inner marker present, resetting incomplete packet: %p, fragment count: %d, offset: %u, length: %u -----",
                                atsc3_stltp_baseband_packet_pending,
                                atsc3_stltp_baseband_packet_pending->fragment_count,
                                atsc3_stltp_baseband_packet_pending->payload_offset,
                                atsc3_stltp_baseband_packet_pending->payload_length);

        atsc3_stltp_baseband_packet_free(&atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending);
        atsc3_stltp_baseband_packet_pending = NULL;
    }

    if(!atsc3_stltp_baseband_packet_pending && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
        atsc3_stltp_baseband_packet_pending = atsc3_stltp_baseband_packet_new_and_init(atsc3_stltp_tunnel_packet_current);
        atsc3_stltp_baseband_packet_pending->ip_udp_rtp_packet_inner->rtp_header->marker = 0; //hack so we don't wipe our our concatenating payloads

        atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending = atsc3_stltp_baseband_packet_pending;
        
    } else if(atsc3_stltp_baseband_packet_pending != NULL && atsc3_stltp_baseband_packet_pending->payload) {
    	__STLTP_PARSER_DEBUG("atsc3_stltp_baseband_packet_extract: using pending baseband packet: %p, data: %p", atsc3_stltp_baseband_packet_pending, atsc3_stltp_baseband_packet_pending->payload);
    } else {
        if(!atsc3_stltp_baseband_packet_pending) {
            __STLTP_PARSER_ERROR("      ----baseband packet: atsc3_stltp_baseband_packet_pending is null, discarding -----");
        } else if(!atsc3_stltp_baseband_packet_pending->payload) {
            __STLTP_PARSER_ERROR("      ----baseband packet: atsc3_stltp_baseband_packet_pending->payload is null, discarding, preamble_packet_pending: %p, discarding -----", atsc3_stltp_baseband_packet_pending);
        } else {
            __STLTP_PARSER_ERROR("atsc3_stltp_baseband_packet_extract: failure in atsc3_stltp_baseband_packet_pending");
            assert(false);
        }
        
        if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
        }
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
        return NULL;
    }
    
    //keep a temporary reference to our rtp header via ip_udp_rtp_packet_pending_concatenation_inner dismbiguation
    //concatenation use case when concatentating outer/outer/inner
    //free and steal if anyone else is in midst of concatenation without a marker
    //finally, free if our packet is competed concatenation
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
    }
    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
    
	if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
		/**
		ATSC A/324:2018 - Section 8.3
		When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
		‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
		field will contain the total length of the Baseband Packet data structure in bytes. This allows
		the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
		 */
        
        uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet_pending->ip_udp_rtp_packet_inner->rtp_header->packet_offset; //SSRC packet length
        //jdj-2019-07-31 - hack for A324 - 2019 compatability
        baseband_header_packet_length &= 0xFFFF;
        assert(baseband_header_packet_length);
        if(atsc3_stltp_baseband_packet_pending->payload) {
        	//this should never happen, but OK
    		__STLTP_PARSER_WARN("atsc3_stltp_baseband_packet_pending->payload is not null, ptr: %p, len: %d, freeing before new calloc", atsc3_stltp_baseband_packet_pending->payload, atsc3_stltp_baseband_packet_pending->payload_length);
    		atsc3_stltp_baseband_packet_pending->payload_length = 0;
    		freeclean((void**)&atsc3_stltp_baseband_packet_pending->payload);
        }
        
        atsc3_stltp_baseband_packet_pending->payload_length = baseband_header_packet_length;
        atsc3_stltp_baseband_packet_pending->payload = calloc(baseband_header_packet_length, sizeof(uint8_t));
        assert(atsc3_stltp_baseband_packet_pending->payload);

		__STLTP_PARSER_DEBUG("      ----baseband packet: new, pending length: %u, bbp_plp: %d, dport: %d -----", atsc3_stltp_baseband_packet_pending->payload_length,
				atsc3_stltp_baseband_packet_pending->plp_num,
				(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner ? atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->udp_flow.dst_port : -1));
	} else {
        __STLTP_PARSER_DEBUG("      ----baseband packet: append, offset: %u, length: %u, bbp_plp: %d, dport: %d -----",
        		atsc3_stltp_baseband_packet_pending->payload_offset,
				atsc3_stltp_baseband_packet_pending->payload_length,
				atsc3_stltp_baseband_packet_pending->plp_num,
				(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner ? atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->udp_flow.dst_port : -1));
    }
    
    
    block_t* packet = atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data; //don't ref with just a local var
    uint32_t packet_remaining_length = block_Remaining_size(packet);
    
    //clamp to the smaller of our phy frame (packet_remaining_length) or the remaining bbp packet length
    uint32_t block_remaining_length = __MIN(packet_remaining_length, (atsc3_stltp_baseband_packet_pending->payload_length - atsc3_stltp_baseband_packet_pending->payload_offset));
    
    __STLTP_PARSER_DEBUG("      bbp: fragment number: %u, offset: %u, fragment size: %u, computed bbp bytes remaining after append fragment: %u",
                         atsc3_stltp_baseband_packet_pending->fragment_count,
                         atsc3_stltp_baseband_packet_pending->payload_offset,
                         block_remaining_length,
                         atsc3_stltp_baseband_packet_pending->payload_length - atsc3_stltp_baseband_packet_pending->payload_offset - block_remaining_length);
    __STLTP_PARSER_DEBUG("      stltp-inner: before seek: pos: %u, size: %u, after seek: %u, inner p_size: %u,  frag len remaining: %u",
                         packet->i_pos,
                         block_Remaining_size(packet),
                         packet->i_pos + block_remaining_length,
                         packet->p_size,
                         packet->p_size - (packet->i_pos + block_remaining_length));
    
	memcpy(&atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset], block_Get(packet), block_remaining_length);

	if(block_remaining_length > 3) {
		__STLTP_PARSER_DEBUG("      peek at position: %u, 4 bytes: 0x%02x 0x%02x 0x%02x 0x%02x", atsc3_stltp_baseband_packet_pending->payload_offset,
							 atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset],
							 atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+1],
							 atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+2],
							 atsc3_stltp_baseband_packet_pending->payload[atsc3_stltp_baseband_packet_pending->payload_offset+3]);
	}
    atsc3_stltp_baseband_packet_pending->fragment_count++;
	atsc3_stltp_baseband_packet_pending->payload_offset += block_remaining_length;
    block_Seek_Relative(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, block_remaining_length);

	if(atsc3_stltp_baseband_packet_pending->payload_offset >= atsc3_stltp_baseband_packet_pending->payload_length) {
        __STLTP_PARSER_DEBUG("     ----baseband packet: complete, seeking: %u, new position is: %u, size: %u -----",
        		block_remaining_length,
				atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
				atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);
        
		atsc3_stltp_baseband_packet_pending->is_complete = true;
        atsc3_stltp_tunnel_packet_add_atsc3_stltp_baseband_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_baseband_packet_pending);
        
        //append to our refragmented stltp payload baseband container
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending = NULL;

        //clean up pending concatenation now we are completed
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);

        //clean up baseband packet inner/outer payload data
        atsc3_stltp_baseband_packet_free_outer_inner_data(atsc3_stltp_baseband_packet_pending);

    } else {
        __STLTP_PARSER_DEBUG("     ----baseband packet: pending, seeking: %u, new position is: %u, size: %u -----", block_remaining_length, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);
    }

	return atsc3_stltp_baseband_packet_pending;
}


/*
 The RTP header fields of the Preamble Payload Packet Set shall be as described below, configured with the marker (M) bit of the packet containing the beginning of a Preamble Payload data structure set to ‘1’. The marker (M) bits of remaining packets shall be set to ‘0’. This allows the Transmission system on the Data Consumer end of the STL to reconstruct the Preamble Payload data structure after any resequencing takes place. The timestamps of the packets of a given Preamble Payload Packet Set shall have the same values. The timestamp values are derived from a subset of the “Bootstrap_Timing_Data ()” appearing in Table 8.3, providing a mechanism to uniquely associate each of the Preamble Payload packets with a specific Physical Layer frame. The format of the timestamp field is described in Table 8.2.
 
  The resultant Stream of Preamble Payload packets shall have destination address 239.0.51.48 and destination port 30064 before application of channel number offset of the port number in the case of multichannel carriage within a single STL Tunnel Packet Stream.
 On reception, the Preamble Payload Packet Set shall be resequenced, when necessary, and extracted into the Preamble Payload data structure as described in Table 8.1. The Data Consumer can accumulate RTP packets until it has received all of the bytes defined by the length field in the first packet. If a packet is missed, as determined by a missing sequence number, or if a packet with the marker (M) bit set to ‘1’ is received prematurely, indicating the start of the next Preamble Payload Packet Set, then one or more packets have been lost and the entire Preamble Payload data set has been lost. Any accumulated data shall be discarded.
 
 The marker (M) bit shall be set to '1’ to indicate that the first byte of the payload is the start of the
 Preamble Payload data. A ‘0’ value shall indicate that the payload is a continuation of the
 Preamble Payload data from the previous packet.
 
 
 **/

atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {
    
    atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending;

    //if our inner tunnel packet has a marker set, and we have a pending packet - force clear out pending packet
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker && atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending) {
        __STLTP_PARSER_ERROR("atsc3_stltp_preamble_packet_extract: force clearing preamble pending packet due to inner marker");
        atsc3_stltp_preamble_packet_free(&atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending);
        atsc3_stltp_preamble_packet_pending = NULL;
    }
    
    //create a new preamble pending packet if we have a marker
    if(!atsc3_stltp_preamble_packet_pending && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
		atsc3_stltp_preamble_packet_pending = calloc(1, sizeof(atsc3_stltp_preamble_packet_t));
        atsc3_stltp_preamble_packet_pending->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
		atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = atsc3_stltp_preamble_packet_pending;
       
    } else if(atsc3_stltp_preamble_packet_pending != NULL && atsc3_stltp_preamble_packet_pending->payload) {
        __STLTP_PARSER_DEBUG("atsc3_stltp_preamble_packet_extract: appending to pending preamble packet: %p, data: %p", atsc3_stltp_preamble_packet_pending, atsc3_stltp_preamble_packet_pending->payload);
    } else {
        //error cases for no preamble packet pending (and no marker) or missing inner payload
        //TODO: if we have an outer marker, seek to this position?
        if(!atsc3_stltp_preamble_packet_pending) {
            __STLTP_PARSER_ERROR("      ----preamble packet: atsc3_stltp_preamble_packet_pending is null, discarding -----");
        } else if(!atsc3_stltp_preamble_packet_pending->payload) {
            __STLTP_PARSER_ERROR("      ----preamble packet: atsc3_stltp_preamble_packet_pending->payload is null, discarding, preamble_packet_pending: %p, discarding -----", atsc3_stltp_preamble_packet_pending);
        } else {
            __STLTP_PARSER_ERROR("atsc3_stltp_preamble_packet_extract: failure in atsc3_stltp_preamble_packet_pending");
        }
        
        if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
        }
        
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
        return NULL;
    }
    
    //keep a temporary reference to our rtp header via ip_udp_rtp_packet_pending_concatenation_inner dismbiguation, concatenation use case when concatentating outer/outer/inner
    //free and steal if anyone else is in midst of concatenation without a marker, carry over our pending concatenation if needed
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
    }
    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
    
    block_t* packet = atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data; //don't ref with just a local var

    //&& !atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending->fragment_count
	if(atsc3_stltp_preamble_packet_pending->ip_udp_rtp_packet_inner->rtp_header->marker) {
		//read the first uint16_t for our preamble length
        //The length field shall contain the number of bytes in the Preamble Payload data structure following the length field excluding the crc16 bytes
		atsc3_stltp_preamble_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet)))) + 2 + 2; //extra +2 is for the crc16 not included in the length field
		atsc3_stltp_preamble_packet_pending->ip_udp_rtp_packet_inner->rtp_header->packet_offset = atsc3_stltp_preamble_packet_pending->payload_length;
		atsc3_stltp_preamble_packet_pending->payload = calloc(atsc3_stltp_preamble_packet_pending->payload_length, sizeof(uint8_t));
        atsc3_stltp_preamble_packet_pending->ip_udp_rtp_packet_inner->rtp_header->marker = 0; //hack so we don't wipe our our concatenating payloads
        __STLTP_PARSER_DEBUG("      ----preamble packet: new -----");
    } else {
        __STLTP_PARSER_DEBUG("      ----preamble packet: append -----");
    }
    __STLTP_PARSER_DEBUG("       preamble length:    %u (payload: %p)",  atsc3_stltp_preamble_packet_pending->payload_length, atsc3_stltp_preamble_packet_pending->payload);

    uint32_t block_remaining_length = block_Remaining_size(packet);
    
    if(block_remaining_length + atsc3_stltp_preamble_packet_pending->payload_offset > atsc3_stltp_preamble_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_preamble_packet_pending->payload_length - atsc3_stltp_preamble_packet_pending->payload_offset);
    }
    __STLTP_PARSER_DEBUG("       fragment %u length:  %u",  atsc3_stltp_preamble_packet_pending->fragment_count, block_remaining_length);

    memcpy(&atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], block_Get(packet), block_remaining_length);
	__STLTP_PARSER_DEBUG("       first bytes:         0x%02x 0x%02x", atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset+1]);
    __STLTP_PARSER_DEBUG("       preamble_payload_length: %u", atsc3_stltp_preamble_packet_pending->payload_length);

    atsc3_stltp_preamble_packet_pending->fragment_count++;
    atsc3_stltp_preamble_packet_pending->payload_offset += block_remaining_length;
    block_Seek_Relative(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, block_remaining_length);

	if(atsc3_stltp_preamble_packet_pending->payload_offset >= atsc3_stltp_preamble_packet_pending->payload_length) {
		//process the completed preamble data structures
		__STLTP_PARSER_DEBUG("       ----preamble packet: complete-----");
		atsc3_stltp_preamble_packet_pending->is_complete = true;
        
        atsc3_stltp_tunnel_packet_add_atsc3_stltp_preamble_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_preamble_packet_pending);
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = NULL;

        //clean up for outer/outer/inner concatenation
        if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
        }
        
        //clean up preamble packet inner/outer payload data only if we have completed packet
        atsc3_stltp_preamble_packet_free_outer_inner_data(atsc3_stltp_preamble_packet_pending);
    }

    __STLTP_PARSER_DEBUG("     ----preamble packet: return, seeking: %u, new position is: %u, size: %u -----",
                         block_remaining_length,
                         atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                         atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);

	return atsc3_stltp_preamble_packet_pending;
}

/*
 
 The payload data for each Timing and Management Stream RTP/UDP/IP packet shall be a fragment of the TMP() data structure described in Table 8.3.
 To provide validation that the TMP() structure is delivered correctly over the STL, a 16-bit cyclic redundancy check is provided as part of the TMP() data.
 
 The resultant stream of TMP() packets shall have IP destination address 239.0.51.48 and destination port 30065
 
 The RTP header fields of the TMP Packet Set shall be as described below, configured with the marker (M) bit of the packet containing the beginning of a
 TMP() data structure set to one ‘1’. The marker (M) bits of remaining packets shall be set to zero ‘0’.
 
 This allows the transmission system on the consumer end of the STL to reconstruct the TMP() data structure after any ordering correction takes place.
 
 The timestamps of the packets of a given TMP Packet Set shall have the same values.
 
 The timestamp values are derived from a subset of the Bootstrap_Timing_Data, providing a mechanism to uniquely associate each of the TMP packets with a
 specific Physical Layer frame.
 
 ..
 Timing & Management_Packet (TMP) ()
 {
     Structure_Data () {
         length             uint16_t
         version_major
         version_minor
         maj_log_rep_cnt_pre
         maj_log_rep_cnt_tim
         bootstrap_major
         bootstrap_minor
         min_time_to_next
         system_bandwidth
         bsr_coefficient
         preamble_structure
         ea_wakeup
         num_emission_tim
         num_xmtrs_in_group
         xmtr_group_num
         maj_log_override
         num_miso_filt_codes
         tx_carrier_offset
         reserved
     }
 **/

//jjustman-2019-08-08 - workaround for TMP packets not containing inner marker flag
#define _ATSC3_D_HACK_TMP_PACKET_MARKER false

atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {
    
    atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending;
    
    //if our inner tunnel packet has a marker set, and we have a pending packet - force clear out pending packet
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker && atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending) {
        __STLTP_PARSER_ERROR("atsc3_stltp_timing_management_packet_extract: force clearing timing_management pending packet due to inner marker");
        atsc3_stltp_timing_management_packet_free(&atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending);
        atsc3_stltp_timing_management_packet_pending = NULL;
    }
    
#ifdef _ATSC3_D_HACK_TMP_PACKET_MARKER
    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker = 1;
#endif
    
    //create a new TMP pending packet if we have a marker
	if(!atsc3_stltp_timing_management_packet_pending && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
		atsc3_stltp_timing_management_packet_pending = calloc(1, sizeof(atsc3_stltp_timing_management_packet_t));
        atsc3_stltp_timing_management_packet_pending->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
		atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = atsc3_stltp_timing_management_packet_pending;
    } else if(atsc3_stltp_timing_management_packet_pending && atsc3_stltp_timing_management_packet_pending->payload) {
    	__STLTP_PARSER_DEBUG("atsc3_stltp_timing_management_packet_extract: appending pending timing_management packet: %p, data: %p", atsc3_stltp_timing_management_packet_pending, atsc3_stltp_timing_management_packet_pending->payload);
    } else {
        uint32_t tm_remaining_bytes = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);
        //error cases for no TMP packet pending (and no marker) or missing inner payload
        if(!atsc3_stltp_timing_management_packet_pending){
            __STLTP_PARSER_ERROR("      ----timing_management packet: atsc3_stltp_timing_management_packet_pending is null, discarding -----");
        } else if(!atsc3_stltp_timing_management_packet_pending->payload) {
            __STLTP_PARSER_ERROR("      ----timing_management packet: atsc3_stltp_timing_management_packet_pending->payload is null, discarding -----");
        } else {
            __STLTP_PARSER_ERROR("      ----timing_management packet: failure in atsc3_stltp_timing_management_packet_extract");
        }
        
        if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
        }
        
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
        return NULL;
    }

    //keep a temporary reference to our rtp header via ip_udp_rtp_packet_pending_concatenation_inner dismbiguation, concatenation use case when concatentating outer/outer/inner
    //free and steal if anyone else is in midst of concatenation without a marker, carry over our pending concatenation if needed
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
    }
    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
    
    block_t* packet = atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data; //don't ref with just a local var

    //&& !atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending->fragment_count
    if(atsc3_stltp_timing_management_packet_pending->ip_udp_rtp_packet_inner->rtp_header->marker ) {
		//read the first uint16_t for our preamble length
		atsc3_stltp_timing_management_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet))));
		atsc3_stltp_timing_management_packet_pending->ip_udp_rtp_packet_inner->rtp_header->packet_offset = atsc3_stltp_timing_management_packet_pending->payload_length;
		atsc3_stltp_timing_management_packet_pending->payload = calloc(atsc3_stltp_timing_management_packet_pending->payload_length, sizeof(uint8_t));
        atsc3_stltp_timing_management_packet_pending->ip_udp_rtp_packet_inner->rtp_header->marker = 0; //hack so we don't wipe our our concatenating payloads
        __STLTP_PARSER_DEBUG("       ----timing_management packet: new -----");

    } else {
        __STLTP_PARSER_DEBUG("       ----timing_management packet: append -----");
    }
    __STLTP_PARSER_DEBUG("       TMP length:    %u (payload: %p)",  atsc3_stltp_timing_management_packet_pending->payload_length, atsc3_stltp_timing_management_packet_pending->payload);

    uint32_t block_remaining_length = block_Remaining_size(packet);

    if(block_remaining_length + atsc3_stltp_timing_management_packet_pending->payload_offset > atsc3_stltp_timing_management_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_timing_management_packet_pending->payload_length - atsc3_stltp_timing_management_packet_pending->payload_offset);
    }
    __STLTP_PARSER_DEBUG("       fragment %u, length:  %u",  atsc3_stltp_timing_management_packet_pending->fragment_count, block_remaining_length);
    
	memcpy(&atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], block_Get(packet), block_remaining_length);
    if(atsc3_stltp_timing_management_packet_pending->payload_length  > 1) {
        __STLTP_PARSER_DEBUG("       first bytes:         0x%02x 0x%02x", atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset+1]);
    }
	__STLTP_PARSER_DEBUG("       payload_offset now: %u, preamble_payload_length: %u", atsc3_stltp_timing_management_packet_pending->payload_offset, atsc3_stltp_timing_management_packet_pending->payload_length);
    
    atsc3_stltp_timing_management_packet_pending->fragment_count++;
    atsc3_stltp_timing_management_packet_pending->payload_offset += block_remaining_length;
    block_Seek_Relative(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, block_remaining_length);

	if(atsc3_stltp_timing_management_packet_pending->payload_offset >= atsc3_stltp_timing_management_packet_pending->payload_length) {
		//process the preamble data structures
		__STLTP_PARSER_DEBUG("     ----timing_management packet: complete-----");
		atsc3_stltp_timing_management_packet_pending->is_complete = true;
        atsc3_stltp_tunnel_packet_add_atsc3_stltp_timing_management_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_timing_management_packet_pending);
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = NULL;
        
        //clean up for outer/outer/inner concatenation
        if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_pending_concatenation_inner);
        }
        
        //clean up timing_management packet inner/outer payload data only if we have completed packet
        atsc3_stltp_timing_management_packet_free_outer_inner_data(atsc3_stltp_timing_management_packet_pending);
	}
    //clean up timing packet inner/outer payload data


    __STLTP_PARSER_DEBUG("     ----timing_management packet: return, seeking: %u, new position is: %u, size: %u -----",
                         block_remaining_length,
                         atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                         atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);
    
	return atsc3_stltp_timing_management_packet_pending;
}





//timing_management_packet_t

atsc3_timing_management_packet_t* atsc3_stltp_parse_timing_management_packet(atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet) {
    
    uint8_t *binary_payload = atsc3_stltp_timing_management_packet->payload;
    uint8_t *binary_payload_start = binary_payload;

    uint32_t binary_payload_length = atsc3_stltp_timing_management_packet->payload_length;
    
    atsc3_timing_management_packet_t* atsc3_timing_management_packet = calloc(1, sizeof(atsc3_timing_management_packet_t));

    atsc3_timing_management_packet_set_bootstrap_timing_ref_from_stltp_preamble_packet(atsc3_timing_management_packet, atsc3_stltp_timing_management_packet);


    //length: 16
    atsc3_timing_management_packet->length = ntohs(*((uint16_t*)binary_payload));
    binary_payload += 2;
    
    __STLTP_PARSER_DEBUG("---------------------------------------");
    __STLTP_PARSER_DEBUG("Timing Management Packet Header: pointer: %p, sequence_number: %d, port: %d, length: %u, TMP.Structure_Data.length: %d",
                      binary_payload,
                      atsc3_stltp_timing_management_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                      atsc3_stltp_timing_management_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port,
                      atsc3_stltp_timing_management_packet->payload_length,
                      atsc3_timing_management_packet->length);
    __STLTP_PARSER_DEBUG("Raw hex: 0x%02hhX 0x%02hhX 0x%02hhX 0x%02hhX", binary_payload[0], binary_payload[1], binary_payload[2], binary_payload[3]);
    __STLTP_PARSER_DEBUG("---------------------------------------");
    
    /* debugging
    
    for(int i=0; i < atsc3_stltp_timing_management_packet->payload_length; i+=4) {
        __STLTP_PARSER_INFO("byte: %d: 0x%02x 0x%02x 0x%02x 0x%02x", i, binary_payload_start[i], binary_payload_start[i+1], binary_payload_start[i+2], binary_payload_start[i+3]);
    }
    
    */
    
    for(int i=0; i < atsc3_timing_management_packet->length; i+=4) {
        __STLTP_PARSER_TRACE("byte: %d: 0x%02x 0x%02x 0x%02x 0x%02x", i, binary_payload_start[i], binary_payload_start[i+1], binary_payload_start[i+2], binary_payload_start[i+3]);
    }
    
    
    //version_major: 4
    atsc3_timing_management_packet->version_major = (*binary_payload >> 4) & 0xF;
    //version_minor: 4
    atsc3_timing_management_packet->version_minor = (*binary_payload) & 0xF;
    binary_payload++;
    
    //maj_log_rep_cnt_pre: 4
    atsc3_timing_management_packet->maj_log_rep_cnt_pre = (*binary_payload >> 4) & 0xF;
    //maj_log_rep_cnt_tim: 4
    atsc3_timing_management_packet->maj_log_rep_cnt_tim = (*binary_payload) & 0xF;
    binary_payload++;
    
    //bootstrap_major: 4
    atsc3_timing_management_packet->bootstrap_major = (*binary_payload >> 4) & 0xF;
    //bootstrap_minor: 4
    atsc3_timing_management_packet->bootstrap_minor = (*binary_payload) & 0xF;
    binary_payload++;
       
    //5 bytes
    atsc3_timing_management_packet->min_time_to_next = (*binary_payload >> 3) & 0x1F;
    //2 bytes
    atsc3_timing_management_packet->system_bandwidth = (*binary_payload >> 1) & 0x3;
    
    //bsr_coefficient: 7
    atsc3_timing_management_packet->bsr_coefficient = (*binary_payload & 0x1) << 6;
    binary_payload++;
    atsc3_timing_management_packet->bsr_coefficient |= (*binary_payload >> 2 ) & 0x3F;
    
    //preamble_structure: 8
    atsc3_timing_management_packet->preamble_structure = (*binary_payload & 0x3) << 6;
    binary_payload++;
    atsc3_timing_management_packet->preamble_structure |= (*binary_payload >> 6) & 0x3F;
    
    //ea_wakeup: 2
    atsc3_timing_management_packet->ea_wakeup = (*binary_payload) & 0x3;
    binary_payload++;
    
    //num_emission_tim: 6
    atsc3_timing_management_packet->num_emission_tim = (*binary_payload >> 2) & 0x3F;
    
    //num_xmtrs_in_group: 6 - 2 bytes
    atsc3_timing_management_packet->num_xmtrs_in_group = (*binary_payload & 0x3) << 4;
    binary_payload++;
    //4 bytes remaining
    atsc3_timing_management_packet->num_xmtrs_in_group |= (*binary_payload >> 4) & 0xF;
    
    //xmtr_group_num:7
    atsc3_timing_management_packet->xmtr_group_num = ((*binary_payload) & 0xF) << 4;
    //remaining 3 bytes
    binary_payload++;
    atsc3_timing_management_packet->xmtr_group_num |= (*binary_payload >> 5) & 0x7;
    
    //remaining 5 bytes
    //maj_log_override: 3
    atsc3_timing_management_packet->maj_log_override = (*binary_payload >> 2) & 0x7;
    
    //num_miso_filt_codes: 2
    atsc3_timing_management_packet->maj_log_override = (*binary_payload) & 0x3;
    binary_payload++;
    
    //tx_carrier_offset: 2
    atsc3_timing_management_packet->tx_carrier_offset = (*binary_payload >> 6) & 0x3;
    
    //reserved: 6 bits, all 1's
    atsc3_timing_management_packet->_reserved = (*binary_payload) & 0x3F;
    binary_payload++;
    
    if(atsc3_timing_management_packet->_reserved != 0x3F) {
        __STLTP_PARSER_WARN("timing management packet reserved is not 0x3F (0011 1111), val is: 0x%02x", atsc3_timing_management_packet->_reserved);
    }
    
    //process bootstrap_timing_data
    __STLTP_PARSER_DEBUG("timing management: processing bootstrap_timing with %d num_emission_tim entries at pos: %ld", atsc3_timing_management_packet->num_emission_tim, binary_payload - binary_payload_start);
    for(int i=0; i <= atsc3_timing_management_packet->num_emission_tim; i++) {
        atsc3_bootstrap_timing_data_t* atsc3_bootstrap_timing_data = calloc(1, sizeof(atsc3_bootstrap_timing_data_t));
        atsc3_bootstrap_timing_data->seconds = ntohl(*((uint32_t*)binary_payload));
        binary_payload += 4;
        atsc3_bootstrap_timing_data->nanoseconds = ntohl(*((uint32_t*)binary_payload));
        binary_payload += 4;

        //jjustman-2020-08-31 - create our bootstrap_timing_data_timestamp_short_reference here
        //22 least significant bits (LSBs) of the seconds field
        atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.seconds_pre = 0x3FFFFF & atsc3_bootstrap_timing_data->seconds;
        // 10-bit value identical to the value contained in the 3rd through 12 MSBs
        atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre = 0x3FF & (((int)(atsc3_bootstrap_timing_data->nanoseconds * ATSC3_A324_A_MILLISECOND_PERIOD))  >> 20);

        __STLTP_PARSER_DEBUG("timing management: adding num_emission: %d, bootstrap_timing with sec.ns: %d.%d, seconds_pre: 0x%04x, a_milliseconds_pre: 0x%02x",
        		i,
				atsc3_bootstrap_timing_data->seconds,
				atsc3_bootstrap_timing_data->nanoseconds,
				atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.seconds_pre,
				atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre
		);

        atsc3_timing_management_packet_add_atsc3_bootstrap_timing_data(atsc3_timing_management_packet, atsc3_bootstrap_timing_data);
    }
        
    //process per_transmitter_data
    __STLTP_PARSER_DEBUG("timing management: processing per transmitter data with %d per_transmitter_data entries, pos: %ld", atsc3_timing_management_packet->num_xmtrs_in_group, binary_payload - binary_payload_start);

    for(int i=0; i <= atsc3_timing_management_packet->num_xmtrs_in_group; i++) {
        //atsc3_per_transmitter_data_t
        atsc3_per_transmitter_data_t* atsc3_per_transmitter_data = calloc(1, sizeof(atsc3_per_transmitter_data_t));
        //xmtr_id: 13 -> 8, 5
        atsc3_per_transmitter_data->xmtr_id = (*binary_payload << 5);
        binary_payload++;
        atsc3_per_transmitter_data->xmtr_id |= (*binary_payload >> 3) & 0x1F;
        
        //tx_time_offset: 16
        atsc3_per_transmitter_data->tx_time_offset = ((*binary_payload) & 0x7) << 13; //3 bits
        binary_payload++;
        atsc3_per_transmitter_data->tx_time_offset |= (*binary_payload) << 5; //8 bits
        binary_payload++;
        atsc3_per_transmitter_data->tx_time_offset |= (*binary_payload >> 3) & 0x1F; //remaining 5 bits
        
        //3 bits remaining
        //txid_injection_lvl: 4
        atsc3_per_transmitter_data->txid_injection_lvl = ((*binary_payload) & 0x7) << 1;
        binary_payload++;
        atsc3_per_transmitter_data->txid_injection_lvl |= ((*binary_payload) >> 7 ) & 0x1;
        
        //miso_filt_code_index: 2
        atsc3_per_transmitter_data->miso_filt_code_index = ((*binary_payload) >> 5) & 0x3;
        
        //5 bits remaining
        //reserved should be all 1's
        //atsc3_per_transmitter_data->_reserved =
        //
        binary_payload+=4;
    
        __STLTP_PARSER_DEBUG("timing management: adding transmitter num: %d, xmtr_id: 0x%04x, tx_time_offset: 0x%04x (%0.1f uS), txid_injection_lvl: 0x%02x, miso_filt_code: 0x%02x",
                             i,
                             atsc3_per_transmitter_data->xmtr_id,
                             atsc3_per_transmitter_data->tx_time_offset,
                             ((int16_t) atsc3_per_transmitter_data->tx_time_offset) / 10.0,
                             atsc3_per_transmitter_data->txid_injection_lvl,
                             atsc3_per_transmitter_data->miso_filt_code_index);

        atsc3_timing_management_packet_add_atsc3_per_transmitter_data(atsc3_timing_management_packet, atsc3_per_transmitter_data);
    }
    
    //packet release time
    //pkt_rls_seconds: 4
    atsc3_timing_management_packet->packet_release_time.pkt_rls_seconds = (*binary_payload >> 4) & 0xF;
    
    /* pkt_rls_a-milliseconds shall be the milliseconds portion of the time of release from the Broadcast Gateway of the specific Timing and Management packet in which the value is found.
     Its value shall be expressed as 10 bits representing the 3rd through 12th MSBs of the nanoseconds value of the TAI time when the first bit of the IP header of the Timing and Management
     packets is released from the Broadcast Gateway.
     Its range will be from 0 to 953 (decimal) as a consequence of the Period of an a-millisecond being slightly longer than precisely a millisecond.
     
     See the definition of an a-millisecond in Section 3.4.
     
     a-millisecond – A time interval approximately equal to one millisecond derived from a binary
     count of nanoseconds and actually equaling 220 nanoseconds, which represents 1,048,576
     nanoseconds (i.e., having a Period of 1.048576 milliseconds).
     
     */
    
    //pkt_rls_a_miliseconds: 10, (4 | 6)
    atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds = (*binary_payload & 0xF) << 6;
    binary_payload++;
    atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds |= (*binary_payload >> 2) & 0x3F;
    uint32_t pkt_rls_a_miliseconds_temp = atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds;
    atsc3_timing_management_packet->packet_release_time.pkt_rls_computed_milliseconds = ((pkt_rls_a_miliseconds_temp << 20) * ATSC3_A324_A_MILLISECOND_PERIOD);
    
    atsc3_timing_management_packet->packet_release_time._reserved = (*binary_payload) & 0x3;
    binary_payload++;
    
    
    if(atsc3_timing_management_packet->packet_release_time._reserved != 0x3) {
        __STLTP_PARSER_WARN("timing management packet: packet_release_time reserved is not 0x3 (0011), val is: 0x%02x", atsc3_timing_management_packet->packet_release_time._reserved);
    }
    
    __STLTP_PARSER_DEBUG("timing management packet: pkt_rls_seconds: %02d.%09d (a-milliseconds: %4d, 0x%04x)",
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_seconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_computed_milliseconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds);
    
    atsc3_timing_management_packet->error_check_data.crc16 = ntohs(*((uint16_t*)binary_payload));
    //jjustman-2020-08-31 - TODO: calculate crc16 check
    binary_payload+=2;
    
    int parsed_length = binary_payload - binary_payload_start;
    
    __STLTP_PARSER_DEBUG("timing management packet: payload len: %d, parsed len: %d: (start: %p, binary_payload: %p)", atsc3_timing_management_packet->length, parsed_length, binary_payload_start, binary_payload);
    
    return atsc3_timing_management_packet;

cleanup:
    if(atsc3_timing_management_packet) {
        free(atsc3_timing_management_packet);
        atsc3_timing_management_packet = NULL;
    }
    
    return NULL;
}



/*
    parse A/324 preamble packet from STLTP inner payload
 */


atsc3_preamble_packet_t* atsc3_stltp_parse_preamble_packet(atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet) {
    uint8_t *binary_payload = atsc3_stltp_preamble_packet->payload;
    uint8_t *binary_payload_start = binary_payload;
    uint32_t binary_payload_length = atsc3_stltp_preamble_packet->payload_length;
   
    block_t* block = block_Duplicate_from_ptr(binary_payload, binary_payload_length);
    block_Rewind(block);
    
    atsc3_preamble_packet_t* atsc3_preamble_packet = calloc(1, sizeof(atsc3_preamble_packet_t));

    atsc3_preamble_packet_set_bootstrap_timing_ref_from_stltp_preamble_packet(atsc3_preamble_packet, atsc3_stltp_preamble_packet);

    //length: 16
    atsc3_preamble_packet->length = block_Read_uint16_ntohs(block);

    __STLTP_PARSER_DEBUG("---------------------------------------");
    __STLTP_PARSER_DEBUG("preamble packet header: pointer: %p, sequence_number: %d, port: %d, length: %u, PreamblePayload.length: %d",
                 binary_payload,
                 atsc3_stltp_preamble_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                 atsc3_stltp_preamble_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port,
                 atsc3_stltp_preamble_packet->payload_length,
                 atsc3_preamble_packet->length);
    __STLTP_PARSER_DEBUG("preamble: raw hex: 0x%02hhX 0x%02hhX 0x%02hhX 0x%02hhX", binary_payload[0], binary_payload[1], binary_payload[2], binary_payload[3]);
    __STLTP_PARSER_DEBUG("---------------------------------------");

    /* debugging
    */
    
    //3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_version = block_Read_uint8_bitlen(block, 3);
    //1 bit
    atsc3_preamble_packet->L1_basic_signaling.L1B_mimo_scattered_pilot_encoding = block_Read_uint8_bitlen(block, 1);
    //1 bit
    atsc3_preamble_packet->L1_basic_signaling.L1B_lls_flag = block_Read_uint8_bitlen(block, 1);
    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag = block_Read_uint8_bitlen(block, 2);
    //1 bit
    atsc3_preamble_packet->L1_basic_signaling.L1B_return_channel_flag = block_Read_uint8_bitlen(block, 1);
    
    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_papr_reduction = block_Read_uint8_bitlen(block, 2);
    //1 bit
    atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length_mode = block_Read_uint8_bitlen(block, 1);
    
    if(atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length_mode == 0) {
        //10 bits
        atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length = block_Read_uint16_bitlen(block, 10);
        //13 bits
        atsc3_preamble_packet->L1_basic_signaling.L1B_excess_samples_per_symbol = block_Read_uint16_bitlen(block, 13);
        
    } else {
        //16
        atsc3_preamble_packet->L1_basic_signaling.L1B_time_offset = block_Read_uint16_bitlen(block, 16);
        //7 bits
        atsc3_preamble_packet->L1_basic_signaling.L1B_additional_samples = block_Read_uint16_bitlen(block, 7);
    }
    
    //8 bits, 6 avail
    atsc3_preamble_packet->L1_basic_signaling.L1B_num_subframes = block_Read_uint8_bitlen(block, 8);
    //3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_preamble_num_symbols = block_Read_uint8_bitlen(block, 3);
    //3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_preamble_reduced_carriers = block_Read_uint8_bitlen(block, 3);
    
    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_content_tag = block_Read_uint8_bitlen(block, 2);
    
    //13 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_size_bytes = block_Read_uint16_bitlen(block, 13);

    //L1B_L1_Detail_fec_type: 3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_fec_type = block_Read_uint8_bitlen(block, 3);
    
    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_additional_parity_mode = block_Read_uint8_bitlen(block, 2);
    
    //19 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_total_cells = block_Read_uint32_bitlen(block, 19);

    //L1B_first_sub_mimo:1
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_mimo = block_Read_uint8_bitlen(block, 1);

    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_miso = block_Read_uint8_bitlen(block, 2);
    //2 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_fft_size = block_Read_uint8_bitlen(block, 2);
    //3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_reduced_carriers = block_Read_uint8_bitlen(block, 3);
    
    //4 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_guard_interval = block_Read_uint8_bitlen(block, 4);

    //11 bits - 5 = 6
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_num_ofdm_symbols = block_Read_uint16_bitlen(block, 11);
    
    //L1B_first_sub_scattered_pilot_pattern:5
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_scattered_pilot_pattern = block_Read_uint8_bitlen(block, 5);
    //3 bits
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_scattered_pilot_boost = block_Read_uint8_bitlen(block, 3);
    //1 bit
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_first = block_Read_uint8_bitlen(block, 1);
    //1 bit even...
    atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_last = block_Read_uint8_bitlen(block, 1);
    
    //TODO: check reserved
    block->i_pos += 6; //6*8 = 48bits
    
    atsc3_preamble_packet->L1_basic_signaling.L1B_crc = block_Read_uint32_ntohl(block);

    //L1B done..now onto L1D
    
    int bytes_processed = block->i_pos;
    int bytes_remaining = block->p_size - block->i_pos;
    
    //4 bits
    atsc3_preamble_packet->L1_detail_signaling.L1D_version = block_Read_uint8_bitlen(block, 4);
    
    //3 bits
    atsc3_preamble_packet->L1_detail_signaling.L1D_num_rf = block_Read_uint8_bitlen(block, 3);

    for(int i=1; i < atsc3_preamble_packet->L1_detail_signaling.L1D_num_rf; i++) {
        L1D_bonded_bsid_block_t* L1D_bonded_bsid_block = L1D_bonded_bsid_block_new();
        
        //16 bits
		L1D_bonded_bsid_block->L1D_bonded_bsid =  block_Read_uint16_bitlen(block, 16);
		
        //reserved 3 bits
		L1D_bonded_bsid_block->reserved = block_Read_uint8_bitlen(block, 3);
        
        L1_detail_signaling_add_L1D_bonded_bsid_block(&atsc3_preamble_packet->L1_detail_signaling, L1D_bonded_bsid_block);
    }
	
    if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x0) {
        //32
		atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_sec = block_Read_uint32_bitlen(block, 32);
        
        //10 bits
		atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_msec = block_Read_uint16_bitlen(block, 10);
		
		if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x01) {
            //10 bits
            //time usec
            atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_usec = block_Read_uint16_bitlen(block, 10);
            if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x02) {
                //10bits
                //time nsec
                atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_nsec = block_Read_uint16_bitlen(block, 10);
            }
        }
    }
    
    for(int i=0; i <= atsc3_preamble_packet->L1_basic_signaling.L1B_num_subframes; i++) {
		L1D_subframe_parameters_t* L1D_subframe_parameters = L1D_subframe_parameters_new();
		
        if(i > 0) {
			//1 bit
			L1D_subframe_parameters->L1D_mimo = block_Read_uint8_bitlen(block, 1);
			//2 bit
			L1D_subframe_parameters->L1D_miso = block_Read_uint8_bitlen(block, 2);
			//2 bit
			L1D_subframe_parameters->L1D_fft_size = block_Read_uint8_bitlen(block, 2);
			//3 bit
			L1D_subframe_parameters->L1D_reduced_carriers = block_Read_uint8_bitlen(block, 3);
			//4 bit
			L1D_subframe_parameters->L1D_guard_interval = block_Read_uint8_bitlen(block, 4);
			//11 bit
			L1D_subframe_parameters->L1D_num_ofdm_symbols = block_Read_uint16_bitlen(block, 11);
			//5 bit
			L1D_subframe_parameters->L1D_scattered_pilot_pattern = block_Read_uint8_bitlen(block, 5);
			//3 bit
			L1D_subframe_parameters->L1D_scattered_pilot_boost = block_Read_uint8_bitlen(block, 3);
			//1 bit
			L1D_subframe_parameters->L1D_sbs_first = block_Read_uint8_bitlen(block, 1);
			//1 bit
			L1D_subframe_parameters->L1D_sbs_last = block_Read_uint8_bitlen(block, 1);
        }
        
        if(atsc3_preamble_packet->L1_basic_signaling.L1B_num_subframes > 0) {
            //1 bit
            L1D_subframe_parameters->L1D_subframe_multiplex = block_Read_uint8_bitlen(block, 1);
        }
        
        //1
		L1D_subframe_parameters->L1D_frequency_interleaver = block_Read_uint8_bitlen(block, 1);

		if (((i==0) && (atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_first ||
					   atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_last)) ||
			((i>0) && (L1D_subframe_parameters->L1D_sbs_first || L1D_subframe_parameters->L1D_sbs_last))) {
			
            //13
            //L1D_sbs_null_cells
			L1D_subframe_parameters->L1D_sbs_null_cells = block_Read_uint16_bitlen(block, 13);

        }
        
        //L1d_num_plp: 6 bits
		L1D_subframe_parameters->L1D_num_plp = block_Read_uint8_bitlen(block, 6);
        
		for (int j=0; j <= L1D_subframe_parameters->L1D_num_plp; j++) {
			L1D_PLP_parameters_t* L1D_PLP_parameters = L1D_PLP_parameters_new();
			
			//6 bits
            L1D_PLP_parameters->L1D_plp_id = block_Read_uint8_bitlen(block, 6);

			//1 bit
            L1D_PLP_parameters->L1D_plp_lls_flag = block_Read_uint8_bitlen(block, 1);

			//2 bit
            L1D_PLP_parameters->L1D_plp_layer = block_Read_uint8_bitlen(block, 2);

            //24 bit
            L1D_PLP_parameters->L1D_plp_start = block_Read_uint32_bitlen(block, 24);

            //24 bit
            L1D_PLP_parameters->L1D_plp_size = block_Read_uint32_bitlen(block, 24);

            //2 bits
            L1D_PLP_parameters->L1D_plp_scrambler_type = block_Read_uint8_bitlen(block, 2);

            //4 bits
            L1D_PLP_parameters->L1D_plp_fec_type = block_Read_uint8_bitlen(block, 4);

			if (L1D_PLP_parameters->L1D_plp_fec_type >= 0 && L1D_PLP_parameters->L1D_plp_fec_type <= 5) {
				//4 bits
				L1D_PLP_parameters->L1D_plp_mod = block_Read_uint8_bitlen(block, 4);
				//4 bits
				L1D_PLP_parameters->L1D_plp_cod = block_Read_uint8_bitlen(block, 4);
				//2 bits
            }
			
			L1D_PLP_parameters->L1D_plp_TI_mode = block_Read_uint8_bitlen(block, 2);

            if (L1D_PLP_parameters->L1D_plp_TI_mode == 0x0) {
				//15 bits
				L1D_PLP_parameters->L1D_plp_fec_block_start = block_Read_uint16_bitlen(block, 15);
            } else if (L1D_PLP_parameters->L1D_plp_TI_mode == 0x1) {
				//22 bits
				L1D_PLP_parameters->L1D_plp_CTI_fec_block_start = block_Read_uint32_bitlen(block, 22);
            }
            
            if (atsc3_preamble_packet->L1_detail_signaling.L1D_num_rf > 0) {
				//3 bits
				L1D_PLP_parameters->L1D_plp_num_channel_bonded = block_Read_uint8_bitlen(block, 3);
				
                if (L1D_PLP_parameters->L1D_plp_num_channel_bonded > 0) {
					//2 bits
					L1D_PLP_parameters->L1D_plp_channel_bonding_format = block_Read_uint8_bitlen(block, 2);
                    
					for (int k=0; k <= L1D_PLP_parameters->L1D_plp_num_channel_bonded; k++) {
						L1D_plp_bonded_rf_id_t* L1D_plp_bonded_rf_id = L1D_plp_bonded_rf_id_new();
						//3 bits
						L1D_plp_bonded_rf_id->L1D_plp_bonded_rf_id = block_Read_uint8_bitlen(block, 2);
						L1D_PLP_parameters_add_L1D_plp_bonded_rf_id(L1D_PLP_parameters, L1D_plp_bonded_rf_id);
                    }
                }
            }
			
			
            if ((i == 0 && atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_mimo == 1) ||
				(i > 0 && L1D_subframe_parameters->L1D_mimo == 1)) {
                //1 bit
				L1D_PLP_parameters->L1D_plp_mimo_stream_combining = block_Read_uint8_bitlen(block, 1);
                //1 bit
                L1D_PLP_parameters->L1D_plp_mimo_IQ_interleaving = block_Read_uint8_bitlen(block, 1);
                //1 bit
				L1D_PLP_parameters->L1D_plp_mimo_PH = block_Read_uint8_bitlen(block, 1);
            }
			
			
            if (L1D_PLP_parameters->L1D_plp_layer == 0) {
				//1 bit
                L1D_PLP_parameters->L1D_plp_type = block_Read_uint8_bitlen(block, 1);

                if (L1D_PLP_parameters->L1D_plp_type == 1) {
					//14
                    L1D_PLP_parameters->L1D_plp_num_subslices = block_Read_uint16_bitlen(block, 14);
					//24
                    L1D_PLP_parameters->L1D_plp_subslice_interval = block_Read_uint32_bitlen(block, 24);
                }
				
                if (((L1D_PLP_parameters->L1D_plp_TI_mode == 0x1) ||
					 (L1D_PLP_parameters->L1D_plp_TI_mode == 0x2)) && (L1D_PLP_parameters->L1D_plp_mod == 0x0)) {
					//1 bit
                    L1D_PLP_parameters->L1D_plp_TI_extended_interleaving = block_Read_uint8_bitlen(block, 1);
                }

				if (L1D_PLP_parameters->L1D_plp_TI_mode == 0x1) {
					//3
                    L1D_PLP_parameters->L1D_plp_CTI_depth = block_Read_uint8_bitlen(block, 3);

					//11
                    L1D_PLP_parameters->L1D_plp_CTI_start_row = block_Read_uint16_bitlen(block, 11);

				} else if (L1D_PLP_parameters->L1D_plp_TI_mode == 0x2) {
					//1
					L1D_PLP_parameters->L1D_plp_HTI_inter_subframe = block_Read_uint8_bitlen(block, 1);
					//4 bits
					L1D_PLP_parameters->L1D_plp_HTI_num_ti_blocks = block_Read_uint8_bitlen(block, 4);
					//12
					L1D_PLP_parameters->L1D_plp_HTI_num_fec_blocks_max = block_Read_uint16_bitlen(block, 12);
				}
				
				if (L1D_PLP_parameters->L1D_plp_HTI_inter_subframe == 0) {
					//12
					L1D_PLP_parameters->L1D_plp_HTI_num_fec_blocks = block_Read_uint16_bitlen(block, 12);
				} else {
					for (int k=0; k <= L1D_PLP_parameters->L1D_plp_HTI_num_ti_blocks; k++) {
						//fix me
						L1D_plp_HTI_num_fec_blocks_t* L1D_plp_HTI_num_fec_blocks = L1D_plp_HTI_num_fec_blocks_new();
						
						//12
						L1D_plp_HTI_num_fec_blocks->L1D_plp_HTI_num_fec_blocks = block_Read_uint16_bitlen(block, 12);
						L1D_PLP_parameters_add_L1D_plp_HTI_num_fec_blocks(L1D_PLP_parameters, L1D_plp_HTI_num_fec_blocks);
					}
				}
				//1 bit
				L1D_PLP_parameters->L1D_plp_HTI_cell_interleaver = block_Read_uint8_bitlen(block, 1);

            } else {
				//5 bits
				L1D_PLP_parameters->L1D_plp_ldm_injection_level = block_Read_uint8_bitlen(block, 5);
            }

			//
			L1D_subframe_parameters_add_L1D_PLP_parameters(L1D_subframe_parameters, L1D_PLP_parameters);
			
        }

		L1_detail_signaling_add_L1D_subframe_parameters(&atsc3_preamble_packet->L1_detail_signaling, L1D_subframe_parameters);
    }

	//16 bits
    atsc3_preamble_packet->L1_detail_signaling.L1D_bsid = block_Read_uint16_bitlen(block, 16);

    //as needed...
	//L1D_reserved

    atsc3_preamble_packet->L1_detail_signaling.L1D_crc = block_Read_uint32_bitlen(block, 32);

    
    
    
    __STLTP_PARSER_DEBUG("preamble: L1B_parsed: consumed %d bytes, leaving %d bytes for L1D (payload_start: %p, binary_payload: %p",
                        bytes_processed, bytes_remaining,
                        binary_payload_start, binary_payload);
    
    
    return atsc3_preamble_packet;
    
cleanup:
    if(atsc3_preamble_packet) {
        free(atsc3_preamble_packet);
        atsc3_preamble_packet = NULL;
    }
    
    return NULL;
}


void atsc3_timing_management_packet_dump(atsc3_timing_management_packet_t* atsc3_timing_management_packet) {
	__STLTP_PARSER_DUMP("---------");
	//atsc3_timing_management_packet
	__STLTP_PARSER_DUMP("timing_management: seconds_pre: 0x%06x, a_milli_pre: 0x%04x, length: %d, version_major: %d (0x%01x), verion_minor: %d (0x%01x), maj_log_rep_cnt_pre: %d, maj_log_rep_cnt_tim: %d",
			atsc3_timing_management_packet->bootstrap_timing_data_timestamp_short_reference.seconds_pre,
			atsc3_timing_management_packet->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre,
			atsc3_timing_management_packet->length,
			atsc3_timing_management_packet->version_major,
			atsc3_timing_management_packet->version_major,
			atsc3_timing_management_packet->version_minor,
			atsc3_timing_management_packet->version_minor,
			atsc3_timing_management_packet->maj_log_rep_cnt_pre,
			atsc3_timing_management_packet->maj_log_rep_cnt_tim
			);

	__STLTP_PARSER_DUMP("timing_management: bootstrap_major: %d (0x%01x), bootstrap_minor: %d (0x%01x), min_time_to_next: %d (0x%01x), system_bandwidth: %d, bsr_coefficient: %d",
				atsc3_timing_management_packet->bootstrap_major,
				atsc3_timing_management_packet->bootstrap_major,
				atsc3_timing_management_packet->bootstrap_minor,
				atsc3_timing_management_packet->bootstrap_minor,
				atsc3_timing_management_packet->min_time_to_next,
				atsc3_timing_management_packet->min_time_to_next,
				atsc3_timing_management_packet->system_bandwidth,
				atsc3_timing_management_packet->bsr_coefficient
				);

	__STLTP_PARSER_DUMP("timing_management: preamble_structure: 0x%01x, ea_wakeup: 0x%01x, num_emission_tim: 0x%01x, num_xmtrs_in_group: 0x%01x, xmtr_group_num: 0x%01x, maj_log_override: 0x%01x, num_miso_filt_codes: 0x%01x, tx_carrier_offset: 0x%01x",
				atsc3_timing_management_packet->preamble_structure,
				atsc3_timing_management_packet->ea_wakeup,
				atsc3_timing_management_packet->num_emission_tim,
				atsc3_timing_management_packet->num_xmtrs_in_group,
				atsc3_timing_management_packet->xmtr_group_num,
				atsc3_timing_management_packet->maj_log_override,
				atsc3_timing_management_packet->num_miso_filt_codes,
				atsc3_timing_management_packet->tx_carrier_offset
				);

	//todo: atsc3_stltp_parser.c    :1606:DUMP :1598929905.8389:timing_management: atsc3_bootstrap_timing_data: entry: 0, seconds: 1598917415, nanoseconds: 295144111

	for(int i=0; i < atsc3_timing_management_packet->atsc3_bootstrap_timing_data_v.count; i++) {
		atsc3_bootstrap_timing_data_t* atsc3_bootstrap_timing_data = atsc3_timing_management_packet->atsc3_bootstrap_timing_data_v.data[i];
		__STLTP_PARSER_DUMP("timing_management: atsc3_bootstrap_timing_data: entry: %d, seconds: %d, nanoseconds: %d, seconds_pre: 0x%06x, a_milli_pre: 0x%04x",
				i,
				atsc3_bootstrap_timing_data->seconds,
				atsc3_bootstrap_timing_data->nanoseconds,
				atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.seconds_pre,
				atsc3_bootstrap_timing_data->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre
		);

	}

	for(int i=0; i < atsc3_timing_management_packet->atsc3_per_transmitter_data_v.count; i++) {
			atsc3_per_transmitter_data_t* atsc3_per_transmitter_data = atsc3_timing_management_packet->atsc3_per_transmitter_data_v.data[i];
			__STLTP_PARSER_DUMP("timing_management: atsc3_per_transmitter_data: entry: %d, xmtr_id: 0x%04x, tx_time_offset: 0x%04x (%0.1f uS), txid_injection_lvl: 0x%02x, miso_filt_code: 0x%02x",
					i,
					atsc3_per_transmitter_data->xmtr_id,
					atsc3_per_transmitter_data->tx_time_offset,
                    ((int16_t) atsc3_per_transmitter_data->tx_time_offset) / 10.0,
					atsc3_per_transmitter_data->txid_injection_lvl,
					atsc3_per_transmitter_data->miso_filt_code_index
			);

	}
	__STLTP_PARSER_DUMP("timing_management: pkt_rls_seconds: %02d.%09d (a-milliseconds: %4d, 0x%04x), error_check_data.crc16: 0x%02x",
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_seconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_computed_milliseconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds,
                        atsc3_timing_management_packet->packet_release_time.pkt_rls_a_milliseconds,
						atsc3_timing_management_packet->error_check_data.crc16);

}

void atsc3_baseband_packet_dump(atsc3_baseband_packet_t* atsc3_baseband_packet) {
	__STLTP_PARSER_DUMP("---------");

    __STLTP_PARSER_DUMP("baseband: PLP: %d, seconds_pre: 0x%06x, a_milli_pre: 0x%04x, base_field_mode: 0x%x, base field pointer: 0x%02x, option_field_mode: 0x%01x, ext_type: 0x%01x, ext_len: 0x%02x, extension: 0x%04x",
    		atsc3_baseband_packet->plp_num,
			atsc3_baseband_packet->bootstrap_timing_data_timestamp_short_reference.seconds_pre,
			atsc3_baseband_packet->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre,
			atsc3_baseband_packet->base_field_mode,
			atsc3_baseband_packet->base_field_pointer,
			atsc3_baseband_packet->option_field_mode,
			atsc3_baseband_packet->ext_type,
			atsc3_baseband_packet->ext_len,
			atsc3_baseband_packet->extension);

}

void atsc3_preamble_packet_dump(atsc3_preamble_packet_t* atsc3_preamble_packet) {
    
	__STLTP_PARSER_DUMP("---------");

    __STLTP_PARSER_DUMP("preamble: seconds_pre: 0x%06x, a_milli_pre: 0x%04x, L1B: version: %d, mimo: %d, lls_flag: %d, time_info: %d, return_channel: %d, papr_reduction: %d, frame_length mode: %d",
    		atsc3_preamble_packet->bootstrap_timing_data_timestamp_short_reference.seconds_pre,
			atsc3_preamble_packet->bootstrap_timing_data_timestamp_short_reference.a_milliseconds_pre,
    		atsc3_preamble_packet->L1_basic_signaling.L1B_version,
			atsc3_preamble_packet->L1_basic_signaling.L1B_mimo_scattered_pilot_encoding,
			atsc3_preamble_packet->L1_basic_signaling.L1B_lls_flag,
			atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag,
			atsc3_preamble_packet->L1_basic_signaling.L1B_return_channel_flag,
			atsc3_preamble_packet->L1_basic_signaling.L1B_papr_reduction,
			atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length_mode
    );

    if(atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length_mode == 0) {
    	__STLTP_PARSER_DUMP("preamble: L1B: frame length: %d, excess samples per symbol: %d",
    			atsc3_preamble_packet->L1_basic_signaling.L1B_frame_length,
				atsc3_preamble_packet->L1_basic_signaling.L1B_excess_samples_per_symbol);
    } else {
    	__STLTP_PARSER_DUMP("preamble: L1B: time offset: %d, additional samples: %d",
    			atsc3_preamble_packet->L1_basic_signaling.L1B_time_offset,
				atsc3_preamble_packet->L1_basic_signaling.L1B_additional_samples);
    }
    
    __STLTP_PARSER_DUMP("preamble: L1B: num subframes: %d, preamble num symbols: %d, preamble reduced carriers: %d, l1_detail content tag: %d, l1_detail size bytes: %d",
    		atsc3_preamble_packet->L1_basic_signaling.L1B_num_subframes,
			atsc3_preamble_packet->L1_basic_signaling.L1B_preamble_num_symbols,
			atsc3_preamble_packet->L1_basic_signaling.L1B_preamble_reduced_carriers,
			atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_content_tag,
			atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_size_bytes
    );
    
    __STLTP_PARSER_DUMP("preamble: L1B: l1 detail fec type: %d, additional parity mode: %d, l1d total cells: %d",
    		atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_fec_type,
			atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_additional_parity_mode,
			atsc3_preamble_packet->L1_basic_signaling.L1B_L1_Detail_total_cells
    );
    
    __STLTP_PARSER_DUMP("preamble: L1B: l1b first sub mimo: %d, first sub miso: %d, first sub fft size: %d, first sub reduced carriers: %d, first sub guard interval: %d",
    		atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_mimo,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_miso,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_fft_size,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_reduced_carriers,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_guard_interval
    );
	
    __STLTP_PARSER_DUMP("preamble: L1B: first sub num ofdm symbols: %d, first sub scattered pilot pattern: %d, first sub scatterd pilot boost: %d, first sub sbs_first: %d, first sub sbs_last: %d",
    		atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_num_ofdm_symbols,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_scattered_pilot_pattern,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_scattered_pilot_boost,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_first,
			atsc3_preamble_packet->L1_basic_signaling.L1B_first_sub_sbs_last
    );
	
	
    __STLTP_PARSER_DUMP("preamble: L1D: version: %d, num_rf: %d, l1d_subframe_count: %d, time_info flag: %d",
    		atsc3_preamble_packet->L1_detail_signaling.L1D_version,
			atsc3_preamble_packet->L1_detail_signaling.L1D_num_rf,
			atsc3_preamble_packet->L1_detail_signaling.L1D_subframe_parameters_v.count,
			atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag
    );

	if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x0) {
		if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x01) {
            if(atsc3_preamble_packet->L1_basic_signaling.L1B_time_info_flag != 0x02) {
            	__STLTP_PARSER_DUMP("preamble: L1D: time: sec: %d, ms: %d, us: %d, ns: %d",
									atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_sec,
									atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_msec,
									atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_usec,
									atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_nsec
									);
			} else {
				__STLTP_PARSER_DUMP("preamble: L1D: time: sec: %d, ms: %d, us: %d",
												atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_sec,
												atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_msec,
												atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_usec
												);
			}
		} else {
			__STLTP_PARSER_DUMP("preamble: L1D: time: sec: %d, ms: %d",
											atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_sec,
											atsc3_preamble_packet->L1_detail_signaling.L1D_time_sec_block.L1D_time_msec
											);
		}
	}
	
	//jjustman-2020-01-22: TODO: iterate over L1D_subframe_parameters -> plp, etc
	
	for(int i=0; i < atsc3_preamble_packet->L1_detail_signaling.L1D_subframe_parameters_v.count; i++) {
		L1D_subframe_parameters_t* L1D_subframe_parameters = atsc3_preamble_packet->L1_detail_signaling.L1D_subframe_parameters_v.data[i];
		if(i > 0) {
			__STLTP_PARSER_DUMP("preamble: L1D: subframe: %d, L1D_mimo: %d, L1D_miso: %d, L1D_fft_size: %d, L1D_reduced_carriers: %d, L1D_guard_interval: %d, L1D_num_ofdm_symbols: %d",
								i,
								L1D_subframe_parameters->L1D_mimo,
								L1D_subframe_parameters->L1D_miso,
								L1D_subframe_parameters->L1D_fft_size,
								L1D_subframe_parameters->L1D_reduced_carriers,
								L1D_subframe_parameters->L1D_guard_interval,
								L1D_subframe_parameters->L1D_num_ofdm_symbols);
			
			__STLTP_PARSER_INFO("preamble: L1D: subframe: %d, L1D_scattered_pilot_pattern: %d, L1D_scattered_pilot_boost: %d, L1D_sbs_first: %d, L1D_sbs_last: %d, L1D_subframe_multiplex: %d",
								i,
								L1D_subframe_parameters->L1D_scattered_pilot_pattern,
								L1D_subframe_parameters->L1D_scattered_pilot_boost,
								L1D_subframe_parameters->L1D_sbs_first,
								L1D_subframe_parameters->L1D_sbs_last,
								L1D_subframe_parameters->L1D_subframe_multiplex);
		}
		
		__STLTP_PARSER_DUMP("preamble: L1D: subframe: %d, L1D_frequency_interleaver: %d, L1D_sbs_null_cells: %d, num_plp: %d ",
							i,
							L1D_subframe_parameters->L1D_frequency_interleaver,
							L1D_subframe_parameters->L1D_sbs_null_cells,
							L1D_subframe_parameters->L1D_num_plp);
		
		for(int k=0; k < L1D_subframe_parameters->L1D_PLP_parameters_v.count; k++) {
			L1D_PLP_parameters_t* L1D_PLP_parameters = L1D_subframe_parameters->L1D_PLP_parameters_v.data[k];
			
			__STLTP_PARSER_DUMP("preamble: L1D: plp: id: %d, lls_flag: %d, layer: %d, start: %d, size: %d, scrambler: %d, fec: %d, mod: %d, cod: %d",
								L1D_PLP_parameters->L1D_plp_id,
								L1D_PLP_parameters->L1D_plp_lls_flag,
								L1D_PLP_parameters->L1D_plp_layer,
								L1D_PLP_parameters->L1D_plp_start,
								L1D_PLP_parameters->L1D_plp_size,
								L1D_PLP_parameters->L1D_plp_scrambler_type,
								L1D_PLP_parameters->L1D_plp_fec_type,
								L1D_PLP_parameters->L1D_plp_mod,
								L1D_PLP_parameters->L1D_plp_cod);
			
			__STLTP_PARSER_DUMP("preamble: L1D: plp: id: %d, L1D_plp_TI_mode: %d, L1D_plp_type: %d, L1D_plp_num_subslices: %d, L1D_plp_subslice_interval: %d, L1D_plp_ldm_injection_level: %d",
								L1D_PLP_parameters->L1D_plp_id,
								L1D_PLP_parameters->L1D_plp_TI_mode,
								L1D_PLP_parameters->L1D_plp_type,
													
								L1D_PLP_parameters->L1D_plp_num_subslices,
								L1D_PLP_parameters->L1D_plp_subslice_interval,
													
								L1D_PLP_parameters->L1D_plp_ldm_injection_level);
		}
	}
	
    
	__STLTP_PARSER_DUMP("preamble: L1D: bsid: %d", atsc3_preamble_packet->L1_detail_signaling.L1D_bsid);
}
