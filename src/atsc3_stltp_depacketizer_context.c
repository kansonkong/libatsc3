/*
 * atsc3_stltp_depacketizer_context.c
 *
 *  Created on: Aug 18, 2020
 *      Author: jjustman
 */


#include "atsc3_stltp_depacketizer_context.h"

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context_new() {
	atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = calloc(1, sizeof(atsc3_stltp_depacketizer_context_t));

	atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();
	atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000; //just to be safe as PLP0 if we never get set...

	atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection = calloc(64, sizeof(atsc3_stltp_tunnel_baseband_packet_pending_by_plp_t));

	return atsc3_stltp_depacketizer_context;
}


void atsc3_stltp_depacketizer_context_free(atsc3_stltp_depacketizer_context_t** atsc3_stltp_depacketizer_context_p) {
	if(atsc3_stltp_depacketizer_context_p) {
		atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = *atsc3_stltp_depacketizer_context_p;
		if(atsc3_stltp_depacketizer_context) {

			if(atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection) {
				for(int i=0; i < 64; i++) {
					atsc3_stltp_tunnel_baseband_packet_pending_by_plp_t* atsc3_stltp_tunnel_baseband_packet_pending_by_plp = &atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection[i];
					if(atsc3_stltp_tunnel_baseband_packet_pending_by_plp) { //should never happen that we're null with one block calloc
						if(atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_baseband_packet_short_fragment) {
							block_Destroy(&atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_baseband_packet_short_fragment);
						}

						if(atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending) {
							atsc3_stltp_baseband_packet_free(&atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_stltp_baseband_packet_pending);
						}

						if(atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_alp_packet_pending) {
							atsc3_alp_packet_free(&atsc3_stltp_tunnel_baseband_packet_pending_by_plp->atsc3_alp_packet_pending);
						}
					}
				}
				free(atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection);
				atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection = NULL;
			}

			//cleanup
			if(atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed) {
				atsc3_stltp_tunnel_packet_destroy(&atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_packet_processed);
			}

			if(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection) {
				//atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection
				atsc3_alp_packet_collection_free_atsc3_alp_packet(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection);
				atsc3_alp_packet_collection_free_atsc3_baseband_packet(atsc3_stltp_depacketizer_context->atsc3_alp_packet_collection);

			}

			free(atsc3_stltp_depacketizer_context);
			atsc3_stltp_depacketizer_context = NULL;

		}
		*atsc3_stltp_depacketizer_context_p = NULL;
	}
}


void atsc3_stltp_depacketizer_context_set_all_plps(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context) {
	atsc3_stltp_depacketizer_context->inner_rtp_port_filter = ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_INNER_RTP_PORT;
}


void atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {
	int plp_num = 0;
	if(!ip_udp_rtp_packet_inner) {
		_ATSC3_STLTP_DEPACKETIZER_CONTEXT_ERROR("atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp - ip_udp_rtp_packet_inner is NULL, returning plp0 by default");

	} else {

		if(ip_udp_rtp_packet_inner->udp_flow.dst_port >= 30000 && ip_udp_rtp_packet_inner->udp_flow.dst_port < 30064) {
			plp_num = ip_udp_rtp_packet_inner->udp_flow.dst_port - 30000;
		}
	}

	atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp = &atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection[plp_num];
}


void atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_plp(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, uint8_t from_plp_num, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current) {
	int plp_num = 0;
	if(from_plp_num > 63) {
		_ATSC3_STLTP_DEPACKETIZER_CONTEXT_ERROR("atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_plp - from_plp_num too large: %d, using 0", from_plp_num);

	} else {
		plp_num = from_plp_num;
	}

	atsc3_stltp_tunnel_packet_current->atsc3_stltp_tunnel_baseband_packet_pending_by_plp = &atsc3_stltp_depacketizer_context->atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection[plp_num];
}


