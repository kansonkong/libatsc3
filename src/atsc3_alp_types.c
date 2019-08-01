/*
 * atsc3_alp_types.c
 *
 *  Created on: Jul 31, 2019
 *      Author: jjustman
 */


#include "atsc3_alp_types.h"

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_alp_packet_collection);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_alp_packet_collection, atsc3_baseband_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_alp_packet_collection, atsc3_alp_packet);

atsc3_alp_packet_t* atsc3_alp_packet_clone(atsc3_alp_packet_t* atsc3_alp_packet) {
    atsc3_alp_packet_t* atsc3_alp_packet_new = calloc(1, sizeof(atsc3_alp_packet_t));
    memcpy(&atsc3_alp_packet_new->alp_packet_header, &atsc3_alp_packet->alp_packet_header, sizeof(alp_packet_header_t));
    atsc3_alp_packet_new->alp_payload = block_Duplicate(atsc3_alp_packet->alp_payload);
    atsc3_alp_packet_new->is_alp_payload_complete = atsc3_alp_packet->is_alp_payload_complete;
    
    return atsc3_alp_packet_new;
}

void atsc3_alp_packet_free_alp_payload(atsc3_alp_packet_t* atsc3_alp_packet) {
	if(atsc3_alp_packet) {
		if(atsc3_alp_packet->alp_payload) {
			block_Release(&atsc3_alp_packet->alp_payload);
		}
	}
}


//non-ref free, used for freeing inner payloads and chaining with                 atsc3_alp_packet_collection_clear_atsc3_baseband_packet(atsc3_alp_packet_collection);

void atsc3_baseband_packet_free_v(atsc3_baseband_packet_t* atsc3_baseband_packet) {
    if(atsc3_baseband_packet) {
        
        if(atsc3_baseband_packet->extension) {
            free(atsc3_baseband_packet->extension);
            atsc3_baseband_packet->extension = NULL;
        }
        
        if(atsc3_baseband_packet->alp_payload_pre_pointer) {
            block_Release(&atsc3_baseband_packet->alp_payload_pre_pointer);
        }
        if(atsc3_baseband_packet->alp_payload_post_pointer) {
            block_Release(&atsc3_baseband_packet->alp_payload_post_pointer);
        }
    }
}

void atsc3_baseband_packet_free(atsc3_baseband_packet_t** atsc3_baseband_packet_p) {
    atsc3_baseband_packet_t* atsc3_baseband_packet = *atsc3_baseband_packet_p;
    if(atsc3_baseband_packet) {
        
        if(atsc3_baseband_packet->extension) {
            free(atsc3_baseband_packet->extension);
            atsc3_baseband_packet->extension = NULL;
        }

        if(atsc3_baseband_packet->alp_payload_pre_pointer) {
            block_Release(&atsc3_baseband_packet->alp_payload_pre_pointer);
        }
        if(atsc3_baseband_packet->alp_payload_post_pointer) {
            block_Release(&atsc3_baseband_packet->alp_payload_post_pointer);
        }
        
        free(atsc3_baseband_packet);
        atsc3_baseband_packet = NULL;
    }
    *atsc3_baseband_packet_p = NULL;
}

