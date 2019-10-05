/*
 * atsc3_hevc_nal_extractor.c
 *
 *  Created on: Oct 5, 2019
 *      Author: jjustman
 */

#include "atsc3_hevc_nal_extractor.h"

int _ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED = 0;
int _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED = 0;
int _ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED = 0;

//default atsc3_vector_builder collection for hevc nal types

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_vps);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_sps);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_pps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_prefix_sei);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_suffix_sei);

hevc_decoder_configuration_record_t* atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t*) {

	return NULL;
}
