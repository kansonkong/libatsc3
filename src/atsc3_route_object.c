/*
 * atsc3_route_object.c
 *
 *  Created on: Jul 27, 2020
 *      Author: jjustman
 */

#include "atsc3_route_object.h"

int _ROUTE_OBJECT_INFO_ENABLED = 0;
int _ROUTE_OBJECT_DEBUG_ENABLED = 0;
int _ROUTE_OBJECT_TRACE_ENABLED = 0;

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_object, atsc3_route_object_lct_packet_received);
//default free for this item since we don't calloc any members
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_object_lct_packet_received);

//jjustman-2020-07-27 - TODO: add ATSC3_VECTOR_BUILDER_METHODS_PARENT_ITEM_FREE
void atsc3_route_object_free(atsc3_route_object_t** atsc3_route_object_p) {
	if(atsc3_route_object_p) {
		atsc3_route_object_t* atsc3_route_object = *atsc3_route_object_p;
		if(atsc3_route_object) {
			atsc3_route_object_dealloc_atsc3_route_object_lct_packet_received(atsc3_route_object);

			free(atsc3_route_object);
			atsc3_route_object = NULL;
		}
		*atsc3_route_object_p = NULL;
	}
}


#define ATSC3_ROUTE_OBJECT_RECEVIED_SOURCE_BYTES_INDEX_LENGTH(toi_length) ((toi_length / 256) + 1)

void atsc3_route_object_set_toi_and_length(atsc3_route_object_t* atsc3_route_object, uint32_t toi, uint32_t toi_length) {
	atsc3_route_object->toi = toi;
	atsc3_route_object->object_length = toi_length;


}
void atsc3_route_object_mark_received_byte_range(atsc3_route_object_t* atsc3_route_object,uint32_t source_byte_range_start, uint32_t source_byte_range_end) {
	//make sure we're in the bounding range

}

bool atsc3_route_object_is_recovered(atsc3_route_object_t* atsc3_route_object) {
	bool has_missing_source_block_bytes = false;


	return has_missing_source_block_bytes;
}
