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

//copy/paste warning...
void atsc3_stltp_baseband_packet_free_v(atsc3_stltp_baseband_packet_t** atsc3_stltp_baseband_packet_p) {
    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = *atsc3_stltp_baseband_packet_p;
    if(atsc3_stltp_baseband_packet) {
        if(atsc3_stltp_baseband_packet->rtp_header) {
            __STLTP_TYPES_TRACE("atsc3_stltp_baseband_packet_free: freeing atsc3_stltp_baseband_packet->rtp_header: %p", atsc3_stltp_baseband_packet->rtp_header);
            atsc3_rtp_header_free(&atsc3_stltp_baseband_packet->rtp_header);
        }
        if(atsc3_stltp_baseband_packet->payload) {
            free(atsc3_stltp_baseband_packet->payload);
            atsc3_stltp_baseband_packet->payload = NULL;
        }
        if(atsc3_stltp_baseband_packet->ip_udp_rtp_packet) {
            atsc3_ip_udp_rtp_packet_and_data_free(&atsc3_stltp_baseband_packet->ip_udp_rtp_packet);
        }
        free(atsc3_stltp_baseband_packet);
        atsc3_stltp_baseband_packet = NULL;
        *atsc3_stltp_baseband_packet_p = NULL;
    }
}

void atsc3_stltp_preamble_packet_free_v(atsc3_stltp_preamble_packet_t** atsc3_stltp_preamble_packet_p) {
    atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = *atsc3_stltp_preamble_packet_p;
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
            atsc3_ip_udp_rtp_packet_and_data_free(&atsc3_stltp_preamble_packet->ip_udp_rtp_packet);
        }
        free(atsc3_stltp_preamble_packet);
        atsc3_stltp_preamble_packet = NULL;
        *atsc3_stltp_preamble_packet_p = NULL;
    }
}

//copy-paste warning
void atsc3_stltp_timing_management_packet_free_v(atsc3_stltp_timing_management_packet_t** atsc3_stltp_timing_management_packet_p) {
    atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = *atsc3_stltp_timing_management_packet_p;
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
            atsc3_ip_udp_rtp_packet_and_data_free(&atsc3_stltp_timing_management_packet->ip_udp_rtp_packet);
        }
        free(atsc3_stltp_timing_management_packet);
        atsc3_stltp_timing_management_packet = NULL;
        *atsc3_stltp_timing_management_packet_p = NULL;
    }
}
