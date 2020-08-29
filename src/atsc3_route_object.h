/*
 * atsc3_route_object.h
 *
 *  Created on: Jul 27, 2020
 *      Author: jjustman
 *
 *
 	 	 A/331 A3.10.2 Basic Delivery Object Recovery

	 	 The ROUTE receiver continuously acquires packet payloads for the object as long as all of the following conditions are satisfied:
             i) there is at least one entry in RECEIVED still set to false;
             ii) the object has not yet expired; and
             iii) the application has not given up on reception of this object.
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include <search.h>

#ifndef ATSC3_ROUTE_OBJECT_H_
#define ATSC3_ROUTE_OBJECT_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_listener_udp.h"
#include "atsc3_alc_rx.h"


#include "libtree.h"

#if defined (__cplusplus)
extern "C" {
#endif


//ALC dump object output path
#define __ALC_DUMP_OUTPUT_PATH__ "route/"

#define __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_FLUSH_BYTES__ 	(1500 * 32) //48KB


/*
 * atsc3_route_object_lct_packet_received: keep track of this object's received lct packets
 * 		for object recovery and completion tracking
 *
 * 		should look similar to atsc3_alc_packet_t
 *
 *
 *
  	 A/331/2020 A3.10.2:
	 	 The ROUTE receiver allocates a Boolean array RECEIVED[0..T-1] or RECEIVED[0..T’-1], as appropriate, with all entries initialized to false to track
	 	 received object symbols.

	 jjustman: instead, we will keep a collection of our LCT packet headers, for the following use cases:

	  1.) for any sparse gaps for NRT recovery with carousel
	  2.) raptorQ SBN+ESI for recovery packet collection
	  3.) incomplete tracking of ROUTE/DASH media fragments
	  4.) purging of t-30s ROUTE/DASH media fragements for local on-disk cache management


	 The ROUTE receiver continuously acquires packet payloads for the object as long as all of the following conditions are satisfied:

		i) there is at least one entry in RECEIVED still set to false;
		ii) the object has not yet expired; and
		iii) the application has not given up on reception of this object. More details are provided below.


		todo: sort by start_offset?
*/

typedef struct atsc3_route_object_lct_packet_received {
	long					first_received_timestamp;		//populate with gtl();

	uint32_t				carousel_count;					//0
	long					most_recent_received_timestamp;	//populate with gtl();

	uint8_t					codepoint;
	uint8_t 				fec_encoding_id; 	//jjustman-2020-07-27 - todo: extract from fec building block?

	bool					use_sbn_esi;		//for fec_encoding_id=6 - raptorQ
	uint8_t					sbn;
	uint32_t				esi:24;
	uint32_t				sbn_esi_merged;		//combined for tsearch

	bool					use_start_offset;	//for all other fec_encoding_id's
	uint32_t				start_offset;

    uint8_t 				close_object_flag;
    uint8_t 				close_session_flag;

    bool 	 				ext_route_presentation_ntp_timestamp_set;
    uint64_t				ext_route_presentation_ntp_timestamp;

	unsigned int 			packet_len;
	unsigned long long 		object_len;

	block_t*				pending_alc_payload_to_persist; 	//promote the atsc3_alc_packet->alc_payload to block_t so we can flush to disk,
																//perform sparse merge to reduce fseek/fwrite calls when flushing out threshold

} atsc3_route_object_lct_packet_received_t;

typedef struct atsc3_sls_alc_flow atsc3_sls_alc_flow_t;

typedef struct atsc3_route_object_lct_packet_received_node {
        uint32_t key;
        atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received;
        struct avltree_node node;
} atsc3_route_object_lct_packet_received_node_t;

typedef struct atsc3_route_object {
	atsc3_sls_alc_flow_t*	atsc3_sls_alc_flow;

	uint32_t				tsi;											//keep reference for our tsi / toi just to be sure...
	uint32_t				toi;
	bool					is_toi_init;

	block_t*				recovery_file_buffer;							//in-flight recovery buffer of block_t pre-alloc for alc packet re-aggregation
	int64_t					recovery_file_buffer_position;					//position in our file_handle of where the current recovery_file_buffer (e.g. mmap) should be written to
																			//-1 means its a new allocation - we do not have a reference to where in the recovery_file_handle

	//jjustman-2020-08-05 - TODO for RaptorQ recovery_repair_symbols_v 		if(atsc3_route_object_lct_packet_received->use_sbn_esi) atsc3_route_object_repair_symbol_add(...)

	FILE*					recovery_file_handle;							//keep tracek of our recovery file handle instead of fopen/fclose on every lct packet

	char*					temporary_object_recovery_filename; 			//temporary reference so we can remove from on-disk if we end up being marked as 'given up'
	char*					final_object_recovery_filename_for_eviction;	//filename of the completed recovery route object for eventual reaping...
	char*					final_object_recovery_filename_for_logging;		//filename of the completed recovery route object for eventual reaping...

	uint32_t 				object_length;									//persisted object_length (if known)
	uint32_t				cumulative_lct_packet_len;						//alternative strategy for atsc3_route_object_is_complete pre-flight check

	uint32_t				expected_route_object_lct_packet_count; 		//guesstimate of ( object_length / packet_len ) +1
	uint32_t				expected_route_object_lct_packet_len_for_count; //pin and recompute if packet_len > expected_route_object_lct_packet_len_for_count

	uint32_t				expiration;
	bool					has_given_up;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_object_lct_packet_received);
	struct avltree			atsc3_route_object_lct_packet_received_tree;

	atsc3_route_object_lct_packet_received_t* 	most_recent_atsc3_route_object_lct_packet_received;

	long 					recovery_complete_timestamp; 					//gtl() for when atsc3_route_object_set_object_recovery_complete was invoked on this object


} atsc3_route_object_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_object, atsc3_route_object_lct_packet_received);
ATSC3_VECTOR_BUILDER_METHODS_PARENT_INTERFACE_FREE(atsc3_route_object);

void atsc3_route_object_add_atsc3_route_object_lct_packet_len(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received);
void atsc3_route_object_set_is_toi_init_object(atsc3_route_object_t* atsc3_route_object, bool is_toi_init);

void atsc3_route_object_set_temporary_object_recovery_filename_if_null(atsc3_route_object_t* atsc3_route_object, char* temporary_filename);
char* atsc3_route_object_get_temporary_object_recovery_filename_strdup(atsc3_route_object_t* atsc3_route_object);

void atsc3_route_object_clear_temporary_object_recovery_filename(atsc3_route_object_t* atsc3_route_object);
void atsc3_route_object_set_final_object_recovery_filename_for_eviction(atsc3_route_object_t* atsc3_route_object, char* final_object_recovery_filename_for_eviction);
void atsc3_route_object_set_final_object_recovery_filename_for_logging(atsc3_route_object_t* atsc3_route_object, char* final_object_recovery_filename_for_eviction);

bool atsc3_route_object_lct_packet_received_promote_atsc3_alc_packet_alc_payload_to_pending_block(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* alc_packet);

//flush out to disk a block size of __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_FLUSH_BYTES__
//int atsc3_route_object_persist_recovery_block_from_lct_packet_vector(atsc3_route_object_t* atsc3_route_object);
int atsc3_route_object_persist_recovery_buffer_all_pending_lct_packet_vector(atsc3_route_object_t* atsc3_route_object);


//bool atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* alc_packet);


int64_t atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object_t* atsc3_route_object);

void atsc3_route_object_set_object_recovery_complete(atsc3_route_object_t* atsc3_route_object);

void atsc3_route_object_recovery_file_handle_assign(atsc3_route_object_t* atsc3_route_object, FILE* recovery_file_handle);
void atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object_t* atsc3_route_object);
void atsc3_route_object_recovery_file_handle_abandon_and_close(atsc3_route_object_t* atsc3_route_object);

void atsc3_route_object_calculate_expected_route_object_lct_packet_count(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received);
bool atsc3_route_object_is_complete(atsc3_route_object_t* atsc3_route_object);

void atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received(atsc3_route_object_t* atsc3_route_object);

//used in atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects, unlink abandonded / stale objects from disk after N seconds
void atsc3_route_object_reset_and_free_and_unlink_recovery_file_atsc3_route_object_lct_packet_received(atsc3_route_object_t* atsc3_route_object);
void atsc3_route_object_free_lct_packet_received_tree(atsc3_route_object_t* atsc3_route_object);

//jjustman-2020-08-05 - slight refactoring...
char* alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow_t *udp_flow, atsc3_alc_packet_t *alc_packet);

#define _ATSC3_ROUTE_OBJECT_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ROUTE_OBJECT_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);;
#define _ATSC3_ROUTE_OBJECT_INFO(...)    if(_ROUTE_OBJECT_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ROUTE_OBJECT_DEBUG(...)   if(_ROUTE_OBJECT_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_ROUTE_OBJECT_TRACE(...)   if(_ROUTE_OBJECT_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_ROUTE_OBJECT_H_ */
