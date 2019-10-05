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

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);

/*
 * create a new hevc_decoder_configuration_record
 * set our configuration_version to 1,
 * 	todo: set MSB bitmasks to 1 to match ISO 14496-15:2019 spec
 */
hevc_decoder_configuration_record_t* hevc_decoder_configuration_record_new() {
	hevc_decoder_configuration_record_t* hevc_decoder_configuration_record = calloc(1, sizeof(hevc_decoder_configuration_record_t));
	hevc_decoder_configuration_record->configuration_version = 1;

	return hevc_decoder_configuration_record;
}

/*
 * legacy (read: non ATSC/3.0) avcC SPS/PPS support
 */

avc1_decoder_configuration_record_t* avc1_decoder_configuration_record_new() {
	avc1_decoder_configuration_record_t* avc1_decoder_configuration_record = calloc(1, sizeof(avc1_decoder_configuration_record_t));
	return avc1_decoder_configuration_record;
}


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



//atsc3_avc1_nal_unit_sps
void atsc3_avc1_nal_unit_sps_free(atsc3_avc1_nal_unit_sps_t** atsc3_avc1_nal_unit_sps_p) {
	if(atsc3_avc1_nal_unit_sps_p) {
		atsc3_avc1_nal_unit_sps_t* atsc3_avc1_nal_unit_sps = *atsc3_avc1_nal_unit_sps_p;
		if(atsc3_avc1_nal_unit_sps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_avc1_nal_unit_sps->nal_unit);

			free(atsc3_avc1_nal_unit_sps);
		}
		atsc3_avc1_nal_unit_sps = NULL;
		*atsc3_avc1_nal_unit_sps_p = NULL;
	}
}

//atsc3_avc1_nal_unit_pps
void atsc3_avc1_nal_unit_pps_free(atsc3_avc1_nal_unit_pps_t** atsc3_avc1_nal_unit_pps_p) {
	if(atsc3_avc1_nal_unit_pps_p) {
		atsc3_avc1_nal_unit_pps_t* atsc3_avc1_nal_unit_pps = *atsc3_avc1_nal_unit_pps_p;
		if(atsc3_avc1_nal_unit_pps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_avc1_nal_unit_pps->nal_unit);

			free(atsc3_avc1_nal_unit_pps);
		}
		atsc3_avc1_nal_unit_pps = NULL;
		*atsc3_avc1_nal_unit_pps_p = NULL;
	}
}

video_decoder_configuration_record_t* video_decoder_configuration_record_new() {
    video_decoder_configuration_record_t* video_decoder_configuration_record = calloc(1, sizeof(video_decoder_configuration_record_t));
    return video_decoder_configuration_record;
}

//process either avcC or hvcC, use video_decoder_configuration_record (containing both avcC and hvcC decoder records)

video_decoder_configuration_record_t* atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t* mpu_metadata_block) {
    video_decoder_configuration_record_t* video_decoder_configuration_record = video_decoder_configuration_record_new();
    
	if(!mpu_metadata_block || mpu_metadata_block->p_size < 8) {
		goto error;
	}

	block_Rewind(mpu_metadata_block);

	//todo: search for isobmff box: hvcC

	_ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: mpu_metadata_block_t: %p, p_buffer: %p, pos: %d, size: %d",
			mpu_metadata_block,
			mpu_metadata_block->p_buffer,
			mpu_metadata_block->i_pos,
			mpu_metadata_block->p_size);

	uint8_t* mpu_ptr = block_Get(mpu_metadata_block);

	bool has_hvcC_match = false;
	int  hvcC_match_index = 0;

	bool has_avcC_match = false;
	int  avcC_match_index = 0;


	for(int i=0; !(has_hvcC_match || has_avcC_match) && (i < mpu_metadata_block->p_size - 4); i++) {

		_ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
				i,
				mpu_ptr[i], mpu_ptr[i],
				mpu_ptr[i+1], mpu_ptr[i+1],
				mpu_ptr[i+2], mpu_ptr[i+2],
				mpu_ptr[i+3], mpu_ptr[i+3]);

		//look for our HEVC hvcC first, then fallback to avcC
		if(mpu_ptr[i] == 'h' && mpu_ptr[i+1] == 'v' && mpu_ptr[i+2] == 'c' && mpu_ptr[i+3] == 'C') {
			_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: found matching hvcC at position: %d", i);
			has_hvcC_match = true;
			hvcC_match_index = i + 4;
		} else if(mpu_ptr[i] == 'a' && mpu_ptr[i+1] == 'v' && mpu_ptr[i+2] == 'c' && mpu_ptr[i+3] == 'C') {
			_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: AVC: found matching avcC at position: %d", i);
			has_avcC_match = true;
			avcC_match_index = i + 4;
		}
	}

	if(!has_hvcC_match && !has_avcC_match) {
		goto error;
	}

    //process as HEVC w/ hvcC match
	if(has_hvcC_match) {
        hevc_decoder_configuration_record_t* hevc_decoder_configuration_record = hevc_decoder_configuration_record_new();

		//todo: parse trailing 22 bytes after hvcC box name

		//hack
		hvcC_match_index += 22;
		//todo: parse out nals:
		/*
		 * unsigned int(8) numOfArrays;
		 *
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

		hevc_decoder_configuration_record->num_of_arrays = mpu_ptr[hvcC_match_index];
		_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: num_of_arrays: %d", hevc_decoder_configuration_record->num_of_arrays);

		for(int i=0; i < hevc_decoder_configuration_record->num_of_arrays; i++) {

		}
    
        video_decoder_configuration_record->hevc_decoder_configuration_record = hevc_decoder_configuration_record;
        return video_decoder_configuration_record;
	}

	if(has_avcC_match) {
		/**
		 *  from https://github.com/aizvorski/h264bitstream/blob/master/h264_avcc.c
		 * 	avcc->configurationVersion = bs_read_u8(b);				//8
			  avcc->AVCProfileIndication = bs_read_u8(b);			//16
			  avcc->profile_compatibility = bs_read_u8(b);			//24
			  avcc->AVCLevelIndication = bs_read_u8(b);				//32

			  * int reserved = bs_read_u(b, 6); // '111111'b;		//40
			  avcc->lengthSizeMinusOne = bs_read_u(b, 2);

			  * int reserved = bs_read_u(b, 3); // '111'b;			//48 = 6 bytes
  	  	  	  avcc->numOfSequenceParameterSets = bs_read_u(b, 5);

		 */
		int avc_offset = avcC_match_index;

		avc1_decoder_configuration_record_t* avc1_decoder_configuration_record = avc1_decoder_configuration_record_new();
		avc1_decoder_configuration_record->configuration_version = mpu_ptr[avc_offset++];
		avc1_decoder_configuration_record->avc_profile_indication = mpu_ptr[avc_offset++];
		avc1_decoder_configuration_record->profile_compatibility = mpu_ptr[avc_offset++];
		avc1_decoder_configuration_record->avc_level_indication = mpu_ptr[avc_offset++];
        avc1_decoder_configuration_record->length_size_minus_one = mpu_ptr[avc_offset++];

		uint8_t num_of_sequence_parameter_sets_temp = mpu_ptr[avc_offset++];

		if((num_of_sequence_parameter_sets_temp >> 5) != 0x7) { //3 MSBits set to '111' reserved
			_ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: num_of_sequence_parameter_sets_temp 3 MSB != 111");
		}
        
        //SPS count
		avc1_decoder_configuration_record->num_of_sequence_parameter_sets = num_of_sequence_parameter_sets_temp & 0x1F;
		_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: num_of_sequence_parameter_sets: %u",  avc1_decoder_configuration_record->num_of_sequence_parameter_sets);

		//start parsing SPS here
		for(int i = 0; i < avc1_decoder_configuration_record->num_of_sequence_parameter_sets; i++) {
			atsc3_avc1_nal_unit_sps_t* atsc3_avc1_nal_unit_sps = atsc3_avc1_nal_unit_sps_new();
			atsc3_avc1_nal_unit_sps->nal_unit_length = ntohs(*((uint16_t*)(&mpu_ptr[avc_offset])));
			avc_offset+=2;

			atsc3_avc1_nal_unit_sps->nal_unit = block_Duplicate_from_ptr(&mpu_ptr[avc_offset], atsc3_avc1_nal_unit_sps->nal_unit_length);
			avc1_decoder_configuration_record_add_atsc3_avc1_nal_unit_sps(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);
			avc_offset += atsc3_avc1_nal_unit_sps->nal_unit_length;

			_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: adding SPS: %d, nal_length: %u, nal_unit->p_size: %d",
                                            i, atsc3_avc1_nal_unit_sps->nal_unit_length, atsc3_avc1_nal_unit_sps->nal_unit->p_size);
		}

		//PPS count
		avc1_decoder_configuration_record->num_of_picture_parameter_sets = mpu_ptr[avc_offset++];
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: num_of_picture_parameter_sets: %u",  avc1_decoder_configuration_record->num_of_picture_parameter_sets);

		//start parsing PPS here
		for(int i = 0; i < avc1_decoder_configuration_record->num_of_sequence_parameter_sets; i++) {
			atsc3_avc1_nal_unit_pps_t* atsc3_avc1_nal_unit_pps = atsc3_avc1_nal_unit_pps_new();
			atsc3_avc1_nal_unit_pps->nal_unit_length = ntohs(*((uint16_t*)(&mpu_ptr[avc_offset])));
            avc_offset+=2;
            
			atsc3_avc1_nal_unit_pps->nal_unit = block_Duplicate_from_ptr(&mpu_ptr[avc_offset], atsc3_avc1_nal_unit_pps->nal_unit_length);
			avc1_decoder_configuration_record_add_atsc3_avc1_nal_unit_pps(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);
			avc_offset += atsc3_avc1_nal_unit_pps->nal_unit_length;

        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: adding pps: %d, nal_length: %u, nal_unit->p_size: %d", i, atsc3_avc1_nal_unit_pps->nal_unit_length,  atsc3_avc1_nal_unit_pps->nal_unit->p_size);
		}
        
        video_decoder_configuration_record->avc1_decoder_configuration_record = avc1_decoder_configuration_record;
        return video_decoder_configuration_record;

	}


    _ATSC3_HEVC_NAL_EXTRACTOR_ERROR("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: error processing: ptr: %p, size: %d",
            mpu_metadata_block, mpu_metadata_block->p_size);

	//hevc_decoder_configuration_record; //or avc1_decoder_configuration_record

error:

	_ATSC3_HEVC_NAL_EXTRACTOR_ERROR("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: error processing: ptr: %p, size: %d",
			mpu_metadata_block, mpu_metadata_block->p_size);

	if(video_decoder_configuration_record) {
		free(video_decoder_configuration_record);
		video_decoder_configuration_record = NULL;
	}

	return NULL;
}


