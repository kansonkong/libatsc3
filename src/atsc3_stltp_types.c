/*
 * atsc3_stltp_types.c
 *
 *  Created on: Jul 23, 2019
 *      Author: jjustman
 */


#include "atsc3_stltp_types.h"

int _STLTP_TYPES_DEBUG_ENABLED = 1;
int _STLTP_TYPES_TRACE_ENABLED = 1;


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_stltp_tunnel_packet)

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_baseband_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_preamble_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_timing_management_packet);

inline const char *ATSC3_CTP_STL_PAYLOAD_TYPE_TO_STRING(int code) {
    switch (code) {
        case ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL:
            return ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL_STRING;
        case ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET:
            return ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET_STRING;
        case ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET:
            return ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET_STRING;
        case ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET:
            return ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET_STRING;
    }
    return "";
}

/*
 create a new stltp_baseband packet for stltp inner extraction, then hand off to baseband re-segmentation
*/
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_new_and_init(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet) {

    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_baseband_packet_new();
    
    //ref ip_udp_rtp_packet_outer w/ rtp_header_outer
    atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    atsc3_stltp_baseband_packet->rtp_header_outer = atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer->rtp_header;

    //ref ip_udp_rtp_packet_inner w/ rtp_header_inner
    atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner = atsc3_ip_udp_rtp_packet_duplicate(atsc3_stltp_tunnel_packet->ip_udp_rtp_packet_inner);
    atsc3_stltp_baseband_packet->rtp_header_inner = atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->rtp_header;
    
    //TODO: inner packet size, only if we are from a marker
    atsc3_stltp_baseband_packet->payload_length = atsc3_stltp_baseband_packet->rtp_header_inner->packet_offset;
    
    return atsc3_stltp_baseband_packet;
}


/*
 free only the ip_udp_rtp_packet outer/inner data payloads, as we will have rebuilt what the baseband packet needs in its ->payload
 do not remove the rtp_header outer and inner, as this data may be needed for coorelation of the timestamp field(s)
 */
void atsc3_stltp_baseband_packet_free_inner_outer_data(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet) {
 
    if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer && atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer->data) {
        block_Release(&atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer->data);
    }
    
    if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner && atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->data) {
        block_Release(&atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner->data);
    }
}

/*
 free the complete baseband packet, including its internal payload buffer for baseband re-assembly
 */
void atsc3_stltp_baseband_packet_free_v(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet) {
    if(atsc3_stltp_baseband_packet) {

        if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_baseband_packet->ip_udp_rtp_packet_outer);
        }
        atsc3_stltp_baseband_packet->rtp_header_outer = NULL; //atsc3_ip_udp_rtp_packet_free will free our shared pointer

        if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_baseband_packet->ip_udp_rtp_packet_inner);
        }
        atsc3_stltp_baseband_packet->rtp_header_inner = NULL; //atsc3_ip_udp_rtp_packet_free will free our shared pointer
        
        //TODO: move this over to block_t
        if(atsc3_stltp_baseband_packet->payload) {
            free(atsc3_stltp_baseband_packet->payload);
            atsc3_stltp_baseband_packet->payload = NULL;
        }
    }
}

void atsc3_stltp_preamble_packet_free_v(atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet) {
    if(atsc3_stltp_preamble_packet) {
        if(atsc3_stltp_preamble_packet->rtp_header) {
            __STLTP_TYPES_TRACE("atsc3_stltp_baseband_packet_free: freeing atsc3_stltp_preamble_packet->rtp_header: %p", atsc3_stltp_preamble_packet->rtp_header);
            atsc3_rtp_header_free(&atsc3_stltp_preamble_packet->rtp_header);
        }
        if(atsc3_stltp_preamble_packet->payload) {
            free(atsc3_stltp_preamble_packet->payload);
            atsc3_stltp_preamble_packet->payload = NULL;
        }
        if(atsc3_stltp_preamble_packet->ip_udp_rtp_packet) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_preamble_packet->ip_udp_rtp_packet);
        }
        //let vector_v initiate the pointer free
        //free(atsc3_stltp_preamble_packet);
    }
}

//copy-paste warning
void atsc3_stltp_timing_management_packet_free_v(atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet) {
    if(atsc3_stltp_timing_management_packet) {
        if(atsc3_stltp_timing_management_packet->rtp_header) {
            //todo - memset 0 this block
            __STLTP_TYPES_TRACE("atsc3_stltp_baseband_packet_free: freeing atsc3_stltp_timing_management_packet->rtp_header: %p", atsc3_stltp_timing_management_packet->rtp_header);
            atsc3_rtp_header_free(&atsc3_stltp_timing_management_packet->rtp_header);
        }
        if(atsc3_stltp_timing_management_packet->payload) {
            free(atsc3_stltp_timing_management_packet->payload);
            atsc3_stltp_timing_management_packet->payload = NULL;
        }
        if(atsc3_stltp_timing_management_packet->ip_udp_rtp_packet) {
            atsc3_ip_udp_rtp_packet_free(&atsc3_stltp_timing_management_packet->ip_udp_rtp_packet);
        }
        
        //let vector_v initiate the pointer free
        //free(atsc3_stltp_timing_management_packet);
    }
}
