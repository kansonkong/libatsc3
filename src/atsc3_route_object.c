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
	atsc3_route_object->recovery_file_buffer_position = -1;

	return atsc3_route_object;
}

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
			atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object);

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

void atsc3_route_object_lct_packet_received_free(atsc3_route_object_lct_packet_received_t** atsc3_route_object_lct_packet_received_p) {
	if(atsc3_route_object_lct_packet_received_p) {
		atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = *atsc3_route_object_lct_packet_received_p;
		if(atsc3_route_object_lct_packet_received) {
			if(atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist) {
				block_Destroy(&atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist);
			}

			free(atsc3_route_object_lct_packet_received);
			atsc3_route_object_lct_packet_received = NULL;
		}
		*atsc3_route_object_lct_packet_received_p = NULL;
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

char* atsc3_route_object_get_temporary_object_recovery_filename_strdup(atsc3_route_object_t* atsc3_route_object) {
	char* temporary_filename = NULL;
	if(atsc3_route_object->temporary_object_recovery_filename) {
		temporary_filename = strdup(atsc3_route_object->temporary_object_recovery_filename);
	}

	return temporary_filename;
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


bool atsc3_route_object_lct_packet_received_promote_atsc3_alc_packet_alc_payload_to_pending_block(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* alc_packet) {
	if(!atsc3_route_object_lct_packet_received || !alc_packet || !alc_packet->alc_payload || !alc_packet->alc_len) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_lct_packet_received_promote_atsc3_alc_packet_alc_payload_to_pending_block: can't create pending_alc_payload_to_persist, atsc3_route_object_lct_packet_received: %p, alc_packet: %p",
				atsc3_route_object_lct_packet_received,
				alc_packet);
		return false;
	}

	if(atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_lct_packet_received_promote_atsc3_alc_packet_alc_payload_to_pending_block: pre-existing pending_alc_payload_to_persist, destroying block_t, atsc3_route_object_lct_packet_received: %p,  pending_alc_payload_to_persist: %p, size: %d",
				atsc3_route_object_lct_packet_received,
				atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist,
				atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist->p_size);

		block_Destroy(&atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist);
	}

	atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist = block_Duplicate_from_ptr(alc_packet->alc_payload, alc_packet->alc_len);

	return true;
}
//
//bool atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* alc_packet) {
//	if(!atsc3_route_object) {
//		return false;
//	}
//
//	if(alc_packet->use_sbn_esi) {
//		//jjustman-2020-08-05: TODO - for raptorQ fec - atsc3_route_object_repair_symbol_add(...) for this alc_packet
//		//bail
//		return false;
//	} else if(alc_packet->use_start_offset) {
//		if(atsc3_route_object->recovery_file_buffer_position == -1 || !atsc3_route_object->recovery_file_buffer) {
//
//			//we have to force-destroy this in-flight block
//			//jjustman-2020-08-05 - log this
//			if(atsc3_route_object->recovery_file_buffer) {
//				block_Destroy(&atsc3_route_object->recovery_file_buffer);
//			}
//
//			//borrowed from atsc3_route_object_set_object_recovery_complete
//			atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
//			atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
//			atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);
//			atsc3_route_object->cumulative_lct_packet_len = 0;
//			atsc3_route_object->recovery_file_buffer_position = -1;
//
//			if(!atsc3_route_object->object_length) {
//				//bail early
//				return false;
//			}
//
//			uint32_t recovery_file_buffer_chunk_position = (alc_packet->start_offset / __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_BYTES__) * __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_BYTES__;
//			atsc3_route_object->recovery_file_buffer  = recovery_file_buffer_chunk_position;
//
//			uint32_t block_size_to_alloc = __MIN(__ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_BYTES__, atsc3_route_object->object_length - alc_packet->start_offset);
//			return false;
//		}
//		//otherwise, we should have
//		return true;
//	} else {
//		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_route_object: %p, tsi: %d, toi: %d, alc_packet: %p is not use_sbn_esi AND not use_start_offset?!",
//							atsc3_route_object,
//							atsc3_route_object->tsi,
//							atsc3_route_object->toi,
//							alc_packet);
//
//		return false;
//	}
//}



//
//
///*
// *
// * jjustman-2020-08-05 - chunked flushing of contigious atsc3_route_object_lct_packet_received block_t*
// * 	 merge and block_write to temporary_object_recovery filename rather than large block_alloc and fseek/fwrite for every lct packet
// *
// * TODO: buffer up to __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_BYTES__  of block_t before flushing, avoiding most small pre_allocation for small objects
// *
// * object is then flushed and closed to disk with atsc3_route_object_recovery_file_handle_flush_and_close
// *
// *
// * first pass: only dump our full payload to disk if we are logically "complete"
// */
//
//int atsc3_route_object_persist_recovery_block_from_lct_packet_vector(atsc3_route_object_t* atsc3_route_object) {
//	int bytesWritten = 0;
//	char* temporary_recovery_filename = NULL;
//	bool is_source_symbol = false;
//
//	if(!atsc3_route_object || !atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received) {
//		_ATSC3_SLS_ALC_FLOW_WARN("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: atsc3_route_object: %p, most_recent_atsc3_route_object_lct_packet_received is NULL, discarding!", atsc3_route_object);
//		return -2;
//	}
//
//
//
//	is_source_symbol = atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object, alc_packet);
//
//	//if we are a repair symbol, skip source symbol recovery and defer processing for raptorQ
//	if(!is_source_symbol) {
//		return 0;
//	}
//
//	int32_t block_remaining_size = block_Remaining_size(atsc3_route_object->recovery_file_buffer);
//
//	//compute gap between atsc3_route_object->recovery_file_buffer_position and our start_offset
//	int32_t relative_buffer_write_position = alc_packet->start_offset - atsc3_route_object->recovery_file_buffer_position;
//	int32_t updated_file_buffer_position_after_write = alc_packet->start_offset + alc_packet->alc_len;
//	int32_t required_buffer_size = updated_file_buffer_position_after_write - atsc3_route_object->recovery_file_buffer_position;
//
//	if(relative_buffer_write_position >= 0) { //this is an "append"
//
//		if(block_remaining_size >= required_buffer_size) {		//if we have enough space, write to our block_t
//			block_Seek(atsc3_route_object->recovery_file_buffer, relative_buffer_write_position);
//			block_Write(atsc3_route_object->recovery_file_buffer, alc_packet->alc_payload, alc_packet->alc_len);
//			//note: only set atsc3_route_object->recovery_file_buffer_position) after we have flushed out to disk
//
//			_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: calling block_Write with alc_packet, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, atsc3_route_object->recovery_file_buffer_position: %d",
//					alc_packet->def_lct_hdr->tsi,
//					alc_packet->def_lct_hdr->toi,
//					alc_packet->start_offset,
//					alc_packet->alc_len,
//					block_Remaining_size(atsc3_route_object->recovery_file_buffer),
//					atsc3_route_object->recovery_file_buffer_position);
//
//
//		} else {
//			//otherwise flush out disk and create new recovery_file_buffer and write our alc_packet
//			_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: flushing and reallocing recovery_file_buffer, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, required_buffer_size size: %d, alc_len: %d",
//								alc_packet->def_lct_hdr->tsi,
//								alc_packet->def_lct_hdr->toi,
//								alc_packet->start_offset,
//								alc_packet->alc_len,
//								block_Remaining_size(atsc3_route_object->recovery_file_buffer),
//								required_buffer_size,
//								alc_packet->alc_len);
//
//			bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
//			atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object, alc_packet);
//			block_Write(atsc3_route_object->recovery_file_buffer, alc_packet->alc_payload, alc_packet->alc_len);
//			atsc3_route_object->recovery_file_buffer_position = updated_file_buffer_position_after_write;
//
//		}
//	} else if(relative_buffer_write_position < 0) {
//		//we might have a carousel packet, but cheat for our flush to disk
//		_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: buffer_gap_size < 0! flushing and reallocing recovery_file_buffer, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, relative_buffer_write_position: %d, alc_len: %d",
//										alc_packet->def_lct_hdr->tsi,
//										alc_packet->def_lct_hdr->toi,
//										alc_packet->start_offset,
//										alc_packet->alc_len,
//										block_Remaining_size(atsc3_route_object->recovery_file_buffer),
//										relative_buffer_write_position,
//										alc_packet->alc_len);
//
//		bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
//		//hack...!  let fseek do the hard work
//		atsc3_route_object->recovery_file_buffer = block_Duplicate_from_ptr(alc_packet->alc_payload, alc_packet->alc_len);
//		atsc3_route_object->recovery_file_buffer_position = alc_packet->start_offset;
//		bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
//	}
//
//	return bytesWritten;
//}


//REQUIRED: must have previously called atsc3_route_object_is_complete() to invoked qsort against
//	qsort((void**)atsc3_route_object->atsc3_route_object_lct_packet_received_v.data, atsc3_route_object->atsc3_route_object_lct_packet_received_v.count, sizeof(atsc3_route_object_lct_packet_received_t**), atsc3_route_object_lct_packet_received_generic_sbn_start_offset_comparator);

int atsc3_route_object_persist_recovery_buffer_all_pending_lct_packet_vector(atsc3_route_object_t* atsc3_route_object) {
	int recovery_rebuilt_payload_size = 0;
	atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = NULL;

	if(atsc3_route_object->recovery_file_buffer) {
		block_Destroy(&atsc3_route_object->recovery_file_buffer);
	}

	if(!atsc3_route_object->object_length) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_persist_recovery_buffer_all_pending_lct_packet_vector: atsc3_route_object: %p, object_length is invalid: %d",
				atsc3_route_object,
				atsc3_route_object->object_length);
		return -1;
	}

	atsc3_route_object->recovery_file_buffer = block_Alloc(atsc3_route_object->object_length);
	atsc3_route_object->recovery_file_buffer_position = 0;

	//TODO:  check to make sure that our atsc3_route_objec passed atsc3_route_object_is_complete()

	//step 1 - rebuild our atsc3_route_object->recovery_file_buffer
	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count; i++) {
		atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];
		if(atsc3_route_object_lct_packet_received->use_start_offset && atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist) {
			block_Seek(atsc3_route_object->recovery_file_buffer, atsc3_route_object_lct_packet_received->start_offset);
			block_AppendFull(atsc3_route_object->recovery_file_buffer, atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist);
			recovery_rebuilt_payload_size += atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist->p_size;


		} else {
			_ATSC3_ROUTE_OBJECT_ERROR("atsc3_route_object_persist_recovery_buffer_all_pending_lct_packet_vector: i: %d, atsc3_route_object_lct_packet_received: %p, ERROR: use_start_offset: %d, atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist: %p",
					i,
					atsc3_route_object_lct_packet_received,
					atsc3_route_object_lct_packet_received->use_start_offset,
					atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist);

		}
	}

	//step 2 - free our atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist entries

	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count; i++) {
		atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];
		if(atsc3_route_object_lct_packet_received->use_start_offset && atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist) {
			block_Destroy(&atsc3_route_object_lct_packet_received->pending_alc_payload_to_persist);
		}
	}

	return recovery_rebuilt_payload_size;

}

int64_t atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object_t* atsc3_route_object) {
	int64_t bytes_written = 0;
	int res = 0;
	FILE* fp = NULL;

	if(!atsc3_route_object || !atsc3_route_object->recovery_file_buffer || !atsc3_route_object->recovery_file_buffer->p_buffer || !atsc3_route_object->recovery_file_buffer->p_size) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_route_object: %p, recovery_file_buffer is invalid!",
							atsc3_route_object);
		bytes_written = -4;
		goto free_recovery_file_buffer;
	}

	if(!atsc3_route_object->temporary_object_recovery_filename || atsc3_route_object->recovery_file_buffer_position < 0) {
		_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_route_object: %p, temporary_object_recovery_filename is NULL!, recovery_file_buffer_position is: %lld",
							atsc3_route_object,
							atsc3_route_object->recovery_file_buffer_position);
		bytes_written = -3;
		goto free_recovery_file_buffer;
	}

	if(atsc3_route_object->recovery_file_handle) {
		int res = 0;
		//try a sanity check for seeking to 0
		res = fseek(atsc3_route_object->recovery_file_handle, 0, SEEK_SET);

		if(!res) {
			fp = atsc3_route_object->recovery_file_handle;
		} else {
			_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_route_object: %p, atsc3_route_object->recovery_file_handle: %p,  fseek to (0, SEEK_SET) returned error: %d",
								atsc3_route_object,
								atsc3_route_object->recovery_file_handle,
								res);
			fclose(atsc3_route_object->recovery_file_handle);
			atsc3_route_object->recovery_file_handle = NULL;
		}
	}

	if(!fp) {
		//make sure we have our dump output path
		struct stat st = {0};

		if(stat(__ALC_DUMP_OUTPUT_PATH__, &st) == -1) {
			mkdir(__ALC_DUMP_OUTPUT_PATH__, 0777);
		}

		fp = atsc3_object_open(atsc3_route_object->temporary_object_recovery_filename);
		if(!fp) {
			_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_alc_object_open:: atsc3_route_object: %p, atsc3_route_object->temporary_object_recovery_filename: %s failed",
								atsc3_route_object,
								atsc3_route_object->temporary_object_recovery_filename);
			bytes_written = -2;
			goto free_recovery_file_buffer;
		} else {
			atsc3_route_object->recovery_file_handle = fp;
		}
	}


	res = fseek(fp, atsc3_route_object->recovery_file_buffer_position, SEEK_SET);
	if(!res) {
		res = fwrite(atsc3_route_object->recovery_file_buffer->p_buffer, atsc3_route_object->recovery_file_buffer->p_size, 1, fp);
		if(res != 1) {
			_ATSC3_ROUTE_OBJECT_WARN("atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position: atsc3_route_object: %p, atsc3_route_object->recovery_file_handle: %p, fwrite of %d bytes failed from block_t: %p - res: %d",
							atsc3_route_object,
							atsc3_route_object->recovery_file_handle,
							atsc3_route_object->recovery_file_buffer->p_size,
							atsc3_route_object->recovery_file_buffer->p_buffer,
							res);
			fclose(fp);
			fp = NULL;
			atsc3_route_object->recovery_file_handle = NULL;

			bytes_written = -1;
			goto free_recovery_file_buffer;
		} else {
			atsc3_route_object->recovery_file_buffer_position = (uint32_t) ftell(fp);
			bytes_written = atsc3_route_object->recovery_file_buffer->p_size;
			block_Destroy(&atsc3_route_object->recovery_file_buffer);
			goto done;
		}
	}

free_recovery_file_buffer:
	if(atsc3_route_object->recovery_file_buffer) {
		block_Destroy(&atsc3_route_object->recovery_file_buffer);
	}
	//borrowed from atsc3_route_object_set_object_recovery_complete
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;
	atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);

	atsc3_route_object->recovery_file_buffer_position = -1;

	//hack, if we wern't able to flush this buffer to disk, clear out our

done:
	return bytes_written;
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
	atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object);

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

void atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object_t* atsc3_route_object) {
	if(atsc3_route_object && atsc3_route_object->recovery_file_buffer) {
		atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
	}
	
	//just to be safe
	if(atsc3_route_object->recovery_file_buffer) {
		block_Destroy(&atsc3_route_object->recovery_file_buffer);
	}

	atsc3_route_object->recovery_file_buffer_position = -1;
	if(atsc3_route_object->recovery_file_handle) {
		fclose(atsc3_route_object->recovery_file_handle);
		atsc3_route_object->recovery_file_handle = NULL;
	}
}

void atsc3_route_object_recovery_file_handle_abandon_and_close(atsc3_route_object_t* atsc3_route_object) {
	if(atsc3_route_object->recovery_file_buffer) {
		block_Destroy(&atsc3_route_object->recovery_file_buffer);
	}
	atsc3_route_object->recovery_file_buffer_position = -1;

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

	atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object);

	//jjustman-2020-08-04 - important, always call these two together..
	atsc3_route_object_free_atsc3_route_object_lct_packet_received(atsc3_route_object);
	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = NULL;

	atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object);

	atsc3_route_object->is_toi_init = false;
	atsc3_route_object->expected_route_object_lct_packet_count = 0;
	atsc3_route_object->expected_route_object_lct_packet_len_for_count = 0;
	atsc3_route_object->cumulative_lct_packet_len = 0;


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


	atsc3_route_object_recovery_file_handle_abandon_and_close(atsc3_route_object);

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


/* jjustman-2019-09-17: TODO - free temporary filename when done */

char* alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow_t *udp_flow, atsc3_alc_packet_t *alc_packet) {
	char* temporary_file_name = (char *)calloc(256, sizeof(char));
	if(alc_packet->def_lct_hdr) {
		snprintf(temporary_file_name, 255, "%s%u.%u.%u.%u.%u.%u-%u.recovering",
			__ALC_DUMP_OUTPUT_PATH__,
			__toipandportnonstruct(udp_flow->dst_ip_addr, udp_flow->dst_port),
			alc_packet->def_lct_hdr->tsi,
			alc_packet->def_lct_hdr->toi);
	}

	return temporary_file_name;
}

