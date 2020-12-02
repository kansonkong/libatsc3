//
// Created by Jason Justman on 12/1/20.
//
#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_VIDEO_DECODER_CONFIGURATION_RECORD_H_
#define ATSC3_VIDEO_DECODER_CONFIGURATION_RECORD_H_


#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_isobmff_box_parser_tools.h"

#ifdef __cplusplus
extern "C" {
#endif



//video parameter set
typedef struct atsc3_nal_unit_vps {
    uint8_t		array_completeness:1;
    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_nal_unit_vps_t;

//sequence parameter set
typedef struct atsc3_nal_unit_sps {
    uint8_t		array_completeness:1;

    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_nal_unit_sps_t;

//avc1 sequence parameter set
typedef struct atsc3_avc1_nal_unit_sps {
    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_avc1_nal_unit_sps_t;


//picture parameter set
typedef struct atsc3_nal_unit_pps {
    uint8_t		array_completeness:1;

    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_nal_unit_pps_t;

//avc1 picture parameter set
typedef struct atsc3_avc1_nal_unit_pps {
    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_avc1_nal_unit_pps_t;



//prefex_SEI
typedef struct atsc3_nal_unit_prefix_sei {
    uint8_t		array_completeness:1;

    uint16_t	nal_unit_length;
    block_t*	nal_unit;

} atsc3_nal_unit_prefix_sei_t;


//suffix_SEI
typedef struct atsc3_nal_unit_suffix_sei {
    uint8_t		array_completeness:1;

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
    block_t*	box_data_original;

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

    uint8_t 	num_of_arrays;	//interim while we built our vectors

    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_vps);
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_sps);
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_pps);

    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_prefix_sei);
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_nal_unit_suffix_sei);

    block_t*     hevc_nals_combined;

} hevc_decoder_configuration_record_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_vps);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_sps);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_pps);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_prefix_sei);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(hevc_decoder_configuration_record, atsc3_nal_unit_suffix_sei);

typedef struct avc1_decoder_configuration_record {

    uint8_t 		configuration_version;
    uint8_t 		avc_profile_indication;
    uint8_t 		profile_compatibility;
    uint8_t  		avc_level_indication;
    uint8_t         length_size_minus_one:2;

    //upper 3 msb: 111
    uint8_t			num_of_sequence_parameter_sets:5;
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_avc1_nal_unit_sps);

    uint8_t			num_of_picture_parameter_sets;
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_avc1_nal_unit_pps);

} avc1_decoder_configuration_record_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);


//since we don't know if we will get hvcC or avcC markers, be prepared for both configuration record types
typedef struct atsc3_video_decoder_configuration_record {
    bool has_hev1_box;
    bool has_hvcC_box;

    hevc_decoder_configuration_record_t* hevc_decoder_configuration_record;

    bool has_avc1_box;
    bool has_avcC_box;
    avc1_decoder_configuration_record_t* avc1_decoder_configuration_record;

    //extracted from moov/init segment
    uint32_t width;
    uint32_t height;
    uint32_t timebase;

    //extracted from moof/fragment metadata
    uint32_t sample_duration;

} atsc3_video_decoder_configuration_record_t;

atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record_new();
void atsc3_video_decoder_configuration_record_free(atsc3_video_decoder_configuration_record_t** atsc3_video_decoder_configuration_record_p);

void atsc3_hevc_decoder_configuration_record_free(hevc_decoder_configuration_record_t** hevc_decoder_configuration_record_p);
void atsc3_avc1_decoder_configuration_record_free(avc1_decoder_configuration_record_t** avc1_decoder_configuration_record_p);

#ifdef __cplusplus
}
#endif

#endif //ATSC3_VIDEO_DECODER_CONFIGURATION_RECORD_H_
