/*
 * atsc3_mmt_mpu_parser.h
 *
 *  Created on: Jan 26, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_MPU_PARSER_H_
#define ATSC3_MMT_MPU_PARSER_H_

#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_packet_types.h"

//packet type=v0/v1 have an upper bound of ~1432
#define UPPER_BOUND_MPU_FRAGMENT_SIZE 1432
#define MPU_REASSEMBLE_MAX_BUFFER 8192000
#define MIN(a,b) (((a)<(b))?(a):(b))

//simple box type parsing for muli and sbg+ateme+enensys extensions
#define _BOX_MFU_MULI 0x6d756c69

/*
 *
    Box Type: `mjsd`
	Container: muli
	Mandatory: No
	Quantity: Zero or Exactly one per sample
	aligned(8) class mjae_sample_timestamp_descriptor extends Box("mjsd") {
		unsigned int(64) sample_presentation_time;
		unsigned int(64) sample_decode_time;
	}

	sample_presentation_time
	indicates the presentation time of the in the designated MFU sample by the 64-bit NTP time stamp format

	sample_decode_time
	indicates the decode time of the in the designated MFU sample by the 64-bit NTP time stamp format
 */
#define _BOX_MFU_MJSD 0x6d6a7364

/*
 	Box Type: `mjgp`
	Container: mjsd
	Mandatory: No
	Quantity: zero or more
	aligned(8) class mjae_scte35_binary_payload extends Box ("mjgp"){
		unsigned bit(7) reserved '1111111' unsigned bit(33) pts_timestamp;
	bit(8) signal_data[];
	pts_timestamp
	indicates the MPEG-TS clock reference (90,000Hz)of the SCTE35_signal muxed in relation to the sample_presentation_time in the parent box
	signal_data
	binary payload of the raw SCTE35 splice command
 *
 */

#define _BOX_MFU_MJGP 0x6d6a6770

#if defined (__cplusplus)
extern "C" {
#endif

mmtp_mpu_packet_t* mmtp_mpu_packet_parse_from_block_t(mmtp_packet_header_t* mmtp_packet_header, block_t* udp_packet);


//mpu_data_unit_payload_fragments_t* mpu_data_unit_payload_fragments_find_mpu_sequence_number(mpu_data_unit_payload_fragments_vector_t *vec, uint32_t mpu_sequence_number);
//mpu_data_unit_payload_fragments_t* mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet(mpu_data_unit_payload_fragments_vector_t *vec, mmtp_payload_fragments_union_t *mpu_type_packet);
//
//mpu_fragments_t* mpu_fragments_get_or_set_packet_id(mmtp_sub_flow_t* mmtp_sub_flow, uint16_t mmtp_packet_id);
//void mpu_fragments_assign_to_payload_vector(mmtp_sub_flow_t* mmtp_sub_flow, mmtp_payload_fragments_union_t* mpu_type_packet);
//deprecated 2019-05-06 mpu_fragments_t* mpu_fragments_find_packet_id(mmtp_sub_flow_vector_t *vec, uint16_t mmtp_packet_id);

void mmt_mpu_free_payload(mmtp_mpu_packet_t* mmtp_mpu_packet);

#if defined (__cplusplus)
}
#endif


#define __MMT_MPU_PARSER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMT_MPU_PARSER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMT_MPU_PARSER_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMT_MPU_PARSER_DEBUG(...)   if(_MPU_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMT_MPU_PARSER_TRACE(...)   if(_MPU_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_MMT_MPU_PARSER_H_ */
