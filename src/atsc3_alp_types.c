/*
 * atsc3_alp_types.c
 *
 *  Created on: Jul 31, 2019
 *      Author: jjustman
 */


#include "atsc3_alp_types.h"

//TODO: move these into atsc3_alp_types.h
ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_alp_packet_collection);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_alp_packet_collection, atsc3_alp_packet);



void atsc3_alp_packet_free_alp_payload(atsc3_alp_packet_t* atsc3_alp_packet) {
	if(atsc3_alp_packet) {
		if(atsc3_alp_packet->alp_payload) {
			block_Release(&atsc3_alp_packet->alp_payload);
		}
	}
}

