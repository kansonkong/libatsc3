/*
 * atsc3_route_object.c
 *
 *  Created on: Jul 27, 2020
 *      Author: jjustman
 *
 *
 *
jjustman@sdg-komo-mac188 tools % tail -f debug.log.5003| grep "atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received\|atsc3_route_object_is_complete: true"
atsc3_route_object.c    : 153:DEBUG:1595953002.4722:atsc3_route_object_is_complete: true, complete, with tsi: 1060, toi: 3, atsc3_route_object_lct_packet_received_v: 1, last_object_position: 780, atsc3_route_object->object_length: 780
atsc3_route_object.c    : 166:DEBUG:1595953002.4724:atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: atsc3_route_object: 0x7fc740004db0, tsi: 1060, toi: 3, clearing 1 route_object_lct_packet_received entries
atsc3_route_object.c    : 153:DEBUG:1595953002.6641:atsc3_route_object_is_complete: true, complete, with tsi: 1060, toi: 3, atsc3_route_object_lct_packet_received_v: 1, last_object_position: 780, atsc3_route_object->object_length: 780
atsc3_route_object.c    : 166:DEBUG:1595953002.6642:atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: atsc3_route_object: 0x7fc740004db0, tsi: 1060, toi: 3, clearing 1 route_object_lct_packet_received entries
 *
 */

#include "atsc3_route_object.h"

int _ROUTE_OBJECT_INFO_ENABLED = 0;
int _ROUTE_OBJECT_DEBUG_ENABLED = 1;
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


int atsc3_route_object_lct_packet_received_generic_sbn_start_offset_comparator(const void *a_dp, const void *b_dp) {
	atsc3_route_object_lct_packet_received_t* a = *(atsc3_route_object_lct_packet_received_t**)a_dp;
	atsc3_route_object_lct_packet_received_t* b = *(atsc3_route_object_lct_packet_received_t**)b_dp;

	if(a->use_sbn_esi && b->use_sbn_esi) {

		_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_lct_packet_received_generic_sbn_start_offset_comparator, using sbn_esi, with a: %d, b: %d", a->sbn, b->sbn);

		if ( a->sbn <  b->sbn) return -1;
		if ( a->sbn == b->sbn) return  0;
		if ( a->sbn >  b->sbn) return  1;

	} else if(a->use_start_offset && b->use_start_offset) {

		if ( a->start_offset <  b->start_offset) return -1;
		if ( a->start_offset == b->start_offset) return  0;
		if ( a->start_offset >  b->start_offset) return  1;

	} else {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_lct_packet_received_generic_sbn_start_offset_comparator: mismatched a: use_sbn_esi: %d, use_start_offset: %d, b: use_sbn_esi: %d, use_start_offset: %d",
				a->use_sbn_esi,
				a->use_start_offset,
				b->use_sbn_esi,
				b->use_start_offset);
	}
	return 0;
}

//jjustman-2020-07-28 - first try, check to make sure we have all of our lct_packet's received and the most recent packet would be
//						a fully recovered object...
//	TODO: 		    qsort((void**)atsc3_global_statistics->packet_id_vector, atsc3_global_statistics->packet_id_n, sizeof(packet_id_mmt_stats_t**), comparator_packet_id_mmt_stats_t);

bool atsc3_route_object_is_complete(atsc3_route_object_t* atsc3_route_object) {
	bool has_missing_source_blocks = false;
	atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = NULL;

	uint32_t last_object_position = 0;

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count; i++) {
		atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];

		_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: BEFORE qsort loop: %d, using start_offset - tsi: %d, toi: %d, last_object_position: %d, use start offset: %d, start_offset: %d, packet_len: %d",
								i,
								atsc3_route_object->tsi,
								atsc3_route_object->toi,
								last_object_position,
								atsc3_route_object_lct_packet_received->use_start_offset,
								atsc3_route_object_lct_packet_received->start_offset,
								atsc3_route_object_lct_packet_received->packet_len);

		last_object_position += atsc3_route_object_lct_packet_received->packet_len;

	}
	last_object_position = 0;
#endif

	qsort((void**)atsc3_route_object->atsc3_route_object_lct_packet_received_v.data, atsc3_route_object->atsc3_route_object_lct_packet_received_v.count, sizeof(atsc3_route_object_lct_packet_received_t**), atsc3_route_object_lct_packet_received_generic_sbn_start_offset_comparator);

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count; i++) {
		atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];

		_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: AFTER qsort loop: %d, using start_offset - tsi: %d, toi: %d, use start offset: %d, start_offset: %d, packet_len: %d, computed last_object_position: %d",
								i,
								atsc3_route_object->tsi,
								atsc3_route_object->toi,
								atsc3_route_object_lct_packet_received->use_start_offset,
								atsc3_route_object_lct_packet_received->start_offset,
								atsc3_route_object_lct_packet_received->packet_len,
								last_object_position);

		last_object_position += atsc3_route_object_lct_packet_received->packet_len;

	}
	last_object_position = 0;
#endif

	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count && !has_missing_source_blocks; i++) {
		atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];

		if(atsc3_route_object_lct_packet_received->use_sbn_esi) {
			if(last_object_position != atsc3_route_object_lct_packet_received->esi) {
				has_missing_source_blocks = true;
				_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: has_missing_source_blocks - using sbn_esi - tsi: %d, toi: %d, last_object_position: %d, esi: %d",
						atsc3_route_object->tsi, atsc3_route_object->toi, last_object_position, atsc3_route_object_lct_packet_received->esi);
			} else {
				last_object_position += atsc3_route_object_lct_packet_received->packet_len;
			}
		} else if(atsc3_route_object_lct_packet_received->use_start_offset) {
			if(last_object_position != atsc3_route_object_lct_packet_received->start_offset) {
				has_missing_source_blocks = true;
				_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: has_missing_source_blocks - using start_offset - idx: %d, tsi: %d, toi: %d, last_object_position: %d, start_offset: %d",
										i, atsc3_route_object->tsi, atsc3_route_object->toi, last_object_position, atsc3_route_object_lct_packet_received->start_offset);
			} else {
#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__

				_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: loop: %d, using start_offset - tsi: %d, toi: %d, last_object_position: %d, start_offset: %d, packet_len: %d",
										i,
										atsc3_route_object->tsi, atsc3_route_object->toi, last_object_position, atsc3_route_object_lct_packet_received->start_offset, atsc3_route_object_lct_packet_received->packet_len);
#endif
				last_object_position += atsc3_route_object_lct_packet_received->packet_len;
			}
		}

	}

	if(!atsc3_route_object->object_length || last_object_position != atsc3_route_object->object_length) {
		_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: false, has_missing_source_blocks at end, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v: %d, last_object_position: %d, atsc3_route_object->object_length: %d",
				atsc3_route_object->tsi, atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
				last_object_position,
				atsc3_route_object->object_length);

		has_missing_source_blocks = true;
	} else {

		_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: true, object_length: %8d,            atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
						atsc3_route_object->object_length,
						atsc3_route_object,
						atsc3_route_object->tsi, atsc3_route_object->toi,
						atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
	}


	return !has_missing_source_blocks;
}


void atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received(atsc3_route_object_t* atsc3_route_object) {
	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);


	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);

}

//deprecated


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
