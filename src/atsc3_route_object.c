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

int _ROUTE_OBJECT_INFO_ENABLED = 1;
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

			freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
			freeclean((void**)&atsc3_route_object->final_object_recovery_filename);

			free(atsc3_route_object);
			atsc3_route_object = NULL;
		}
		*atsc3_route_object_p = NULL;
	}
}


void atsc3_route_object_set_temporary_object_recovery_filename_if_null(atsc3_route_object_t* atsc3_route_object, char* temporary_filename) {
	if(!atsc3_route_object->temporary_object_recovery_filename) {
		atsc3_route_object->temporary_object_recovery_filename = strdup(temporary_filename);
	}
}

void atsc3_route_object_clear_temporary_object_recovery_filename(atsc3_route_object_t* atsc3_route_object) {
	freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
}

void atsc3_route_object_set_final_object_recovery_filename(atsc3_route_object_t* atsc3_route_object, char* final_object_recovery_filename) {
	freeclean((void**)&atsc3_route_object->final_object_recovery_filename);
	atsc3_route_object->final_object_recovery_filename = strdup(final_object_recovery_filename);
}

void atsc3_route_object_set_object_recovery_complete(atsc3_route_object_t* atsc3_route_object) {
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->recovery_complete_timestamp = gtl();
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

bool atsc3_route_object_is_complete(atsc3_route_object_t* atsc3_route_object) {
	atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = NULL;
	bool has_missing_source_blocks = false;
	uint32_t last_object_position = 0;

	//short-circuit if we don't have at least 7/8 of route_object_lct_packets from estimate (-2 for single digit lct_packet counts that may get lost...)
	//or only do this calculation of expected_
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
				_ATSC3_ROUTE_OBJECT_TRACE("atsc3_route_object_is_complete: has_missing_source_blocks - using start_offset - idx: %d, tsi: %d, toi: %d, last_object_position: %d, start_offset: %d",
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

	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->expected_route_object_lct_packet_count = 0;
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = 0;

	freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
	freeclean((void**)&atsc3_route_object->final_object_recovery_filename);


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
 *		used for media fragment cleanup when 		            	atsc3_route_object_set_final_object_recovery_filename(atsc3_route_object, new_file_name); is set
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

	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->expected_route_object_lct_packet_count = 0;
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = 0;

	//jjustman-2020-07-28: unlink temporary_object_recovery_filename
	if(atsc3_route_object->temporary_object_recovery_filename) {
		FILE* f = atsc3_object_open(atsc3_route_object->temporary_object_recovery_filename);
		if(f) {
			fclose(f);
			_ATSC3_ROUTE_OBJECT_INFO("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: removing temporary_object_recovery_filename: %s", atsc3_route_object->temporary_object_recovery_filename);

	        remove(atsc3_route_object->temporary_object_recovery_filename);
		}
		freeclean((void**)&atsc3_route_object->temporary_object_recovery_filename);
	}

	//jjustman-2020-07-28: unlink final_object_recovery_filename
	if(atsc3_route_object->final_object_recovery_filename) {
		FILE* f = atsc3_object_open(atsc3_route_object->final_object_recovery_filename);
		if(f) {
			fclose(f);

			_ATSC3_ROUTE_OBJECT_INFO("atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received: removing final_object_recovery_filename: %s", atsc3_route_object->final_object_recovery_filename);

			remove(atsc3_route_object->final_object_recovery_filename);
		}
		freeclean((void**)&atsc3_route_object->final_object_recovery_filename);
	}

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__

	_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received: after: atsc3_route_object: %p, tsi: %d, toi: %d, atsc3_route_object_lct_packet_received_v.count: %d",
				atsc3_route_object,
				atsc3_route_object->tsi,
				atsc3_route_object->toi,
				atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
#endif

}

