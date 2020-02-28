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

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_link_mapping_table);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_link_mapping_table, atsc3_link_mapping_table_plp);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_link_mapping_table_plp, atsc3_link_mapping_table_multicast);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_link_mapping_table_plp);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_link_mapping_table_multicast);

//TODO: jjustman-2019-08-08 - fixme: clone is a shallow clone (memcpy) and MAY leave dangling pointer references

atsc3_alp_packet_t* atsc3_alp_packet_clone(atsc3_alp_packet_t* atsc3_alp_packet) {
    atsc3_alp_packet_t* atsc3_alp_packet_new = calloc(1, sizeof(atsc3_alp_packet_t));
    memcpy(&atsc3_alp_packet_new->alp_packet_header, &atsc3_alp_packet->alp_packet_header, sizeof(alp_packet_header_t));
    atsc3_alp_packet_new->alp_payload = block_Duplicate(atsc3_alp_packet->alp_payload);
    atsc3_alp_packet_new->is_alp_payload_complete = atsc3_alp_packet->is_alp_payload_complete;
    
    return atsc3_alp_packet_new;
}


void atsc3_alp_packet_free(atsc3_alp_packet_t** atsc3_alp_packet_p) {
    if(atsc3_alp_packet_p) {
        atsc3_alp_packet_t* atsc3_alp_packet = *atsc3_alp_packet_p;
        if(atsc3_alp_packet) {
            if(atsc3_alp_packet->alp_payload) {
                block_Destroy(&atsc3_alp_packet->alp_payload);
            }
            free(atsc3_alp_packet);
            atsc3_alp_packet = NULL;
        }
        *atsc3_alp_packet_p = NULL;
    }
}

void atsc3_alp_packet_free_alp_payload(atsc3_alp_packet_t* atsc3_alp_packet) {
	if(atsc3_alp_packet) {
		if(atsc3_alp_packet->alp_payload) {
			//block_Release(&atsc3_alp_packet->alp_payload);
            block_Destroy(&atsc3_alp_packet->alp_payload);
		}
	}
}

void atsc3_link_mapping_table_free(atsc3_link_mapping_table_t** atsc3_link_mapping_table_p) {
    if(atsc3_link_mapping_table_p) {
        atsc3_link_mapping_table_t* atsc3_link_mapping_table = *atsc3_link_mapping_table_p;
        if(atsc3_link_mapping_table) {
            //chain destructors
            atsc3_link_mapping_table_free_atsc3_link_mapping_table_plp(atsc3_link_mapping_table);
            free(atsc3_link_mapping_table);
            atsc3_link_mapping_table = NULL;
        }
        *atsc3_link_mapping_table_p = NULL;
    }
}
