/*
 * atsc3_stltp_alp_listener_reflector_test.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 * stltp listener for atsc a/324
 *
 * jjustman: 2020-06-02 - renamed from atsc3_stltp_listener_alp_reflector.cpp to atsc3_stltp_alp_listener_reflector_test.cpp to match makefile build artifact
 *
 */
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <string.h>

#include "../atsc3_listener_udp.h"
#include "../atsc3_stltp_parser.h"
#include "../atsc3_alp_parser.h"
#include "../atsc3_logging_externs.h"

#define PREAMBLE_PACKET_PARSE_AND_LOG true
#define TIMING_MANAGEMENT_PACKET_PARSE_AND_LOG true

int PACKET_COUNTER = 0;

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t stltp_ip_port_filter = 30000;

extern pcap_t* descrInject;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_processed = NULL;
atsc3_alp_packet_collection_t* atsc3_alp_packet_collection = NULL;

//TODO - add SMPTE-2022.1 FEC decoding (see fork of prompeg-decoder - https://github.com/jjustman/prompeg-decoder)

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	__DEBUG("");
	__DEBUG("process_packet: pcap packet: %p, pcap len: %d", packet, pkthdr->len);
    
    //extract our outer ip/udp/rtp packet
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = atsc3_ip_udp_rtp_process_packet_from_pcap(user, pkthdr, packet);
    if(!ip_udp_rtp_packet) {
        return;
    }
    
    //dispatch for STLTP decoding and reflection
    if(ip_udp_rtp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && ip_udp_rtp_packet->udp_flow.dst_port == *dst_ip_port_filter) {
        atsc3_stltp_tunnel_packet_processed = atsc3_stltp_raw_packet_extract_inner_from_outer_packet(atsc3_stltp_depacketizer_context, ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_processed);
        
        if(!atsc3_stltp_tunnel_packet_processed) {
            __ERROR("process_packet: atsc3_stltp_tunnel_packet_processed is null, error processing packet: %p, size: %u",  ip_udp_rtp_packet, ip_udp_rtp_packet->data->p_size);
            goto cleanup;
        }
        
        if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count) {
        	__DEBUG(">>>stltp atsc3_stltp_baseband_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count);
            
            //TODO: jjustman-2019-08-09 refactor stltp baseband to alp processing logic out
            
            for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count; i++) {
                atsc3_alp_packet_t* atsc3_alp_packet = NULL;
                atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.data[i];
                __DEBUG("atsc3_baseband_packet: sequence_num: %d, port: %d",
                       atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header->sequence_number,
                       atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);
                
				if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port != stltp_ip_port_filter) {
					__DEBUG("ignorning stltp_baseband_packet port: %d",  atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->udp_flow.dst_port);
                    //atsc3_stltp_baseband_packet_free(&atsc3_stltp_baseband_packet);
                    continue;
                }

                //make sure we get a packet back, base field pointer (13b) : 0x1FFF (8191 bytes) will return NULL
                atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_stltp_parse_baseband_packet(atsc3_stltp_baseband_packet);
                if(!atsc3_baseband_packet) {
                	__DEBUG("no baseband packet returned, ^^^ should be only padding");
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
                        __DEBUG("atsc3_baseband_packet: alp_packet_pending: size: %d, fragment remaining bytes: %d, bb pre_pointer frame bytes remaining: %d, bb pre_pointer size: %d",
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
                            	__DEBUG("atsc3_baseband_packet: carry over pending packet: short:  %d bytes still remaining", final_alp_packet_short_bytes_remaining);
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
                            __DEBUG("atsc3_baseband_packet: pushed:  bb pre_pointer frame bytes remaining: %d, bb pre_pointer size: %d",
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
                    //(stltp_ip_port_filter - 30000)
                    if(atsc3_baseband_packet->alp_payload_pre_pointer && block_Remaining_size(atsc3_baseband_packet->alp_payload_pre_pointer)) {

                        while((atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_baseband_packet->alp_payload_pre_pointer))) {
                        	__DEBUG("  atsc3_baseband_packet: carry over:  parse alp_payload_pre_pointer: pos: %d, size: %d",
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
                    	__DEBUG("atsc3_baseband_packet: starting alp_payload_post_pointer: pos: %d, size: %d",
                               atsc3_baseband_packet->alp_payload_post_pointer->i_pos,
                               atsc3_baseband_packet->alp_payload_post_pointer->p_size);
                        


                        while((atsc3_alp_packet = atsc3_alp_packet_parse((stltp_ip_port_filter - 30000), atsc3_baseband_packet->alp_payload_post_pointer))) {
                        	__DEBUG("  atsc3_baseband_packet: after parse alp_payload_post_pointer: pos: %d, size: %d",
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
                    	__DEBUG("atsc3_alp_packet_pending: no alp_payload_post_pointer - carrying over pkt: %p", atsc3_alp_packet_collection->atsc3_alp_packet_pending);
                 
                    }
                }
            }

            //send our ALP IP packets, then clear the collection
            atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection);
            
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
        	__DEBUG("preamble: >>>stltp atsc3_stltp_preamble_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count);
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
        	__DEBUG("timing management: >>>stltp atsc3_stltp_timing_management_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count);
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

int main(int argc,char **argv) {
    _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 0;
    _IP_UDP_RTP_PARSER_TRACE_ENABLED = 0;

    _ATSC3_UTILS_TRACE_ENABLED = 0;
    
    char *dev;
    char *devInject;
    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_stltp_plp_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
	
    char errbuf[PCAP_ERRBUF_SIZE];
    char errbufInject[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;
    
    struct bpf_program fpInject;
    bpf_u_int32 maskpInject;
    bpf_u_int32 netpInject;
    
    atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();
    atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();


    if(argc < 5) {
        println("%s - an atsc3 stltp udp mulitcast reflector ", argv[0]);
        println("---");
        println("args: devListen ip port devInject");
        println(" devListen : device to listen for stltp udp multicast");
        println(" ip        : ip address for stltp");
        println(" port      : port for single stltp");
        println(" devInject : device to inject for ALP IP reflection");
		println(" 	(optional) PLP_num: PLP number to extract (e.g. 0 -> STLTP inner port: 30000, 63 -> STLTP inner port 300063");

        exit(1);
    } else {
        dev = argv[1];
        filter_dst_ip = argv[2];
        filter_dst_port = argv[3];
        devInject = argv[4];
        
        //parse ip
        dst_ip_addr_filter = (uint32_t*)calloc(1, sizeof(uint32_t));
        char* pch = strtok (filter_dst_ip,".");
        int offset = 24;
        while (pch != NULL && offset>=0) {
            uint8_t octet = atoi(pch);
            *dst_ip_addr_filter |= octet << offset;
            offset-=8;
            pch = strtok (NULL, ".");
        }
        
        //parse port
        
        dst_port_filter_int = atoi(filter_dst_port);
        dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
        *dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
	
		if(argc == 6) {
			//parse out custom PLP_num
			filter_stltp_plp_id = argv[5];

			uint8_t stltp_plp_id = atoi(filter_stltp_plp_id);
			if(stltp_plp_id >=0 && stltp_plp_id <= 63) {
				stltp_ip_port_filter = stltp_plp_id + 30000;
				println("using PLP: %d (stltp inner port: %d)", stltp_plp_id, stltp_ip_port_filter);
			} else {
				println("ignoring PLP: %d, defaulting to PLP 0 (stltp inner port: %d)", stltp_plp_id, stltp_ip_port_filter);
			}
		}
    }
    
    
    println("%s -an atsc3 stltp udp mulitcast reflector , listening on dev: %s, refleting: %s", argv[0], dev, devInject);
    
    pcap_lookupnet(dev, &netp, &maskp, errbuf);
    descr = pcap_open_live(dev, MAX_PCAP_LEN, 1, 1, errbuf);
    
    if(descr == NULL) {
        printf("pcap_open_live(): %s",errbuf);
        exit(1);
    }
    
    char filter[] = "udp";
    if(pcap_compile(descr,&fp, filter,0,netp) == -1) {
        fprintf(stderr,"Error calling pcap_compile");
        exit(1);
    }
    
    if(pcap_setfilter(descr,&fp) == -1) {
        fprintf(stderr,"Error setting filter");
        exit(1);
    }
   
    //inject
    pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 1, errbufInject);
    atsc3_alp_packet_collection->descrInject = descrInject;
    
    pcap_loop(descr, -1, process_packet, NULL);
    
    return 0;
}

