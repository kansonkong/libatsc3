/*
 * atsc3_baseband_packet_types.c
 *
 *  Created on: Aug 9, 2019
 *      Author: jjustman
 */

#include "atsc3_baseband_packet_types.h"

//non-ref free, used for freeing inner payloads and chaining with
//atsc3_alp_packet_collection_clear_atsc3_baseband_packet(atsc3_alp_packet_collection);

void atsc3_baseband_packet_free_v(atsc3_baseband_packet_t* atsc3_baseband_packet) {
    if(atsc3_baseband_packet) {

        if(atsc3_baseband_packet->extension) {
            free(atsc3_baseband_packet->extension);
            atsc3_baseband_packet->extension = NULL;
        }

        if(atsc3_baseband_packet->alp_payload_pre_pointer) {
            block_Destroy(&atsc3_baseband_packet->alp_payload_pre_pointer);
        }
        if(atsc3_baseband_packet->alp_payload_post_pointer) {
        	block_Destroy(&atsc3_baseband_packet->alp_payload_post_pointer);
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
        	block_Destroy(&atsc3_baseband_packet->alp_payload_pre_pointer);
        }
        if(atsc3_baseband_packet->alp_payload_post_pointer) {
        	block_Destroy(&atsc3_baseband_packet->alp_payload_post_pointer);
        }

        free(atsc3_baseband_packet);
        atsc3_baseband_packet = NULL;
    }
    *atsc3_baseband_packet_p = NULL;
}


void atsc3_baseband_packet_free_no_alp_pointer_release(atsc3_baseband_packet_t** atsc3_baseband_packet_p) {
    atsc3_baseband_packet_t* atsc3_baseband_packet = *atsc3_baseband_packet_p;
    if(atsc3_baseband_packet) {
        if(atsc3_baseband_packet->extension) {
            free(atsc3_baseband_packet->extension);
            atsc3_baseband_packet->extension = NULL;
        }
        
        //TODO: trace: %p atsc3_baseband_packet->alp_payload_pre_pointer
        //TODO: trace: %p atsc3_baseband_packet->alp_payload_post_pointer
        
        free(atsc3_baseband_packet);
        atsc3_baseband_packet = NULL;
    }
    *atsc3_baseband_packet_p = NULL;
}
