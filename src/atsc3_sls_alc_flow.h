/*
 * atsc3_sls_alc_flow.h
 *
 *  Created on: Jul 28, 2020
 *      Author: jjustman
 */

#include <search.h>

#ifndef ATSC3_SLS_ALC_FLOW_H_
#define ATSC3_SLS_ALC_FLOW_H_


#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_listener_udp.h"
#include "atsc3_alc_rx.h"
#include "atsc3_sls_metadata_fragment_types.h"
#include "atsc3_route_object.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
	atsc3_sls_alc_flow: ROUTE object data pertaining to a TSI (transport stream id)

	note: route_s_tsid metadata fragments are transient - e.g. subject to release in the atsc3_route_sls_processor via:
			atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);

 */

typedef struct atsc3_sls_alc_flow {

	uint32_t 					tsi;			//transport stream ID

	//delayed objects...
	uint32_t					sls_toi;		//keep track of the SLS TOI for version and format changes - see A/331:2020 - Annex C: Filtering for Signaling Fragments
	uint8_t						s_tsid_version;	//keep track of the mbms-envelope for s-tsid version changes

	atsc3_route_s_tsid_RS_LS_t* 								atsc3_route_s_tsid_RS_LS;  //pin to our s-tsid RS_LS reference

	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* 	media_info;	//optional


	/* jjustman-2020-07-27 - todo: deprecate these */

	uint32_t	last_closed_toi;	//for media fragments, keep track of the last closed fragment for mpd patching
	uint32_t	toi_init;			//ignore the init fragment toi when performing mpd patching

	//only for NRT payloads
//	char*		fdt_file_content_type;
//	uint32_t 	toi_init; 			//init toi fragment (if applicable for RT media)
//	uint32_t 	toi_init_length;	//init toi fragement length (if applicable for RT media)
//
//	uint32_t	toi;					//current toi fragment OR nrt (if known)
//	uint32_t 	toi_length;				//current toi fragment OR nrt length (if known)
//
//	uint32_t	last_inflight_toi;			//last toi fragment OR nrt (if known)
//	uint32_t 	last_inflight_toi_length;	//last toi fragment OR nrt length (if known)
//
//	uint32_t	last_closed_toi;			//last closed toi fragment OR nrt (if known)
//	uint32_t 	last_closed_toi_length;	//last closed toi fragment OR nrtlength (if known)

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_object);

} atsc3_sls_alc_flow_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_sls_alc_flow, atsc3_route_object);

ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT(atsc3_sls_alc_flow);
ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_INTERFACE(atsc3_sls_alc_flow);

void atsc3_sls_alc_flow_typedef_free(atsc3_sls_alc_flow_t** atsc3_sls_alc_flow_p);
//used for RT media fragment delivery (e.g. codepoint=8)

//jjustman-2020-07-14 - TODO: make sure when this monitor is torn down,
//void atsc3_sls_alc_flow_free(atsc3_sls_alc_flow_t** atsc3_sls_alc_flow_p) {
//is invoked...


//jjustman-2020-07-28 - add a reference for our alc_packet_t* recv'd into the applicable atsc3_sls_alc_flow
//TODO: scope with sls_toi and s_tsid_version
atsc3_route_object_t* atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* alc_packet);
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_or_create_entry_from_alc_packet(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* alc_packet);
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_from_alc_packet(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* alc_packet);

atsc3_route_object_t* atsc3_sls_alc_flow_find_or_create_route_object_from_alc_packet(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* alc_packet);
atsc3_route_object_t* atsc3_sls_alc_flow_find_route_object_entry_from_alc_packet(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* alc_packet);


//jjustman-2020-07-28 -todo: refactor into atsc3_sls_alc_flow_route_object_utils class?
void atsc3_route_object_set_alc_flow_and_tsi_toi(atsc3_route_object_t* atsc3_route_object, atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* atsc3_alc_packet);

atsc3_route_object_lct_packet_received_t* atsc3_route_object_add_or_update_lct_packet_received(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* atsc3_alc_packet);
atsc3_route_object_lct_packet_received_t* atsc3_route_object_find_lct_packet_received(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* atsc3_alc_packet);

//for tsearch/tfind with atsc3_route_object->atsc3_route_object_lct_packet_received_tree_root_p
int atsc3_route_object_lct_packet_compare_sbn_esi_merged(const void *pa, const void *pb);
int atsc3_route_object_lct_packet_compare_start_offset(const void *pa, const void *pb);

void atsc3_route_object_lct_packet_received_set_source_attributes_from_alc_packet(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet);
void atsc3_route_object_lct_packet_received_set_attributes_from_alc_packet(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet);
void atsc3_route_object_lct_packet_received_update_carousel_count(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet);

//update our route object with persisted length (if present)
void atsc3_route_object_lct_packet_received_update_atsc3_route_object(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received);

//for matching contentInfo.mediaInfo@repId
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info);
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info);
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init);

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi);

//TODO: replace these with proper atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t setter / cleanup

//void atsc3_sls_alc_flow_set_rep_id_if_null(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, char* rep_id);
//void atsc3_sls_alc_flow_set_lang_if_null(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, char* lang);

//used for NRT package delivery (e.g. codepoint=1/2/3/4)
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_nrt(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi);
void atsc3_sls_alc_flow_nrt_set_fdt_file_content_type_if_null(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, char* fdt_file_content_type);

uint32_t atsc3_sls_alc_flow_get_first_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow);
uint32_t atsc3_sls_alc_flow_get_last_closed_toi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow);
uint32_t atsc3_sls_alc_flow_get_first_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow);


#define _ATSC3_SLS_ALC_FLOW_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_SLS_ALC_FLOW_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);;
#define _ATSC3_SLS_ALC_FLOW_INFO(...)    if(_SLS_ALC_FLOW_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_SLS_ALC_FLOW_DEBUG(...)   if(_SLS_ALC_FLOW_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_SLS_ALC_FLOW_TRACE(...)   if(_SLS_ALC_FLOW_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_SLS_ALC_FLOW_H_ */
