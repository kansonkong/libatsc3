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

/**
 * add default _free methods for each one of these to block_Destroy nal_unit, e.g.:
 *
 * void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t** atsc3_nal_unit_vps_p) {
	if(atsc3_nal_unit_vps_p) {
		atsc3_nal_unit_vps_t* atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
		if(atsc3_nal_unit_vps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_vps->nal_unit);

			free(atsc3_nal_unit_vps);
		}
		atsc3_nal_unit_vps = NULL;
		*atsc3_nal_unit_vps_p = NULL;
	}
}

s/nal_unit_vps/nal_unit_sps ...etc
*/

//atsc3_nal_unit_vps_free
void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t** atsc3_nal_unit_vps_p) {
	if(atsc3_nal_unit_vps_p) {
		atsc3_nal_unit_vps_t* atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
		if(atsc3_nal_unit_vps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_vps->nal_unit);

			free(atsc3_nal_unit_vps);
		}
		atsc3_nal_unit_vps = NULL;
		*atsc3_nal_unit_vps_p = NULL;
	}
}

//atsc3_nal_unit_sps
void atsc3_nal_unit_sps_free(atsc3_nal_unit_sps_t** atsc3_nal_unit_sps_p) {
	if(atsc3_nal_unit_sps_p) {
		atsc3_nal_unit_sps_t* atsc3_nal_unit_sps = *atsc3_nal_unit_sps_p;
		if(atsc3_nal_unit_sps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_sps->nal_unit);

			free(atsc3_nal_unit_sps);
		}
		atsc3_nal_unit_sps = NULL;
		*atsc3_nal_unit_sps_p = NULL;
	}
}

//atsc3_nal_unit_pps
void atsc3_nal_unit_pps_free(atsc3_nal_unit_pps_t** atsc3_nal_unit_pps_p) {
	if(atsc3_nal_unit_pps_p) {
		atsc3_nal_unit_pps_t* atsc3_nal_unit_pps = *atsc3_nal_unit_pps_p;
		if(atsc3_nal_unit_pps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_pps->nal_unit);

			free(atsc3_nal_unit_pps);
		}
		atsc3_nal_unit_pps = NULL;
		*atsc3_nal_unit_pps_p = NULL;
	}
}

//atsc3_nal_unit_prefix_sei
void atsc3_nal_unit_prefix_sei_free(atsc3_nal_unit_prefix_sei_t** atsc3_nal_unit_prefix_sei_p) {
	if(atsc3_nal_unit_prefix_sei_p) {
		atsc3_nal_unit_prefix_sei_t* atsc3_nal_unit_prefix_sei = *atsc3_nal_unit_prefix_sei_p;
		if(atsc3_nal_unit_prefix_sei) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_prefix_sei->nal_unit);

			free(atsc3_nal_unit_prefix_sei);
		}
		atsc3_nal_unit_prefix_sei = NULL;
		*atsc3_nal_unit_prefix_sei_p = NULL;
	}
}

//atsc3_nal_unit_suffix_sei
void atsc3_nal_unit_suffix_sei_free(atsc3_nal_unit_suffix_sei_t** atsc3_nal_unit_suffix_sei_p) {
	if(atsc3_nal_unit_suffix_sei_p) {
		atsc3_nal_unit_suffix_sei_t* atsc3_nal_unit_suffix_sei = *atsc3_nal_unit_suffix_sei_p;
		if(atsc3_nal_unit_suffix_sei) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_suffix_sei->nal_unit);

			free(atsc3_nal_unit_suffix_sei);
		}
		atsc3_nal_unit_suffix_sei = NULL;
		*atsc3_nal_unit_suffix_sei_p = NULL;
	}
}



hevc_decoder_configuration_record_t* atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t* mpu_metadata_block) {
	hevc_decoder_configuration_record_t* hevc_decoder_configuration_record = hevc_decoder_configuration_record_new();
	hevc_decoder_configuration_record->configuration_version = 1;

	//todo: search for isobmff box: hvcC

	//todo: parse trailing 22 bytes after hvcC box name

	//todo: parse out nals:
	/*
	 *  for (j=0; j < numOfArrays; j++) {
 	 unsigned int(1) array_completeness;
 	 bit(1) reserved = 0;
 	 unsigned int(6) NAL_unit_type;
 	 unsigned int(16) numNalus;
 	 for (i=0; i< numNalus; i++) {
 	 	 unsigned int(16) nalUnitLength;
 	 	 bit(8*nalUnitLength) nalUnit;
	}
 }
	 */


	return NULL;
}
