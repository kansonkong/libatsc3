/*
 * atsc3_mmtp_parser.h
 *
 *  Created on: Jan 3, 2019
 *      Author: jjustman
 */

#ifndef MODULES_DEMUX_MMT_MMTP_PARSER_H_
#define MODULES_DEMUX_MMT_MMTP_PARSER_H_

#include <assert.h>
#include <limits.h>

#include "atsc3_logging_externs.h"
#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_listener_udp.h"
#include "atsc3_mmt_signalling_message.h"
#include "atsc3_mmtp_packet_types.h"

#define MIN_MMTP_SIZE 32
#define MAX_MMTP_SIZE 1514


/*
 * MMTP as specified in Clause 9 of ISO/IEC23008-1 [37] shall be used to deliver MPUs. The following constraints shall be applied to MMTP:
   The value of the version field of MMTP packets shall be '01'.
 */

#define _ISO23008_1_MMTP_VERSION_0x00_SUPPORT_ false


/*
 * The value of the packet_id field of MMTP packets shall be between 0x0010 and 0x1FFE for easy conversion of an MMTP stream to an MPEG-2 TS.
 * The value of the packet_id field of MMTP packets for each content component can be randomly assigned, but the value of the packet_id field of
 * MMTP packets carrying two different components shall not be the same in a single MMTP session.
 *
 * See A/331 Section 7.2.3 for exceptions regarding signaling flow for SLS which shall be 0x0000
 *
 * korean samples have this defect
 */

//#define _ATSC3_MMT_PACKET_ID_MPEGTS_COMPATIBILITY_ true

/*
 * The value of ‘0x01’ for the type field of an MMTP packet header shall not be used, i.e. the Generic File Delivery (GFD) mode specified in
 * subclauses 9.3.3 and 9.4.3 of [37] shall not be used.
 *
 * jjustman-2019-02-05 - I think this was a poor choice for real-time interactive object delivery use cases and experiences.
 */
#define _ISO230081_1_MMTP_GFD_SUPPORT_ false

/**
 *
 * MMTP packet parsing
 *
 *
 * mmtp_packet_header_parse_from_block_t: parse a full udp datagram into its applicable mmtp_payload_type:
 *
 * 	packet header cast for determining payload type:
 *
 * 		if(mmtp_packet_header->mmtp_payload_type == 0x0) {
 *
 * 	supported types:
 *
 * 		MPU=0x0					mmtp_mpu_type_packet_header
 * 	 	 (mpu_timed_flag==1)	mpu_data_unit_payload_fragments_timed
 * 	 	 (mpu_timed_flag==0)	mpu_data_unit_payload_fragments_nontimed
 *
 * 		signaling message=0x2	mmtp_signalling_message_fragments
 *
 *

 *
 * packet types not supported:
 *
 *		generic_object=0x1 (restricted usage in atsc3 in favor of ROUTE)
 * 		repair_signal=0x3 are not supported
 *
 */


#if defined (__cplusplus)
extern "C" {
#endif

mmtp_packet_header_t* mmtp_packet_header_parse_from_block_t(block_t* udp_packet);
void mmtp_packet_header_dump(mmtp_packet_header_t* mmtp_packet_header);

/**
 * internal packet handling methods below, you probably don't want to invoke these...
 */


//returns pointer from udp_raw_buf where we completed header parsing

//TODO: purge
////think of this as castable to the base fields as they are the same size layouts
//mmtp_payload_fragments_union_t* mmtp_packet_create(block_t * raw_packet,
//												uint8_t mmtp_packet_version,
//												uint8_t mmtp_payload_type,
//												uint16_t mmtp_packet_id,
//												uint32_t packet_sequence_number,
//												uint32_t packet_counter,
//												uint32_t mmtp_timestamp);
/**
 * mmtp sub_flow vector management for re-assembly
 */
//
//mmtp_sub_flow_t* mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector_t *vec, udp_flow_t* udp_flow, uint16_t mmtp_packet_id);
//mmtp_sub_flow_t* mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector_t *vec, udp_flow_t* udp_flow, uint16_t mmtp_packet_id);
//
//void mmtp_sub_flow_push_mmtp_packet(mmtp_sub_flow_t *mmtp_sub_flow, mmtp_payload_fragments_union_t *mmtp_packet);
//void mmtp_sub_flow_remove_mmtp_packet(mmtp_sub_flow_t *mmtp_sub_flow, mmtp_payload_fragments_union_t *mmtp_packet);


#if defined (__cplusplus)
}
#endif

#define __MMTP_PARSER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_PARSER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_PARSER_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_PARSER_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_PARSER_TRACE(...)   if(_MMTP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif /* MODULES_DEMUX_MMT_MMTP_PARSER_H_ */
