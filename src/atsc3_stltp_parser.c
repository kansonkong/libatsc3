/*
 * atsc3_stltp_parser.c
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

int _STLTP_PARSER_DEBUG_ENABLED = 1;


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
            block_Seek(ip_udp_rtp_packet_outer->data, ip_udp_rtp_packet_outer->rtp_header->packet_offset); //move our outer to the marker position
            
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
    inner_packet_data = block_Refcount(ip_udp_rtp_packet_outer->data); //return a reference
    
    return inner_packet_data;
}


/*
 only use this for re-fragmentation parsing, which block_t is duplicated (not original pointer)
 */
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {

	__STLTP_PARSER_DEBUG("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data: parsing udp_packet_inner: %p, to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data->i_pos);

    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner_new = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner->data);
    
    if(!ip_udp_rtp_packet_inner_new) {
        __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data: unable to parse inner packet");
        return NULL;
    }
    
    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    }
    
    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = ip_udp_rtp_packet_inner_new;

	return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
}


atsc3_ip_udp_rtp_packet_t* atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    
    __STLTP_PARSER_DEBUG("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data: parsing ip_udp_rtp_packet_outer: %p, to: %u", atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer, atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data->i_pos);
    
    //todo - refactor to not create a duplicate data block_t
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner_new = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);
    
    if(!ip_udp_rtp_packet_inner_new) {
        __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data: unable to parse inner packet");
        return NULL;
    }
    
    if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    }
    
    ip_udp_rtp_packet_inner_new->data = block_Refcount(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer->data);
    
    atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner = ip_udp_rtp_packet_inner_new;
    
    return atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner;
}

/*
 invoked from main pcap listener
 */

atsc3_stltp_tunnel_packet_t* atsc3_stltp_raw_packet_extract_inner_from_outer_packet(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_last) {
	__STLTP_PARSER_DEBUG(" ----atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet, size: %u, pkt: %p-----",
                         ip_udp_rtp_packet->data->p_size,
                         ip_udp_rtp_packet->data);

    
    atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current = calloc(1, sizeof(atsc3_stltp_tunnel_packet_t));
    
    //rewind raw packet buffer to outer packet
    block_Rewind(ip_udp_rtp_packet->data);
	atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer = atsc3_ip_udp_rtp_packet_duplicate(ip_udp_rtp_packet);

	if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: atsc3_stltp_tunnel_packet_current->udp_packet_outer is null");
		return NULL;
	}
    
 	atsc3_rtp_header_dump_outer(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header);

    //make sure our outer packet is type: 97 - tunnel packet
	if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header || atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->payload_type != ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL) {
		__STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: unknown outer tunnel packet: %u", (atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header ? atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->payload_type : -1));
		return NULL;
	}
    
    bool processed_refragmentation_or_concatenation_tunnel_packet = false;
    
    //for recovering refragmentation of prior frame due to being short for ip_udp_rtp header parsing
    if(atsc3_stltp_tunnel_packet_last && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer) {
        
        //first, try and parse any remaining bytes in atsc3_stltp_tunnel_packet_last that did not get an inner packet started
        int last_outer_packet_bytes_remaining_to_parse = block_Remaining_size(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->data);
        if(last_outer_packet_bytes_remaining_to_parse > 0) {
            __STLTP_PARSER_INFO("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment of len: %u", last_outer_packet_bytes_remaining_to_parse);
            //check to make sure our last outer sequence number == current sequence number +1
            
            atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer); //this will create our new inner packet from the remaining fragment from our previous packet
            
            //read from outer packet data, either up to marker or the full payload
            block_t* inner_payload_current = atsc3_stltp_read_from_outer_packet(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, false);
            
            if(inner_payload_current) {
                block_Merge(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data, inner_payload_current);
                block_Release(&inner_payload_current);
            } else {
                //todo: figure out what to do here
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet from last outer position failed");
                block_Release(&inner_payload_current);

                return NULL;
            }
            
            //todo: refactor this common pattern out here
            atsc3_ip_udp_rtp_packet_t* inner_packet = atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_inner_data(atsc3_stltp_tunnel_packet_current);
            
            if(!inner_packet) {
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment failed, last len: %u, current rebuilt len: %u",
                                    last_outer_packet_bytes_remaining_to_parse,
                                    atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);
                return NULL;
            }
            
            __STLTP_PARSER_INFO("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation from previous packet fragment of len: %u, fragment payload type: %u, merged length is: %u",
                                last_outer_packet_bytes_remaining_to_parse,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);
            
            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);
            
            if(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header &&
               (atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header->sequence_number - 1 == atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->sequence_number)) {
                
                atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
                processed_refragmentation_or_concatenation_tunnel_packet = true;
                //todo: end - refactor this common pattern out here
            } else {
                //sequence gap
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: re-fragmentation sequence gap, from: %u to %u",
                                     atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_outer->rtp_header->sequence_number,
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->sequence_number);
                
                
            }
            
            //refcount?
            atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_last);

        }
    }
    
    //for concatenation of prior tunneled packet that is not yet complete
    if(atsc3_stltp_tunnel_packet_last && atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_inner) {
        //check to make sure our last outer sequence number == current sequence number +1

        //seek past the outer packet header data, as we are re-using our inner ip_udp_rtp header from our last inner packet
        block_Seek(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data, ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE);
        
        //map any pending packets over to our packet_current
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_pending = atsc3_stltp_tunnel_packet_last->atsc3_stltp_baseband_packet_pending;
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_baseband_packet_pending = NULL;
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = atsc3_stltp_tunnel_packet_last->atsc3_stltp_preamble_packet_pending;
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_preamble_packet_pending = NULL;
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = atsc3_stltp_tunnel_packet_last->atsc3_stltp_timing_management_packet_pending;
        atsc3_stltp_tunnel_packet_last->atsc3_stltp_timing_management_packet_pending = NULL;
        
        
        //duplicate our ip_udp_rtp header so we won't get a doublefree
        atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate_no_data_block_t(atsc3_stltp_tunnel_packet_last->ip_udp_rtp_packet_inner);
        atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data = block_Refcount(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data);
        
        int current_outer_packet_bytes_remaining_to_parse = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data);
        if(current_outer_packet_bytes_remaining_to_parse > 0) {
            __STLTP_PARSER_INFO("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: concatenation from previous packet fragment:");
            
            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);
            
            atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
            processed_refragmentation_or_concatenation_tunnel_packet = true;
        }
    }
    
    atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_last);
    
    //slightly messy
    if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker) {
        if(processed_refragmentation_or_concatenation_tunnel_packet) {
            __STLTP_PARSER_INFO("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: processed_refragmentation_or_concatenation_tunnel_packet - clearing outer only");

        //    atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_current); //clear outer if we re-concatentated but don't have a marker
        } else {
            __STLTP_PARSER_WARN("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: no refragmentation/concatention and no marker, bailing!");
        }
        atsc3_stltp_tunnel_packet_free(&atsc3_stltp_tunnel_packet_current); //don't carry over if we don't have a marker

        return atsc3_stltp_tunnel_packet_current;
    }
    
    //if we don't have an inner payload yet due to marker, then we still need to parse 2x headers
    while(block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data) >
          (atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->marker && atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->rtp_header->packet_offset > atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos ? ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE * 2 : ATSC_STLTP_IP_UDP_RTP_HEADER_SIZE)) {
        //read from outer packet data,
        block_t* outer_reference_inner_payload_current = atsc3_stltp_read_from_outer_packet(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer, true);
        if(outer_reference_inner_payload_current) {
            
            atsc3_ip_udp_rtp_packet_t* inner_packet = atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header_outer_data(atsc3_stltp_tunnel_packet_current);
            if(!inner_packet) {
                __STLTP_PARSER_ERROR("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: processing inner loop packet fragment failed, pos: %u,  len: %u",
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->i_pos,
                                     atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data->p_size);
                return NULL;
            }
            
            __STLTP_PARSER_INFO("atsc3_stltp_tunnel_packet_extract_inner_from_outer_packet: processing inner loop packet fragment of len: %u, fragment payload type: %u, packet length is: %u",
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->i_pos,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->payload_type,
                                atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);
           
            atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);
            
            bool last_inner_packet_complete = atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_current);
            block_Release(&outer_reference_inner_payload_current);
        }
    }
    
    if(block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data)  > 0) {
        __STLTP_PARSER_INFO("block remaining size: %u, inner: %p", block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_outer->data), atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
    }
    
    return atsc3_stltp_tunnel_packet_current;
}

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
 **/

//atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_header_t* atsc3_rtp_header_tunnel) {
                                            
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {

    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_pending;
    
	if(!atsc3_stltp_baseband_packet_pending) {
        if(!atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker) {
            __STLTP_PARSER_ERROR("atsc3_stltp_baseband_packet_extract: no pending baseband packet and no marker, can't compute size!");
            return NULL;
        }

        atsc3_stltp_baseband_packet_pending = calloc(1, sizeof(atsc3_stltp_baseband_packet_t));
        atsc3_stltp_baseband_packet_pending->ip_udp_rtp_packet = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
        atsc3_stltp_baseband_packet_pending->rtp_header = atsc3_rtp_header_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);
        
        atsc3_stltp_baseband_packet_pending->payload_length = atsc3_stltp_baseband_packet_pending->rtp_header->packet_offset;
        
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_pending = atsc3_stltp_baseband_packet_pending;
	}

    block_t* packet = block_Refcount(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);
    uint32_t block_remaining_length = block_Remaining_size(packet);
    
    if(block_remaining_length + atsc3_stltp_baseband_packet_pending->payload_offset > atsc3_stltp_baseband_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_baseband_packet_pending->payload_length - atsc3_stltp_baseband_packet_pending->payload_offset);
    }
   
	if(atsc3_stltp_baseband_packet_pending->rtp_header->marker && !atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_pending->fragment_count) {
		/**
		ATSC A/324:2018 - Section 8.3
		When the marker (M) bit is zero ‘0’, the Synchronization Source (SSRC) Identifier shall be set to zero
		‘0’. When the marker (M) bit is set to one ‘1’, indicating the first packet of the BPPS, the SSRC
		field will contain the total length of the Baseband Packet data structure in bytes. This allows
		the Data Consumer to know how much data is to be delivered within the payloads of the BPPS.
		 */

		uint32_t baseband_header_packet_length = atsc3_stltp_baseband_packet_pending->rtp_header->packet_offset; //SSRC packet length
		assert(baseband_header_packet_length < 65535);
		atsc3_stltp_baseband_packet_pending->payload_length = baseband_header_packet_length;

		__STLTP_PARSER_DEBUG(" ----baseband packet: new -----");
		__STLTP_PARSER_DEBUG("     total packet length:  %u",  atsc3_stltp_baseband_packet_pending->payload_length);
        __STLTP_PARSER_DEBUG("     fragment 0 length:    %u (payload: %p)",  block_remaining_length, atsc3_stltp_baseband_packet_pending->payload);
        
        atsc3_stltp_baseband_packet_pending->payload = calloc(baseband_header_packet_length, sizeof(uint8_t));
        assert(atsc3_stltp_baseband_packet_pending->payload);


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
		__STLTP_PARSER_DEBUG(" ----baseband packet: complete-----");
		atsc3_stltp_baseband_packet_pending->is_complete = true;
		atsc3_stltp_tunnel_packet_add_atsc3_stltp_baseband_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_baseband_packet_pending);
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_baseband_packet_pending = NULL;
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner); //clear our inner rtp header reference when complete
	}
           
	block_Seek_Relative(packet, block_remaining_length);
    __STLTP_PARSER_DEBUG(" ----baseband packet: return, position is: %u, size: %u -----", packet->i_pos, packet->p_size);
    block_Release(&packet);
    
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
    
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker && atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending) {
        //force clear out pending packet
        __STLTP_PARSER_ERROR("atsc3_stltp_preamble_packet_extract: force clearing preamble pending packet due to inner marker");

        free(atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending);
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = NULL;
    }
    
	atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending;
	if(!atsc3_stltp_preamble_packet_pending) {
		atsc3_stltp_preamble_packet_pending = calloc(1, sizeof(atsc3_stltp_preamble_packet_t));
		atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = atsc3_stltp_preamble_packet_pending;
	}

    atsc3_stltp_preamble_packet_pending->rtp_header = atsc3_rtp_header_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);
    block_t* packet = block_Refcount(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);

	if(atsc3_stltp_preamble_packet_pending->rtp_header->marker && !atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending->fragment_count) {
		//read the first uint16_t for our preamble length

        //The length field shall contain the number of bytes in the Preamble Payload data structure following the length field excluding the crc16 bytes
		atsc3_stltp_preamble_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet)))) + 2 + 2; //extra +2 is for the crc16 not included in the length field
		__STLTP_PARSER_DEBUG(" ----preamble packet: new -----");
        __STLTP_PARSER_DEBUG("     preamble length:    %u (payload: %p)",  atsc3_stltp_preamble_packet_pending->payload_length, atsc3_stltp_preamble_packet_pending->payload);
        
		atsc3_stltp_preamble_packet_pending->payload = calloc(atsc3_stltp_preamble_packet_pending->payload_length, sizeof(uint8_t));
	}
    
    uint32_t block_remaining_length = block_Remaining_size(packet);
    
    if(block_remaining_length + atsc3_stltp_preamble_packet_pending->payload_offset > atsc3_stltp_preamble_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_preamble_packet_pending->payload_length - atsc3_stltp_preamble_packet_pending->payload_offset);
    }
    __STLTP_PARSER_DEBUG("     fragment %u length:  %u",  atsc3_stltp_preamble_packet_pending->fragment_count, block_remaining_length);

    memcpy(&atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], block_Get(packet), block_remaining_length);
	__STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset], atsc3_stltp_preamble_packet_pending->payload[atsc3_stltp_preamble_packet_pending->payload_offset+1]);

    __STLTP_PARSER_DEBUG("      preamble_payload_length: %u", atsc3_stltp_preamble_packet_pending->payload_length);

    atsc3_stltp_preamble_packet_pending->fragment_count++;
    atsc3_stltp_preamble_packet_pending->payload_offset += block_remaining_length;

	if(atsc3_stltp_preamble_packet_pending->payload_offset >= atsc3_stltp_preamble_packet_pending->payload_length) {
		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----preamble packet: complete-----");
		atsc3_stltp_preamble_packet_pending->is_complete = true;
		atsc3_stltp_tunnel_packet_add_atsc3_stltp_preamble_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_preamble_packet_pending);

        atsc3_stltp_tunnel_packet_current->atsc3_stltp_preamble_packet_pending = NULL;
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner); //clear our inner rtp header reference when complete
	}

    block_Seek_Relative(packet, block_remaining_length);
    block_Release(&packet);
    
	return atsc3_stltp_preamble_packet_pending;
}

/*
 
 The payload data for each Timing and Management Stream RTP/UDP/IP packet shall be a fragment of the TMP() data structure described in Table 8.3. To provide validation that the TMP() structure is delivered correctly over the STL, a 16-bit cyclic redundancy check is provided as part of the TMP() data. The resultant stream of TMP() packets shall have IP destination address 239.0.51.48 and destination port 30065
 
 The RTP header fields of the TMP Packet Set shall be as described below, configured with the marker (M) bit of the packet containing the beginning of a TMP() data structure set to one ‘1’. The marker (M) bits of remaining packets shall be set to zero ‘0’. This allows the transmission system on the consumer end of the STL to reconstruct the TMP() data structure after any ordering correction takes place. The timestamps of the packets of a given TMP Packet Set shall have the same values. The timestamp values are derived from a subset of the Bootstrap_Timing_Data, providing a mechanism to uniquely associate each of the TMP packets with a specific Physical Layer frame.
 
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

atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {
    
    if(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header->marker && atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending) {
        //force clear out pending packet
        __STLTP_PARSER_ERROR("atsc3_stltp_timing_management_packet_extract: force clearing timing_management pending packet due to inner marker");

        free(atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending);
        atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = NULL;
    }
    
	atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_pending = atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending;
	if(!atsc3_stltp_timing_management_packet_pending) {
		atsc3_stltp_timing_management_packet_pending = calloc(1, sizeof(atsc3_stltp_timing_management_packet_t));
		atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = atsc3_stltp_timing_management_packet_pending;
	}

    atsc3_stltp_timing_management_packet_pending->rtp_header = atsc3_rtp_header_duplicate(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->rtp_header);

    block_t* packet = block_Refcount(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);

    if(atsc3_stltp_timing_management_packet_pending->rtp_header->marker && !atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending->fragment_count) {
		//read the first uint16_t for our preamble length

		atsc3_stltp_timing_management_packet_pending->payload_length = ntohs(*((uint16_t*)(block_Get(packet))));
		__STLTP_PARSER_DEBUG(" ----timing_management packet: new -----");
        __STLTP_PARSER_DEBUG("     preamble length:    %u (payload: %p)",  atsc3_stltp_timing_management_packet_pending->payload_length, atsc3_stltp_timing_management_packet_pending->payload);
		atsc3_stltp_timing_management_packet_pending->payload = calloc(atsc3_stltp_timing_management_packet_pending->payload_length, sizeof(uint8_t));
    } else {
        uint32_t tm_remaining_bytes = block_Remaining_size(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data);

        __STLTP_PARSER_ERROR(" ----timing_management packet: missing length due to no pending packet and no marker, reading to end of frame: len: %u, bytes remaining: %u -----", atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size, tm_remaining_bytes);
        
        block_Seek(atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner->data->p_size);
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner);
        block_Release(&packet);
        
        return NULL;
    }
    
    uint32_t block_remaining_length = block_Remaining_size(packet);

    if(block_remaining_length + atsc3_stltp_timing_management_packet_pending->payload_offset > atsc3_stltp_timing_management_packet_pending->payload_length ) {
        block_remaining_length = __MIN(block_remaining_length, atsc3_stltp_timing_management_packet_pending->payload_length - atsc3_stltp_timing_management_packet_pending->payload_offset);
    }
    __STLTP_PARSER_DEBUG("     fragment %u length:  %u",  atsc3_stltp_timing_management_packet_pending->fragment_count, block_remaining_length);
    
	memcpy(&atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], block_Get(packet), block_remaining_length);
    if(atsc3_stltp_timing_management_packet_pending->payload_length  > 1) {
        __STLTP_PARSER_DEBUG("     first bytes:         0x%02x 0x%02x", atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset], atsc3_stltp_timing_management_packet_pending->payload[atsc3_stltp_timing_management_packet_pending->payload_offset+1]);
    }
	__STLTP_PARSER_DEBUG("     payload_offset now: %u, preamble_payload_length: %u", atsc3_stltp_timing_management_packet_pending->payload_offset, atsc3_stltp_timing_management_packet_pending->payload_length);
    
    atsc3_stltp_timing_management_packet_pending->fragment_count++;
    atsc3_stltp_timing_management_packet_pending->payload_offset += block_remaining_length;

	if(atsc3_stltp_timing_management_packet_pending->payload_offset >= atsc3_stltp_timing_management_packet_pending->payload_length) {
		//process the preamble data structures
		__STLTP_PARSER_DEBUG(" ----timing_management packet: complete-----");
		atsc3_stltp_timing_management_packet_pending->is_complete = true;
		atsc3_stltp_tunnel_packet_add_atsc3_stltp_timing_management_packet(atsc3_stltp_tunnel_packet_current, atsc3_stltp_timing_management_packet_pending);

		atsc3_stltp_tunnel_packet_current->atsc3_stltp_timing_management_packet_pending = NULL;
        atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner); //clear our inner rtp header reference when complete
	}
    block_Seek_Relative(packet, block_remaining_length);
    block_Release(&packet);
    
	return atsc3_stltp_timing_management_packet_pending;
}

/**
 
 todo: 2019-03-16
 
 free any udp_packet reassembly payloads
 
 todo: 2019-07-23 - clear vector(s)
 
 **/

void atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {
    if(atsc3_stltp_tunnel_packet) {
        if(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_v.count) {
            for(int i=0; i < atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_v.count; i++) {
                atsc3_stltp_baseband_packet_free_v(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_v.data[i]);
            }
        	atsc3_stltp_tunnel_packet_clear_atsc3_stltp_baseband_packet(atsc3_stltp_tunnel_packet);
        }

        if(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_v.count) {
            for(int i=0; i < atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_v.count; i++) {
                atsc3_stltp_preamble_packet_free_v(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_v.data[i]);
            }
        	atsc3_stltp_tunnel_packet_clear_atsc3_stltp_preamble_packet(atsc3_stltp_tunnel_packet);
        }

        if(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_v.count) {
            for(int i=0; i < atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_v.count; i++) {
                atsc3_stltp_timing_management_packet_free_v(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_v.data[i]);
            }
        	atsc3_stltp_tunnel_packet_clear_atsc3_stltp_timing_management_packet(atsc3_stltp_tunnel_packet);
        }
    }
}


void atsc3_rtp_header_dump_outer(atsc3_rtp_header_t* atsc3_rtp_header) {
	__STLTP_PARSER_DEBUG(" ---outer---");
	atsc3_rtp_header_dump(atsc3_rtp_header, 1);

}

void atsc3_rtp_header_dump_inner(atsc3_rtp_header_t* atsc3_rtp_header) {
	__STLTP_PARSER_DEBUG("   ---inner---");
	atsc3_rtp_header_dump(atsc3_rtp_header, 3);
}
void atsc3_rtp_header_dump(atsc3_rtp_header_t* atsc3_rtp_header, int spaces) {

	__STLTP_PARSER_DEBUG("%*sversion:         %x", spaces, "", atsc3_rtp_header->version);
	__STLTP_PARSER_DEBUG("%*spadding:         %x", spaces, "", atsc3_rtp_header->padding);
	__STLTP_PARSER_DEBUG("%*sextension:       %x", spaces, "", atsc3_rtp_header->extension);
	__STLTP_PARSER_DEBUG("%*scsrc_count:      %x", spaces, "", atsc3_rtp_header->csrc_count);
	__STLTP_PARSER_DEBUG("%*smarker:          %x", spaces, "", atsc3_rtp_header->marker);
	__STLTP_PARSER_DEBUG("%*spayload_type:    0x%x (%hhu)", spaces, "", atsc3_rtp_header->payload_type, 	atsc3_rtp_header->payload_type);
	__STLTP_PARSER_DEBUG("%*ssequence_number: 0x%x (%u)", spaces, "", atsc3_rtp_header->sequence_number, atsc3_rtp_header->sequence_number);
	__STLTP_PARSER_DEBUG("%*stimestamp:       0x%x (%u)", spaces, "", atsc3_rtp_header->timestamp, 		atsc3_rtp_header->timestamp);
    if(atsc3_rtp_header->payload_type == 0x61) {
        __STLTP_PARSER_DEBUG("%*spacket_offset:   0x%x (%u)", spaces, "", atsc3_rtp_header->packet_offset, 	atsc3_rtp_header->packet_offset);
    } else {
        __STLTP_PARSER_DEBUG("%*spacket length:   0x%x (%u)", spaces, "", atsc3_rtp_header->packet_offset,     atsc3_rtp_header->packet_offset);
    }
}


/**
 free both inner and outer packets if inner/outer data block_t don't match (refragmentation or concatenation/segmentation),
 oterwise only block_release one _t
 **/

void atsc3_stltp_tunnel_packet_free(atsc3_stltp_tunnel_packet_t** atsc3_stltp_tunnel_packet_p) {
    if(atsc3_stltp_tunnel_packet_p) {
        atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet = *atsc3_stltp_tunnel_packet_p;
        if(atsc3_stltp_tunnel_packet) {
            if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner) {
                atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
            }
            if(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer) {
                atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_outer);
            } 
            
            if(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_v.data) {
                free(atsc3_stltp_tunnel_packet->atsc3_stltp_baseband_packet_v.data);
            }
            if(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_v.data) {
                free(atsc3_stltp_tunnel_packet->atsc3_stltp_preamble_packet_v.data);
            }
            if(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_v.data) {
                free(atsc3_stltp_tunnel_packet->atsc3_stltp_timing_management_packet_v.data);
            }

            atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet);
            free(atsc3_stltp_tunnel_packet);
            atsc3_stltp_tunnel_packet = NULL;
        }
        *atsc3_stltp_tunnel_packet_p = NULL;
    }
}
