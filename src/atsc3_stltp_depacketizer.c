/*
 * atsc3_stltp_depacketizer.c
 *
 *  Created on: Aug 11, 2020
 *      Author: jjustman
 */

#include "atsc3_stltp_depacketizer.h"

int _ATSC3_STLTP_DEPACKETIZER_INFO_ENABLED = 0;
int _ATSC3_STLTP_DEPACKETIZER_DEBUG_ENABLED = 0;
int _ATSC3_STLTP_DEPACKETIZER_TRACE_ENABLED = 0;


atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context_new() {

	atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = calloc(1, sizeof(atsc3_stltp_depacketizer_context_t));

	atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();

	return atsc3_stltp_depacketizer_context;
}

void atsc3_stltp_depacketizer_context_free(atsc3_stltp_depacketizer_context_t** atsc3_stltp_depacketizer_context_p) {
	if(atsc3_stltp_depacketizer_context_p) {
		atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = *atsc3_stltp_depacketizer_context_p;
		if(atsc3_stltp_depacketizer_context) {

			//cleanup
			if(atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed) {
				atsc3_stltp_tunnel_packet_destroy(&atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed);
			}

			if(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection) {
				//atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection
				atsc3_alp_packet_collection_free_atsc3_alp_packet(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection);
				atsc3_alp_packet_collection_free_atsc3_baseband_packet(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection);
				if(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection->atsc3_alp_packet_pending) {
					atsc3_alp_packet_free(&atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection->atsc3_alp_packet_pending);
				}
			}

			free(atsc3_stltp_depacketizer_context);
			atsc3_stltp_depacketizer_context = NULL;

		}
		*atsc3_stltp_depacketizer_context_p = NULL;
	}

}




//TODO - add SMPTE-2022.1 FEC decoding (see fork of prompeg-decoder - https://github.com/jjustman/prompeg-decoder)


void atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context) {
	atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_processed = NULL;

	_ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_stltp_depacketizer_from_ip_udp_rtp_packet: packet: %p, pcap len: %d", ip_udp_rtp_packet, ip_udp_rtp_packet->data->p_size);

	atsc3_alp_packet_collection_t* atsc3_alp_packet_collection = atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection;

    //dispatch for STLTP decoding and reflection
    if(ip_udp_rtp_packet->udp_flow.dst_ip_addr == atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr && ip_udp_rtp_packet->udp_flow.dst_port == atsc3_stltp_depacketizer_context->destination_flow_filter.dst_port) {
    	atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed = atsc3_stltp_raw_packet_extract_inner_from_outer_packet(ip_udp_rtp_packet, atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed);
    	atsc3_stltp_tunnel_packet_processed = atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed;

        if(!atsc3_stltp_tunnel_packet_processed) {
            __ERROR("process_packet: atsc3_stltp_tunnel_packet_processed is null, error processing packet: %p, size: %u",  ip_udp_rtp_packet, ip_udp_rtp_packet->data->p_size);
            goto cleanup;
        }

        if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count) {
        	_ATSC3_STLTP_DEPACKETIZER_DEBUG(">>>stltp atsc3_stltp_baseband_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count);

            //TODO: jjustman-2019-08-09 refactor stltp baseband to alp processing logic out

            for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count; i++) {
                atsc3_alp_packet_t* atsc3_alp_packet = NULL;
                atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.data[i];
                _ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_baseband_packet: sequence_num: %d, port: %d",
                       atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                       atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);

				if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port != atsc3_stltp_depacketizer_context->inner_rtp_port_filter) {
					_ATSC3_STLTP_DEPACKETIZER_DEBUG("ignorning stltp_baseband_packet port: %d",  atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);
                    //atsc3_stltp_baseband_packet_free(&atsc3_stltp_baseband_packet);
                    continue;
                }

                //make sure we get a packet back, base field pointer (13b) : 0x1FFF (8191 bytes) will return NULL
                atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_stltp_parse_baseband_packet(atsc3_stltp_baseband_packet);
                if(!atsc3_baseband_packet) {
                	_ATSC3_STLTP_DEPACKETIZER_DEBUG("no baseband packet returned, ^^^ should be only padding");
                }

                if(atsc3_baseband_packet) {
                    atsc3_alp_packet_collection_add_atsc3_baseband_packet(atsc3_alp_packet_collection, atsc3_baseband_packet);

                    //hack to carry over 1 (or N) byte payload(s) that is too small from our last run...trumps pending packets
                    if(atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment) {
                        if(atsc3_baseband_packet->alp_payload_pre_pointer) {
                            __WARN("atsc3_baseband_packet: carry over: atsc3_baseband_packet_short_fragment: atsc3_baseband_packet_short_fragment: ptr: %p, size: %d, alp_payload_pre_pointer: ptr: %p, size: %d, new size: %d, sequence: %d, port: %d",
                                  atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment,
                                  atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size,
                                  atsc3_baseband_packet->alp_payload_pre_pointer,
                                  atsc3_baseband_packet->alp_payload_pre_pointer->p_size,
                                  (atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size + atsc3_baseband_packet->alp_payload_pre_pointer->p_size),
                                  atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                                  atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);

                            uint32_t holdover_alp_payload_size = atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size;
                            uint32_t old_alp_payload_size = atsc3_baseband_packet->alp_payload_pre_pointer->p_size;
                            block_Merge(atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment, atsc3_baseband_packet->alp_payload_pre_pointer);
                            block_Release(&atsc3_baseband_packet->alp_payload_pre_pointer);
                            atsc3_baseband_packet->alp_payload_pre_pointer = atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment;
                            atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment = NULL; //null instead of clone/release since we just blcok_merged

                        } else {
                            __WARN("atsc3_baseband_packet: carry over: atsc3_baseband_packet_short_fragment: atsc3_baseband_packet_short_fragment: ptr: %p, size: %d, alp_payload_pre_pointer: ptr: %p, size: %d, new size: %d, sequence: %d, port: %d",
                                   atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment,
                                   atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size,
                                   atsc3_baseband_packet->alp_payload_post_pointer,
                                   atsc3_baseband_packet->alp_payload_post_pointer->p_size,
                                   (atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size + atsc3_baseband_packet->alp_payload_post_pointer->p_size),
                                   atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                                   atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);


                            uint32_t holdover_alp_payload_size = atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_size;
                            uint32_t old_alp_payload_size = atsc3_baseband_packet->alp_payload_post_pointer->p_size;
                            block_Merge(atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment, atsc3_baseband_packet->alp_payload_post_pointer);
                            block_Release(&atsc3_baseband_packet->alp_payload_post_pointer);
                            atsc3_baseband_packet->alp_payload_post_pointer = atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment;
                            atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment = NULL; //null instead of clone/release since we just blcok_merged


                        }
                    }

                    //if we have a pending packet and a pre-pointer baseband frame block
                    if(atsc3_alp_packet_collection->atsc3_alp_packet_pending && atsc3_baseband_packet->alp_payload_pre_pointer) {
                        //merge block_t pre_pointer by computing the difference...
                        uint32_t remaining_packet_pending_bytes = block_Remaining_size(atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload);
                        uint32_t remaining_baseband_frame_bytes = block_Remaining_size(atsc3_baseband_packet->alp_payload_pre_pointer);
                        _ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_baseband_packet: alp_packet_pending: size: %d, fragment remaining bytes: %d, bb pre_pointer frame bytes remaining: %d, bb pre_pointer size: %d",
                               atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload->p_size,
                               remaining_packet_pending_bytes,
                               remaining_baseband_frame_bytes,
                               atsc3_baseband_packet->alp_payload_pre_pointer->p_size);

                        //we may overrun this clamping...
                        block_Seek(atsc3_baseband_packet->alp_payload_pre_pointer, __MIN(remaining_packet_pending_bytes, remaining_baseband_frame_bytes));

                        //this is messy, as atsc3_baseband_packet will have alp_payload pointers to alp_payload..
                        atsc3_alp_packet_t* atsc3_alp_packet_pre_pointer = atsc3_alp_packet_collection->atsc3_alp_packet_pending;

                        block_Append(atsc3_alp_packet_pre_pointer->alp_payload, atsc3_baseband_packet->alp_payload_pre_pointer);
                        uint32_t final_alp_packet_short_bytes_remaining = block_Remaining_size(atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload);

                        //if our refragmenting is short, try and recover based upon baseband packet split
                        if(final_alp_packet_short_bytes_remaining) {
                            if(atsc3_baseband_packet->alp_payload_post_pointer) {
                               __WARN("atsc3_baseband_packet: pending packet: short and post pointer: %d bytes still remaining", final_alp_packet_short_bytes_remaining);

                            } else {
                            	_ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_baseband_packet: carry over pending packet: short:  %d bytes still remaining", final_alp_packet_short_bytes_remaining);
                            }

                            //do not attempt to free atsc3_alp_packet_collection->atsc3_alp_packet_pending, as it will free our interm reference to atsc3_alp_packet
                            atsc3_alp_packet_pre_pointer->is_alp_payload_complete  = false;
                            atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet_clone(atsc3_alp_packet_pre_pointer);
                            atsc3_alp_packet_free(&atsc3_alp_packet_pre_pointer);

                        } else {
                            //packet is complete after refragmenting -
                            atsc3_alp_packet_pre_pointer->is_alp_payload_complete = true;
                            atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet_pre_pointer);
                            atsc3_alp_packet_collection->atsc3_alp_packet_pending = NULL;
                            atsc3_alp_packet_pre_pointer = NULL;

                            uint32_t remaining_baseband_frame_bytes = block_Remaining_size(atsc3_baseband_packet->alp_payload_pre_pointer);
                            _ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_baseband_packet: pushed:  bb pre_pointer frame bytes remaining: %d, bb pre_pointer size: %d",
                                   remaining_baseband_frame_bytes,
                                   atsc3_baseband_packet->alp_payload_pre_pointer->p_size);

                            if(remaining_baseband_frame_bytes) {
                                //push remaining bytes to post-pointer,
                                if(atsc3_baseband_packet->alp_payload_post_pointer) {
                                    __INFO("atsc3_baseband_packet: prepending alp_payload_pre_pointer with alp_payload_post_pointer");
                                    block_t* alp_payload_post_pointer_orig = atsc3_baseband_packet->alp_payload_post_pointer;

                                    atsc3_baseband_packet->alp_payload_post_pointer = block_Duplicate_from_position(atsc3_baseband_packet->alp_payload_pre_pointer);

                                    block_Merge(atsc3_baseband_packet->alp_payload_post_pointer, alp_payload_post_pointer_orig);
                                    block_Release(&alp_payload_post_pointer_orig);
                                } else {
                                    atsc3_baseband_packet->alp_payload_post_pointer = block_Duplicate_from_position(atsc3_baseband_packet->alp_payload_pre_pointer);
                                    block_Release(&atsc3_baseband_packet->alp_payload_pre_pointer);
                                }
                            }
                        }
                    }

                    //process our pre_pointers from a baseband fragment for alp
                    if(atsc3_baseband_packet->alp_payload_pre_pointer && block_Remaining_size(atsc3_baseband_packet->alp_payload_pre_pointer)) {

                        while((atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_baseband_packet->alp_payload_pre_pointer))) {
                        	_ATSC3_STLTP_DEPACKETIZER_DEBUG("  atsc3_baseband_packet: carry over:  parse alp_payload_pre_pointer: pos: %d, size: %d",
                                   atsc3_baseband_packet->alp_payload_pre_pointer->i_pos,
                                   atsc3_baseband_packet->alp_payload_pre_pointer->p_size);

                            if(atsc3_alp_packet->is_alp_payload_complete) {
                                atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                            } else {
                                if(atsc3_alp_packet_collection->atsc3_alp_packet_pending && atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload) {
                                    __ERROR("  incomplete fragment for atsc3_alp_packet_pending, discarding! atsc3_alp_packet_pending: %p, pos: %d, size: %d, alp_packet_header.type: %d, alp_packet_header.type: %d",
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload->i_pos,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload->p_size,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_packet_header.alp_packet_header_mode.header_mode,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_packet_header.alp_packet_header_mode.length);
                                } else if(atsc3_alp_packet_collection->atsc3_alp_packet_pending) {
                                    __ERROR("  incomplete fragment for atsc3_alp_packet_pending, discarding! atsc3_alp_packet_pending: %p, alp_packet_header.type: %d, alp_packet_header.type: %d",
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_packet_header.alp_packet_header_mode.header_mode,
                                            atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_packet_header.alp_packet_header_mode.length);
                                } else {
                                    __ERROR("  fragment for atsc3_alp_packet_pending is NULL, discarding!");
                                }

                                //jjustman-2019-08-08 - free before stoping on pending payload reference
                                if(atsc3_alp_packet_collection->atsc3_alp_packet_pending) {
                                    atsc3_alp_packet_free(&atsc3_alp_packet_collection->atsc3_alp_packet_pending);
                                }
                                atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet_clone(atsc3_alp_packet);
                                atsc3_alp_packet_free(&atsc3_alp_packet);
                                break;
                            }
                        }
                        block_Release(&atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment);
                    }

                    //process post_pointer for our baseband frame
                    if(atsc3_baseband_packet->alp_payload_post_pointer) {
                    	_ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_baseband_packet: starting alp_payload_post_pointer: pos: %d, size: %d",
                               atsc3_baseband_packet->alp_payload_post_pointer->i_pos,
                               atsc3_baseband_packet->alp_payload_post_pointer->p_size);


                        while((atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_baseband_packet->alp_payload_post_pointer))) {
                        	_ATSC3_STLTP_DEPACKETIZER_DEBUG("  atsc3_baseband_packet: after parse alp_payload_post_pointer: pos: %d, size: %d",
                                   atsc3_baseband_packet->alp_payload_post_pointer->i_pos,
                                   atsc3_baseband_packet->alp_payload_post_pointer->p_size);


                            if(atsc3_alp_packet->is_alp_payload_complete) {
                                atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                            } else {
                                //carry-over as atsc3_alp_packet_collection->atsc3_alp_packet_pending

                                //jjustman-2019-08-08 - free before stoping on pending payload reference
                                if(atsc3_alp_packet_collection->atsc3_alp_packet_pending) {
                                    atsc3_alp_packet_free(&atsc3_alp_packet_collection->atsc3_alp_packet_pending);
                                }

                                if(atsc3_alp_packet) {
                                    atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet_clone(atsc3_alp_packet);
                                    atsc3_alp_packet_free(&atsc3_alp_packet);
                                }
                                break;
                            }
                        }

                        //anything remaining in post_pointer needs to be carried over for next alp_packet processing
                        uint32_t remaining_size = block_Remaining_size(atsc3_baseband_packet->alp_payload_post_pointer);
                        if(remaining_size) {
                            atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment = block_Duplicate_from_position(atsc3_baseband_packet->alp_payload_post_pointer);
                            __WARN(" !!!TODO: Carrying over one byte in alp_payload_post_pointer: %d, peek: 0x%02x", remaining_size, atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment->p_buffer[0]);
                        }
                    } else {
                    	_ATSC3_STLTP_DEPACKETIZER_DEBUG("atsc3_alp_packet_pending: no alp_payload_post_pointer - carrying over pkt: %p", atsc3_alp_packet_collection->atsc3_alp_packet_pending);

                    }
                }
            }

            //send our ALP IP packets, then clear the collection
            if(atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback) {
            	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback(atsc3_stltp_depacketizer_context_get_plp_from_context(atsc3_stltp_depacketizer_context), atsc3_alp_packet_collection);
            }

        	//generic context for class instance re-scoping
            if(atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_context && atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_context) {
            	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_context(atsc3_stltp_depacketizer_context_get_plp_from_context(atsc3_stltp_depacketizer_context), atsc3_alp_packet_collection, atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_context);
            }


            //explicit callback context for pcap_t reflection
            if(atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference && atsc3_stltp_depacketizer_context->atsc3_baseband_alp_output_pcap_device_reference) {
            	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference(atsc3_stltp_depacketizer_context_get_plp_from_context(atsc3_stltp_depacketizer_context), atsc3_alp_packet_collection, atsc3_stltp_depacketizer_context->atsc3_baseband_alp_output_pcap_device_reference);
            }

            //TODO: jjustman-2019-11-23: move this to atsc3_alp_packet_free ATSC3_VECTOR_BUILDER free method
            //clear out our inner payloads, then let collection_clear free the object instance
            for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
                atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
                atsc3_alp_packet_free_alp_payload(atsc3_alp_packet);
            }
            atsc3_alp_packet_collection_clear_atsc3_alp_packet(atsc3_alp_packet_collection);
            //todo: refactor to _free(..) for vector_t
            if(atsc3_alp_packet_collection->atsc3_alp_packet_v.data) {
                free(atsc3_alp_packet_collection->atsc3_alp_packet_v.data);
                atsc3_alp_packet_collection->atsc3_alp_packet_v.data = NULL;
            }

            for(int i=0; i < atsc3_alp_packet_collection->atsc3_baseband_packet_v.count; i++) {
                atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_alp_packet_collection->atsc3_baseband_packet_v.data[i];
                atsc3_baseband_packet_free_v(atsc3_baseband_packet);
            }
            atsc3_alp_packet_collection_clear_atsc3_baseband_packet(atsc3_alp_packet_collection);
            if(atsc3_alp_packet_collection->atsc3_baseband_packet_v.data) {
                free(atsc3_alp_packet_collection->atsc3_baseband_packet_v.data);
                atsc3_alp_packet_collection->atsc3_baseband_packet_v.data = NULL;
            }
        }

        if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count) {
        	_ATSC3_STLTP_DEPACKETIZER_DEBUG("preamble: >>>stltp atsc3_stltp_preamble_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count);
            for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count; i++) {

#ifdef PREAMBLE_PACKET_PARSE_AND_LOG
			  atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.data[i];
			  atsc3_preamble_packet_t* atsc3_preamble_packet = atsc3_stltp_parse_preamble_packet(atsc3_stltp_preamble_packet);
			  if(!atsc3_preamble_packet) {
				  __WARN("atsc3_preamble_packet is NULL for i: %d", i);
			  } else {
				  atsc3_preamble_packet_dump(atsc3_preamble_packet);
			  }
#endif
            }
        }

        if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count) {
        	_ATSC3_STLTP_DEPACKETIZER_DEBUG("timing management: >>>stltp atsc3_stltp_timing_management_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count);
            for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count; i++) {

#ifdef TIMING_MANAGEMENT_PACKET_PARSE_AND_LOG
				atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.data[i];
				atsc3_timing_management_packet_t* atsc3_timing_management_packet = atsc3_stltp_parse_timing_management_packet(atsc3_stltp_timing_management_packet);
            	if(!atsc3_timing_management_packet) {
            		__WARN("atsc3_timing_management_packet is NULL for i: %d", i);
            	} else {
                   	 atsc3_timing_management_packet_dump(atsc3_timing_management_packet);
                }
#endif
            }
        }

        //this method will clear _v.data inner references
        atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet_processed);
    }

cleanup:
    atsc3_ip_udp_rtp_packet_destroy(&ip_udp_rtp_packet);
}

/*
 * note: packet ownership is transferred to atsc3_stltp_depacketizer and ip_udp_rtp packet, so do NOT try and free *block_t unless we return false;
 */
bool atsc3_stltp_depacketizer_from_blockt(block_t** packet_p, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context) {
	atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = atsc3_ip_udp_rtp_packet_process_from_blockt_pos(*packet_p);
	if(!ip_udp_rtp_packet) {
		return false;
	}

	atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(ip_udp_rtp_packet, atsc3_stltp_depacketizer_context);
	*packet_p = NULL;
	return true;
}

void atsc3_stltp_depacketizer_from_pcap_frame(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context) {

    //extract our outer ip/udp/rtp packet
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = atsc3_ip_udp_rtp_process_packet_from_pcap(user, pkthdr, packet);
    if(!ip_udp_rtp_packet) {
        return;
    }

    atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(ip_udp_rtp_packet, atsc3_stltp_depacketizer_context);
}


uint8_t atsc3_stltp_depacketizer_context_get_plp_from_context(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context) {
	uint8_t plp = 0;

	if(atsc3_stltp_depacketizer_context->inner_rtp_port_filter > 30000 && atsc3_stltp_depacketizer_context->inner_rtp_port_filter <= 30064) {
		plp = atsc3_stltp_depacketizer_context->inner_rtp_port_filter - 30000;
	}

	return plp;
}
