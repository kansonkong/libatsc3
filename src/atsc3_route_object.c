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

//default free for this item since we don't calloc any members
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_object_lct_packet_received);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_object, atsc3_route_object_lct_packet_received);

int _ROUTE_OBJECT_INFO_ENABLED = 1;
int _ROUTE_OBJECT_DEBUG_ENABLED = 0;
int _ROUTE_OBJECT_TRACE_ENABLED = 0;

int atsc3_route_object_lct_packet_received_cmp_fn(const struct avltree_node *a, const struct avltree_node *b)
{
        atsc3_route_object_lct_packet_received_node_t *p = avltree_container_of(a, atsc3_route_object_lct_packet_received_node_t, node);
        atsc3_route_object_lct_packet_received_node_t *q = avltree_container_of(b, atsc3_route_object_lct_packet_received_node_t, node);

        return (p->key < q->key)? -1 : (p->key > q->key) ? 1 : 0 ;
}

atsc3_route_object_t* atsc3_route_object_new() {
	atsc3_route_object_t* atsc3_route_object = calloc(1, sizeof(atsc3_route_object_t));

	avltree_init(&atsc3_route_object->atsc3_route_object_lct_packet_received_tree, atsc3_route_object_lct_packet_received_cmp_fn, 0);

	return atsc3_route_object;
}

//jjustman-2020-07-27 - TODO: add ATSC3_VECTOR_BUILDER_METHODS_PARENT_ITEM_FREE
void atsc3_route_object_free(atsc3_route_object_t** atsc3_route_object_p) {
	if(atsc3_route_object_p) {
		atsc3_route_object_t* atsc3_route_object = *atsc3_route_object_p;
		if(atsc3_route_object) {

			_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_free: p: %p, tsi: %d, toi: %d, before closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d), final_object_recovery_filename_for_logging: %s",
					atsc3_route_object,
					atsc3_route_object->tsi,
					atsc3_route_object->toi,
					atsc3_route_object->recovery_file_handle,
					atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
					atsc3_route_object->atsc3_route_object_lct_packet_received_v.size,
					atsc3_route_object->final_object_recovery_filename_for_logging

			);

			freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
			freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_eviction);
			freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_logging);


			//most important to clear the lct packets recv
			atsc3_route_object_recovery_file_handle_close(atsc3_route_object);

			//jjustman-2020-08-04 - important, always call these two together..
			atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
			atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

			atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);

			_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_free: p: %p, tsi: %d, toi: %d, after closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d)",
					atsc3_route_object,
							atsc3_route_object->tsi,
							atsc3_route_object->toi,
					atsc3_route_object->recovery_file_handle,
								atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
								atsc3_route_object->atsc3_route_object_lct_packet_received_v.size
						);
			atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

			free(atsc3_route_object);
			atsc3_route_object = NULL;
		}
		*atsc3_route_object_p = NULL;
	}
}

void atsc3_route_object_add_atsc3_route_object_lct_packet_len(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received) {
	atsc3_route_object->cumulative_lct_packet_len += atsc3_route_object_lct_packet_received->packet_len;
}

void atsc3_route_object_set_is_toi_init_object(atsc3_route_object_t* atsc3_route_object, bool is_toi_init) {
	atsc3_route_object->is_toi_init = is_toi_init;
}


void atsc3_route_object_set_temporary_object_recovery_filename_if_null(atsc3_route_object_t* atsc3_route_object, char* temporary_filename) {
	if(!atsc3_route_object->temporary_object_recovery_filename) {
		atsc3_route_object->temporary_object_recovery_filename = strdup(temporary_filename);
	}
}

void atsc3_route_object_clear_temporary_object_recovery_filename(atsc3_route_object_t* atsc3_route_object) {
	freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
}

void atsc3_route_object_set_final_object_recovery_filename_for_eviction(atsc3_route_object_t* atsc3_route_object, char* final_object_recovery_filename_for_eviction) {
	freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_eviction);
	atsc3_route_object->final_object_recovery_filename_for_eviction = strdup(final_object_recovery_filename_for_eviction);

}

void atsc3_route_object_set_final_object_recovery_filename_for_logging(atsc3_route_object_t* atsc3_route_object, char* final_object_recovery_filename_for_logging) {
	freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_logging);
	atsc3_route_object->final_object_recovery_filename_for_logging = strdup(final_object_recovery_filename_for_logging);
}

/*
 * method: atsc3_route_object_set_object_recovery_complete
 *
 * used to mark this atsc3_route_object as fully recovered and persisted on disk
 *
 * only run this when you are done working with atsc3_route_object_lct_packets,
 * will release internal LCT packet container/counts
 *
 * 	invoked: atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows
 *
 * borrowed from atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received
 */
void atsc3_route_object_set_object_recovery_complete(atsc3_route_object_t* atsc3_route_object) {
 	atsc3_route_object->recovery_complete_timestamp = gtl();

// 	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_set_object_recovery_complete: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d, timestamp: %.2lu",
// 			atsc3_route_object,
// 			atsc3_route_object->tsi,
// 			atsc3_route_object->toi,
// 			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
// 			atsc3_route_object->recovery_complete_timestamp);
 	//this will be invoked from atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects
 	//atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received(atsc3_route_object);

 	//borrowed from atsc3_route_object_free

 	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_set_object_recovery_complete: p: %p, tsi: %d, toi: %d, before closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d)",
 			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->recovery_file_handle,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.size
	);
	atsc3_route_object_recovery_file_handle_close(atsc3_route_object);

	//jjustman-2020-08-04 - important, always call these two together..
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

	atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);
	//end borrowed from atsc3_route_object_free

	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_set_object_recovery_complete: p: %p, tsi: %d, toi: %d, after closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d)",
			atsc3_route_object,
 			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->recovery_file_handle,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.size
	);
}


void atsc3_route_object_recovery_file_handle_assign(atsc3_route_object_t* atsc3_route_object, FILE* recovery_file_handle) {
	if(!recovery_file_handle) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_handle_assign: recovery_file_handle is NULL!");
		return;
	}

	if(atsc3_route_object->recovery_file_handle) {
		if(atsc3_route_object->recovery_file_handle != recovery_file_handle) {
			_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_handle_assign: existing atsc3_route_object->recovery_file_handle is: %p, new is: %p, releasing atsc3_route_object handle!",
					atsc3_route_object->recovery_file_handle, recovery_file_handle);
			fclose(atsc3_route_object->recovery_file_handle);
			atsc3_route_object->recovery_file_handle = NULL;
		}
	}

	atsc3_route_object->recovery_file_handle = recovery_file_handle;
}

void atsc3_route_object_recovery_file_handle_close(atsc3_route_object_t* atsc3_route_object) {
	if(atsc3_route_object->recovery_file_handle) {
		fclose(atsc3_route_object->recovery_file_handle);
		atsc3_route_object->recovery_file_handle = NULL;
	}
}


void atsc3_route_object_calculate_expected_route_object_lct_packet_count(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received) {
	//always use the MAX of our lct_packet_len so we don't indadvertantly grow our target count upwards on the final packet (short payload)
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = __MAX(atsc3_route_object->expected_route_object_lct_packet_len_for_count, atsc3_route_object_lct_packet_received->packet_len);

	uint32_t route_object_lct_packet_received_target_count = __MAX(1, (atsc3_route_object->object_length / (__MAX(atsc3_route_object->expected_route_object_lct_packet_len_for_count, 1))));
	atsc3_route_object->expected_route_object_lct_packet_count = route_object_lct_packet_received_target_count;

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
	_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_calculate_expected_route_object_lct_packet_count: tsi: %d, toi: %d, object_length: %d, lct_packet_len: %d, expected_route_object_lct_packet_len_for_count: %d, expected_route_object_lct_packet_count: %d, current count: %d, threshold: %d",
			atsc3_route_object->tsi, atsc3_route_object->toi,
			atsc3_route_object->object_length,
			atsc3_route_object_lct_packet_received->packet_len,
			atsc3_route_object->expected_route_object_lct_packet_len_for_count,
			atsc3_route_object->expected_route_object_lct_packet_count,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
			(atsc3_route_object->expected_route_object_lct_packet_count * 7) / 8);
#endif

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

/*
 * method 1: estimate total lct_packets needed for this object - results in lots of overhead with large objects (~100's of qsort calls for one object)
 *
	//short-circuit if we don't have at least 7/8 of route_object_lct_packets from estimate (-2 for single digit lct_packet counts that may get lost...)
	//or only do this calculation of packet_len ~ object_len?
	int expected_route_object_lct_packet_count_threshold_adjusted = __MAX(1, (((int)atsc3_route_object->expected_route_object_lct_packet_count * 7) / 8) - 2);
	uint32_t expected_route_object_lct_packet_count_threshold = (uint32_t)expected_route_object_lct_packet_count_threshold_adjusted;

	if(atsc3_route_object->atsc3_route_object_lct_packet_received_v.count < expected_route_object_lct_packet_count_threshold) {
		#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
		_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: tsi: %d, toi: %d, pre-flight check with atsc3_route_object->atsc3_route_object_lct_packet_received_v.count: %d,  expected_route_object_lct_packet_count_threshold: %d",
				atsc3_route_object->tsi, atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count, expected_route_object_lct_packet_count_threshold);
		#endif
		return false;
	}

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
 */

bool atsc3_route_object_is_complete(atsc3_route_object_t* atsc3_route_object) {
	atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = NULL;
	bool has_missing_source_blocks = false;
	uint32_t last_object_position = 0;

	int atsc3_route_object_length_threshold = __MAX(1, (int)atsc3_route_object->object_length - 1500);

	if(atsc3_route_object->recovery_complete_timestamp) {
		_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: true, using recovery_complete_timestamp, atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d, recovery_complete_timestamp: %lu, final_object_recovery_filename_for_logging: %s",
						atsc3_route_object,
						atsc3_route_object->tsi, atsc3_route_object->toi,
						atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
						atsc3_route_object->recovery_complete_timestamp,
						atsc3_route_object->final_object_recovery_filename_for_logging);
		return true;
	}

	//short circuit in that our cumulative_lct_packet_len should be close to our object length
	if(atsc3_route_object->cumulative_lct_packet_len < atsc3_route_object_length_threshold) {
		#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
		_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: tsi: %d, toi: %d, pre-flight check with atsc3_route_object->atsc3_route_object_lct_packet_received_v.count: %d,  cumulative_lct_packet_len: %d, < atsc3_route_object_length_threshold: %d",
				atsc3_route_object->tsi, atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
				atsc3_route_object->cumulative_lct_packet_len,
				atsc3_route_object_length_threshold);
		#endif
		return false;
	}

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
#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
				_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: has_missing_source_blocks - using sbn_esi - tsi: %d, toi: %d, last_object_position: %d, esi: %d",
						atsc3_route_object->tsi, atsc3_route_object->toi, last_object_position, atsc3_route_object_lct_packet_received->esi);
#endif
			} else {
				last_object_position += atsc3_route_object_lct_packet_received->packet_len;
			}
		} else if(atsc3_route_object_lct_packet_received->use_start_offset) {
			if(last_object_position != atsc3_route_object_lct_packet_received->start_offset) {
				has_missing_source_blocks = true;
#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
				_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: has_missing_source_blocks - using start_offset - idx: %d, tsi: %d, toi: %d, last_object_position: %d, start_offset: %d",
										i, atsc3_route_object->tsi, atsc3_route_object->toi, last_object_position, atsc3_route_object_lct_packet_received->start_offset);
#endif
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
		_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: false, has_missing_source_blocks at end, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v: %d, last_object_position: %d, atsc3_route_object->object_length: %d",
				atsc3_route_object->tsi, atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
				last_object_position,
				atsc3_route_object->object_length);

		has_missing_source_blocks = true;
	} else {

		_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_is_complete: true, object_length: %8d,            atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d, recovery_complete_timestamp: %lu, final_object_recovery_filename_for_logging: %s",
						atsc3_route_object->object_length,
						atsc3_route_object,
						atsc3_route_object->tsi, atsc3_route_object->toi,
						atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
						atsc3_route_object->recovery_complete_timestamp,
						atsc3_route_object->final_object_recovery_filename_for_logging);
	}


	return !has_missing_source_blocks;
}


/*
 *	jjustman-2020-07-28 - atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received:
 *
 *		frees atsc3_route_bject_lct_packet_received objects, and removes as candidate from "given_up" eviction, but will NOT unlink temporary or final object recovery filenames
 *
 *		used for SLS flows
 *
*/

void atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received(atsc3_route_object_t* atsc3_route_object) {

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
	_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: before: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
#endif

	//jjustman-2020-08-04 - important, always call these two together..
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

	atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);

	atsc3_route_object->is_toi_init = false;
	atsc3_route_object->expected_route_object_lct_packet_count = 0;
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = 0;
	atsc3_route_object->cumulative_lct_packet_len = 0;
	atsc3_route_object_recovery_file_handle_close(atsc3_route_object);

	freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
	freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_eviction);


#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__

	_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: after: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
				atsc3_route_object,
				atsc3_route_object->tsi,
				atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
#endif

}


/*
 *	jjustman-2020-07-28 - atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received:
 *
 *		frees atsc3_route_bject_lct_packet_received objects, and removes as candidate from "given_up" eviction, and WILL unlink temporary/final object recovery on disk after
 *
 *		used for media fragment cleanup when 		            	atsc3_route_object_set_final_object_recovery_filename_for_eviction(atsc3_route_object, new_file_name); is set
 *
 *		invoked from atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects
*/

void atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received(atsc3_route_object_t* atsc3_route_object) {

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
	_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: before: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
#endif

	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: p: %p, tsi: %d, toi: %d, before closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d)",
			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->recovery_file_handle,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.size
	);


	atsc3_route_object_recovery_file_handle_close(atsc3_route_object);

	//jjustman-2020-08-04 - important, always call these two together..
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

	atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);

	atsc3_route_object->is_toi_init = false;
	atsc3_route_object->expected_route_object_lct_packet_count = 0;
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = 0;
	atsc3_route_object->cumulative_lct_packet_len = 0;


	//jjustman-2020-07-28: unlink temporary_object_recovery_filename
	if(atsc3_route_object->temporary_object_recovery_filename) {
		struct stat st = {0};
		if(stat(atsc3_route_object->temporary_object_recovery_filename, &st) == 0) {
			_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: removing temporary_object_recovery_filename: %s", atsc3_route_object->temporary_object_recovery_filename);
	        remove(atsc3_route_object->temporary_object_recovery_filename);
		}
		freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
	}

	//jjustman-2020-07-28: unlink final_object_recovery_filename_for_eviction
	if(atsc3_route_object->final_object_recovery_filename_for_eviction) {
		struct stat st = {0};
		if(stat(atsc3_route_object->final_object_recovery_filename_for_eviction, &st) == 0) {
			_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: removing final_object_recovery_filename_for_eviction: %s", atsc3_route_object->final_object_recovery_filename_for_eviction);
			remove(atsc3_route_object->final_object_recovery_filename_for_eviction);
		}
		freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_eviction);
	}

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__

	_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: after: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
				atsc3_route_object,
				atsc3_route_object->tsi,
				atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
#endif

	_ATSC3_ROUTE_OBJECT_DEBUG("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: p: %p, tsi: %d, toi: %d, after closing fp: %p, atsc3_route_object_lct_packet_received count: %d (size: %d), final_object_recovery_filename_for_logging: %s",
			atsc3_route_object,
			atsc3_route_object->tsi,
			atsc3_route_object->toi,
			atsc3_route_object->recovery_file_handle,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.count,
			atsc3_route_object->atsc3_route_object_lct_packet_received_v.size,
			atsc3_route_object->final_object_recovery_filename_for_logging
	);

	freeclean((void**)&atsc3_route_object->final_object_recovery_filename_for_logging);
}

void atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object_t* atsc3_route_object) {
	if(!atsc3_route_object) {
		return;
	}

	//clean up our stale avltree entries
	struct avltree_node* node;
	while((node = avltree_first(&atsc3_route_object->atsc3_route_object_lct_packet_received_tree))) {
		avltree_remove(node, &atsc3_route_object->atsc3_route_object_lct_packet_received_tree);
		atsc3_route_object_lct_packet_received_node_t *p = avltree_container_of(node, atsc3_route_object_lct_packet_received_node_t, node);
		freesafe((void*)p);
	}

	//clear out any remaining pointers just to be safe...does not alloc just sets everything to NULL
	avltree_init(&atsc3_route_object->atsc3_route_object_lct_packet_received_tree, atsc3_route_object_lct_packet_received_cmp_fn, 0);

}

