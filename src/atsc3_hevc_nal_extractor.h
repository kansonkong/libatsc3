/*
 * atsc3_hevc_nal_extractor.h
 *
 *  Created on: Oct 5, 2019
 *      Author: jjustman
 *
 *  for extraction of NAL (sps/pps/vps) from ISOBMFF MPU metadata fragments
 *	for use with HEVC decoder initialization
 *
 *	Specification Reference: ISO 14496-15:2019 - https://www.iso.org/standard/74429.html
 *
 *	Consumer Implementation Use Case: Android MediaCodec: Codec Specific Data (HEVC) - https://developer.android.com/reference/android/media/MediaCodec
 *
 *
 *  It is recommended that the arrays be in the order VPS, SPS, PPS, prefix SEI, suffix SEI.
 */

#ifndef ATSC3_HEVC_NAL_EXTRACTOR_H_
#define ATSC3_HEVC_NAL_EXTRACTOR_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

//video parameter set
typedef struct atsc3_nal_unit_vps {
	uint16_t	nal_unit_length;
	block_t*	nal_unit;

} atsc3_nal_unit_vps_t;

//sequence parameter set
typedef struct atsc3_nal_unit_sps {
	uint16_t	nal_unit_length;
	block_t*	nal_unit;

} atsc3_nal_unit_sps_t;

//picture parameter set
typedef struct atsc3_nal_unit_pps {
	uint16_t	nal_unit_length;
	block_t*	nal_unit;

} atsc3_nal_unit_pps_t;


//prefex_SEI
typedef struct atsc3_nal_unit_prefix_sei {
	uint16_t	nal_unit_length;
	block_t*	nal_unit;

} atsc3_nal_unit_prefix_sei_t;


//suffix_SEI
typedef struct atsc3_nal_unit_suffix_sei {
	uint16_t	nal_unit_length;
	block_t*	nal_unit;

} atsc3_nal_unit_suffix_sei_t;


/*
 * unsigned int(8) numOfArrays;
for (j=0; j < numOfArrays; j++) {
    bit(1) array_completeness;
    unsigned int(1) reserved = 0;
    unsigned int(6) NAL_unit_type;
    unsigned int(16) numNalus;
    for (i=0; i< numNalus; i++) {
        unsigned int(16) nalUnitLength;
        bit(8*nalUnitLength) nalUnit;
    }
}
 */

/*
 * ISO/IEC 14496-15:2019(E) - 8.3.3.1.2 Syntax
aligned(8) class HEVCDecoderConfigurationRecord {
 unsigned int(8) configurationVersion = 1;
 unsigned int(2) general_profile_space;
 unsigned int(1) general_tier_flag;
 unsigned int(5) general_profile_idc;

 unsigned int(32) general_profile_compatibility_flags;
 unsigned int(48) general_constraint_indicator_flags;

 unsigned int(8) general_level_idc;
 bit(4) reserved = '1111'b;
 unsigned int(12) min_spatial_segmentation_idc;
 bit(6) reserved = '111111'b;
 unsigned int(2) parallelismType;
 bit(6) reserved = '111111'b;
 unsigned int(2) chroma_format_idc;
 bit(5) reserved = '11111'b;
 unsigned int(3) bit_depth_luma_minus8;
 bit(5) reserved = '11111'b;
 unsigned int(3) bit_depth_chroma_minus8;
 unsigned int(16) avgFrameRate;
 unsigned int(2) constantFrameRate;
 unsigned int(3) numTemporalLayers;
 unsigned int(1) temporalIdNested;
 unsigned int(2) lengthSizeMinusOne;
 unsigned int(8) numOfArrays;

 for (j=0; j < numOfArrays; j++) {
 	 unsigned int(1) array_completeness;
 	 bit(1) reserved = 0;
 	 unsigned int(6) NAL_unit_type;
 	 unsigned int(16) numNalus;
 	 for (i=0; i< numNalus; i++) {
 	 	 unsigned int(16) nalUnitLength;
 	 	 bit(8*nalUnitLength) nalUnit;
	}
 }
}
*/

typedef struct hevc_decoder_configuration_record {
	uint8_t		configuration_version; //default = 1

	uint8_t		general_profile_space:2;
	uint8_t		general_tier_flag:1;
	uint8_t		general_profile_idc:5;
	uint32_t	general_profile_compatibility_flags;
	uint64_t	general_constraint_indicator_flags:48;

	uint8_t 	general_level_idc;

	//upper 4 msb reserved: 1111 -  bit(4) reserved = '1111'b;
	uint16_t 	min_spatial_segmentation_idc:12;

	//bit(6) reserved = '111111'b;
	uint8_t 	parallelismType:2;

	//bit(6) reserved = '111111'b;
	uint8_t 	chroma_format_idc:2;

	//bit(5) reserved = '11111'b;
	uint8_t 	bit_depth_luma_minus8:3;

	//bit(5) reserved = '11111'b;
	uint8_t		bit_depth_chroma_minus8:3;

	uint16_t	avg_frame_rate;
	uint8_t		constant_frame_rate:2;
	uint8_t		num_temporal_layers:3;
	uint8_t 	temporal_id_nested:1;
	uint8_t 	length_size_minus_one:2;

	uint8_t 					num_of_arrays;	//interim while we built our vectors

	uint8_t						atsc3_nal_unit_vps_array_completeness:1;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_vps);
	uint8_t						atsc3_nal_unit_sps_array_completeness:1;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_sps);
	uint8_t						atsc3_nal_unit_pps_array_completeness:1;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_pps);

	uint8_t						atsc3_nal_unit_prefix_sei_array_completeness:1;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_prefix_sei);
	uint8_t						atsc3_nal_unit_suffix_sei_array_completeness:1;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_suffix_sei);

} hevc_decoder_configuration_record_t;

hevc_decoder_configuration_record_t* atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t*);



ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_vps);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_sps);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_pps);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_prefix_sei);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_suffix_sei);


#define _ATSC3_HEVC_NAL_EXTRACTOR_ERROR(...)  	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_INFO(...)   	if(_ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG(...)  	if(_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_HEVC_NAL_EXTRACTOR_TRACE(...)	if(_ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_HEVC_NAL_EXTRACTOR_H_ */
